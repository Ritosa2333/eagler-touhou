#pragma once

#include <nlohmann/json_fwd.hpp>
#include <memory>

namespace th09::desktop {
class RoomAudio {
public:
    RoomAudio();
    ~RoomAudio();
    void command(const nlohmann::json& message);
    nlohmann::json poll();

private:
    struct Player;
    std::unique_ptr<Player> player_;
};
}
