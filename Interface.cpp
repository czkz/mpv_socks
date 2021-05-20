#include "Interface.h"

#include <iostream>


namespace Interface {
    void onInit() {
        std::cout << "Interface::onInit()\n";
    }
    void onTick() {
        // std::cout << "Interface::onTick()\n";
    }
    void onPause(float playback_time) {
        std::cout << "Interface::onPause(" << playback_time << ")\n";
    }
    void onUnpause(float playback_time) {
        std::cout << "Interface::onUnpause(" << playback_time << ")\n";
    }
    void onSeek(float playback_time) {
        std::cout << "Interface::onSeek(" << playback_time << ")\n";
    }
    void onQuit() {
        std::cout << "Interface::onQuit()\n";
    }
}
