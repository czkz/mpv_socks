#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>

#include "MpvSocket.h"

class RecvThread {
public:
    using cmd_callback_t = std::function<void(std::string)>;
private:
    static void threadFunc(MpvSocket& sock,
                           cmd_callback_t callback,
                           const std::atomic_bool& stop_flag)
    try {
        while (stop_flag.load() == false) {
            while(true) {
                std::string cmd = sock.Receive();
                if (cmd.empty()) { break; }
                callback(cmd);
            }
            std::this_thread::yield();
        }
    } catch (const std::exception& e) {
        std::cerr << "threadFunc() caught exception: " << e.what() << '\n';
        fprintf(stderr, "errno: %d %s\n", errno, strerror(errno));
    } catch (...) {  // while loop is broken, so this is somewhat reasonable
        std::cerr << "threadFunc() caught an unknown exception\n";
    }

    std::atomic_bool stop_flag;
    std::thread thread_handle;

    cmd_callback_t cmd_callback;

public:
    RecvThread() = default;

    ~RecvThread() {
        this->Stop();
        this->Wait();
    }

    bool SetCmdCallback(cmd_callback_t v) {
        if (thread_handle.joinable()) { return false; }
        cmd_callback = v;
        return true;
    }

    void Start(MpvSocket& sock) {
        thread_handle = std::thread(threadFunc,
                                    std::ref(sock),
                                    cmd_callback,
                                    std::cref(stop_flag));
    }

    void Stop() {
        if (thread_handle.joinable()) {
            stop_flag.store(true);
        }
    }

    /// aka std::thread::join()
    void Wait() {
        if (thread_handle.joinable()) {
            thread_handle.join();
        }
    }
};
