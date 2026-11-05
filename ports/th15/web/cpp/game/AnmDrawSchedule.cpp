#include "AnmDrawSchedule.hpp"
namespace th15 {
const std::array<AnmDrawSchedule::Entry,38> AnmDrawSchedule::entries{{
 {0,5},
 {1,7},
 {2,9},
 {4,11},
 {3,10,3,true,true,false,false,false},
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
 {18,38},
 {19,40},
 {20,43,1,false,true,true,false,false},
 {21,44},
 {22,50},
 {23,52},
 {26,59},
 {28,74},
 {29,76},
 {30,79},
 {25,57},
 {24,56,2,false,true,true,true,false},
 {27,61,0,false,false,false,false,true},
 {35,53,2,false,true,true,false,false},
 {36,58},
 {37,60},
 {38,62,0,false,false,false,false,true},
 {39,75},
 {40,77},
 {41,80}
}};
AnmDrawSchedule::AnmDrawSchedule(FrameScheduler& f,AnmManager& a,AnmRenderer& r,ZunGraphics& g,AnmDrawServices& s):scheduler(f),animations(a),renderer(r),graphics(g),services(s){for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.index=i;c.callback.owner=&c;c.callback.enabled=true;c.callback.run=[](void* p){auto& c=*static_cast<Callback*>(p);return c.owner->draw(c.index)?i32(FrameAction::Continue):i32(FrameAction::Error);};if(scheduler.add(c.callback,FramePass::Draw,entries[i].priority)<0)error="Animation draw callback registration failed";}}
AnmDrawSchedule::~AnmDrawSchedule(){for(auto& c:callbacks)scheduler.remove(c.callback);}
bool AnmDrawSchedule::draw(u32 index){if(!error.empty())return false;const auto& e=entries[index];if(e.camera>=0){renderer.invalidate();if(!services.camera(DrawCamera(e.camera),e.refresh)){error="Animation camera selection failed";return false;}if(e.disable_fog)graphics.set_fog(false);if(e.disable_depth_write)graphics.set_depth_mask(false);if(e.disable_fog||e.disable_depth_write)graphics.set_depth_compare(touhou::graphics::Compare::Always);if(e.zero_offset)renderer.offset={};}
 if(!renderer.draw_layer(animations.registry.layer(e.layer))){error=renderer.error;return false;}if(e.restore){renderer.invalidate();if(!services.camera(DrawCamera::Fullscreen,false)){error="Animation full-screen camera restoration failed";return false;}}return true;}
}
