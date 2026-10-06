#include "PopupManager.hpp"
#include <cstdio>
namespace th15 {
bool PopupManager::initialize(i32 resource){glyph=AnmVm{};glyph.environment=&animations.scene_environment();glyph.resource=animations.resource(resource);if(!glyph.resource){error="Floating number resource is unavailable";return false;}glyph.object_host=&animations;glyph.visual.flags=7;glyph.visual.render_flags=0x384000;glyph.visible=true;if(!glyph.select_sprite(259)){error="Floating number sprite is unavailable";return false;}return true;}
void PopupManager::number(const Vec3& position,i32 value,u32 color){
    if(next_number>=10)next_number=0;const auto slot=next_number++;presentation_generations[slot]=++AnmVm::presentation_counter;auto& e=entries[slot];e.active=1;u32 n=0;if(value>0){while(value){const i32 quotient=value/10;e.digits[n++]=u8(value-quotient*10);value=quotient;}}else{e.digits[0]=value<0?10:0;n=1;}e.digit_count=u8(n);e.color=color;e.age.set(0);e.position=position;e.velocity=1;
}
void PopupManager::bonus(const Vec3& position,i32 value,u32 color,float multiplier){
    for(u32 n=13;n<18;++n)if(!entries[n].active){auto& e=entries[n];e.color=color;e.age.set(0);e.position=position;if(e.position.y<0)e.position.y=0;if(e.position.x<-168)e.position.x=-168;if(e.position.x>168)e.position.x=168;e.bonus=value;e.multiplier=multiplier;e.active=1;break;}
}
void PopupManager::update(float rate){
    for(u32 n=0;n<18;++n){auto& e=entries[n];if(!e.active)continue;if(n<13){e.position.y=float(e.position.y-float(e.velocity*rate));e.velocity=float(e.velocity*.95f);}e.age.tick(&rate);if(e.age.current<=60)continue;if(n<13)e.active=0;else{const i32 alpha=i32(e.color>>24)-4;if(alpha<=0)e.active=0;else e.color=(e.color&0xffffff)|(u32(alpha)<<24);}}
}
bool PopupManager::draw(PopupDrawServices& out,const Vec3& player){
    for(u32 n=0;n<13;++n){const auto& e=entries[n];if(!e.active)continue;float advance=8;if(e.age.current<8)advance=float(advance/e.age.fractional);Vec3 position=e.position;position.z=0;position.x=float(position.x-float(float(e.digit_count)*advance*.5f));
        const float dx=float(player.x-e.position.x),dy=float(player.y-e.position.y);const i32 distance=truncate_int(float(float(dx*dx)+float(dy*dy)));u32 alpha=distance>16384?255:distance>4096?u32((distance-4096)*128/12288+128):128;
        i32 threshold=(28-e.digit_count)*2;for(i32 digit=e.digit_count-1;digit>=0;--digit,threshold+=2){const u32 value=e.digits[digit];i32 sprite=259+value;bool visible=true;if(e.age.current>=threshold-4&&value!=10){if(e.age.current<threshold)sprite=270+value;else if(e.age.current<threshold+4)sprite=280+value;else visible=false;}
            if(visible){if(!glyph.select_sprite(sprite)){error="Floating number sprite is unavailable";return false;}glyph.visual.flags|=8;glyph.presentation_motion=true;glyph.presentation_generation=presentation_generations[n];glyph.presentation_part=u32(digit);glyph.age=e.age;glyph.visual.translation=position;glyph.visual.color=(e.color&0xffffff)|(alpha<<24);if(!out.draw_glyph(glyph)){error="Floating number draw failed";return false;}}position.x=float(position.x+advance);}
    }
    for(u32 n=13;n<18;++n){const auto& e=entries[n];if(!e.active)continue;Vec3 position=e.position;position.x=float(position.x+224);position.y=float(position.y+16);char value[96];if(e.bonus<0){if(!out.draw_text(position,e.color,"NO BONUS"))return false;}else{std::snprintf(value,sizeof value,"BONUS %.1f",double(e.multiplier));if(!out.draw_text(position,e.color,value))return false;position.y=float(position.y+11);std::snprintf(value,sizeof value,"%d",e.bonus);if(!out.draw_text(position,e.color,value))return false;}}return true;
}
}
