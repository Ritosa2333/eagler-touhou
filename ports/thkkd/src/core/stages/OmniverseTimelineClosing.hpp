#pragma once
#include "core/stages/OmniverseTimelineEngine.hpp"
namespace kaikeidou::omniverse_timeline_detail {
inline void Engine::closing(){
 if(t().subphase==2){
  if(frame()==1){if(s.phase.result==0)t().subphase=3;s.allocation.field_a7ea5c=255;s.protection.boss_state_a7ea60=0;
   if(enemy().word(4)-10u==7&&character()==1){enemy_at(integer(0)).word(8,0);enemy_at(integer(0)).word(40,1);enemy_at(integer(0)).word(44,0);}}
  else if(frame()==32){if(enemy().word(4)-10u==7){if(character()==0)effect_at(1,{{0,0,0}},constant(0x6d1a60),constant(0x6d1a90));else if(character()==1&&enemy_at(integer(0)).word(0)==1){enemy_at(integer(0)).word(20,0);enemy_at(integer(0)).word(28,0);enemy_at(integer(0)).word(0,0);const auto y=enemy_at(integer(0)).quad(64),x=enemy_at(integer(0)).quad(56);effect_at(1,{{0,0,0}},x,y);}}services.retire_boss(0);}
  else if(frame()==256)s.runtime.ending.word(32,1);
  else if(frame()==511){services.pause_background();services.stop_music();s.runtime.outer_scene=10;services.return_to_title(fire.replay_variant);services.initialize_title_visuals();}
  return;
 }
 if(t().subphase!=3)return;
 if(frame()==32){enemy().word(8,17);
  if(lt(m.c(0x6d2c60),q(64))&&gt(m.c(0x6d2c10),q(64))&&lt(m.c(0x6d2c58),q(56))&&gt(m.c(0x6d2c08),q(56))){enemy().quad(168,m.store(m.i(lt(m.q(s.player.quad(144)),q(56))?544u:0u-160u)));const auto base=m.store(m.sub(q(64),m.c(0x6d1aa8)));const auto value=rnd(0x6d1f20);put(176,m.add(m.mul(value,m.c(0x6d1960)),m.q(base)));}
  else if(ge(m.c(0x6d1c58),q(56))){enemy().quad(168,constant(0x6d1dd0));const bool subtract=ge(m.c(0x6d2398),q(64));const auto value=rnd(0x6d2028);put(176,subtract?m.sub(q(64),m.mul(value,m.c(0x6d1960))):m.add(m.mul(value,m.c(0x6d1960)),q(64)));}
  else if(lt(m.c(0x6d1dc8),q(56))){enemy().quad(168,constant(0x6d1dc0));const bool subtract=ge(m.c(0x6d2398),q(64));const auto value=rnd(0x6d2028);put(176,subtract?m.sub(q(64),m.mul(value,m.c(0x6d1960))):m.add(m.mul(value,m.c(0x6d1960)),q(64)));}
  else{const auto base=m.store(m.sub(q(56),m.c(0x6d1aa8)));const auto value=rnd(0x6d1f20);put(168,m.add(m.mul(value,m.c(0x6d1960)),m.q(base)));enemy().quad(176,gt(m.c(0x6d2398),q(64))?constant(0x6d1dd0):constant(0x6d2c00));}
  auto value=rnd(0x6b25b8);put(184,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6b25f8)));value=rnd(0x6b25b8);put(192,m.add(m.mul(value,m.c(0x6d1960)),m.c(0x6b25f8)));
 }else if(frame()==140)services.retire_boss(2);else if(frame()==170){s.lifecycle.scene=6;s.boss.enemy_prefix.word(0x58,0);s.boss.enemy_prefix.word(0x60,0);}
}
}
