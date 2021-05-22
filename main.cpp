#include <iostream>
#include "ChildProcess.h"
#include "MpvSocket.h"
#include "MpvController.h"
#include "Interface.h"

class InterfaceInitGuard {
public:
    const bool initSuccessful;
    InterfaceInitGuard() : initSuccessful(Interface::onInit()) { }
    ~InterfaceInitGuard() { Interface::onDestroy(); }
};

void registerEventHandlers(MpvSocket& mpv,
                           MpvController& mpv_controller,
                           std::optional<bool>& paused,
                           bool& file_ready_seek)
{
    mpv.on("unpause", [&](nlohmann::json) {
        mpv.GetProperty("playback-time", [&](nlohmann::json v) {
            if (paused.value_or(true) == false) { return; }
            Interface::onUnpause(mpv_controller, v["data"]);
            paused = false;
        });
    });
    mpv.on("pause", [&paused, &mpv, &mpv_controller](nlohmann::json) {
        mpv.GetProperty("playback-time", [&](nlohmann::json v) {
            if (paused.value_or(false) == true) { return; }
            Interface::onPause(mpv_controller, v["data"]);
            paused = true;
        });
    });
    // "seek" event gives wrong playback-time
    mpv.on("playback-restart", [&](nlohmann::json) {
        if (!file_ready_seek) {
            file_ready_seek = true;
            Interface::onReady(mpv_controller);
        } else {
            mpv.GetProperty("playback-time", [&](nlohmann::json v) {
                Interface::onSeek(mpv_controller, v["data"]);
            });
        }
    });
}

int main() try {
    InterfaceInitGuard init_guard { };
    if (!init_guard.initSuccessful) {
        return 1;
    }

    ChildProcess mpv_process;
    mpv_process.Start("mpv --input-ipc-server=./mpvsocket \
            --loop --mute --no-terminal --fs=no --pause -- " "https://youtu.be/r2LpOUwca94");

    MpvSocket mpv ("./mpvsocket");
    while ( !mpv.Connect() ) {
        if (mpv_process.Finished()) {
            std::cerr << "mpv process exited unexpectedly with code ";
            std::cerr << mpv_process.Wait() << '\n';
            return 1;
        }
        std::this_thread::yield();
    }

    MpvController mpv_controller { mpv };

    std::optional<bool> paused;
    bool file_ready_seek = false;
    registerEventHandlers(mpv, mpv_controller, paused, file_ready_seek);

    // Main loop
    try {
        while (!mpv_process.Finished()) {
            mpv.onTick();
            if (file_ready_seek) { Interface::onTick(mpv_controller); }
            std::this_thread::yield();
        }
    } catch (const UnixSocket& e) {
        // send() broken pipe
        if (errno != 32) { throw; }
    }
    int code = mpv_process.Wait();
    if (code != 0) { std::cout << "mpv return code: " << code << '\n'; }

    Interface::onQuit();
    (void) mpv_controller;  // Must still exist

} catch (const std::exception& e) {
    std::cerr << "Caught exception: " << e.what() << '\n';
    fprintf(stderr, "errno: %d %s\n", errno, strerror(errno));
    return 2;
}
