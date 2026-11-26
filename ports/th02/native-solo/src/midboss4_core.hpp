#pragma once
#include "midboss1_core.hpp"
#include <stdexcept>

namespace soew {
// ReC98 th02_main.asm:21559–22139. Rendering and slot allocation are adapter-owned.
struct Midboss4Core {
    static constexpr std::array<unsigned,2> triggers{0x3b0,0x660};
    struct Hooks : Midboss1Core::Hooks {
        std::function<unsigned()> random8;
        std::function<void(int,int,unsigned,unsigned,unsigned)> laser;
        std::function<void(int,int,unsigned)> item;
    };
    std::array<int,2> left{},top{};
    int x=0,y=0,direction=1,target_x=-1,target_y=-1;
    unsigned entries=0,phase_frame=0,death_frame=0,pattern=148,attack=0,cycles=0;
    BossDamage damage_taken;
    bool active=false,dying=false,visible=false,hit_flash=false,flying_in=true;

    void begin_frame(unsigned page) {
        if(page>1)throw std::runtime_error("Invalid midboss4 page");
        if(active){left[page]=left[page^1];top[page]=top[page^1];}
    }
    void trigger(unsigned progress) {
        if(active||entries>=triggers.size()||progress!=triggers[entries])return;
        ++entries;active=true;phase_frame=0;
    }
    void update(unsigned page,unsigned scroll_line,unsigned rank,const Hooks &h) {
        if(page>1||scroll_line>=400||rank>4)throw std::runtime_error("Invalid midboss4 context");
        visible=hit_flash=false;
        if(!active)return;
        target_x=x+24;target_y=y+24;
        ++phase_frame;
        if(phase_frame==1){
            direction=entries==1?1:-1;pattern=direction==1?150:149;
            left.fill(direction==1?64:320);top.fill(-16);
            damage_taken=0;death_frame=0;cycles=0;attack=0;flying_in=true;dying=false;
        }
        const auto retire=[&]{active=false;phase_frame=0;death_frame=0;target_x=target_y=-1;};
        y=top[page];const int muzzle_y=y+48;
        if(dying){
            if(phase_frame%24==0&&h.spark)h.spark({x+32,y+32,128,24,true});
            pattern=10+death_frame/8;
            if(++death_frame>=64){retire();return;}
            visible=true;return;
        }
        const auto sound=[&](unsigned id){if(h.sound)h.sound(id);};
        const auto rng=[&]{if(!h.random8)throw std::runtime_error("Midboss4 RNG unavailable");return h.random8()&255u;};
        const auto bullet=[&](int sy,unsigned angle,unsigned group,unsigned speed,unsigned cel=0){
            if(!h.bullet)return;
            EnemyBulletRequest r;r.left=x+28+direction*16;r.top=sy;r.angle=std::uint8_t(angle);r.group=std::uint8_t(group);r.speed=speed;
            if(cel){r.kind=EnemyBulletRequest::Bullet16;r.pattern=std::uint8_t(cel);}
            h.bullet(r);
        };
        if(flying_in){
            x=(left[page]+=direction*8);y=(top[page]+=2);
            if(phase_frame%8==0){sound(7);bullet(muzzle_y,64,0x20,80);}
            if(phase_frame==32){flying_in=false;phase_frame=2;pattern=148;direction=-direction;attack=rng()%5;}
        }else if(phase_frame>50){
            pattern=direction<0?149:150;
            x=(left[page]+=direction*4);
            switch(attack){
            case 0:if(phase_frame%16==0){sound(3);bullet(96,64,0x20,80);}break;
            case 1:if(phase_frame%16==0){sound(3);bullet(96,64,rank==0?0x0f:0x14,phase_frame-20);}break;
            case 2:if(phase_frame%8==0)bullet(96,64,0x80,8,80);break;
            case 3:if(phase_frame%(16-2*rank)==0&&h.laser)h.laser(x+28+direction*16,80,6,111,16);break;
            case 4:if(phase_frame%16==0){sound(7);for(unsigned i=0;i<4;++i)bullet(muzzle_y,(rng()&63u)+32,0x20,70);}break;
            }
            if(cycles==12&&!h.lock_timeout){if(x<=0||x>=384){retire();return;}}
            else if(phase_frame>=114){phase_frame=2;pattern=148;direction=-direction;attack=rng()%5;if(cycles<12)++cycles;}
        }
        const auto d=h.damage?h.damage(x+8,y,48,48):0;
        if(d){
            damage_taken.add(d,h.damage_divisor);
            const int vram_y=y+int(scroll_line);const int wrapped=vram_y>=400?vram_y-400:vram_y;
            if(enemy_s16(damage_taken)>=400&&wrapped>96&&wrapped<336){
                dying=true;sound(2);if(h.score)h.score(50000);if(h.item)h.item(x+24,y,entries==1?2:4);
            }
            hit_flash=true;sound(4);
        }
        visible=true;
    }
};
}
