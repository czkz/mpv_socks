#pragma once
#include "MpvSocket.h"

// Non-owning
class MpvController {
    MpvSocket& mpv;
public:
    MpvController(MpvSocket& s) : mpv(s) { }

    void Play() {
        mpv.SetProperty("pause", false);
    }
    void Pause() {
        mpv.SetProperty("pause", true);
    }
    void SetPlaybackTime(float time) {
        mpv.SetProperty("playback_time", time);
    }
    void Seek(float time_offset) {
        mpv.Command({"seek", time_offset, "relative"});
    }
    void Quit() {
        mpv.Command({"quit"});
    }
};
