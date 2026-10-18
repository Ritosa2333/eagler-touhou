#pragma once

#include <cstdint>
#include <string>
#include "PurpleParams.hpp"

namespace thprac::portable {

enum class Game : std::uint8_t { Th06, Th07 };

struct Session : PracticeRuntime::PurpleParams {
    Game game = Game::Th06;
    bool active = false;
    std::int32_t mode = 0;
    std::int32_t stage = 0;
    std::int32_t warp = 0;
    std::int32_t section = 0;
    std::int32_t phase = 0;
    std::int32_t frame = 0;
    bool dialogue = false;
    std::int64_t score = 0;
    std::int32_t life = 8;
    std::int32_t bomb = 8;
    std::int32_t power = 128;
    std::int32_t graze = 0;
    std::int32_t point = 0;
    std::int32_t pointTotal = 0;
    std::int32_t pointStage = 0;
    std::int32_t cherry = 0;
    std::int32_t cherryMax = 200000;
    std::int32_t cherryPlus = 0;
    std::int32_t spellBonus = 0;
    std::int32_t rank = 0;
    bool rankLock = false;
    std::int32_t fakeType = 0;
    std::int32_t difficulty = 0;
    std::int32_t shotType = 0;
};

void Normalize(Session& session);
std::string SerializeReplaySession(const Session& session);
bool DeserializeReplaySession(const std::string& json, Game expectedGame, Session& output);

} // namespace thprac::portable
