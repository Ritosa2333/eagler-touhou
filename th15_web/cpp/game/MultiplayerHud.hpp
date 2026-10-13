#pragma once
#if TH15_MULTIPLAYER
#include "GameBattle.hpp"
#include "GameHud.hpp"
namespace th15 {
// Each seat uses managed instances of the original front.anm resource widgets.
class MultiplayerHud {
    GameBattle& battle;GameHud& hud;bool fail(const std::string&);
    std::array<std::array<u32,8>,3> lives{},bombs{};
    std::array<std::array<i32,5>,3> displayed{};
    std::vector<u32> widgets;
    float top(unsigned)const;float team_top()const;
    u32 widget(i32 script,float original_y,float y);
public:
    std::string error;
    MultiplayerHud(GameBattle&,GameHud&);~MultiplayerHud();
    bool prepare(AnmRenderer&);bool queue_text(HudDrawServices&);
};
}
#endif
