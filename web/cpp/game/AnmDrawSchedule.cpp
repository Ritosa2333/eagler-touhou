#include "AnmDrawSchedule.hpp"
#include <unordered_map>
namespace th13 {
namespace {
using GhostIcons = std::unordered_map<u32, u8>;
std::unordered_map<AnmManager*, GhostIcons> hud_ghost_icons;
}
void AnmDrawSchedule::set_hud_ghost_icons(AnmManager& manager,
    const std::array<std::array<u32, 21>, 3>& icons, u8 ghost) {
 auto& selected = hud_ghost_icons[&manager];
 selected.clear();
 for (size_t seat = 0; seat < icons.size(); ++seat)
  if ((ghost >> seat) & 1)
   for (u32 handle : icons[seat])
    if (handle) selected[handle] = 140;
}
const std::array<AnmDrawSchedule::Entry,31> AnmDrawSchedule::entries{{
 {0,5},
 {1,7},
 {2,9},
 {4,11},
 {3,10,3,true,true,false,false,true},
 {5,13},
 {6,16},
 {7,18},
 {8,19},
 {9,20},
 {10,21},
 {11,23},
 {12,26},
 {13,27},
 {14,30},
 {15,31},
 {16,33},
 {17,35},
 {18,39},
 {19,43},
 // 46ef30 clears 4dcbe8/ec and camera+0xd0/d4 immediately before layer20.
 {20,44,3,false,true,true,true,false,true},
 // L21/22/23 优先级按原版注册序对齐（r4/render/integration.md；objdump
 // 46bf25..46c7f0 `mov ebx,prio`、回调体 46ed40..46f090）：
 //   {21,54} ← 46ef60@0x36（全屏相机）；{22,55} ← 46ef20@0x37（HUD front00
 //   边条层）；{23,57} ← 46ef10@0x39。三者必须晚于 PlayfieldTargets 的
 //   RT 采样 52/复合 53（RT 链 43a440 prio 表）——此前 44/50/52 使 HUD 边条
 //   早于 52 进 RT，被 [128,512]→[32,416] 采样重投影成 319..415.5 窄条。
 {21,54},
 {22,55},
 {23,57},
 {26,69}, // 46c63c EBX=0x45, callback46eef0 draws layer26
 {28,74},
 // Layers29/30 belong to PlayfieldTargets' opposite-RT composition passes
 // (0043c870/0043c920), not independent final-screen callbacks.
 {25,68}, // 46c5ed EBX=0x44, callback46ef00 draws layer25 after background23
 {24,56,2,false,true,true,true,false},
 {27,61,0,false,false,false,false,true},
 // 0046c7c9/0046c77a：副链 draw32=70、draw31=71。
 // 0046f0e0 使用 interface 相机并恢复 fullscreen；0046f090 使用 fullscreen。
 // Pause/Menu/文字子树经 ins501 进入副链，不能只 tick 而不生产绘制。
 {32,70,0,false,true,true,true,true},
 {31,71,2,false,true,true,true,false}
}};
AnmDrawSchedule::AnmDrawSchedule(FrameScheduler& f,AnmManager& a,AnmRenderer& r,ZunGraphics& g,AnmDrawServices& s):scheduler(f),animations(a),renderer(r),graphics(g),services(s){for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.index=i;c.callback.owner=&c;c.callback.enabled=true;c.callback.run=[](void* p){auto& c=*static_cast<Callback*>(p);return c.owner->draw(c.index)?i32(FrameAction::Continue):i32(FrameAction::Error);};if(scheduler.add(c.callback,FramePass::Draw,entries[i].priority)<0)error="Animation draw callback registration failed";}}
AnmDrawSchedule::~AnmDrawSchedule(){for(auto& c:callbacks)scheduler.remove(c.callback);hud_ghost_icons.erase(&animations);}
bool AnmDrawSchedule::draw(u32 index){if(!error.empty())return false;const auto& e=entries[index];if(e.camera>=0){renderer.invalidate();if(!services.camera(DrawCamera(e.camera),e.refresh)){error="Animation camera selection failed";return false;}if(e.disable_fog)graphics.set_fog(false);if(e.disable_depth_write)graphics.set_depth_mask(false);if(e.disable_fog||e.disable_depth_write)graphics.set_depth_compare(touhou::graphics::Compare::Always);if(e.zero_offset)renderer.offset={};if(e.clear_shake)animations.scene_environment().screen_shake_offset={};}
 // 战场层 4..20 的 2D 裁剪块是 0x43be9b 的 {116,2,408,476}（随屏幕尺寸居中）；
 // 背景层 0..3 与 HUD/前景层（21+）用全屏 viewport（原生 layer2 背景 quad
 // 可到 x=-256，layer22/23 前栏在 x>=540，均证明这两段不是战场块）。
 // 原生 0x467350 只读该块，与 D3D viewport 无关，故这里只切换
 // renderer.clip，不动 camera/viewport。
 if(e.layer>=4&&e.layer<=20){const auto& env=animations.scene_environment();renderer.clip={u32(truncate_int(float(float(env.screen_width)-408.f)*.5f)),u32(truncate_int(float(float(env.screen_height)-476.f)*.5f)),408,476,0,1};renderer.clip_set=true;}else renderer.clip_set=false;
 for (auto* vm : animations.registry.layer(e.layer)) {
  const auto group = hud_ghost_icons.find(&animations);
  const auto handle = animations.registry.handle(*vm);
  const auto opacity = group == hud_ghost_icons.end() ? GhostIcons::const_iterator{} : group->second.find(handle);
  const int result = group != hud_ghost_icons.end() && opacity != group->second.end()
      ? renderer.draw_with_opacity(*vm, opacity->second) : renderer.draw(*vm);
  if (result == -2) {error=renderer.error;return false;}
 }
 if(e.restore){renderer.invalidate();if(!services.camera(DrawCamera::Fullscreen,false)){error="Animation full-screen camera restoration failed";return false;}}return true;}
}
