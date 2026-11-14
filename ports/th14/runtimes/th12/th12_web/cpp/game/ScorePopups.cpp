#include "ScorePopups.hpp"
#include "AsciiText.hpp"
#include <cstdio>
#include <cmath>
namespace th11 {
void ScorePopups::add(Vec3 position,i32 value,u32 color,const float* rate){
    if(cursor>=10)cursor=0;auto& p=entries[cursor++];p.active=1;p.length=0;
    if(value<0){p.digits[0]=10;p.length=1;}
    else do{p.digits[p.length++]=u8(value%10);value/=10;}while(value);
    p.color=color;p.position=position;p.age.set(0,rate);
#if PORT_TITLE == 14
    p.velocity=2.f;
#endif
}
void ScorePopups::update(float rate){for(auto& p:entries)if(p.active){
#if PORT_TITLE == 14
    p.position.y=p.position.y-float(p.velocity*rate);p.velocity=p.velocity*.95f;
#else
    p.position.y=float(double(p.position.y)-double(rate)*.5);
#endif
    p.age.tick();if(p.age.current>60)p.active=0;}
#if PORT_TITLE == 14
    for(auto& p:bonuses)if(p.active){p.age.tick();if(p.age.current>60){
        const i32 alpha=i32(p.color>>24)-4;
        if(alpha<=0)p.active=0;else p.color=(p.color&0xffffff)|(u32(alpha)<<24);
    }}
#endif
}
#if PORT_TITLE == 14
void ScorePopups::add_bonus(Vec3 position,i32 value,u32 color,float multiplier,const float* rate){
    for(auto& p:bonuses)if(!p.active){
        p.color=color;p.age.set(0,rate);p.position=position;
        if(p.position.y<0)p.position.y=0;
        if(p.position.x<-168.f)p.position.x=-168.f;
        else if(p.position.x>168.f)p.position.x=168.f;
        p.bonus_value=value;p.multiplier=multiplier;p.active=1;return;
    }
}
bool ScorePopups::queue_bonuses(AsciiText& ascii)const{
    for(const auto& p:bonuses)if(p.active){
        // 40b920 selects camera 4d9190 and its playfield viewport for pass2.
        AsciiStyle style;style.color=p.color;style.font=2;style.pass=2;style.playfield=true;
        auto centered=[&](const char* text,Vec3 position){
            // 40c1b0 and 40c27a: font2 centered alignment subtracts
            // each character's half advance separately, then half height.
            for(const char* s=text;*s;++s)position.x=position.x-(*s=='.'?2.f:3.5f);
            position.y=position.y-5.f;return ascii.add(text,position,style);
        };
        Vec3 position{p.position.x+224.f,p.position.y+16.f,p.position.z};char text[64];
        if(p.bonus_value<0){if(!centered("NO BONUS",position))return false;}
        else{
            std::snprintf(text,sizeof text,"BONUS %.1f",double(p.multiplier));if(!centered(text,position))return false;
            position.y=position.y+11.f;std::snprintf(text,sizeof text,"%d",p.bonus_value);if(!centered(text,position))return false;
        }
    }
    return true;
}
#endif
std::vector<ScoreGlyph> ScorePopups::glyphs(Vec3 player)const{
    std::vector<ScoreGlyph> result;result.reserve(100);
    for(const auto& p:entries){if(!p.active)continue;
        const float spacing=p.age.current<8?(kPortTitle==14?8.f/p.age.fractional:float(8./p.age.fractional)):8.f;
        // Native division by zero on the birth frame produces invisible
        // nonfinite vertices. Omit that draw instead of sending NaNs to GL.
        if(!std::isfinite(spacing))continue;
        float x;float y;float dx,dy;i32 distance;
#if PORT_TITLE == 14
        x=p.position.x-float(float(p.length)*spacing)*.5f;y=p.position.y;
        dx=player.x-p.position.x;dy=player.y-p.position.y;
        distance=truncate_int(float(float(dx*dx)+float(dy*dy)));
#else
        x=float((double(p.position.x)-double(p.length)*spacing*.5)+32+192);
        y=float(double(p.position.y)+16);dx=float(double(player.x)-p.position.x);dy=float(double(player.y)-p.position.y);
        distance=truncate_int(double(dx)*dx+double(dy)*dy);
#endif
        const u8 alpha=distance>0x4000?208:distance>0x1000?u8((kPortTitle==14?((distance-0x1000)*160):((distance*5-0x5000)*32))/0x3000+48):48;
        for(i32 i=p.length-1;i>=0;--i){const i32 digit=p.digits[i];i32 base;
#if PORT_TITLE == 14
            const i32 threshold=(28-(i+1))*2;
            if(p.age.current<threshold-4||digit==10)base=259;
            else if(p.age.current<threshold)base=270;
            else if(p.age.current<threshold+4)base=280;
            else {x=x+spacing;continue;}
#else
            base=p.age.current<52||digit==10?196:p.age.current<56?207:217;
#endif
            result.push_back({base+digit,x,y,(p.color&0xffffff)|(u32(alpha)<<24)});x=kPortTitle==14?x+spacing:float(double(x)+spacing);
        }
    }return result;
}
bool ScorePopups::draw(AnmRenderer& renderer,AnmResource& resource,Vec3 player){
    AnmVm vm;vm.initialize();vm.resource=&resource;vm.file_index=2;
#if PORT_TITLE == 14
    // 45780c: the detached score VM uses the intermediate playfield origin.
    vm.flags2=0x1c0000;
#endif
    for(const auto& g:glyphs(player)){if(!vm.bind_sprite(g.sprite))return false;vm.position={g.x,g.y,0};vm.color=g.color;vm.flags|=8;if(renderer.draw_ascii_sprite(vm)==-2)return false;}
    return true;
}
}
