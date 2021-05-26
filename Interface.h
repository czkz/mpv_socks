#pragma once
#include "MpvController.h"

namespace Interface {
    bool onInit    (char** argv);     // Will exit() if false is returned
    void onReady   (MpvController&);  // Mpv is ready, call mpv.Play()
    void onTick    (MpvController&);  // Called once per frame
    void onPause   (MpvController&, float playback_time);
    void onUnpause (MpvController&, float playback_time);
    void onSeek    (MpvController&, float playback_time);
    void onQuit    ();                // Mpv exited
    void onDestroy ();                // Destructor for onInit()
}
