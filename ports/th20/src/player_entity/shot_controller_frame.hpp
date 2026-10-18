#pragma once
#include "frame_helpers.hpp"
namespace th20::source::player_entity {
class ShotControllerServices {
public:
    virtual ~ShotControllerServices()=default;
    virtual ShotFrameServices& frames()=0;
    virtual bool world_allows_shooting()=0; //HUD, Enemy0+124, stage-clear owner, Game+e8 bit7
    virtual bool input_blocked()=0; //Game+e8 bit19
    virtual int input_slot(int view)=0; //498fb0, actual Controller+2e30/+2e34
    virtual std::uint32_t held(int slot,std::uint32_t mask)=0; //4973a0, ButtonState+29c
    virtual void shoot_weapons(game_session::Context&,int frame,int secondary,int power)=0; //5331e0
    virtual void activate_weapons(game_session::Context&,bool inactive)=0; //5330a0
};
ShotControllerServices& shot_controller_services();
int update_shooting(ShotController&,ShotControllerServices&); //505a30
void update_shot_controller(ShotController&,ShotControllerServices&); //504210
void update_shot_controller(ShotController&);
}
