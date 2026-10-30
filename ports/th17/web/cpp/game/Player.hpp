#pragma once
#include "PlayerFrame.hpp"
#include "PlayerOptions.hpp"
#include "AnmRenderer.hpp"
namespace th17 {
struct PlayerHost:PlayerFrameHost {
    virtual bool sound(i32,float,PlayerShots::SoundAction)=0;
    virtual bool hit_sound(i32)=0;virtual bool life_hud(i32,i32)=0;virtual bool bomb_hud(i32,i32)=0;
};
class Player final:public PlayerDamageHost,private PlayerFrameHost {
    PlayerHost& world;AnmManager& animations;i32 character;
    bool allow_bomb()override{return world.allow_bomb();}bool begin_bomb()override{return world.begin_bomb();}
    bool end_roaring(bool death)override{return world.end_roaring(death);}
    bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{return world.cancel_lasers_near(p,r,kind,flag);}
    bool cancel_bullets_near(const Vec3& p,float r,i32 kind,bool all)override{return world.cancel_bullets_near(p,r,kind,all);}
    bool cancel_lasers(i32 kind,bool flag)override{return world.cancel_lasers(kind,flag);}
    bool item(i32 kind,const Vec3& p,float a,float s)override{return world.item(kind,p,a,s);}
    bool options_changed()override{return options.configure(session.power,session.power_step,resource.header.max_power_level*session.power_step);}
    bool game_over()override{return world.game_over();}
    bool respawn_damage(const Vec3& p,float r,float g,i32 frames,i32 value)override{return damage.circle(p,r,g,frames,value)>0;}
    bool shot_audio(i32 id,ShotSoundAction action,float x)override{return world.shot_audio(id,action,x);}
    bool refresh(PlayerFrameContext& c)override{return world.refresh(c);}
public:
    ShtResource resource;PlayerMotion motion;PlayerAnmHost visuals;EffectManager& effects;PlayerLifeSession& session;PlayerSpellStatus& spell;
    DamageSources damage;PlayerShotContext shot_context;PlayerShots shots;PlayerOptions options;PlayerLife life;PlayerFrame frame;std::string error;bool initialized=false;
    Player(const ShtResource&,AnmManager&,EffectManager&,Rng&,Rng&,PlayerLifeSession&,PlayerSpellStatus&,PlayerHost&,i32 character,i32 player_resource,i32 effect_resource);
    bool initialize();bool configure_options(){return options_changed();}bool update(const PlayerFrameContext&,float& rate);
    bool reset_for_stage();bool finish_stage_options();
    bool draw(AnmRenderer&);
    void hit()override{if(!life.hit()){error=life.error;return;}frame.collision.state=life.state;frame.collision.invulnerability=life.invulnerability.current;}
};
}
