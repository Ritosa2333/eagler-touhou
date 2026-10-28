#include "AnmDrawSchedule.hpp"
namespace th13 {
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
 {20,44,3,false,true,true,true,false},
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
AnmDrawSchedule::~AnmDrawSchedule(){for(auto& c:callbacks)scheduler.remove(c.callback);}
bool AnmDrawSchedule::draw(u32 index){if(!error.empty())return false;const auto& e=entries[index];if(e.camera>=0){renderer.invalidate();if(!services.camera(DrawCamera(e.camera),e.refresh)){error="Animation camera selection failed";return false;}if(e.disable_fog)graphics.set_fog(false);if(e.disable_depth_write)graphics.set_depth_mask(false);if(e.disable_fog||e.disable_depth_write)graphics.set_depth_compare(touhou::graphics::Compare::Always);if(e.zero_offset)renderer.offset={};}
 if(!renderer.draw_layer(animations.registry.layer(e.layer))){error=renderer.error;return false;}if(e.restore){renderer.invalidate();if(!services.camera(DrawCamera::Fullscreen,false)){error="Animation full-screen camera restoration failed";return false;}}return true;}
}
