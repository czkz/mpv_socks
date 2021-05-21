#pragma once
#include <functional>
#include <thread>
#include <atomic>

class ChildProcess {
private:
    static void threadFunc(const char* cmd, ChildProcess* _this) {
        int code = std::system(cmd);
        _this->return_code = code;
        _this->finished = true;
    }

    int return_code;
    std::thread thread_handle;
    std::atomic_bool finished = false;

public:
    // ~ChildProcess() {
    //     if (thread_handle.joinable()) {
    //         thread_handle.join();
    //     }
    // }

    ChildProcess() = default;
    ChildProcess(const char* cmd) { this->Start(cmd); }

    bool Start(const char* cmd) {
        if (thread_handle.joinable()) { return false; }
        thread_handle = std::thread(threadFunc, cmd, this);
        return true;
    }

    bool Finished() const {
        return finished;
    }

    int Wait() {
        thread_handle.join();
        return return_code;
    }
};
