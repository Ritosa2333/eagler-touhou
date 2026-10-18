#pragma once
#include "firing.hpp"
namespace th20::source::player_entity {
class ShotCallbackEnvironment {
public:
    virtual ~ShotCallbackEnvironment()=default;
    virtual FiringServices& firing()=0;
    virtual scheduler::List& enemies(game_session::Context&)=0;
    virtual bool excluded_enemy(const void*)=0;
    virtual sprite::Vec3 enemy_position(const void*)=0;
    virtual std::uint32_t nearest_enemy(game_session::Context&,const sprite::Vec2&,float)=0;
    virtual void* find_enemy(std::uint32_t&)=0; //4aaac0 resolves against global player0
    virtual float random_angle()=0;           //504a60, preserves its unusual denominator
    virtual sprite::Animation* find_animation(std::uint32_t)=0; //44cd00 no caller-handle mutation
    virtual void interrupt(std::uint32_t,int)=0; //44ef90
    virtual void sound_pan(int,float)=0; //429090
    virtual void stop_sound(int)=0; //428890
    virtual void retire_damage(damage::Region&)=0; //4c1f30
    virtual bool dialogue_active()=0; //nullable5c06a4 ->478160
    virtual int weapon_pattern()=0; //global overlay534210
    virtual int weapon_phase()=0; //global overlay4ff6d0
    virtual bool weapon_shooting()=0; //global overlay506950
    virtual sprite::Animation* animation_child(std::uint32_t&,int,int)=0; //44c670
    virtual void cancel_rectangles(game_session::Context&,const sprite::Vec3&,const sprite::Vec3&,float)=0; //47cc90/4c9db0
};
ShotCallbackEnvironment& shot_callback_environment();
float shot_random_angle(std::uint32_t value,std::uint32_t modulus) noexcept; //504a60 arithmetic after423ee0
bool pattern_has_laser(const void* shot_data,std::uint32_t option,int pattern); //5068a0
int initialize_shot_callback(Shot&,unsigned index,int frame,ShotCallbackEnvironment&);
int update_shot_callback(Shot&,unsigned index,ShotCallbackEnvironment&);
std::uintptr_t shot_initialization_callback(unsigned);
namespace unrecovered {
std::uintptr_t shot_update_callback(unsigned);
std::uintptr_t shot_extra_callback(unsigned);
std::uintptr_t shot_hit_callback(unsigned);
}
}
