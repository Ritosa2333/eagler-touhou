#pragma once
#include "midboss1_core.hpp"
namespace soew {
// th02_main.asm11342..11593. No invented contact damage/drop or HP scaling.
struct Midboss2Core {
    bool active=false,dying=false,visible=false,hit_flash=false,present=true;
    std::uint16_t phase_frame=0,damage_taken=0,death_frame=0;
    std::array<int,2> left{192,192},top{-16,-16};
    unsigned pattern=137,death_pattern=10;
    int target_x=-1,target_y=-1;
    std::uint64_t shot_groups=0,score_awarded=0;
    using Hooks=Midboss1Core::Hooks;
    void begin_frame(unsigned back){if(back>1)throw std::runtime_error("Invalid midboss2 page");if(active){left[back]=left[back^1];top[back]=top[back^1];if(!present)active=false;}}
    void trigger(unsigned progress){if(progress==80)active=true;}
    void update(unsigned back,unsigned scroll_line,const Hooks &h){
        visible=hit_flash=false;if(!active)return;
        target_x=216;target_y=top[back]+24;++phase_frame;
        if(phase_frame==1){left.fill(192);top.fill(-16);damage_taken=0;present=true;return;}
        pattern=137+((scroll_line>>1)&3);
        if(phase_frame<320){if(!(phase_frame&3))++top[back];const auto dealt=h.damage?h.damage(left[back],top[back],64,58):0;if(dealt){if(h.sound)h.sound(4);++damage_taken;hit_flash=visible=true;}else visible=true;return;}
        if(dying){
            if(death_frame<48&&(death_frame&15)==5&&h.spark)h.spark({left[back]+32,top[back]+32,128,24,true});
            death_pattern=10+(death_frame>>3);++death_frame;
            if(death_frame>=64){death_frame=phase_frame=0;dying=false;present=false;target_x=target_y=-1;return;}visible=true;return;
        }
        if(phase_frame%8==0&&phase_frame%192<64){EnemyBulletRequest r;r.kind=EnemyBulletRequest::Bullet16;r.left=left[back]+28;r.top=top[back]+32;r.angle=std::uint8_t(phase_frame<<2);r.group=0x80;r.pattern=84;r.speed=30;if(h.bullet)h.bullet(r);++shot_groups;}
        int vram_y=top[back]+int(scroll_line);if(vram_y<0)vram_y+=400;else if(vram_y>=400)vram_y-=400;
        const auto dealt=h.damage?h.damage(left[back],top[back],64,58):0;
        if(dealt){damage_taken=std::uint16_t(damage_taken+dealt);if(enemy_s16(damage_taken)>380&&vram_y<304){if(h.sound)h.sound(2);dying=true;if(h.score)h.score(20000);score_awarded+=20000;return;}if(h.sound)h.sound(4);hit_flash=visible=true;return;}
        if(phase_frame>1600)dying=true;
        visible=true;
    }
};
}
