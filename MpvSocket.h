#pragma once
#include <iostream>
#include <functional>
#include <string>
#include <string_view>
#include <set>
#include "LineUnixSocket.h"
#include "json.hpp"

class MpvSocket : private LineUnixSocket {
public:
    using callback_t = std::function<void(nlohmann::json)>;

private:
    size_t id_counter = 1;
    std::map<size_t, callback_t> pending_requests;
    std::map<std::string, callback_t> event_listeners;

    size_t regCallback(callback_t callback) {
        if (callback) {
            pending_requests[++id_counter] = std::move(callback);
            return id_counter;
        }
        return 0;
    }

public:
    using LineUnixSocket::LineUnixSocket;
    using LineUnixSocket::Connect;

    void GetProperty(std::string property, callback_t callback) {
        return Command({"get_property", std::move(property)}, callback);
    }

    void SetProperty(std::string property, nlohmann::json value, callback_t callback = nullptr) {
        return Command({"set_property", std::move(property), std::move(value)}, callback);
    }

    void Command(nlohmann::json args, callback_t callback = nullptr) {
        size_t id = regCallback(callback);
        nlohmann::json cmd = {
            {"command", args},
            {"request_id", id}
        };
        this->Send(cmd.dump() + "\n");
    }

    void on(std::string event, callback_t callback) {
        if (callback) {
            event_listeners[event] = callback;
        } else {
            event_listeners.erase(event);
        }
    }

    void onTick() {
        std::string line = this->Receive();
        if (line.empty()) { return; }
        std::clog << "cmd: " << line << '\n';

        auto j = nlohmann::json::parse(std::move(line));

        if (j.contains("request_id")) {
            size_t id = j["request_id"];

            auto it = pending_requests.find(id);
            if (it != pending_requests.end()) {
                it->second(std::move(j));
                pending_requests.erase(it);
            }
        } else if (j.contains("event")) {
            std::string event = j["event"];

            auto it = event_listeners.find(event);
            if (it != event_listeners.end()) {
                it->second(std::move(j));
            }
        }

    }
};
