#include <iostream>
#include "UnixSocket.h"
#include "RecvThread.h"
#include "ChildProcess.h"


int main() try {

    ChildProcess mpv_process;
    RecvThread recv_th;
    MpvSocket sock ("/home/dek/proj/usock/mpvsocket");

    mpv_process.SetExitCallback( [&recv_th](int code) {
        if (code != 0) { std::cout << code << '\n'; }
        recv_th.Stop();
    });

    recv_th.SetCmdCallback( [](std::string cmd) {
        std::cout << "cmd: " << cmd << '\n';
        if (cmd == R"({"event":"unpause"})") { std::cout << "!!!unpause\n"; }
        if (cmd == R"({"event":"pause"})") { std::cout << "!!!pause\n"; }
        if (cmd == R"({"event":"seek"})") { std::cout << "!!!seek\n"; }
    });


    mpv_process.Start("mpv ~/Downloads/15919603606180.mp4 \
            --input-ipc-server=./mpvsocket --loop --mute --no-terminal --fs=no");

    while ( !sock.Connect() ) {
        if (mpv_process.Finished()) {
            std::cerr << "mpv process exited unexpectedly with code ";
            std::cerr << mpv_process.Wait() << '\n';
            return 1;
        }
        std::this_thread::yield();
    }

    recv_th.Start(sock);


    // auto j_str = JSON_Object::forge("command",
    //                                 JSON_Array::forge("set_property", "pause", true)).str();
    // j_str += '\n';
    // sock.Send(j_str);

    std::string s;
    while (std::getline(std::cin, s)) {
        if (s == "play") {
            sock.Send("{ \"command\": [\"set_property\", \"pause\", false] }\n");
        } else if (s == "pause") {
            sock.Send("{ \"command\": [\"set_property\", \"pause\", true] }\n");
        } else if (s == "pause") {
            sock.Send("{ \"command\": [\"get_property\", \"playback-time\"] }\n");
        } else {
            std::cout << "Unknown command\n";
        }
    }

    sock.Send("{ command: [\"quit\"] }\n");
    mpv_process.Wait();
    recv_th.Wait();

} catch (const std::exception& e) {
    std::cerr << "Caught exception: " << e.what() << '\n';
    fprintf(stderr, "errno: %d %s\n", errno, strerror(errno));
    return 2;
}
