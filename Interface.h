#pragma once

namespace Interface {
    void onInit();
    void onTick();
    void onPause(float playback_time);
    void onUnpause(float playback_time);
    void onSeek(float playback_time);
    void onQuit();
}
