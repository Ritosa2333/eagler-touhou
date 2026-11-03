#pragma once
#include "AnmDrawSchedule.hpp"
#include "SceneMatrices.hpp"
namespace th13 {
struct ScreenView {GraphicsViewport viewport;AnmCamera camera;Vec2 offset{};};
ScreenView screen_view(const GraphicsViewport&,float fov,Vec2 offset={})noexcept;
class ScreenViews final:public AnmDrawServices {
 AnmRenderer& renderer;std::array<ScreenView,4> views;float fov=0.52359879016876220703125f;
 // 406d90/43ba20/43b900 copy 4dcbe8/ec into camera+0xf4/f8 and then into
 // ANM manager+0xd0/d4; a camera switch must not drop that shared offset.
 const AnmEnvironment* environment=nullptr;
public:
 ScreenViews(AnmRenderer& r,const AnmEnvironment& e):renderer(r){configure(e);}
 void configure(const AnmEnvironment&);bool camera(DrawCamera,bool refresh)override;
 // 战斗直绘 pass 入口专用：与当前 renderer 状态逐字段相同的相机不重设
 //（不冲 UP 批）；异值仍走 camera() 的完整路径。
 bool camera_if_changed(DrawCamera,bool refresh)override;
 // 原生 loading/2D 屏幕相机：view 旋转 diag(1,-1,-1)、proj zn=1/zf=10000
 //（battle f0-9 loading quad 实证；与 game scene 相机 zn=100/view 恒等不同）。
 bool loading_camera();
 ScreenView& view(DrawCamera index)noexcept{return views[u32(index)];}
};
}
