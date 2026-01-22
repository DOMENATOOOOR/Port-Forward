#include "../common/Socket.h"
#include "../common/Frame.h"
#include "../common/FrameParser.h"

#include <arpa/inet.h>
#include <sys/select.h>
#include <map>

static int tcpListen(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (sockaddr*)&addr, sizeof(addr));
    listen(fd, 1);
    return fd;
}

static int tcpConnect(const char* host, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);
    connect(fd, (sockaddr*)&addr, sizeof(addr));
    return fd;
}

int main(int argc, char** argv) {
    int listenPort = std::stoi(argv[1]);
    const char* targetHost = argv[2];
    int targetPort = std::stoi(argv[3]);

    Socket listenSock(tcpListen(listenPort));
    Socket tunnel(accept(listenSock.get(), nullptr, nullptr));

    std::map<uint8_t, Socket> servers;
    FrameParser parser;

    while (true) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(tunnel.get(), &rfds);
        int maxfd = tunnel.get();

        for (auto& [id, sock] : servers) {
            FD_SET(sock.get(), &rfds);
            maxfd = std::max(maxfd, sock.get());
        }

        select(maxfd + 1, &rfds, nullptr, nullptr, nullptr);

        if (FD_ISSET(tunnel.get(), &rfds)) {
            uint8_t buf[4096];
            int n = recv(tunnel.get(), buf, sizeof(buf), 0);
            for (int i = 0; i < n; ++i) {
                auto fr = parser.feed(buf[i]);
                if (!fr) continue;

                if (fr->conn == 0 && fr->data[0] == 0x01) {
                    servers.emplace(fr->data[1],
                        Socket(tcpConnect(targetHost, targetPort)));
                } else if (fr->conn == 0 && fr->data[0] == 0x02) {
                    servers.erase(fr->data[1]);
                } else {
                    send(servers[fr->conn].get(),
                         fr->data.data(), fr->data.size(), 0);
                }
            }
        }

        for (auto& [id, sock] : servers) {
            if (FD_ISSET(sock.get(), &rfds)) {
                uint8_t buf[4096];
                int n = recv(sock.get(), buf, sizeof(buf), 0);
                if (n <= 0) {
                    Frame f{0, {0x02, id}};
                    auto enc = encodeFrame(f);
                    send(tunnel.get(), enc.data(), enc.size(), 0);
                    servers.erase(id);
                    break;
                }
                Frame f{id, {buf, buf + n}};
                auto enc = encodeFrame(f);
                send(tunnel.get(), enc.data(), enc.size(), 0);
            }
        }
    }
}
