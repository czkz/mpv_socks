#include "Interface.h"

#include <iostream>
#include <thread>

namespace {
    void thread_func(MpvController& mpv) {
        std::string cmd;
        while (std::cin >> cmd) {
            if (cmd == "play") {
                mpv.Play();
            } else if (cmd == "pause") {
                mpv.Pause();
            } else if (cmd == "quit") {
                mpv.Quit();
                break;
            } else if (cmd == "set") {
                int time;
                std::cin >> time;
                mpv.SetPlaybackTime(time);
            } else if (!cmd.empty()){
                std::clog << "Unknown command\n";
            }
        }
        try {  // If mpv is closed this will throw
            mpv.Quit();
        } catch (const UnixSocket::exception& e) { }
    }

    std::thread cin_thread;
}

namespace Interface {
    void onReady(MpvController& mpv) {
        cin_thread = std::thread(thread_func, std::ref(mpv));
        // mpv.Play();
    }
    void onQuit() {
        std::clog << "Interface::onQuit()\n";
        std::cin.setstate(std::ios::badbit);
        cin_thread.join();
    }

    bool onInit() {
        std::clog << "Interface::onInit()\n";
        return true;
    }
    void onTick(MpvController&) {
        // std::clog << "Interface::onTick()\n";
    }
    void onPause(MpvController&, float) {
        std::cout << "pause\n";
    }
    void onUnpause(MpvController&, float) {
        std::cout << "play\n";
    }
    void onSeek(MpvController&, float) {
    }
    void onDestroy() { }
}
