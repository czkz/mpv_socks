#pragma once
#include <functional>
#include <thread>
#include <atomic>

class ChildProcess {
private:
    static void threadFunc(std::string cmd, ChildProcess* _this) {
        int code = std::system(cmd.c_str());
        _this->return_code = code;
        _this->finished = true;
    }

    int return_code;
    std::thread thread_handle;
    std::atomic_bool finished = false;

public:
    // Helps with debugging a lot
    ~ChildProcess() {
        if (thread_handle.joinable()) {
            thread_handle.join();
        }
    }

    ChildProcess() = default;
    ChildProcess(const char* cmd) { this->Start(cmd); }

    bool Start(std::string cmd) {
        if (thread_handle.joinable()) { return false; }
        thread_handle = std::thread(threadFunc, std::move(cmd), this);
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
