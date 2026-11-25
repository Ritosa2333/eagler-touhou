#pragma once
#include "enemy_effects.hpp"
#include <array>
#include <stdexcept>

namespace soew {
// TH02 main ASM 13368–13737: a timed tile animation, not a damageable boss.
struct ExtraMidbossCore {
    static constexpr unsigned trigger_progress=200;
    static constexpr unsigned last_active_frame=420;
    static constexpr std::array<unsigned,6> images={1,2,16,17,18,9}; // ASM 26851–26854; byte 0 is unused.
    struct Tile {
        unsigned step=0,age=0;
        int left=0,y=0;
        std::uint8_t group=0,angle=0,speed=0;
    };
    std::array<Tile,30> tiles{};
    unsigned frame=0,distance=0,scheduled=0,accepted=0,expired=0;
    bool started=false,active=false,finished=false;

    void trigger(unsigned progress){
        if(!started&&progress==trigger_progress)started=active=true;
    }
    bool schedule(int x,int vram_y,std::uint8_t group,std::uint8_t angle,std::uint8_t speed){
        ++scheduled;
        if(vram_y<0)vram_y+=400;
        else if(vram_y>=400)vram_y-=400;
        for(auto &slot:tiles)if(!slot.step){
            slot={1,0,x,vram_y,group,angle,speed};++accepted;return true;
        }
        return false;
    }
    void update(int scroll_line,const EnemyEffectSink &effects){
        if(!active)return;
        if(++frame==1||frame==96||frame==192)distance=0;
        else if(frame<288&&frame%16==0){
            const int center_x=frame<96?128:frame<192?304:192;
            const int center_y=frame<96?128:frame<192?0:336;
            const std::uint8_t group=frame<96?0x21:frame<192?0x24:0x25;
            const int d=int(distance);
            if(d){
                schedule(center_x-d,center_y,group,0,100);
                schedule(center_x,center_y-d,group,0,100);
                schedule(center_x+d,center_y,group,0,100);
            }
            schedule(center_x,center_y+d,group,0,100);
            distance+=16;
        }
        if(frame>last_active_frame){frame=0;active=false;finished=true;}
        // Scheduled slots get their first age increment on the insertion frame.
        for(auto &slot:tiles){
            if(!slot.step||(++slot.age&7))continue;
            if(!effects.tile||!effects.tile({slot.left,unsigned(slot.y),images[slot.step-1]}))
                throw std::runtime_error("Extra midboss tile consumer not connected");
            if(++slot.step<=images.size())continue;
            int y=slot.y-scroll_line;
            if(y<0)y+=400;
            if(!effects.bullet||!effects.bullet({EnemyBulletRequest::Pellet,slot.left+4,y+4,slot.angle,slot.group,0,slot.speed}))
                throw std::runtime_error("Extra midboss pellet consumer not connected");
            slot.step=0;++expired;
        }
    }
};
}
