#include <iostream>
#include "MpvSocket.h"
#include "ChildProcess.h"
#include "PropertyGetter.h"


int main() try {

    ChildProcess mpv_process;
    mpv_process.Start("mpv ~/Downloads/15919603606180.mp4 \
            --input-ipc-server=./mpvsocket --loop --mute --no-terminal --fs=no");


    MpvSocket sock ("/home/dek/proj/usock/mpvsocket");
    while ( !sock.Connect() ) {
        if (mpv_process.Finished()) {
            std::cerr << "mpv process exited unexpectedly with code ";
            std::cerr << mpv_process.Wait() << '\n';
            return 1;
        }
        std::this_thread::yield();
    }


    // auto j_str = JSON_Object::forge("command",
    //                                 JSON_Array::forge("set_property", "pause", true)).str();
    // j_str += '\n';
    // sock.Send(j_str);

    auto cin_th = std::thread([&sock]() {
        std::string s;
        while (std::getline(std::cin, s)) {
            if (s == "play") {
                sock.Send("{ \"command\": [\"set_property\", \"pause\", false] }\n");
            } else if (s == "pause") {
                sock.Send("{ \"command\": [\"set_property\", \"pause\", true] }\n");
            } else if (s.substr(0, 5) == "seek ") {
                int time = std::atoi(s.substr(5).c_str());
                sock.Send("{ \"command\": [\"seek\", " +
                        std::to_string(time) + ", \"relative\"] }\n");
            } else {
                std::cout << "Unknown command\n";
            }
        }
        try {  // If mpv is closed this will throw
            sock.Send("{ command: [\"quit\"] }\n");
        } catch (const UnixSocket::exception& e) { }
    });


    PropertyGetter propertyGetter { sock };
    while (!mpv_process.Finished()) {
        std::string cmd = sock.Receive();
        if (!cmd.empty()) {
            std::cout << "cmd: " << cmd << '\n';
            propertyGetter.onReceived(cmd);
            // if (cmd == R"({"event":"unpause"})") { std::cout << "!!!unpause\n"; }
            // if (cmd == R"({"event":"pause"})") { std::cout << "!!!pause\n"; }
            if (cmd == R"({"event":"seek"})") {
                std::cout << "!!!seek\n";
                propertyGetter.GetProperty("playback-time", [](std::string v) {
                    float playback_time = std::atof(v.c_str());
                    std::cout << "!!!playback-time: " << playback_time << '\n';
                });
            }
        };
    }
    int code = mpv_process.Wait();
    if (code != 0) { std::cout << code << '\n'; }

    cin_th.join();


} catch (const std::exception& e) {
    std::cerr << "Caught exception: " << e.what() << '\n';
    fprintf(stderr, "errno: %d %s\n", errno, strerror(errno));
    return 2;
}
