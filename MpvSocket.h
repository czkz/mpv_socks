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
    std::map<size_t, std::pair<callback_t, callback_t>> pending_requests;
    std::map<std::string, callback_t> event_listeners;

    size_t regCallback(callback_t success, callback_t failure) {
        if (success || failure) {
            pending_requests[++id_counter] = { success, failure };
            return id_counter;
        }
        return 0;
    }

public:
    using LineUnixSocket::LineUnixSocket;
    using LineUnixSocket::Connect;

    void GetProperty(std::string property,
                     callback_t success,
                     callback_t failure = nullptr)
    {
        return Command({"get_property", std::move(property)}, success, failure);
    }

    void SetProperty(std::string property, nlohmann::json value,
                     callback_t success = nullptr,
                     callback_t failure = nullptr)
    {
        return Command({"set_property", std::move(property), std::move(value)}, success, failure);
    }

    void Command(nlohmann::json args, callback_t success = nullptr, callback_t failure = nullptr) {
        size_t id = regCallback(success, failure);
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
        while (true) {
            std::string line = this->Receive();
            if (line.empty()) { return; }
            // std::clog << "cmd: " << line << '\n';

            auto j = nlohmann::json::parse(std::move(line));

            if (j.contains("request_id")) {
                size_t id = j["request_id"];

                auto it = pending_requests.find(id);
                if (it != pending_requests.end()) {
                    const std::string error = j["error"];
                    if (error == "success" && it->second.first) {
                        it->second.first(std::move(j));
                    } else if (it->second.second) {
                        it->second.second(std::move(j));
                    }
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
    }
};
