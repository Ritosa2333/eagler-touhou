#pragma once
#include <functional>
#include <vector>
namespace th15::sdl {
// Tentative output is replaced when its frame is replayed. Execute this journal
// once, in confirmation order, with the application's capture pointer cleared.
struct FrameEffects {
    std::vector<std::function<bool()>> events;
    bool commit(){for(const auto& event:events)if(!event())return false;events.clear();return true;}
    void clear()noexcept{events.clear();}
};
}
