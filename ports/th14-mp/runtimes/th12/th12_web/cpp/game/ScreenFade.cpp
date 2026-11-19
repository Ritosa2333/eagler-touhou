#include "ScreenFade.hpp"
#include "AnmRenderer.hpp"
#include "PortTitle.hpp"
#include <algorithm>
namespace th11 {
i32 ScreenFade::update(bool stopped,bool frozen
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    ,const ControlPublicationTh14* live_control
#endif
) noexcept {
    if(stopped)return 7;
    if(type==3||(kPortTitle==14&&type==0)){
#if PORT_TITLE == 14
        // 46c419..46c43c performs each multiply/divide/subtract in binary32.
        if(duration)alpha=std::max(0,truncate_int(255.f-float(timer.fractional*255.f)/float(duration)));
#else
        if(duration)alpha=std::max(0,truncate_int(255.-double(timer.fractional)*255./duration));
#endif
        if(timer.current>=duration)return 7;
        timer.tick();return 1;
    }
#if PORT_TITLE == 14
    // 46c98d..46c9a5: a double intermediate can change the integer alpha.
    if(duration)alpha=std::max(0,timer.current<duration?truncate_int(float(timer.fractional*255.f)/float(duration)):255);
#else
    if(duration)alpha=std::max(0,timer.current<duration?truncate_int(double(timer.fractional)*255./duration):255);
#endif
    if(timer.current>=wrapping_add(duration,2))return 7;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    //46c9ca: the live word is read after alpha and the termination check.
    if(live_control)frozen=live_control->frozen();
#endif
    if(!frozen)timer.tick();return 1;
}
void ScreenFade::draw(AnmRenderer& renderer) const {
#if PORT_TITLE == 14
    // Retail46c8d0 and46ca40 read the current internal display dimensions.
    // Type2/3 retain the active viewport; type0/5 first select the full window.
    if(type==0||type==2||type==3||type==5){
        if(type==0||type==5)renderer.set_viewport({0,0,renderer.render_width,renderer.render_height,0,1});
        renderer.solid_rectangle(0,0,float(renderer.render_width),float(renderer.render_height),(u32(alpha)<<24)|color);
    }else renderer.solid_rectangle(32,16,416,464,(u32(alpha)<<24)|color);
#else
    if(type==5){renderer.set_viewport({0,0,640,480,0,1});renderer.solid_rectangle(0,0,640,480,(u32(alpha)<<24)|color);}
    else renderer.solid_rectangle(32,16,416,464,(u32(alpha)<<24)|color);
#endif
}
void ScreenFades::draw(AnmRenderer& renderer,u32 priority) const {for(const auto& effect:effects)if(effect.priority==priority)effect.draw(renderer);}
}
