#pragma once
#include <string>
#include <string_view>
#include <stdexcept>

// Unix socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>

// Unix signal() include
#include <signal.h>


class UnixSocket {
private:
    int sockfd;
    sockaddr_un address;

public:
    class exception : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

public:
    UnixSocket(std::string_view path) {
        if ( path.size() > sizeof(sockaddr_un::sun_path) - 1 ) {
            throw exception("UnixSocket::UnixSocket(): path too long");
        }

        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        address.sun_family = AF_UNIX;

        strcpy(address.sun_path, path.data());
        address.sun_path[path.size()] = '\0';
    }

    ~UnixSocket() {
        close(sockfd);
    }

public:
    bool Connect() {
        int err = connect(sockfd, (sockaddr*)&address, sizeof(address));
        return err != -1;
    }

    void Send(std::string_view data) {
        int err = send(sockfd, data.data(), data.size(), MSG_NOSIGNAL);
        if (err == -1){
            throw exception("In UnixSocket::Send(): write() failed");
        }
    }

    /// Receives all available data
    std::string Receive() {
        std::string ret;
        constexpr size_t BUF_SIZE = 256;
        int n = -1;

        do {
            std::string buf (BUF_SIZE, '\0');
            n = recv(sockfd, buf.data(), buf.size(), MSG_NOSIGNAL | MSG_DONTWAIT);
            if (n < 0) {
                if (errno == EWOULDBLOCK) {
                    n = 0;
                } else {
                    throw exception("In UnixSocket::Receive(): read() failed");
                }
            }
            buf.resize(n);
            ret += buf;
        } while (n != 0);
        return ret;
    }
};

