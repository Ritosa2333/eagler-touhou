#pragma once
#include "StageScene.hpp"
#include "ScreenViews.hpp"
#include "StageDrawState.hpp"
namespace th17 {
struct StageDrawServices {virtual ~StageDrawServices()=default;virtual bool background_fade(i32 duration,i32 update_priority,i32 draw_priority)=0;};
class StageDrawFrame {
    FrameScheduler& scheduler;StageScene& scene;StageDrawState& state;AnmManager& animations;AnmRenderer& renderer;ZunGraphics& graphics;ScreenViews& views;StageDrawServices& services;
    std::array<FrameCallback,2> callbacks;bool fail(const std::string&);void prepare_camera();void fog_parameters();bool layers(i32 first,i32 last);bool background();bool foreground();
public:
    GraphicsViewport viewport;Vec2 playfield_origin{32,16};float rate=1;std::string error;
    StageDrawFrame(FrameScheduler&,StageScene&,StageDrawState&,AnmManager&,AnmRenderer&,ZunGraphics&,ScreenViews&,StageDrawServices&);~StageDrawFrame();
    bool draw(u32 pass);
};
}
