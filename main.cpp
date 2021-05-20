#include <iostream>
#include "ChildProcess.h"
#include "MpvSocket.h"
#include "Interface.h"


int main() try {

    ChildProcess mpv_process;
    mpv_process.Start("mpv ~/Downloads/15919603606180.mp4 \
            --input-ipc-server=./mpvsocket --loop --mute --no-terminal --fs=no");


    MpvSocket mpv ("/home/dek/proj/usock/mpvsocket");
    while ( !mpv.Connect() ) {
        if (mpv_process.Finished()) {
            std::cerr << "mpv process exited unexpectedly with code ";
            std::cerr << mpv_process.Wait() << '\n';
            return 1;
        }
        std::this_thread::yield();
    }


    auto cin_th = std::thread([&mpv]() {
        std::string s;
        while (std::getline(std::cin, s)) {
            if (s == "play") {
                mpv.SetProperty("pause", false);
            } else if (s == "pause") {
                mpv.SetProperty("pause", true);
            } else if (s.substr(0, 5) == "seek ") {
                int time = std::atoi(s.substr(5).c_str());
                mpv.Command({"seek", time, "relative"});
            } else if (!s.empty()){
                std::cout << "Unknown command\n";
            }
        }
        try {  // If mpv is closed this will throw
            mpv.Command({"quit"});
        } catch (const UnixSocket::exception& e) { }
    });


    Interface::onInit();


    std::optional<bool> paused;

    mpv.on("unpause", [&mpv, &paused](nlohmann::json) {
        mpv.GetProperty("playback-time", [&paused](nlohmann::json v) {
            if (paused.value_or(true) == false) { return; }
            Interface::onUnpause(v["data"]);
            paused = false;
        });
    });
    mpv.on("pause", [&mpv, &paused](nlohmann::json) {
        mpv.GetProperty("playback-time", [&paused](nlohmann::json v) {
            if (paused.value_or(false) == true) { return; }
            Interface::onPause(v["data"]);
            paused = true;
        });
    });
    mpv.on("seek", [&mpv](nlohmann::json) {
        mpv.GetProperty("playback-time", [](nlohmann::json v) {
            Interface::onSeek(v["data"]);
        });
    });

    // Main loop
    while (!mpv_process.Finished()) {
        mpv.onTick();
        Interface::onTick();
        std::this_thread::yield();
    }
    int code = mpv_process.Wait();
    if (code != 0) { std::cout << "mpv return code: " << code << '\n'; }

    Interface::onQuit();

    std::cin.setstate(std::ios::eofbit);
    cin_th.join();


} catch (const std::exception& e) {
    std::cerr << "Caught exception: " << e.what() << '\n';
    fprintf(stderr, "errno: %d %s\n", errno, strerror(errno));
    return 2;
}
