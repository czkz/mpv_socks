#pragma once
#include <functional>
#include <string>
#include <string_view>
#include <set>
#include "MpvSocket.h"
#include "json.hpp"

class MpvController {
public:
    using callback_t = std::function<void(nlohmann::json)>;

private:
    MpvSocket& sock;
    size_t id_counter = 1;
    std::map<size_t, callback_t> pending_requests;

    size_t regCallback(callback_t callback) {
        if (callback) {
            pending_requests[++id_counter] = std::move(callback);
            return id_counter;
        }
        return 0;
    }

public:
    MpvController(MpvSocket& sock) : sock(sock) { }

    void GetProperty(std::string property, callback_t callback) {
        return Command({"get_property", property}, callback);
    }

    void SetProperty(std::string property, nlohmann::json value, callback_t callback = nullptr) {
        return Command({"set_property", property, value}, callback);
    }

    void Command(nlohmann::json args, callback_t callback = nullptr) {
        size_t id = regCallback(callback);
        nlohmann::json cmd = {
            {"command", args},
            {"request_id", id}
        };
        sock.Send(cmd.dump() + "\n");
    }

    void onReceived(std::string_view line) {
        auto j = nlohmann::json::parse(line);
        if (!j.contains("request_id")) { return; }
        size_t id = j["request_id"];

        auto it = pending_requests.find(id);
        if (it != pending_requests.end()) {
            it->second(j);
            pending_requests.erase(it);
        }
    }
};
