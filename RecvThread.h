#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>

#include "UnixSocket.h"

class RecvThread {
public:
    using commandHandler_t = std::function<void(std::string)>;
private:
    static void threadFunc(UnixSocket& sock,
                           commandHandler_t callback,
                           const std::atomic_bool& stop_flag) try {
        using std::string;
        while (stop_flag.load() == false) {
            std::istringstream ss { sock.Receive() };

            std::string cmd;
            while (std::getline(ss, cmd)) {
                callback(cmd);
            }
        }
    } catch (const UnixSocket::exception& e) {
        std::cout << "threadFunc() caught exception: " << e.what() << '\n';
        printf("errno: %d %s\n", errno, strerror(errno));
    }

    std::atomic_bool stop_flag;
    std::thread thread_handle;

public:
    RecvThread(UnixSocket& sock, commandHandler_t callback) {
        thread_handle = std::thread(threadFunc,
                                    std::ref(sock),
                                    std::move(callback),
                                    std::cref(stop_flag));
    }

    ~RecvThread() {
        this->Stop();
    }

    void Stop() {
        if (thread_handle.joinable()) {
            stop_flag.store(true);
            thread_handle.join();
        }
    }
};
