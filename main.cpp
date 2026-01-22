#include "Sender.cpp"
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        return 1;
    }

    uint16_t listenPort = std::atoi(argv[1]);
    const char* serverHost = argv[2];
    uint16_t serverPort = std::atoi(argv[3]);

    Sender s(listenPort, serverHost, serverPort);
    s.run();
}
