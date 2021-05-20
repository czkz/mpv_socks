#pragma once
#include <string>
#include <string_view>
#include <stdexcept>

#include "UnixSocket.h"


class LineUnixSocket : private UnixSocket {
    std::string buf;
public:
    using UnixSocket::UnixSocket;
    using UnixSocket::Connect;
    using UnixSocket::Send;
    /// Returns exactly one line (no '\n'), or an empty string,
    /// doesn't block
    std::string Receive() {
        buf += UnixSocket::Receive();

        std::string ret;

        auto i = buf.find('\n');
        if (i != std::string::npos) {
            ret = buf.substr(0, i);
            buf.erase(0, i + 1);
        }
        return ret;
    }
};
