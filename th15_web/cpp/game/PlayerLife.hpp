#pragma once
#include "PlayerAnmHost.hpp"
#include "EffectManager.hpp"
#include "SpellStatus.hpp"
#include "PracticeConfig.hpp"
#include <functional>
namespace th15 {
struct PlayerLifeSession {i32 extra_lives=2,life_pieces=0,deaths=0,enemy_deaths=0,enemy_chain=0,bomb_state=0;u32 mode_flags=0;i32 power=100,power_step=100,bombs=3,bomb_pieces=0,replay_state=0;};
struct PlayerLifeServices {
    virtual ~PlayerLifeServices()=default;
    virtual bool allow_bomb()=0;virtual bool begin_bomb()=0;
    virtual bool cancel_lasers_near(const Vec3&,float,i32,bool)=0;
    virtual bool cancel_bullets_near(const Vec3&,float,i32)=0;
    virtual bool cancel_lasers(i32,bool)=0;
    virtual bool item(i32,const Vec3&,float,float)=0;virtual bool options_changed()=0;
    virtual bool game_over()=0;virtual bool respawn_damage(const Vec3&,float,float,i32,i32)=0;
    virtual bool bomb_hud(i32,i32)=0;
};
struct PlayerLifecycleHost:PlayerLifeServices {virtual bool move()=0;};
class PlayerLife {
    PlayerMotion& motion;PlayerAnmHost& visuals;EffectManager& effects;PlayerLifeSession& session;PlayerSpellStatus& spell;
    void invalidate_spell()noexcept;
public:
    PracticeState* practice=nullptr;
    bool cheat(u32 bit)const noexcept{return practice&&practice->cheat(bit);}
    i32 state=0;Timer age,invulnerability;Vec3 last_death_position{};std::string error;
    std::function<bool(i32)> sound;std::function<bool(i32,i32)> life_hud;
    PlayerLife(PlayerMotion& p,PlayerAnmHost& v,EffectManager& e,PlayerLifeSession& s,PlayerSpellStatus& b):motion(p),visuals(v),effects(e),session(s),spell(b){}
    bool hit();bool commit_death();void cancel_death()noexcept{age.set(60);state=1;}
    bool advance_state(u32 pressed,float& rate,bool hud_available,PlayerLifecycleHost&);
    PlayerLifeSession& session_state()noexcept{return session;}
};
}
