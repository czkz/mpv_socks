#include "Interface.h"

#include <iostream>


namespace Interface {
    void onInit(MpvController& mpv) {
        std::cout << "Interface::onInit()\n";
        mpv.Play();
    }
    void onTick(MpvController&) {
        // std::cout << "Interface::onTick()\n";
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
}
