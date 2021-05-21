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
            } else if (cmd == "seek") {
                int time;
                std::cin >> time;
                mpv.Seek(time);
            } else if (!cmd.empty()){
                std::cout << "Unknown command\n";
            }
        }
        try {  // If mpv is closed this will throw
            mpv.Quit();
        } catch (const UnixSocket::exception& e) { }
    }

    std::thread cin_thread;
}

namespace Interface {
    void onInit(MpvController& mpv) {
        cin_thread = std::thread(thread_func, std::ref(mpv));
        mpv.Play();
    }
    void onQuit() {
        std::cout << "Interface::onQuit()\n";
        std::cin.setstate(std::ios::eofbit);
        cin_thread.join();
    }

    void onTick(MpvController&) { }
    void onPause(MpvController&, float) { }
    void onUnpause(MpvController&, float) { }
    void onSeek(MpvController&, float) { }
}
