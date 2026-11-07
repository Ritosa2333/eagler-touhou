#pragma once
#include "AnmVm.hpp"
namespace th13 {
// The transition owns five embedded animations rather than registry handles.
struct AnmOverlay {
 std::array<std::unique_ptr<AnmVm>,5> panels;
 i32 mode=0,frames=0;
 AnmOverlay()=default;AnmOverlay(const AnmOverlay&);
 bool initialize(AnmResource&,AnmResource&,AnmVm&,GameRandom&);
 i32 tick(GameRandom&,float);
 bool interrupt(i32,GameRandom&,float);
};
}
