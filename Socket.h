#pragma once
#include <unistd.h>

class Socket {
    int fd{-1};

public:
    Socket() = default;
    explicit Socket(int fd) : fd(fd) {}
    ~Socket() { if (fd != -1) close(fd); }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept : fd(other.fd) {
        other.fd = -1;
    }

    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (fd != -1) close(fd);
            fd = other.fd;
            other.fd = -1;
        }
        return *this;
    }

    int get() const { return fd; }
    bool valid() const { return fd != -1; }
};
