#pragma once
#include "ScreenGrid.hpp"
#include "AnmManager.hpp"
namespace th15 {
// Saved game state copies the CPU grid and logical handles by value. Drawing
// captures the screen at capture_handle before submitting its following strips.
struct ScreenGridObject {
 ScreenGrid grid;u32 capture_handle=0;std::array<u32,16> strip_handles{};
 bool initialize(AnmManager&,i32 text_bank,u32 rows=17);
 bool retire(AnmManager&);bool update(AnmManager&,EnemyDistortionState&,const Vec3&,float,const ScreenGridViewport&);
};
}
