#pragma once

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

namespace Netplay {

class NativeLobby {
public:
    struct Config {
        std::string relay;
        std::string product;
        std::string client;
        std::string member;
        unsigned players = 2;
        std::string site;
        std::string accountEndpoint;
    };

    explicit NativeLobby(Config config);
    ~NativeLobby();
    NativeLobby(const NativeLobby&) = delete;
    NativeLobby& operator=(const NativeLobby&) = delete;

    void Send(nlohmann::json action);
    nlohmann::json TakeSnapshot();
    const Config& Settings() const { return config_; }

private:
    struct State;
    Config config_;
    std::unique_ptr<State> state_;
};

} // namespace Netplay
