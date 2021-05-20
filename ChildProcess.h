#pragma once
#include <functional>
#include <thread>
#include <atomic>

class ChildProcess {
public:
    using exit_callback_t = std::function<void(int)>;

private:
    static void threadFunc(const char* cmd, ChildProcess* _this) {
        int code = std::system(cmd);
        _this->return_code = code;
        if (_this->onExit) {
            _this->onExit(code);
        }
        _this->finished = true;
    }

    int return_code;
    std::thread thread_handle;
    exit_callback_t onExit = nullptr;
    std::atomic_bool finished = false;

public:
    // ChildProcess() = default;
    // ~ChildProcess() {
    //     if (thread_handle.joinable()) {
    //         thread_handle.join();
    //     }
    // }

    bool SetExitCallback(exit_callback_t callback) {
        if (thread_handle.joinable()) { return false; }
        onExit = callback;
        return true;
    }

    void Start(const char* cmd) {
        thread_handle = std::thread(threadFunc, cmd, this);
    }

    bool Finished() {
        return finished;
    }

    int Wait() {
        thread_handle.join();
        return return_code;
    }
};
