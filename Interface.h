#pragma once
#include "MpvController.h"

namespace Interface {
    void onInit    (MpvController&);
    void onTick    (MpvController&);
    void onPause   (MpvController&, float playback_time);
    void onUnpause (MpvController&, float playback_time);
    void onSeek    (MpvController&, float playback_time);
    void onQuit    ();
}
