#pragma once
#include "AnmVm.hpp"
namespace th13 {
// The transition owns five embedded animations rather than registry handles.
struct AnmOverlay {
 std::array<std::unique_ptr<AnmVm>,5> panels;
 i32 mode=0,frames=0;
 AnmOverlay()=default;AnmOverlay(const AnmOverlay&);
 bool initialize(AnmResource&,AnmResource&,AnmVm&,GameRandom&);
 // 0x402ff0: the four effect panels are mandatory; the fifth astral panel is
 // conditional on [0x4c2164] (AstralInf, created at 0x42be1d during battle
 // load). Title-launch overlays usually see it null and skip the fifth panel.
 bool initialize(AnmResource& effect,AnmResource* astral,AnmVm&,GameRandom&);
 i32 tick(GameRandom&,float);
 bool interrupt(i32,GameRandom&,float);
};
}
