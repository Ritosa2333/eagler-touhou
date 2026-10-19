#include "ScreenFade.hpp"
#include "AnmRenderer.hpp"
#include <algorithm>
namespace th11 {
i32 ScreenFade::update(bool stopped,bool frozen) noexcept {
    if(stopped)return 7;
    if(type==3){
        if(duration)alpha=std::max(0,truncate_int(255.-double(timer.fractional)*255./duration));
        if(timer.current>=duration)return 7;
        timer.tick();return 1;
    }
    if(duration)alpha=std::max(0,timer.current<duration?truncate_int(double(timer.fractional)*255./duration):255);
    if(timer.current>=wrapping_add(duration,2))return 7;
    if(!frozen)timer.tick();return 1;
}
void ScreenFade::draw(AnmRenderer& renderer) const {
    if(type==5){renderer.set_viewport({0,0,640,480,0,1});renderer.solid_rectangle(0,0,640,480,(u32(alpha)<<24)|color);}
    else renderer.solid_rectangle(32,16,416,464,(u32(alpha)<<24)|color);
}
void ScreenFades::draw(AnmRenderer& renderer,u32 priority) const {for(const auto& effect:effects)if(effect.priority==priority)effect.draw(renderer);}
}
