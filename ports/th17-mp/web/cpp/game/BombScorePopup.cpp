#include "BombScorePopup.hpp"
namespace th17 {
bool BombScorePopup::draw(HudDrawServices& output,const Vec3& player,float rate,std::string& error){
    if(age.current<=0){displayed=-1;return true;}
    if(age.current<61){
        if(displayed<0){displayed=amount;position={player.x,float(player.y-32),player.z};}
        static constexpr u32 colors[]={0x60606060,0xa0b0b080,0xb0b8b880,0xc0c0c080,0xd0d0d080,0xe0e0e080,0xffffff30};
        if(u32(tier)>=7){error="Bomb score color outside range";return false;}
        HudTextDraw text;text.position=position;text.style.color=colors[tier];text.style.font=2;text.style.coordinate_space=1;text.style.alignment=0;text.style.vertical_alignment=2;text.text="+"+std::to_string(truncate_int(displayed));
        if(!output.hud_text(text)){error="Bomb score text submission failed";return false;}
    }
    age.decrement(&rate);return true;
}
}
