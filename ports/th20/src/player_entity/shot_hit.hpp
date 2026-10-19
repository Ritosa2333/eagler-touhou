#pragma once
#include "shot_callbacks.hpp"
namespace th20::source::effects {class Controller;}
namespace th20::source::player_entity {
std::uint32_t remember_shot_effect(effects::Controller&,std::uint32_t,ShotCallbackEnvironment&); //497d20/498e70
std::uint32_t reserve_effect_slot(effects::Controller&,ShotCallbackEnvironment&); //498e70, before spawning4fb820
class ShotHitServices {
public:
    virtual ~ShotHitServices()=default;
    virtual ShotCallbackEnvironment& callbacks()=0;
    virtual int default_hit(Shot&)=0; //504af0
    virtual float random_signed(unsigned stream)=0;
    virtual std::uint32_t random_bounded(unsigned stream,std::uint32_t maximum)=0;
    virtual sprite::AnimationFile& effect_file(game_session::Context&)=0; //41cad0/45d100
    virtual std::uint32_t spawn(sprite::AnimationFile&,const char*,int,const sprite::Vec3&,float)=0; //4790e0
    virtual damage::Region* create_circle(game_session::Context&,const sprite::Vec3&,float,float,int,int)=0; //4c1b80 ->current4c0d00
    virtual void remember_effect(game_session::Context&,std::uint32_t)=0; //497d20
};
ShotHitServices& shot_hit_services();
int hit_shot_callback(Shot&,unsigned,const sprite::Vec3&,const sprite::Vec2*,float,float,ShotHitServices&);
}
