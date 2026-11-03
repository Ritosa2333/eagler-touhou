#include "ScreenViews.hpp"
#include "Probe.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>
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
void ScreenViews::configure(const AnmEnvironment& environment){this->environment=&environment;const float scale=environment.resolution_scale;const auto scaled=[&](float value){return u32(truncate_int(float(value*scale)));};
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
bool ScreenViews::loading_camera(){
 // 原生 loading/2D 屏幕相机（boot draw 门逐位实证）：view 平移与
 // screen_view 相同，但旋转为 diag(1,-1,-1)；proj 同 fov/aspect，zn=1、
 // zf=10000（m10=1.00010002、m14=-1.00010002）。game scene 相机
 //（zn=100、view 恒等）用于 battle 3D/playfield，不适用于 loading quad。
 const u32 width=environment?u32(environment->screen_width):640u;
 const u32 height=environment?u32(environment->screen_height):480u;
 ScreenView view=screen_view({0,0,width,height,0,1},fov);
 view.camera.view.m[5]=-1.f;view.camera.view.m[10]=-1.f;
 view.camera.projection=scene_perspective(fov,float(float(width)/float(height)),1.f,10000.f);
 renderer.set_viewport(view.viewport);renderer.set_camera(view.camera);renderer.offset={};
 return true;
}
namespace {
bool same_viewport(const GraphicsViewport& a,const GraphicsViewport& b)noexcept{
    return a.x==b.x&&a.y==b.y&&a.width==b.width&&a.height==b.height
        &&a.near_depth==b.near_depth&&a.far_depth==b.far_depth;
}
// AnmCamera 全字段比较：跳过 set_camera 必须等价于「按原路径设置同一相机」，
// 否则 renderer.camera 的 eye/reference/fog 字段会与原生分叉（projected_quad
// 用 camera.fog_near/fog_far、world_quad 用 camera.fog_color）。
bool same_camera(const AnmCamera& a,const AnmCamera& b)noexcept{
    return a.eye.x==b.eye.x&&a.eye.y==b.eye.y&&a.eye.z==b.eye.z
        &&a.reference.x==b.reference.x&&a.reference.y==b.reference.y
        &&a.reference.z==b.reference.z
        &&std::memcmp(a.view.m,b.view.m,sizeof a.view.m)==0
        &&std::memcmp(a.projection.m,b.projection.m,sizeof a.projection.m)==0
        &&a.fog_near==b.fog_near&&a.fog_far==b.fog_far
        &&a.fog_rgb.x==b.fog_rgb.x&&a.fog_rgb.y==b.fog_rgb.y
        &&a.fog_rgb.z==b.fog_rgb.z&&a.fog_color==b.fog_color;
}
}  // namespace

// 战斗直绘 pass 入口（RunScene::draw_stage）专用：与当前渲染器状态逐字段
// 相同的相机不得重设。AnmRenderer::set_viewport/set_camera 无条件 flush，
// 会把同一材质键的跨 pass 精灵批拆开——gate-my laser 窗 f5450 op183 原生
// 0x4679a0 的 UP 批是 12 三角形单 draw，web 旧路径拆成 10+2（层 11 的 5 个
// 弹 quad + 自机 pass 的机体 quad；19551dd1e 记录同一现象，efc1422f3 回退
// 了其全局同值跳过）。这里只在 draw_stage 入口跳过同值重设；offset 仍更新
// （shake 在顶点生成时烘焙），异值相机仍走 camera() 的完整路径（含 flush）；
// 层回调/RT 切换/标题/loading 路径的 flush 纪律不变。
bool ScreenViews::camera_if_changed(DrawCamera index,bool refresh){
 auto& view=views[u32(index)];
 if(refresh)view=screen_view(view.viewport,fov,view.offset);
 if(same_viewport(view.viewport,renderer.viewport)&&same_camera(view.camera,renderer.camera)){
  const Vec2 shake=environment?environment->screen_shake_offset:Vec2{};
  renderer.offset={float(view.offset.x+shake.x),float(view.offset.y+shake.y)};
  return true;
 }
 return camera(index,false);
}
bool ScreenViews::camera(DrawCamera index,bool refresh){auto& view=views[u32(index)];if(refresh)view=screen_view(view.viewport,fov,view.offset);renderer.set_viewport(view.viewport);renderer.set_camera(view.camera);const Vec2 shake=environment?environment->screen_shake_offset:Vec2{};renderer.offset={float(view.offset.x+shake.x),float(view.offset.y+shake.y)};
// 临时探针：相机切换取证（-96px 排查）
if(probe::enabled&&cam_budget){--cam_budget;std::printf("[probe-cam] idx=%d vp={%u,%u,%u,%u} view12=%.6g view13=%.6g proj0=%.9g proj5=%.9g\n",int(index),view.viewport.x,view.viewport.y,view.viewport.width,view.viewport.height,double(view.camera.view.m[12]),double(view.camera.view.m[13]),double(view.camera.projection.m[0]),double(view.camera.projection.m[5]));}
return true;}
}
