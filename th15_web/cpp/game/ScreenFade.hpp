#pragma once
#include "FrameScheduler.hpp"
#include "AnmRenderer.hpp"
#include <functional>
namespace th15 {
struct ScreenFadeState {i32 alpha=255,duration=30;u32 color=0;Timer age;};
struct ScreenFadeContext {bool shutdown=false,game_available=false;u32 game_flags=0;float rate=1;};
// A fade owns update/draw callbacks independently of the departing scene.
class ScreenFade {
 FrameScheduler& scheduler;AnmRenderer& renderer;ZunGraphics& graphics;AnmEnvironment& environment;std::array<FrameCallback,2> callbacks;
public:
 AnmVm presentation_vm;ScreenFadeState state;float rate=1;bool shutdown=false,active=true,covering=false,full_screen=false,game_available=false;u32 game_flags=0;std::function<ScreenFadeContext()> context;
 ScreenFade(FrameScheduler&,AnmRenderer&,ZunGraphics&,AnmEnvironment&,i32 duration,i32 update_priority,i32 draw_priority,u32 color=0,bool covering=false,bool full_screen=false);~ScreenFade();
 i32 update();bool draw();void retire()noexcept;
};
}
