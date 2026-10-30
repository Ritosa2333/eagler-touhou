#pragma once
#include "../../cpp/game/EnemyCombat.hpp"
API(enemy_collision_query) void enemy_collision_query(EnemyVariableFixture* p,u32 preview,u32* out){const auto q=enemy_shot_query(p->enemy,preview);out[0]=float_to_bits(q.position.x);out[1]=float_to_bits(q.position.y);out[2]=float_to_bits(q.position.z);out[3]=q.rectangle;out[4]=float_to_bits(q.size.x);out[5]=float_to_bits(q.size.y);out[6]=float_to_bits(q.angle);out[7]=float_to_bits(q.radius);out[8]=q.preview;out[9]=u32(q.target);}
API(enemy_collision_contact) void enemy_collision_contact(EnemyVariableFixture* p,u32 present,u32* out){const auto shape=enemy_contact_shape(p->enemy,present?p->visuals->find(p->enemy.animation_handles[0]):nullptr);out[0]=float_to_bits(shape.position.x);out[1]=float_to_bits(shape.position.y);out[2]=float_to_bits(shape.position.z);out[3]=shape.rectangle;out[4]=float_to_bits(shape.angle);out[5]=float_to_bits(shape.length);out[6]=float_to_bits(shape.width);out[7]=float_to_bits(shape.radius);}

struct EnemyCombatFixture final:EnemyCombatServices,PlayerDamageHost {
    DamageSources damage;PlayerCollision collision;Timer player_timer;EnemyCombat combat;
    i32 bomb=0,bomb_calls=0,hits=0;bool available=true;
    EnemyCombatFixture():combat(damage,collision,*this,player_timer,*this){}
    void hit()override{hits++;}
    bool bomb_damage(const DamageQuery&,i32& out)override{bomb_calls++;out=bomb;return available;}
    bool additional_damage(EnemyState&,i32,i32& out)override{out=0;return false;}
    int enemy_death(EnemyRuntime&,float)override{return -2;}
    bool contact_override(EnemyState&,AnmVm*,i32&,bool& handled)override{handled=false;return true;}
    bool graze(const Vec3&)override{return false;}bool sound(i32,const Vec3&)override{return false;}
};
API(enemy_combat_create) EnemyCombatFixture* enemy_combat_create(){return new EnemyCombatFixture;}
API(enemy_combat_delete) void enemy_combat_delete(EnemyCombatFixture* p){delete p;}
API(enemy_combat_damage) DamageSources* enemy_combat_damage(EnemyCombatFixture* p){return &p->damage;}
API(enemy_combat_timer) Timer* enemy_combat_timer(EnemyCombatFixture* p){return &p->player_timer;}
API(enemy_combat_configure) void enemy_combat_configure(EnemyCombatFixture* p,i32 bomb,u32 available){p->bomb=bomb;p->available=available;p->bomb_calls=0;}
API(enemy_combat_bomb_calls) i32 enemy_combat_bomb_calls(EnemyCombatFixture* p){return p->bomb_calls;}
API(enemy_combat_shot) u32 enemy_combat_shot(EnemyCombatFixture* p,EnemyVariableFixture* e,u32 special,u32* out){EnemyShotDamage hit;hit.position=e->enemy.last_hit_position;if(!p->combat.shot_damage(e->enemy,special,hit))return 0;out[0]=u32(hit.amount);out[1]=hit.direct;out[2]=float_to_bits(hit.position.x);out[3]=float_to_bits(hit.position.y);out[4]=float_to_bits(hit.position.z);return 1;}

