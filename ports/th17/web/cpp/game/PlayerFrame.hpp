#pragma once
#include "PlayerLife.hpp"
#include "PlayerBounds.hpp"
#include "PlayerShooting.hpp"
#include "PlayerShots.hpp"
#include "PlayerCollision.hpp"
namespace th17 {
struct PlayerFrameContext {
    u32 held=0,pressed=0,game_flags=0;bool focus_allowed=false,enemy_present=false,hud_available=false,hud_bomb_active=false;
    Vec3 background_delta{};std::vector<PlayerShotTarget*> targets;i32 roaring_kind=0;
};
struct PlayerFrameHost:PlayerLifeServices {
    virtual bool shot_audio(i32,ShotSoundAction,float)=0;
    virtual bool refresh(PlayerFrameContext&)=0;
};
class PlayerFrame final:private PlayerLifecycleHost {
    PlayerMotion& motion;PlayerAnmHost& visuals;AnmManager& animations;PlayerLife& life;DamageSources& damage;PlayerShots& shots;PlayerShotContext& shot_context;PlayerFrameHost& world;
    i32 character,player_resource;PlayerFrameContext context;float* active_rate=nullptr;
    bool allow_bomb()override{return world.allow_bomb();}bool begin_bomb()override{return world.begin_bomb();}
    bool end_roaring(bool death)override{return world.end_roaring(death);}
    bool move()override{return motion.update(context.held,context.focus_allowed,*active_rate,visuals);}
    bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{return world.cancel_lasers_near(p,r,kind,flag);}
    bool cancel_bullets_near(const Vec3& p,float r,i32 kind,bool all)override{return world.cancel_bullets_near(p,r,kind,all);}
    bool cancel_lasers(i32 kind,bool flag)override{return world.cancel_lasers(kind,flag);}
    bool item(i32 kind,const Vec3& p,float a,float s)override{return world.item(kind,p,a,s);}
    bool options_changed()override{return world.options_changed();}bool game_over()override{return world.game_over();}
    bool respawn_damage(const Vec3& p,float r,float g,i32 frames,i32 value)override{return damage.circle(p,r,g,frames,value)>0;}
    bool visual_feedback(float rate);
public:
    Timer input_age,ready_age;PlayerShootingAnmHost shooting_host;PlayerShooting shooting;PlayerBounds bounds;PlayerCollision collision;std::string error;
    PlayerFrame(PlayerMotion&,PlayerAnmHost&,AnmManager&,PlayerLife&,DamageSources&,PlayerShots&,PlayerShotContext&,PlayerFrameHost&,i32 character,i32 resource);
    bool update(const PlayerFrameContext&,float& rate);
};
}
