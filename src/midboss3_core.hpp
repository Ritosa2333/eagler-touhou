#pragma once
#include "midboss1_core.hpp"
namespace soew {
// ReC98 th02_main.asm:6498-7172. Scene integration remains opt-in.
struct Midboss3Core {
    enum class Flag : std::uint8_t { Alive, Dying, Removed };
    struct Actor {Flag flag=Flag::Alive;std::array<int,2> left{},top{};std::uint16_t damage=0,death_frame=0;bool visible=false,hit_flash=false;unsigned death_pattern=0;};
    std::array<Actor,2> actor{};std::array<std::uint8_t,2> angle{};
    bool active=false;std::uint16_t phase_frame=0;std::uint8_t cycles=0;unsigned pattern=144;
    int target_x=-1,target_y=-1;
    using Hooks=Midboss1Core::Hooks;
    void trigger(unsigned progress){if(progress==103)active=true;}
    void begin_frame(unsigned back){
        if(back>1)throw std::runtime_error("Invalid midboss3 page");
        if(!active)return;
        unsigned removed=0;for(auto &a:actor){if(a.flag==Flag::Removed){++removed;continue;}a.left[back]=a.left[back^1];a.top[back]=a.top[back^1];}if(removed==2)active=false;
    }
    void update(unsigned back,unsigned scroll_line,unsigned scroll_step,unsigned rank,const Hooks &h){
        if(back>1||scroll_line>=400)throw std::runtime_error("Invalid midboss3 context");
        for(auto &a:actor){a.visible=false;a.hit_flash=false;}if(!active)return;
        auto &target=actor[actor[0].flag==Flag::Alive?0:1];target_x=enemy_s16(unsigned(target.left[back]+24));target_y=enemy_s16(unsigned(target.top[back]+16));
        ++phase_frame;
        if(phase_frame==1){actor[0].left.fill(32);actor[1].left.fill(352);for(auto &a:actor){a.top.fill(-16);a.damage=0;}cycles=0;return;}
        pattern=144+(scroll_line&3);
        const auto hit=[&](Actor &a){return h.damage?h.damage(a.left[back]+12,a.top[back],40,30):0u;};
        if(enemy_s16(phase_frame)<128){for(auto &a:actor){a.top[back]=enemy_s16(unsigned(a.top[back]+int(scroll_step)));const auto d=hit(a);if(d){a.damage=std::uint16_t(a.damage+d);a.hit_flash=true;if(h.sound)h.sound(4);}a.visible=true;}return;}
        if(phase_frame==128)++cycles;
        for(unsigned i=0;i<2;++i){auto &a=actor[i];
            if(a.flag==Flag::Dying){if(h.spark)h.spark({a.left[back]+32,a.top[back]+16,60,1,false});a.death_pattern=10+unsigned(enemy_s16(a.death_frame)/6);++a.death_frame;if(enemy_s16(a.death_frame)>=48){if(h.spark)h.spark({a.left[back]+32,a.top[back]+16,128,32,true});a.death_frame=0;a.flag=Flag::Removed;target_x=target_y=-1;}else a.visible=true;continue;}
            if(a.flag!=Flag::Alive)continue;
            const int side=i==0?8:-8,y=a.top[back]+12;const int phase=enemy_s16(phase_frame);
            const auto emit=[&](unsigned ang,unsigned group,unsigned speed){if(h.bullet){EnemyBulletRequest r;r.left=a.left[back]+28;r.top=y;r.angle=std::uint8_t(ang);r.group=group;r.speed=speed;h.bullet(r);}};
            if(phase<152){if(phase==128)angle={0,128};if(!(phase_frame&3)){angle[i]=std::uint8_t(angle[i]+side);emit(angle[i],0x20,40);}}
            else if(phase<192){a.left[back]=enemy_s16(unsigned(a.left[back]+side));if(!(phase_frame&7)){angle[i]=std::uint8_t(angle[i]+2*side);emit(std::uint8_t(128-angle[i]),rank==0?0x02:0x0e,40);}}
            else if(phase<352){a.top[back]=enemy_s16(unsigned(a.top[back]+2));if(a.top[back]>=400)a.top[back]-=400;}
            else if(phase<416){if(phase>=376)a.left[back]=enemy_s16(unsigned(a.left[back]-side));if(!(phase_frame&3)){angle[i]=std::uint8_t(angle[i]-side);emit(angle[i],0x20,50);}}
            else if(phase<576){a.top[back]=enemy_s16(unsigned(a.top[back]-2));if(a.top[back]<0)a.top[back]+=400;if(phase%60==0)emit(0,0x19,40);}
            else phase_frame=127;
            const auto d=hit(a);if(d){a.damage=std::uint16_t(a.damage+d);if(enemy_s16(a.damage)>280){if(h.sound)h.sound(2);a.flag=Flag::Dying;if(h.score)h.score(20000);}else{a.visible=true;a.hit_flash=true;if(h.sound)h.sound(4);}}else a.visible=true;
            if(cycles>=4)a.flag=Flag::Dying;
        }
    }
};
}
