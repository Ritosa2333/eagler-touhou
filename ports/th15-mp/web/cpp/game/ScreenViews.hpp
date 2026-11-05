#pragma once
#include "AnmDrawSchedule.hpp"
#include "StageCamera.hpp"
namespace th15 {
struct ScreenView {GraphicsViewport viewport;AnmCamera camera;Vec2 offset{};};
ScreenView screen_view(const GraphicsViewport&,float fov,Vec2 offset={})noexcept;
class ScreenViews final:public AnmDrawServices {
 AnmRenderer& renderer;std::array<ScreenView,4> views;float fov=0.52359879016876220703125f;
public:
 ScreenViews(AnmRenderer& r,const AnmEnvironment& e):renderer(r){configure(e);}
 void configure(const AnmEnvironment&);bool camera(DrawCamera,bool refresh)override;
 ScreenView& view(DrawCamera index)noexcept{return views[u32(index)];}
};
}
