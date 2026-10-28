#include "ScreenViews.hpp"
#include "Probe.hpp"
#include <cmath>
#include <cstdio>
namespace th13::probe {
bool enabled = false;
unsigned budget = 0;
}  // namespace th13::probe
namespace th13 {
static unsigned cam_budget = 0;  // 临时探针：相机行独立预算
void probe::probe_cam_arm(unsigned n) { cam_budget = n; }
ScreenView screen_view(const GraphicsViewport& viewport,float fov,Vec2 offset)noexcept{
 ScreenView out;out.viewport=viewport;out.offset=offset;
 // 原版相机（0x45dc80 初建 / 0x43b900 每 tick 同构，oracle 捕获实证）：
 // eye=(cx,-cy,-h/2/tan(fov/2)) 向 +z 看、up=(0,1,0) → view 旋转恒等、
 // 平移 (-cx,cy,distance)；PerspFovLH(fov, w/h, zn=100, zf=10000)
 // （proj m22=1.01010096、m14=-101.010101、view[14]=895.6922 为对拍锚点）。
 const float x=float(float(viewport.x)+float(float(viewport.width)*.5f)),y=float(float(viewport.y)+float(float(viewport.height)*.5f));
 const float half=float(fov*.5f),distance=float(float(viewport.height>>1)/float(std::tan(double(half))));
 out.camera.view=scene_look_at({x,-y,-distance},{x,-y,0},{0,1,0});out.camera.projection=scene_perspective(fov,float(float(viewport.width)/float(viewport.height)),100,10000);return out;
}
void ScreenViews::configure(const AnmEnvironment& environment){const float scale=environment.resolution_scale;const auto scaled=[&](float value){return u32(truncate_int(float(value*scale)));};
 views[2]=screen_view({0,0,environment.screen_width,environment.screen_height,0,1},fov);
 // 0043bbc0: 4dc984={32,16,384,448}. This is the final-screen
 // interface viewport, NOT the centered crop of the intermediate @R texture.
 // text.anm84 samples [128,512] and draws [32,416] after game composition.
 // Inferring this viewport from raw +320 sprite vertices confused those domains.
 views[0]=screen_view({scaled(32),scaled(16),scaled(384),scaled(448),0,1},fov);
 views[1]=screen_view({scaled(32),scaled(16),scaled(384),scaled(448),0,1},fov);
 // 0043be9b..0043beb9: {0x74,2,0x198,0x1dc} = {116,2,408,476}.
 views[3]=screen_view({u32(truncate_int(float(float(float(environment.screen_width)-408.f)*.5f))),u32(truncate_int(float(float(float(environment.screen_height)-476.f)*.5f))),408,476,0,1},fov);
}
bool ScreenViews::camera(DrawCamera index,bool refresh){auto& view=views[u32(index)];if(refresh)view=screen_view(view.viewport,fov,view.offset);renderer.set_viewport(view.viewport);renderer.set_camera(view.camera);renderer.offset=view.offset;
// 临时探针：相机切换取证（-96px 排查）
if(probe::enabled&&cam_budget){--cam_budget;std::printf("[probe-cam] idx=%d vp={%u,%u,%u,%u} view12=%.6g view13=%.6g proj0=%.9g proj5=%.9g\n",int(index),view.viewport.x,view.viewport.y,view.viewport.width,view.viewport.height,double(view.camera.view.m[12]),double(view.camera.view.m[13]),double(view.camera.projection.m[0]),double(view.camera.projection.m[5]));}
return true;}
}
