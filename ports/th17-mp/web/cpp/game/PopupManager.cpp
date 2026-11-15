#include "PopupManager.hpp"
#include <cstdio>
namespace th17 {
bool PopupManager::initialize(i32 bank){
    glyph=AnmVm{};
    glyph.environment=&animations.scene_environment();
    glyph.resource=animations.resource(bank);
    if(!glyph.resource){error="Floating number resource unavailable";return false;}
    glyph.object_host=&animations;
    glyph.visual.flags=7;
    glyph.visual.render_flags=0x384000;
    glyph.visible=true;
    if(!glyph.select_sprite(259)){error="Floating number initial sprite unavailable";return false;}
    return true;
}
void PopupManager::number(const Vec3& position,i32 value,u32 color){
    if(next_number>=10)next_number=0;
    auto& entry=entries[next_number++];
    entry.active=1;
    u32 count=0;
    if(value>0){
        do{
            const i32 quotient=value/10;
            entry.digits[count++]=u8(value-quotient*10);
            value=quotient;
        }while(value);
    }else{entry.digits[0]=value<0?10:0;count=1;}
    entry.digit_count=u8(count);
    entry.color=color;
    entry.age.set(0);
    entry.position=position;
    entry.velocity=1;
}
void PopupManager::update(float rate){
    for(u32 i=0;i<entries.size();i++){
        auto& entry=entries[i];
        if(!entry.active)continue;
        if(i<13){
            const float distance=float(entry.velocity*rate);
            entry.velocity=float(entry.velocity*.95f);
            entry.position.y=float(entry.position.y-distance);
        }
        entry.age.tick(&rate);
        if(entry.age.current<=60)continue;
        if(i<13)entry.active=0;
        else{
            const i32 alpha=i32(entry.color>>24)-4;
            if(alpha<1)entry.active=0;
            else entry.color=(entry.color&0xffffff)|(u32(alpha)<<24);
        }
    }
}
bool PopupManager::draw(PopupDrawServices& output,const Vec3& player){
    for(u32 i=0;i<13;i++){
        const auto& entry=entries[i];
        if(!entry.active)continue;
        const float advance=entry.age.current<8?float(8.f/entry.age.fractional):8.f;
        Vec3 position=entry.position;
        // Native drawing retains the embedded glyph's zero Z coordinate.
        position.z=0;
        position.x=float(position.x-float(float(float(entry.digit_count)*advance)*.5f));
        const float x=float(player.x-entry.position.x),y=float(player.y-entry.position.y);
        const i32 distance=truncate_int(float(float(y*y)+float(x*x)));
        const u32 alpha=distance>16384?255:distance>4096?u32((distance-4096)*128/12288+128):128;
        i32 threshold=(28-entry.digit_count)*2;
        for(i32 digit=entry.digit_count-1;digit>=0;digit--,threshold+=2){
            const u32 value=entry.digits[digit];
            i32 sprite=259+value;
            bool visible=true;
            if(entry.age.current>=threshold-4&&value!=10){
                if(entry.age.current<threshold)sprite=270+value;
                else if(entry.age.current<threshold+4)sprite=280+value;
                else visible=false;
            }
            if(visible){
                if(!glyph.select_sprite(sprite)){error="Floating number sprite unavailable";return false;}
                glyph.visual.flags|=8;
                glyph.visual.translation=position;
                glyph.visual.color=(entry.color&0xffffff)|(alpha<<24);
                if(!output.popup_glyph(glyph)){error="Floating number draw failed";return false;}
            }
            position.x=float(position.x+advance);
        }
    }
    for(u32 i=13;i<entries.size();i++){
        const auto& entry=entries[i];
        if(!entry.active)continue;
        HudTextDraw text;
        text.position={float(entry.position.x+224),float(entry.position.y+16),entry.position.z};
        text.style.color=entry.color;
        text.style.font=text.style.coordinate_space=2;
        text.style.alignment=text.style.vertical_alignment=0;
        if(entry.bonus<0)text.text="NO BONUS";
        else{
            char label[64];
            std::snprintf(label,sizeof(label),"BONUS %.1f",double(entry.multiplier));
            text.text=label;
        }
        if(!output.popup_text(text)){error="Bonus text draw failed";return false;}
        if(entry.bonus>=0){
            text.position.y=float(text.position.y+11);
            text.text=std::to_string(entry.bonus);
            if(!output.popup_text(text)){error="Bonus amount draw failed";return false;}
        }
    }
    return true;
}
}
