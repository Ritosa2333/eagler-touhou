#include "ScreenViews.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>
namespace th13 {
ScreenView screen_view(const GraphicsViewport& viewport,float fov,Vec2 offset)noexcept{
 ScreenView out;out.viewport=viewport;out.offset=offset;
 // 原版每 tick 2D 屏幕相机 0x43b900（主循环 0x45d2f0/0x45cef0/0x45d570 帧首
 // 调用；0x46ef60/0x46f0e0 在切视口前后重设同一相机）：
 //   eye=(320,240,240/tan(fov/2))、at=(320,240,0)、up=(0,-1,0)
 //   → D3DXMatrixLookAtLH: view 旋转 diag(1,-1,-1)、平移 (-320,240,240*cot)；
 //   → D3DXMatrixPerspectiveFovLH(fov, 4/3, zn=1, zf=10000)
 // （decomp 0043b900.c:34-45：local_14/10=320/240、up.y=-1、0x3faaaaab、
 //  0x3f800000、0x461c4000）。相机与 viewport 无关：0x43b900 硬编码 320/240
 //  与 4/3，视口只由 SetViewport 切换；屏幕空间 quad 走 transformed 分支
 //  只读 viewport，不受影响。
 // 原生对拍锚点（title oracle window B f650-749）：全部 L22/L23 world quad
 //  view={1,0,0,0, 0,-1,0,0, 0,0,-1,0, -320,240,895.6922,1}、
 //  proj m0=2.79903793/m5=3.73205066/m10=1.00010002/m14=-1.00010002；
 //  battle f0 L31 ascii/loading quad 同（round-18 native-01-boot）。
 // 旧实现复刻的是 boot 相机 0x45dc80（view 恒等、zn=100、按 viewport 取中心/
 // 距离/宽高比），只对屏幕空间 quad 无害；layer21+ 的 world quad（ED staff
 // 条带/st01、选人牌 Y 翻转、loading 条）全部被投到屏幕外——KI-1/KI-2 根因。
 out.camera.projection=scene_perspective(fov,1.3333334f,1.f,10000.f);
 // 0x43b900：D = f32(240.0 / f32(tan(f32(fov·0.5))))——0x43cf51 的 tan 结果
 // 先舍入 f32，fdivr qword[240.0]，单次 fstp（C2 取证：旧式 240·proj.m[5]
 // 的 cot 链经 aspect 因子差 1 ulp，895.69213… vs 原版 895.69219…）。
 const float tan_half = float(std::tan(double(fov * 0.5f)));
 const float distance = float(240.0 / tan_half);
 out.camera.view=scene_look_at({320,240,distance},{320,240,0},{0,-1,0});
 return out;
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
 // loading/2D 屏幕相机 = 0x43b900（screen_view 已按该式重建；battle f0-9
 // loading quad 与 boot draw 门逐位实证：diag(1,-1,-1)、zn=1、zf=10000）。
 const u32 width=environment?u32(environment->screen_width):640u;
 const u32 height=environment?u32(environment->screen_height):480u;
 ScreenView view=screen_view({0,0,width,height,0,1},fov);
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
return true;}
}
