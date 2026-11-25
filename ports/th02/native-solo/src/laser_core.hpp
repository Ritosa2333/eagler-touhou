#pragma once
#include "enemy_math.hpp"
#include "collision_display.hpp"
#include <array>
#include <cstdint>
#include <functional>
namespace soew {
// TH02 sub_129DD/12A19/12A7D/12AE2/12B9E. Lasers are NOT ordinary bullets:
// Bomb does not suppress generation or cancel these slots in original code.
struct LaserCore {
    struct Laser {std::uint8_t flag=0,state=0,charge_age=0,base_pattern=0;int x=0,y=0,delay=0,active_frames=0;bool visible=false,charging=false;unsigned draw_pattern=0,draw_state=0;};
    std::array<Laser,12> slots{};
    unsigned initial_delay=16;
    std::uint64_t spawned=0,removed=0;
    using Sound=std::function<void(unsigned)>;
    bool add(int x,int y,int frames,unsigned pattern,const Sound &sound={}){
        if(x<32||x>=416)return false;
        for(auto &l:slots)if(l.flag==0){l.flag=1;l.state=1;l.x=x;l.y=y;l.delay=int(initial_delay);l.active_frames=frames;l.charge_age=0;l.base_pattern=std::uint8_t(pattern);l.visible=false;++spawned;if(sound)sound(6);return true;}
        return false;
    }
    void begin_frame(std::uint32_t frame){
        for(auto &l:slots){l.visible=false;if(!l.flag)continue;if(l.charge_age<=4&&!(frame&3))++l.charge_age;if(l.flag==2){l.flag=0;++removed;}}
    }
    bool update(std::uint32_t frame,int player_x,int player_y,bool player_hit,const Sound &sound={},CollisionFrame *display=nullptr){
        bool hit=false;
        for(auto &l:slots){if(l.flag!=1)continue;
            if(l.charge_age<4){l.charging=true;l.draw_pattern=30+l.charge_age;l.visible=true;--l.delay;continue;}
            l.charging=false;
            if(l.state==1){--l.delay;if(l.delay<=0)++l.state;}
            else if(l.state<4){if(!(frame&7))++l.state;if(l.state==4&&sound)sound(7);}
            else if(l.state==4){--l.active_frames;if(l.active_frames==0)++l.state;}
            else {if(!(frame&3))++l.state;if(l.state==9){l.flag=2;continue;}}
            l.visible=true;l.draw_state=l.state;
            if(l.state==4){
                const CollisionBounds bounds{CollisionAxis::open(l.x-24,l.x+8),CollisionAxis::at_least(l.y)};
                const CollisionPoint player{player_x,player_y};
                if(display)display->hazard(CollisionSource::Laser,bounds,CollisionSample::Projectile,player);
                if(bounds.contains(player)&&!player_hit){hit=true;player_hit=true;}
            }
        }
        return hit;
    }
};
}
