#pragma once
#include "PlayerFrame.hpp"
#include "PlayerOptions.hpp"
#include "AnmRenderer.hpp"
namespace th15 {
struct PlayerHost:PlayerFrameHost {virtual bool sound(i32,float,PlayerShots::SoundAction)=0;virtual bool hit_sound(i32)=0;virtual bool life_hud(i32,i32)=0;};
class Player final:public PlayerDamageHost,private PlayerFrameHost {
    PlayerHost& world;AnmManager& animations;i32 character;
    bool allow_bomb()override{return world.allow_bomb();}bool begin_bomb()override{return world.begin_bomb();}
    bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{return world.cancel_lasers_near(p,r,kind,flag);}
    bool cancel_bullets_near(const Vec3& p,float r,i32 kind)override{return world.cancel_bullets_near(p,r,kind);}
    bool cancel_lasers(i32 kind,bool flag)override{return world.cancel_lasers(kind,flag);}
    bool item(i32 kind,const Vec3& p,float a,float s)override{return world.item(kind,p,a,s);}
    bool options_changed()override{const bool result=options.configure(session.power,session.power_step,resource.header.max_power_level*session.power_step);shot_context.laser_power[0]=motion.option_count;return result;}
    bool game_over()override{return world.game_over();}
    bool respawn_damage(const Vec3& p,float r,float g,i32 frames,i32 value)override{return damage.circle(p,r,g,frames,value)>0;}
    bool bomb_hud(i32 value,i32 pieces)override{return world.bomb_hud(value,pieces);}
    bool stop_sound(i32 id)override{return world.stop_sound(id);}bool refresh(PlayerFrameContext& c)override{return world.refresh(c);}
public:
    ShtResource resource;PlayerMotion motion;PlayerAnmHost visuals;EffectManager& effects;PlayerLifeSession& session;PlayerSpellStatus& spell;DamageSources damage;PlayerShotContext shot_context;PlayerShots shots;PlayerOptions options;PlayerLife life;PlayerFrame frame;std::string error;bool initialized=false;
    Player(const ShtResource&,AnmManager&,EffectManager&,Rng& game_random,Rng& visual_random,PlayerLifeSession&,PlayerSpellStatus&,PlayerHost&,i32 character,i32 player_resource,i32 effect_resource);
    bool initialize();bool configure_options(){return options_changed();}bool update(const PlayerFrameContext&,float& rate);
    bool reset_for_stage();
    bool draw(AnmRenderer&);
    bool finish_stage_options();
    void hit()override{
        if(!life.hit()){error=life.error;return;}
        // Later projectiles in this callback observe the new deathbomb state.
        frame.collision.state=life.state;frame.collision.invulnerability=life.invulnerability.current;
    }
};
}
