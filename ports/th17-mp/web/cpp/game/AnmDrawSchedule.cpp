#include "AnmDrawSchedule.hpp"
namespace th17 {
const std::array<AnmDrawSchedule::Entry,39> AnmDrawSchedule::entries{{
    {0,5},{1,7},{2,9},{4,11},{3,10,3,true,true},
    {5,13},{6,16},{7,18},{8,20},{9,21},{10,22},{11,24},
    {12,27},{13,28},{14,31},{15,32},{16,35},{17,37},{18,40},{19,43},
    {20,46,1,false,false,true},{21,47},{22,53},{23,55},{26,62},{27,63},
    {29,78},{30,80},{31,83},{25,60},
    {24,59,2,false,false,true,true},{28,65,0,false,false,false,false,true},
    {36,56,2,false,false,true},{37,61},{38,64},{39,66,0,false,false,false,false,true},
    {40,79},{41,81},{42,84}
}};
AnmDrawSchedule::AnmDrawSchedule(FrameScheduler& f,AnmManager& a,AnmRenderer& r,ZunGraphics& g,AnmDrawServices& s):scheduler(f),animations(a),renderer(r),graphics(g),services(s){
    for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.index=i;c.frame.owner=&c;c.frame.enabled=true;c.frame.run=[](void* p)->i32{auto& c=*static_cast<Callback*>(p);return c.owner->draw(c.index)?1:5;};if(scheduler.add(c.frame,FramePass::Draw,entries[i].priority)<0)error="Animation draw registration failed";}
}
AnmDrawSchedule::~AnmDrawSchedule(){for(auto& c:callbacks)scheduler.remove(c.frame);}
bool AnmDrawSchedule::draw(u32 index){
    if(!error.empty())return false;const auto& e=entries[index];
    if(e.camera>=0){renderer.invalidate();if(!services.camera(DrawCamera(e.camera),e.refresh)){error="Animation camera selection failed";return false;}}
    if(e.disable_fog||e.disable_depth_write){renderer.flush();if(e.disable_fog)graphics.set_fog(false);if(e.disable_depth_write){graphics.set_depth_mask(false);graphics.set_depth_compare(touhou::graphics::Compare::Always);}}
    if(e.zero_offset)renderer.offset={};
    if(!renderer.draw_layer(animations.registry.layer(e.layer))){error=renderer.error;return false;}
    if(e.restore){renderer.invalidate();if(!services.camera(DrawCamera::Fullscreen,false)){error="Animation camera restoration failed";return false;}}
    return true;
}
}
