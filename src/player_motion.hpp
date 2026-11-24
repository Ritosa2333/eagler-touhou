#pragma once
#include "core_contract.hpp"
#include <algorithm>
#include <stdexcept>
namespace soew {
// TH02 th02/main/playfld.hpp + player/player.hpp and player.cpp:19..61.
// Coordinates are sprite top-left INTEGER pixels, not TH03/TH04 Q12.4 centers.
struct PlayerMotion {
    enum Pose {Still,LeanLeft,LeanRight};
    int x=208,y=336,shot=1;
    unsigned back_page=0;
    std::uint64_t frame=0;
    Pose pose=Still;
    int last_dx=0,last_dy=0; // Source si/di before player_move clamps position.
    explicit PlayerMotion(int selected_shot=1,unsigned initial_page=0):shot(selected_shot),back_page(initial_page){
        if(shot<0||shot>2||initial_page>1)throw std::runtime_error("Invalid player motion configuration");
    }
    void tick(std::uint16_t input){
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
        pose=dx<0?LeanLeft:dx>0?LeanRight:Still;
        // asm5628..5665: typeA loses one pixel on page1 on BOTH axes.
        // player_invalidate2518..2531 copies front position to back first;
        // this is per-frame movement, not independent two-frame trajectories.
        if(shot==0&&back_page){if(dx)dx+=dx<0?1:-1;if(dy)dy+=dy<0?1:-1;}
        last_dx=dx;last_dy=dy;
        x=std::clamp(x+dx,28,390);y=std::clamp(y+dy,16,344);
        back_page^=1;++frame;
    }
};
}
