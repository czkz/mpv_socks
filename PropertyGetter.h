#pragma once
#include <functional>
#include <string>
#include <string_view>
#include <set>
#include "MpvSocket.h"
#include "json.hpp"

class PropertyGetter {
public:
    using callback_t = std::function<void(std::string)>;

private:
    MpvSocket& sock;
    size_t id_counter = 1;
    std::map<size_t, callback_t> pending_requests;

public:
    PropertyGetter(MpvSocket& sock) : sock(sock) { }

    void GetProperty(std::string property, callback_t callback) {
        ++id_counter;
        nlohmann::json cmd = {
            {"command", {"get_property_string", property}},
            {"request_id", id_counter}
        };
        pending_requests[id_counter] = std::move(callback);
        sock.Send(cmd.dump() + "\n");
    }

    void onReceived(std::string_view line) {
        auto j = nlohmann::json::parse(line);
        if (!j.contains("request_id")) { return; }
        size_t id = j["request_id"];

        auto it = pending_requests.find(id);
        if (it != pending_requests.end()) {
            std::string data = j["data"];
            it->second(data);
            pending_requests.erase(it);
        }
    }
};
