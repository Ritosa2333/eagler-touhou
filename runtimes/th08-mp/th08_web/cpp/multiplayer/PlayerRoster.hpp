#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer roster must not enter an ordinary build
#endif
#include "../game/PlayerSimulation.hpp"
#include "PlayerResources.hpp"
namespace th08 {
// Game owners are bound once, and their resource references never change seats.
// available excludes spirits; native respawn/deathbomb rules remain in Player.
struct PlayerRoster {
    struct Seat {
        PlayerSimulation* player=nullptr;
        PlayerResourceView* resources=nullptr;
        PlayerValues* values=nullptr;
        GameGauge* gauge=nullptr;
        bool available=true;
    } seats[3];
    u32 count=2;
    void bind(u32 seat,PlayerSimulation& p,PlayerResourceView& r,PlayerValues& v,GameGauge& g){
        if(seat<3)seats[seat]={&p,&r,&v,&g,true};
    }
    bool eligible(u32 seat)const{return seat<count&&seats[seat].player&&seats[seat].available;}
    u32 ordered(const Vec3& origin,u32 (&indices)[3])const{
        float distances[3]{};u32 used=0;
        for(u32 seat=0;seat<count;++seat){
            if(!eligible(seat))continue;
            const auto& position=seats[seat].player->status().motion.movement.position;
            const float x=Scalar::sub(position.x,origin.x),y=Scalar::sub(position.y,origin.y);
            const float distance=Scalar::add(Scalar::mul(x,x),Scalar::mul(y,y));
            u32 at=used;while(at&&distance<distances[at-1]){distances[at]=distances[at-1];indices[at]=indices[at-1];--at;}
            distances[at]=distance;indices[at]=seat;++used;
        }
        return used;
    }
    Vec3 target(const Vec3& origin)const{
        u32 indices[3];return ordered(origin,indices)?seats[indices[0]].player->status().motion.movement.position:Vec3{192,384,0};
    }
};
}
