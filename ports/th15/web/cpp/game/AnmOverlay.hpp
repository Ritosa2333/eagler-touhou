#pragma once
#include "AnmVm.hpp"
namespace th15 {
// The transition owns five embedded animations rather than registry handles.
struct AnmOverlay {
 std::array<std::unique_ptr<AnmVm>,5> panels;
 i32 mode=0,frames=0;
 AnmOverlay()=default;AnmOverlay(const AnmOverlay&);
};
}
