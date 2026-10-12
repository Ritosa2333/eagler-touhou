#pragma once
#if TH15_MULTIPLAYER
#include "GameBattle.hpp"
#include "GameHud.hpp"
namespace th15 {
// The shared native glyph queue owns presentation; no battle RNG is consumed.
class MultiplayerHud {
    GameBattle& battle;GameHud& hud;bool fail(const std::string&);
    float top(unsigned)const;float team_top()const;
public:
    std::string error;
    MultiplayerHud(GameBattle& b,GameHud& h):battle(b),hud(h){}
    void prepare(AnmRenderer&);bool queue_text(HudDrawServices&);
};
}
#endif
