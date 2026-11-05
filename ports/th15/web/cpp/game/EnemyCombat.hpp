#pragma once
#include "EnemyDamage.hpp"
#include "EnemyCollision.hpp"
#include "Player.hpp"
namespace th15 {
struct EnemyCombatServices {
    virtual ~EnemyCombatServices()=default;
    virtual bool bomb_damage(const DamageQuery&,i32& amount)=0;
    virtual bool additional_damage(EnemyState&,i32 incoming,i32& amount)=0;
    virtual int enemy_death(EnemyRuntime&,float rate)=0;
    virtual bool contact_override(EnemyState&,AnmVm*,i32& result,bool& handled)=0;
    virtual bool graze(const Vec3&)=0;
    virtual bool sound(i32,const Vec3&)=0;
};
// The original enemy update queries the player damage pool before testing
// contact. Concrete shot callbacks remain owned by PlayerShots.
class EnemyCombat final:public EnemyDamageHost {
    DamageSources& damage;const PlayerCollision& collision;PlayerDamageHost& player;Timer& player_frame;EnemyCombatServices& world;
public:
    EnemyCombat(DamageSources& d,const PlayerCollision& c,PlayerDamageHost& p,Timer& timer,EnemyCombatServices& w):damage(d),collision(c),player(p),player_frame(timer),world(w){}
    EnemyCombat(Player& p,EnemyCombatServices& w):EnemyCombat(p.damage,p.frame.collision,p,p.frame.input_age,w){}
    bool shot_damage(EnemyState&,bool special,EnemyShotDamage&)override;
    bool animation_region_damage(EnemyState&,const AnmVm*,i32 incoming,i32& amount);
    bool additional_damage(EnemyState& e,i32 incoming,i32& amount)override{return world.additional_damage(e,incoming,amount);}
    int die(EnemyRuntime& e,float rate)override{return world.enemy_death(e,rate);}
    bool contact(EnemyState&,AnmVm*,i32& result)override;
    bool graze(const Vec3& p)override{return world.graze(p);}
    bool sound(i32 id,const Vec3& p)override{return world.sound(id,p);}
};
}
