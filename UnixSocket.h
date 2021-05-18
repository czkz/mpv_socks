#pragma once
#include <string>
#include <string_view>
#include <stdexcept>

// Unix socket includes
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>


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
            throw std::runtime_error("UnixSocket::UnixSocket(): path too long");
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
        int err = write(sockfd, data.data(), data.size());
        if (err == -1){
            throw std::runtime_error("In UnixSocket::Send(): write() failed");
        }
    }

    /// Receives an undetermined amount of whole lines
    std::string Receive() {
        std::string ret;
        constexpr size_t BUF_SIZE = 256;
        int n = -1;

        do {
            std::string buf (BUF_SIZE, '\0');
            n = read(sockfd, buf.data(), buf.size());
            if (n < 0) {
                throw std::runtime_error("In UnixSocket::Receive(): read() failed");
            }
            buf.resize(n);
            ret += buf;
        } while (ret.back() != '\n');
        return ret;
    }

};

