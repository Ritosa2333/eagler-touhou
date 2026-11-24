#pragma once
#include "enemy_effects.hpp"
#include "enemy_math.hpp"
#include <array>
#include <cstdint>
#include <functional>
namespace soew {
// Source th02_main.asm10293..10625. No fabricated HP/drop/contact damage.
// Shared boss coordinates remain pixel positions carried across VRAM pages.
struct Midboss1Core {
    bool active=false,dying=false,visible=false,hit_flash=false;
    bool initialized=false,present=true;
    std::uint16_t phase_frame=0,damage_taken=0,death_frame=0;
    std::array<int,2> left{192,192},top{-16,-16};
    unsigned pattern=148,death_pattern=10;
    constexpr static unsigned death_scale=3;
    int target_x=-1,target_y=-1;
    std::uint64_t shot_groups=0,score_awarded=0;
    struct Spark {int x,y,speed,count;bool sprite;};
    struct Hooks {
        std::function<void(const EnemyBulletRequest&)> bullet;
        std::function<unsigned(int,int,int,int)> damage;
        std::function<void(unsigned)> score,sound;
        std::function<void(const Spark&)> spark;
        std::function<void(int,int,unsigned)> tile;
    };
    void begin_frame(unsigned back){
        if(back>1)throw std::runtime_error("Invalid midboss page");
        if(!active)return;
        left[back]=left[back^1];top[back]=top[back^1];
        if(!present)active=false;
    }
    void trigger(unsigned progress){if(progress==116)active=true;}
    void update(unsigned back,unsigned scroll_line,unsigned progress,const Hooks &h){
        visible=false;hit_flash=false;if(!active)return;
        target_x=216;target_y=64;++phase_frame;
        if(phase_frame==1){left.fill(192);top.fill(-16);damage_taken=0;pattern=148;initialized=true;return;}
        if(phase_frame<18){top[back]+=2;visible=true;return;}
        pattern=(scroll_line&1)?149:148;
        if(dying){
            death_pattern=10+(death_frame>>3);++death_frame;
            if(h.spark)h.spark({left[back]+32,top[back]+48,60,1,false});
            if(death_frame>=64){if(h.spark)h.spark({left[back]+32,top[back]+48,128,48,true});death_frame=0;phase_frame=0;target_x=target_y=-1;present=false;return;}
            if(!(death_frame&3))++top[back];
            visible=true;return;
        }
        const unsigned cycle=phase_frame&127;
        if(cycle==0||cycle==48||cycle==96){EnemyBulletRequest r;r.left=left[back]+28;r.top=top[back]+88;r.group=cycle==48?0x0b:0x12;r.speed=cycle==48?30:50;r.angle=0;if(h.bullet)h.bullet(r);++shot_groups;}
        if(!(scroll_line&15)&&h.tile)for(unsigned i=0;i<4;++i)h.tile(left[back]+int(i*16),int((48+scroll_line)%400),60+i);
        int vram_y=top[back]+int(scroll_line);if(vram_y>=400)vram_y-=400;
        const unsigned dealt=h.damage?h.damage(left[back],top[back],64,96):0;
        if(dealt){damage_taken=static_cast<std::uint16_t>(damage_taken+dealt);
            if(enemy_s16(damage_taken)>300&&vram_y<304){dying=true;if(h.sound)h.sound(2);if(h.score)h.score(10000);score_awarded+=10000;return;}
            if(h.sound)h.sound(4);
            hit_flash=true;visible=true;
        }else if(progress>=184&&top[back]<=304){dying=true;if(h.sound)h.sound(2);}
        else visible=true;
    }
};
}
