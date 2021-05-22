#include "Interface.h"

#include <iostream>


namespace Interface {
    bool onInit() {
        std::cout << "Interface::onInit()\n";
        return true;
    }
    void onReady(MpvController& mpv) {
        std::cout << "Interface::onReady()\n";
        mpv.Play();
    }
    void onTick(MpvController&) {
        std::cout << "Interface::onTick()\n";
    }
    void onPause(MpvController&, float playback_time) {
        std::cout << "Interface::onPause(" << playback_time << ")\n";
    }
    void onUnpause(MpvController&, float playback_time) {
        std::cout << "Interface::onUnpause(" << playback_time << ")\n";
    }
    void onSeek(MpvController&, float playback_time) {
        std::cout << "Interface::onSeek(" << playback_time << ")\n";
    }
    void onQuit() {
        std::cout << "Interface::onQuit()\n";
    }
    void onDestroy() {
        std::cout << "Interface::onDestroy()\n";
    }
}
