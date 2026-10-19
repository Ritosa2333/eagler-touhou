#include "ScorePopups.hpp"
#include <cmath>
namespace th11 {
void ScorePopups::add(Vec3 position,i32 value,u32 color,const float* rate){
    if(cursor>=10)cursor=0;auto& p=entries[cursor++];p.active=1;p.length=0;
    if(value<0){p.digits[0]=10;p.length=1;}
    else do{p.digits[p.length++]=u8(value%10);value/=10;}while(value);
    p.color=color;p.position=position;p.age.set(0,rate);
}
void ScorePopups::update(float rate){for(auto& p:entries)if(p.active){p.position.y=float(double(p.position.y)-double(rate)*.5);p.age.tick();if(p.age.current>60)p.active=0;}}
std::vector<ScoreGlyph> ScorePopups::glyphs(Vec3 player)const{
    std::vector<ScoreGlyph> result;result.reserve(100);
    for(const auto& p:entries){if(!p.active)continue;
        const float spacing=p.age.current<8?float(8./p.age.fractional):8.f;
        // Native division by zero on the birth frame produces invisible
        // nonfinite vertices. Omit that draw instead of sending NaNs to GL.
        if(!std::isfinite(spacing))continue;
        float x=float((double(p.position.x)-double(p.length)*spacing*.5)+32+192);
        const float y=float(double(p.position.y)+16),dx=float(double(player.x)-p.position.x),dy=float(double(player.y)-p.position.y);
        const i32 distance=truncate_int(double(dx)*dx+double(dy)*dy);
        const u8 alpha=distance>0x4000?208:distance>0x1000?u8(((distance*5-0x5000)*32)/0x3000+48):48;
        for(i32 i=p.length-1;i>=0;--i){const i32 digit=p.digits[i];const i32 base=p.age.current<52||digit==10?196:p.age.current<56?207:217;
            result.push_back({base+digit,x,y,(p.color&0xffffff)|(u32(alpha)<<24)});x=float(double(x)+spacing);
        }
    }return result;
}
bool ScorePopups::draw(AnmRenderer& renderer,AnmResource& resource,Vec3 player){
    AnmVm vm;vm.initialize();vm.resource=&resource;vm.file_index=2;
    for(const auto& g:glyphs(player)){if(!vm.bind_sprite(g.sprite))return false;vm.position={g.x,g.y,0};vm.color=g.color;vm.flags|=8;if(renderer.draw_ascii_sprite(vm)==-2)return false;}
    return true;
}
}
