#pragma once
#include "death_state.hpp"
#include "shot_controller_frame.hpp"
namespace th20::source::player_entity {
class FrameServices {
public:
    virtual ~FrameServices()=default;
    virtual MovementServices& movement()=0;
    virtual DeathServices& death()=0;
    virtual ShotControllerServices& shots()=0;
    virtual void select_view(int)=0;
    virtual std::uint32_t pressed(int slot,std::uint32_t mask)=0; //4b58e0 +2a8
    virtual bool bomb_exists(game_session::Context&)=0;
    virtual bool can_bomb(game_session::Context&)=0;
    virtual void trigger_bomb(game_session::Context&)=0;
    virtual void cancel_bullets(game_session::Context&,const sprite::Vec3&,float)=0; //47cf60 drop0 limit99999 kind0
    virtual void cancel_near_bullets(game_session::Context&,const sprite::Vec3&,float)=0; //47d100 drop0
    virtual void cancel_lasers(game_session::Context&,const sprite::Vec3&,float,int kind)=0; //4caad0 drop0
    virtual void finish_lasers(game_session::Context&,int,int)=0; //4c9490 virtual44
    virtual void create_damage(game_session::Context&,const sprite::Vec3&,float,float,int,int,bool activate)=0;
    virtual void spawn_power_item(const sprite::Vec3&,float angle)=0; //global0 Item4c3c90 type1
    virtual bool replay_playing()=0; //488800 mode1
    virtual void finish_game()=0; //4e5bd0
    virtual void set_bombs(game_session::Player&,int)=0; //4e15e0 +HUD
    virtual void clock_scale(float)=0;
    virtual void execute_animation(sprite::Animation&)=0;
    virtual float sample_expansion(sprite::Interpolation<float>&)=0; //42a110
};
FrameServices& frame_services();
int update_player(Player&,FrameServices&); //4f7430 entire state dispatch
int update_player(Player&);
}
