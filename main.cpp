#include <iostream>
#include "ChildProcess.h"
#include "MpvSocket.h"
#include "MpvController.h"
#include "Interface.h"


int main() try {

    ChildProcess mpv_process;
    mpv_process.Start("mpv ~/Downloads/15919603606180.mp4 \
            --input-ipc-server=./mpvsocket --loop --mute --no-terminal --fs=no --pause");


    MpvSocket mpv ("/home/dek/proj/usock/mpvsocket");
    while ( !mpv.Connect() ) {
        if (mpv_process.Finished()) {
            std::cerr << "mpv process exited unexpectedly with code ";
            std::cerr << mpv_process.Wait() << '\n';
            return 1;
        }
        std::this_thread::yield();
    }


    MpvController mpv_controller { mpv };
    Interface::onInit(mpv_controller);

    std::optional<bool> paused;
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
        mpv.GetProperty("playback-time", [&](nlohmann::json v) {
            Interface::onSeek(mpv_controller, v["data"]);
        });
    });

    // Main loop
    while (!mpv_process.Finished()) {
        mpv.onTick();
        Interface::onTick(mpv_controller);
        std::this_thread::sleep_for(std::chrono::milliseconds(0));
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
