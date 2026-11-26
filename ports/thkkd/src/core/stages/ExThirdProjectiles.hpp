// 原普通弹113..116；116保留原生成敌135和同帧普通发射顺序。
#pragma once
#include "core/stages/ExThirdArithmetic.hpp"
#include "core/stages/ExThirdRecords.hpp"
#include "core/stages/ExThirdEmitter.hpp"
#include "core/stages/StagePatternContext.hpp"
namespace kaikeidou {
inline void update_ex_third_projectile(ProjectilePools& pool,std::uint32_t index,std::uint32_t behavior,EnemyPool& enemies,BossStageContext& context,const ObjectRecord<240>& player,PatternParameters& p,const StagePatternContext& extra,Random& random,RandomPrecision precision,GameMath& math){
 using namespace ex_third_detail;const Arithmetic m{static_cast<unsigned>(precision)};PoolRecord b{pool,0xa8e660u+index*240u};const auto q=[&](unsigned o){return m.q(b.quad(o));};const auto put=[&](unsigned o,binary_float::Number n){b.quad(o,m.store(n));};const auto val=[&](unsigned o){return crt_sse2::real(b.quad(o));};const auto c=[](std::uint32_t a){return crt_sse2::real(constant(a));};
 const auto outside=[&](){return val(64)<c(0x6d19f0)||val(64)>c(0x6d19e8)||val(72)>c(0x6d19f8)||val(72)<c(0x6d19f0);};
 if(behavior==113){if(signed_word(b.word(12))<=60){put(64,m.add(q(64),q(160)));put(72,m.add(q(72),q(168)));put(160,m.mul(q(160),m.c(0x6d1a20)));put(168,m.mul(q(168),m.c(0x6d1a20)));}else{put(72,m.add(q(72),m.c(0x6d1b18)));if(val(72)>c(0x6d1a48))b.word(0,0);}return;}
 if(behavior==114){if(signed_word(b.word(12))>=120){put(64,m.add(q(64),q(160)));put(72,m.add(q(72),q(168)));if(outside())b.word(0,0);}return;}
 if(behavior==115){put(64,m.add(m.mul(m.sub(m.q(0x3ff0000000000000ull),q(160)),q(80)),q(64)));put(72,m.add(m.mul(m.sub(m.q(0x3ff0000000000000ull),q(160)),q(88)),q(72)));put(160,m.mul(q(160),m.c(0x6d1a20)));if(b.word(12)==50){const auto dx=m.store(m.sub(m.q(player.quad(144)),m.q(enemy_record(enemies,context,p.integers[0]).quad(56))));const auto dy=m.store(m.sub(m.q(player.quad(152)),m.q(enemy_record(enemies,context,p.integers[0]).quad(64))));const auto angle=m.store(math.atan2(dy,dx));auto v=math.cosine(angle);put(80,m.mul(v,m.c(0x6d1910)));v=math.sine(angle);put(88,m.mul(v,m.c(0x6d1910)));b.quad(96,angle);b.quad(160,0x3ff0000000000000ull);ex_third_recolor(pool,index,255,0,0);}if(outside())b.word(0,0);return;}
 if(behavior!=116)throw std::out_of_range("EX third projectile outside113..116");
 if(!enemy_record(enemies,context,p.integers[0]).word(0)){ex_third_remove100(pool,index);return;}
 if(signed_word(b.word(12))<=59){auto v=math.cosine(b.quad(192));put(64,m.add(m.mul(m.sub(m.c(0x6d1ab0),q(176)),v),m.c(0x6d1a60)));v=math.sine(b.quad(192));put(72,m.add(m.mul(m.sub(m.c(0x6d1ab0),q(176)),v),q(184)));put(176,m.mul(q(176),m.c(0x6d1980)));return;}
 auto v=math.cosine(b.quad(192));put(64,m.add(m.mul(v,m.c(0x6d1ab0)),m.c(0x6d1a60)));v=math.sine(b.quad(192));put(72,m.add(m.mul(v,m.c(0x6d1ab0)),q(184)));put(192,m.add(q(192),q(200)));put(200,m.sub(q(200),q(208)));put(208,m.mul(q(208),m.c(0x6d18f0)));
 if(signed_word(b.word(12))>=180&&remainder(b.word(12),20)==0){const auto dx=m.store(m.sub(m.q(player.quad(144)),q(64))),dy=m.store(m.sub(m.q(player.quad(152)),q(72)));const auto angle=m.store(math.atan2(dy,dx));const auto slot=allocate_enemy(enemies);if(slot!=-1){auto& e=enemies.enemies[slot];e.word(4,b.word(120));e.word(24,extra.boss_hp_multiplier*100u);e.word(20,e.word(24));e.word(28,1);e.word(8,135);e.quad(56,b.quad(64));e.quad(64,b.quad(72));v=math.cosine(angle);e.quad(168,m.store(m.mul(v,m.c(0x6d1c90))));v=math.sine(angle);e.quad(176,m.store(m.mul(v,m.c(0x6d1c90))));e.quad(184,0x3ff0000000000000ull);}
  const auto y=b.quad(72),x=b.quad(64);const auto blue=b.word(132),green=b.word(128),red=b.word(124);ex_third_emit(pool,{{12,0,{{red,green,blue}},0,255,1,41},x,y,2,constant(0x6d1860),constant(0x6d1ce0),0,1,0},player,p,random,precision,math);
 }
}
}
