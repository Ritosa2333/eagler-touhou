#pragma once
#include "StageScene.hpp"
#include "ScreenViews.hpp"
namespace th15 {
struct StageDrawState {u32 flags=1,tint_color=0x00ffffff;Timer transition{0,0,0,0,0};};
struct StageDrawServices {virtual ~StageDrawServices()=default;virtual bool background_fade(i32 duration,i32 update_priority,i32 draw_priority)=0;};
// Background callbacks surround the scene's registered foreground animations.
// Their transition timers advance during drawing, as in the original game.
class StageDrawFrame {
 FrameScheduler& scheduler;StageScene& scene;AnmManager& animations;AnmRenderer& renderer;ZunGraphics& graphics;ScreenViews& views;StageDrawServices& services;
 std::array<FrameCallback,2> callbacks;bool fail(const std::string&);void prepare_camera();void fog_parameters();bool layers(i32 first,i32 last);bool background();bool foreground();
public:
 StageDrawState state;GraphicsViewport viewport;Vec2 playfield_origin{32,16};float rate=1;std::string error;
 StageDrawFrame(FrameScheduler&,StageScene&,AnmManager&,AnmRenderer&,ZunGraphics&,ScreenViews&,StageDrawServices&);~StageDrawFrame();
 bool draw(u32 pass);StageSceneFrame update_frame()const noexcept{return {rate,state.flags,state.transition.current};}
 void begin_departure()noexcept{state.flags|=2;state.transition.set(30);}
 void begin_arrival()noexcept{state.flags|=4;state.transition.set(60);}
};
}
