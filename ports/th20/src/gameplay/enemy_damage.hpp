#pragma once
#include "enemy_damage_helpers.hpp"
namespace th20::source::gameplay {
// Required source services at original object boundaries. Test environments
// record calls; production binds the actual shared owners, never dummy objects.
class EnemyDamageServices {
public:
    virtual ~EnemyDamageServices()=default;
    virtual const float* timer_rate()=0;
    virtual game_session::Session& session()=0;
    virtual void* boss_hud()=0;
    virtual sprite::Animation* animation(std::uint32_t)=0;
    virtual void replace_animation(std::uint32_t&,std::int32_t)=0; //44bcd0
    virtual int calculate_damage(game_session::Context&,const sprite::Vec3&,const sprite::Vec2*,float,float,std::uint32_t*,sprite::Vec3*,int,std::uint32_t)=0; //4c0480
    virtual int defeat(void*)=0;                              //4a5640
    virtual void clear_scripts(void*)=0;                      //4973c0,4972c0 in that order
    virtual void select_script(void*,const char*)=0;           //540200
    virtual int run_scripts(void*,float)=0;                   //53e2b0
    virtual void notify_timeout(void* secondary_owner)=0;     //487bb0
    virtual void sound(std::int32_t,float)=0;                  //426eb0
    virtual int player_circle(void*,const sprite::Vec3&,float,int)=0; //4f8ff0
    virtual int player_rectangle(void*,const sprite::Vec3&,float,float,float,int)=0; //4f91d0
    virtual void player_graze(void*,const sprite::Vec3&,int)=0; //4f8b90
};
std::int32_t enemy_phase_stage(game_session::Session&) noexcept; //499330->4994d0
std::int32_t enemy_damage_bonus(game_session::Player&) noexcept; //4aaf50
void add_phase_reward(game_session::Player&,std::int32_t) noexcept; //497c60
const char* enemy_life_phase(EnemyState&,EnemyDamageServices&);    //4aa250
const char* enemy_time_phase(EnemyState&,EnemyDamageServices&);    //4aa400
int update_enemy_damage(EnemyState&,EnemyDamageServices&);       //4a5df0
EnemyDamageServices& enemy_damage_services();
namespace unrecovered {
int defeat_enemy_004a5640(void*);
void clear_enemy_async_004973c0(void*);
void reset_enemy_script_004972c0(void*);
void select_enemy_script_00540200(void*,const char*);
int player_circle_004f8ff0(void*,const sprite::Vec3&,float,int);
int player_rectangle_004f91d0(void*,const sprite::Vec3&,float,float,float,int);
void player_graze_004f8b90(void*,const sprite::Vec3&,int);
}
}
