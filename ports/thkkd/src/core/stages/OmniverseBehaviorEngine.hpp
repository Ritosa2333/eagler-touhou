#pragma once
#include "core/stages/OmniverseBehaviorArithmetic.hpp"
#include "core/stages/OmniverseBehaviorRecords.hpp"
#include "core/stages/OmniverseBehaviorEmitter.hpp"
#include "core/stages/Stage6BossTimelineColoredEffect.hpp"
#include "core/shared/CircleCollision.hpp"
namespace kaikeidou::omniverse_behavior_detail {
struct Engine {
 EnemyPool& enemies;ProjectilePools& pool;OmniverseBehaviorContext& s;Random& random;RandomPrecision precision;GameMath& math;OmniverseBehaviorServices& services;std::uint32_t index,behavior;bool enemy_kind;Arithmetic m;
 Record enemy(std::uint32_t i){return {enemies,pool,s,0xa87fe8u+i*248u,i};}Record bullet(std::uint32_t i){return {enemies,pool,s,0xa8e660u+i*240u,i};}Record effect(std::uint32_t i){return {enemies,pool,s,0xa735c0u+i*112u,i};}
 Record object(){return {enemies,pool,s,enemy_kind?0xa87fe8u+index*248u:behavior==208?0xa8ce60u+index*192u:0xa8e660u+index*240u,index};}
 Number q(unsigned o){return m.q(object().quad(o));}void put(unsigned o,Number v){object().quad(o,m.store(v));}
 std::uint32_t age(){return object().word(enemy_kind?12u:behavior==208?8u:12u);}std::int64_t signed_age(){return signed_word(age());}
 PatternParameters& p(){return s.parameters;}std::uint32_t frame(){return p().game_frame;}
 Number sine(std::uint64_t angle){return omniverse_behavior_sine(math,angle);}Number cosine(std::uint64_t angle){return omniverse_behavior_cosine(math,angle);}
 Number atan2(std::uint64_t y,std::uint64_t x){return omniverse_behavior_atan2(math,y,x);}
 Number rnd(std::uint32_t constant_address){return random.real_number(constant(constant_address),precision);}std::uint32_t random_integer(std::uint32_t max){return random.integer_bits(max,precision,true);}
 std::int32_t allocate(std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t blend=0,std::uint32_t delayed=0){return allocate_projectile(pool,{kind,group,color,blend,255,delayed,41});}
 void emit(const ProjectileEmission& emission){omniverse_behavior_emit(pool,emission,s.player,p(),random,precision,math);}
 void ease(unsigned offset,std::uint64_t target,std::uint64_t divisor){omniverse_behavior_ease_object(object(),offset,target,divisor,m.precision);}
 void move(unsigned vx=80,unsigned vy=88,bool scaled=false){put(64,m.add(scaled?m.mul(q(vx),q(104)):q(vx),q(64)));put(72,m.add(scaled?m.mul(q(vy),q(104)):q(vy),q(72)));}
 void clip(std::uint32_t left,std::uint32_t right,std::uint32_t top,std::uint32_t bottom){if(gt(q(72),m.c(bottom))||lt(q(72),m.c(top))||gt(q(64),m.c(right))||lt(q(64),m.c(left)))object().word(0,0);}
 void simple_enemy();void enemy139();void enemy141();void enemy142();void enemy143();void enemy144();void enemy146_150();
 void simple_projectile();void projectile194();void projectile195();void projectile197();void projectile199_201();void projectile207();void projectile209();void projectile212();void projectile213();void projectile214();void projectile216();void projectile219();void projectile224();void projectile225();
 void ring_initial();void ring_waves();void ring_fade();void ring_groups(std::uint32_t phase);void ring_final();
 void finish_ring_child(Record child){auto value=cosine(child.quad(176));child.quad(64,m.store(m.add(m.mul(value,m.q(child.quad(184))),m.q(child.quad(160)))));value=sine(child.quad(176));child.quad(72,m.store(m.add(m.mul(value,m.q(child.quad(184))),m.q(child.quad(168)))));child.word(8,222);child.word(156,0);}
 void ring_effect(std::uint32_t channel,std::uint64_t x,std::uint64_t y){const auto color=services.color(channel,channel,channel);allocate_effect(s.effects,pool.sound_triggers,{10,color,x,y});}
};
}
