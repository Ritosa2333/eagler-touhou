#pragma once
#include "ScreenCompositor.hpp"
namespace th15 {
// Original offscreen surfaces are named textures in text.anm; the four
// embedded ANM instances are owned independently of registered scene sprites.
class ScreenTargets {
 AnmManager& animations;AnmEnvironment& environment;ScreenViews& views;ScreenCompositor& compositor;i32 text_bank;bool fail(const char*);
public:
 std::array<AnmVm,4> captures;std::string error;
 ScreenTargets(AnmManager&,AnmEnvironment&,ScreenViews&,ScreenCompositor&,i32 text_bank);~ScreenTargets();
 bool prepare();void deactivate()noexcept;
};
}
