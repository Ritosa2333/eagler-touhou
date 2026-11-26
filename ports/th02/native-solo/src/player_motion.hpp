#pragma once
#include "core_contract.hpp"
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
namespace soew {
inline constexpr std::uint64_t pointer_active=1ull<<40,pointer_unlimited=1ull<<41,pointer_begin=1ull<<44;
inline constexpr std::uint64_t pointer_mask=(((1ull<<42)-1)&~4095ull)|pointer_begin;
inline constexpr bool valid_pointer_motion(std::uint64_t value){
    return !(value&~pointer_mask)&&(!value||(value&pointer_active));
}
// TH02 th02/main/playfld.hpp + player/player.hpp and player.cpp:19..61.
// Coordinates are sprite top-left INTEGER pixels, not TH03/TH04 Q12.4 centers.
struct CoopPointerMotion {
    int x=0,y=0;
    bool active=false,unlimited=false;
    void sample(std::uint64_t value,bool restrict_speed){
        if(!(value&pointer_active)){*this={};return;}
        if(value&pointer_begin)x=y=0;
        const auto axis=[](unsigned bits){return int(bits^8192)-8192;};
        x=std::clamp(x+axis(unsigned(value>>12&16383)),-8192,8191);
        y=std::clamp(y+axis(unsigned(value>>26&16383)),-8192,8191);
        active=true;unlimited=(value&pointer_unlimited)&&!restrict_speed;
    }
};
struct PlayerMotion {
    enum Pose {Still,LeanLeft,LeanRight};
    int x=208,y=336,shot=1;
    unsigned back_page=0;
    std::uint64_t frame=0;
    Pose pose=Still;
    int last_dx=0,last_dy=0; // Source si/di before player_move clamps position.
    bool focused=false;
    int focus_rx=0,focus_ry=0; // Signed half-pixel carry; collisions still use integer pixels.
    explicit PlayerMotion(int selected_shot=1,unsigned initial_page=0):shot(selected_shot),back_page(initial_page){
        if(shot<0||shot>2||initial_page>1)throw std::runtime_error("Invalid player motion configuration");
    }
    void clear_focus(){focused=false;focus_rx=focus_ry=0;}
    void tick(std::uint16_t input,CoopPointerMotion *pointer=nullptr){
        int dx=0,dy=0;
        const int aligned=shot==0?5:shot==1?4:3;
        const int diagonal=shot==0?4:3;
        // th02_main.asm:5516..5539: one exact numpad diagonal overrides arrows.
        // Multiple diagonal masks fall through to the exact aligned jump table.
        const auto dir=input&0xf000;
        if(dir==UpLeft){dx=-diagonal;dy=-diagonal;}
        else if(dir==UpRight){dx=diagonal;dy=-diagonal;}
        else if(dir==DownLeft){dx=-diagonal;dy=diagonal;}
        else if(dir==DownRight){dx=diagonal;dy=diagonal;}
        else switch(input&15){
            case Up:dy=-aligned;break;case Down:dy=aligned;break;
            case Left:dx=-aligned;break;case Right:dx=aligned;break;
            case Up|Left:dx=-diagonal;dy=-diagonal;break;
            case Down|Left:dx=-diagonal;dy=diagonal;break;
            case Up|Right:dx=diagonal;dy=-diagonal;break;
            case Down|Right:dx=diagonal;dy=diagonal;break;
            default:break; // Opposite/3-way/4-way inputs are stationary in original.
        }
        // asm5628..5665: typeA loses one pixel on page1 on BOTH axes.
        // player_invalidate2518..2531 copies front position to back first;
        // this is per-frame movement, not independent two-frame trajectories.
        if(shot==0&&back_page){if(dx)dx+=dx<0?1:-1;if(dy)dy+=dy<0?1:-1;}
        if(input&Focus){
            focused=true;
            const auto half=[](int delta,int &carry){
                const int halves=delta+carry;
                carry=halves%2;
                return halves/2;
            };
            dx=half(dx,focus_rx);dy=half(dy,focus_ry);
        }else clear_focus();
        if(pointer&&pointer->active){
            const bool two_axes=std::abs(pointer->x)>=16&&std::abs(pointer->y)>=16;
            const auto limit=pointer->unlimited?8192:
                ((two_axes?diagonal:aligned)-(shot==0&&back_page?1:0))*((input&Focus)?8:16);
            const auto consume=[limit](int &remaining){
                const int amount=std::clamp(remaining,-limit,limit)/16;remaining-=amount*16;return amount;
            };
            dx=consume(pointer->x);dy=consume(pointer->y);
        }
        pose=dx<0?LeanLeft:dx>0?LeanRight:Still;
        last_dx=dx;last_dy=dy;
        x=std::clamp(x+dx,28,390);y=std::clamp(y+dy,16,344);
        if((x==28&&focus_rx<0)||(x==390&&focus_rx>0))focus_rx=0;
        if((y==16&&focus_ry<0)||(y==344&&focus_ry>0))focus_ry=0;
        back_page^=1;++frame;
    }
};
}
