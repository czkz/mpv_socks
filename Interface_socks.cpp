#include "Interface.h"

#include <iostream>
#include <thread>
#include "SockStream.h"

namespace {
    SockClient sock;

    void thread_func(MpvController& mpv) {
        try {
            std::string cmd;
            while (true) {
                cmd = sock.ReceiveFill(1);
                if (cmd == "+") {
                    mpv.Play();
                } else if (cmd == "-") {
                    mpv.Pause();
                } else if (cmd == "q") {
                    mpv.Quit();
                    break;
                } else if (cmd == "t") {
                    float time;
                    sock.ReceiveInto(&time, sizeof(time));
                    std::clog << "Got time " << time << '\n';
                    mpv.Raw().GetProperty("playback-time", [&](auto v) {
                        float local_time = v["data"];
                        float d = std::abs(time - local_time);
                        std::clog << "Current: " << local_time;
                        std::clog << ", requested: " << time;
                        std::clog << ", diff: " << d << "\n";
                        if (d >= 0.5) {
                            mpv.SetPlaybackTime(time);
                        }
                    });
                } else if (!cmd.empty()) {
                    std::clog << "Unknown command\n";
                }
            }
        } catch (const std::exception& e) {
            std::clog << "Sock thread: " << e.what() << '\n';
        }
    }

    std::thread th;
}

namespace Interface {
    bool onInit() {
        std::clog << "Interface::onInit()\n";
        return sock.Connect("localhost", 5555);
    }

    void onReady(MpvController& mpv) {
        th = std::thread(thread_func, std::ref(mpv));
        // mpv.Play();
    }

    void onQuit() {
        sock.Send(std::string("-"));
        std::clog << "Interface::onQuit()\n";
        th.join();
    }

    void onTick(MpvController&) {
        // std::clog << "Interface::onTick()\n";
    }
    void onPause(MpvController&, float) {
        sock.Send(std::string("-"));
        std::clog << "pause\n";
    }
    void onUnpause(MpvController&, float) {
        sock.Send(std::string("+"));
        std::clog << "play\n";
    }
    void onSeek(MpvController&, float time) {
        sock.Send(std::string("t"));
        sock.Send(&time, sizeof(time));
        std::clog << "time\n";
    }
    void onDestroy() { }
}
