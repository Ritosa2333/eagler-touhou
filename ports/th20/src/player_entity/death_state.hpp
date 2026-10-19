#pragma once
#include "movement.hpp"
#include "shot_hit.hpp"
namespace th20::source::player_entity {
bool subtract_power(game_session::Player&,std::int32_t); //4e1680, retains one power unit
void add_death_count(game_session::Player&,std::int32_t) noexcept; //4fe950 +c8
void add_stone_level(game_session::Player&,unsigned slot,std::int32_t) noexcept; //4fea60/4fe9b0/4feab0/4fea00 +74..80
void add_total_deaths(game_session::PlayerTable&,std::int32_t) noexcept; //4feb00 +208
void cancel_death(Player&) noexcept; //4f86c0
std::uint32_t spawn_tracked_effect(effects::Controller&,int file,int script,const sprite::Vec3&,float angle,ShotHitServices&); //4fb820
class DeathServices {
public:
    virtual ~DeathServices()=default;
    virtual MovementServices& movement()=0;
    virtual void mark_enemies()=0; //478190 global0
    virtual void spawn_effect(Player&)=0; //4fb820 EffectInf file0 script21
    virtual void add_lives(game_session::Player&,int)=0; //4e1250
    virtual void add_meter(game_session::Player&,int)=0; //477f60
    virtual void notify_card(game_session::Context&)=0; //487bb0 Context+10
};
DeathServices& death_services();
void begin_death(Player&,DeathServices&); //4f8420
void begin_death(Player&);
}
