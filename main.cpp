#include <iostream>
#include "UnixSocket.h"
#include "RecvThread.h"

int main() try {

    UnixSocket sock ("/home/dek/proj/usock/mpvsocket");
    if ( !sock.Connect() ) {
        std::cout << "Connection failed (is mpv running?)\n";
        return 1;
    }

    RecvThread recv_th { sock, [](std::string cmd) {
        std::cout << "cmd: " << cmd << '\n';
    }};

    std::string j_str = "{ \"command\": [\"set_property\", \"pause\", true] }\n";
    // auto j_str = JSON_Object::forge("command",
    //                                 JSON_Array::forge("set_property", "pause", true)).str();
    j_str += '\n';
    sock.Send(j_str);

    recv_th.Stop();

} catch (const std::exception& e) {
    std::cout << "Caught exception: " << e.what() << '\n';
    printf("errno: %d %s\n", errno, strerror(errno));
    return 2;
}
