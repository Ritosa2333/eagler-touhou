#include "GameWorld.hpp"
#include "PlayerDamage.hpp"
#include "WorldCheckpoint.hpp"
#include "ReplayInput.hpp"
#include <cassert>
#include <iostream>
int main(){
 // Rectangular cuts after the early laser pass register child contacts next visit;
 // ECL circle cuts retain their same-tick contact registration.
 for(int phase:{0,1,2}){
  const bool rectangle=phase!=0,beforeWorldTick=phase==1;
  th19::GameWorld world;world.reward_mode=2;world.battle_frame=10;
  world.player_x=120;world.player_y=25;
  th19::InfiniteLaser laser;laser.id=1;laser.straight=true;laser.phase=2;
  laser.length=laser.straight_target=160;laser.width=8;laser.age=5;laser.motion_frame=beforeWorldTick?11:10;
  world.next_id=2;world.lasers.push_back(laser);
  auto cut=[](float x,float){return x>=48&&x<80;};
  if(rectangle)world.cancel_lasers_region(cut,true,[](float x,float,float){return x>=48&&x<80;});
  else world.cancel_lasers_region(cut);
  assert(world.lasers.size()==2);
  world.battle_frame=11;world.finish_laser_graze(true,3);
  assert(world.lasers[1].graze_clock==(beforeWorldTick?0u:1u));
  if(beforeWorldTick){world.battle_frame=12;world.finish_laser_graze(true,3);assert(world.lasers[1].graze_clock==1);}

 }
 // Two clears may touch the same retiring controller before its next visit.
 {th19::GameWorld world;world.reward_mode=2;world.battle_frame=10;
  th19::InfiniteLaser laser;laser.id=1;laser.straight=true;laser.phase=2;
  laser.length=laser.straight_target=160;laser.width=8;laser.age=5;
  world.next_id=2;world.lasers.push_back(laser);
  world.cancel_lasers_region([](float,float){return true;});
  assert(world.lasers[0].cancel_pending);
  world.cancel_lasers_region([](float x,float){return x>=64&&x<96;});
  assert(world.lasers.size()==2);
  assert(world.lasers[0].cancel_pending&&!world.lasers[1].cancel_pending);
  world.player_x=120;world.player_y=25;world.finish_laser_graze(true,3);
  assert(world.lasers[1].graze_clock==1);
 }
 {th19::PlayerDamage damage;damage.state=th19::PlayerDamage::Pending;damage.frame=2;
  damage.rescue();assert(damage.controllable()&&damage.frame==60);
  int lives=3,invulnerable=40;assert(damage.tick(lives,invulnerable)==th19::PlayerDamage::None);
  assert(lives==3&&damage.state==th19::PlayerDamage::Playing);
 }

 // A newly registered fixed laser is cleared before its first growth visit.
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.id=1;laser.source.type=38;
  laser.source.max_length=512;laser.source.speed=8;laser.source.warning=30;
  laser.length=512;laser.clock=-1;world.lasers.push_back(laser);
  world.tick_wave(false,{},true);assert(world.lasers[0].age==0);
  world.cancel_lasers_circle(0,0,640);
  assert(world.lasers[0].length==0);
  world.finish_new_lasers();assert(world.lasers[0].age==1&&world.lasers[0].length==8);
 }
 // Bounds retire a contact-canceled slot before its pending ANM dispatch.
 // Its replacement retains the unconsumed interrupt and spawn-release value.
 {th19::GameWorld world;world.reward_mode=2;
  std::unordered_map<th19::i32,std::array<float,2>> sizes{{2,{8,8}}};
  world.bullet_sprite_sizes=&sizes;
  auto& old=world.spawn_bullet();old.sprite=2;old.kind=1;old.color=2;
  old.y=451.872894f;old.vy=4.83542109f;old.native_timer=0;
  old.state=th19::Bullet::Canceling;old.contact_cancel=true;
  old.contact_interrupt_pending=true;old.animation.ivar[0]=1;
  const auto slot=old.native_slot;
  world.advance_vs_projectiles();assert(world.bullets[0].state==4);
  auto saved=world;
  auto& replacement=world.spawn_bullet();
  assert(replacement.native_slot==slot&&replacement.contact_interrupt_pending);
  assert(replacement.animation.ivar[0]==1&&replacement.motion_advanced);
  world=saved;auto& replayed=world.spawn_bullet();
  assert(replayed.native_slot==slot&&replayed.contact_interrupt_pending&&replayed.animation.ivar[0]==1);
  world.clear_bullets();auto& fresh=world.spawn_bullet();
  assert(!fresh.contact_interrupt_pending&&fresh.animation.ivar[0]==0);
 }
 th19::ReplayInput replay;bool physical[512]{},restored[512]{};th19::u8 injected[512]{};
 physical[29]=true;replay.begin();for(int i=0;i<120;i++)replay.capture(physical,injected,0,0,false);
 assert(replay.frames==120&&replay.records.size()==1&&replay.records[0].ticks==120);
 replay.transfer.resize(80);std::memcpy(replay.transfer.data(),replay.records.data(),80);assert(replay.load(80));
 float x=0,y=0;bool direct=false;for(int i=0;i<120;i++){assert(replay.next(restored,injected,x,y,direct));assert(restored[29]);}
 assert(!replay.next(restored,injected,x,y,direct));
 replay.transfer[76]=2;assert(!replay.load(80));
 th19::GameWorld w;th19::LaserShooter shot;shot.type=0;shot.count1=64;shot.count2=64;
 shot.speed=shot.speed2=0;
 assert(w.fire_bullets(shot,0,100));
 assert(w.bullets.size()==th19::GameWorld::bullet_capacity);
 assert(w.dropped_bullets==4096-th19::GameWorld::bullet_capacity);
 for(int i=0;i<30;++i)assert(w.fire_bullets(shot,0,100));
 assert(w.bullets.size()==th19::GameWorld::bullet_capacity);
 for(auto&b:w.bullets)b.state=4;
 w.tick_wave();assert(w.bullets.empty());
 assert(w.fire_bullets(shot,0,100));assert(w.bullets.size()==th19::GameWorld::bullet_capacity);
 for(int wave=0;wave<30;++wave){
  for(int i=0;i<500;++i){auto&e=w.spawn_enemy("",0,0,1,0,0);e.active=false;}
  w.tick_wave();assert(w.enemies.empty());
 }
 th19::GameWorld right;th19::EclVm leftVm,rightVm;
 th19::AnmVm::restore_random(123);th19::AnmVm::restore_random(456,true);
 th19::WorldCheckpoint snapshot(w,leftVm,right,rightVm);
 th19::AnmVm draw;draw.read_var_f(10011);draw.read_var_f(10031);
 w.bullets[0].x=90;snapshot.restore(w,leftVm,right,rightVm);
 assert(th19::AnmVm::random_checkpoint()==123&&th19::AnmVm::random_checkpoint(true)==456);
 th19::AnmVm::restore_random(789);th19::AnmVm::restore_random(987,true);
 snapshot.capture(w,leftVm,right,rightVm);draw.read_var_f(10011);draw.read_var_f(10031);
 snapshot.restore(w,leftVm,right,rightVm);
 assert(th19::AnmVm::random_checkpoint()==789&&th19::AnmVm::random_checkpoint(true)==987);
 assert(w.bullets[0].x!=90);assert(w.bullets.size()==2000);
 // C2 shares the native 100-slot release queue without reserving credit.
 // Its completed timer therefore makes the signed reserved count negative.
 {
  th19::GameWorld sender,target;sender.reward_mode=2;
  for(int i=0;i<101;++i)sender.register_native_extra_release();
  assert(sender.native_extra_reservations.size()==100);
  for(int i=0;i<238;++i)sender.send_native_extra_credit(target,{},1,20000+i,false);
  assert(sender.native_extra_reserved_count==0);
  sender.send_native_extra_credit(target,{},1,20238,false);
  assert(sender.native_extra_reserved_count==-100);
  assert(sender.native_extra_reservations.empty());
 }
 // A deferred/nondeferred feedback transition must not lose a protection
 // tick. Native admits damage on the visit after the last held visit.
 for(bool feedbackBefore:{false,true}){
  th19::GameWorld protectedWorld;protectedWorld.reward_mode=2;
  auto& actor=protectedWorld.spawn_enemy("",0,100,210,0,0);
  actor.sprite=2;actor.invulnerability_time=1;actor.spawn_hold=1;
  auto contact=[](th19::Enemy&){return 40;};
  if(feedbackBefore)protectedWorld.finish_hit_feedback(0);
  protectedWorld.tick_wave(false,contact);
  if(!feedbackBefore)protectedWorld.finish_hit_feedback(0);
  assert(protectedWorld.enemies[0].life==210);
  assert(protectedWorld.enemies[0].invulnerability_time==0);
  protectedWorld.tick_wave(false,contact);
  assert(protectedWorld.enemies[0].life==170);
 }
 // Native held-spirit arrival: the registered shot is consumed even though
 // HP stays protected. A later shot is admitted when movement resumes.
 for(unsigned spiritType:{0x100000u,0x180000u,0x200000u,0x400000u}){
  th19::GameWorld arrival;arrival.reward_mode=2;
  auto& actor=arrival.spawn_enemy("",0,100,800,0,0);
  actor.sprite=2;actor.status_flags=spiritType;actor.invulnerability_time=0;
  actor.arrival_motion=th19::Enemy::ArrivalMotion::HoldScriptUpdated;
  int consumed=0;
  auto contact=[&](th19::Enemy&){++consumed;return 40;};
  arrival.tick_wave(false,contact);
  assert(consumed==1);
  assert(arrival.enemies[0].life==800);
  assert(!(arrival.enemies[0].flags&0x10u));
  arrival.tick_wave(false,contact);
  assert(consumed==2);
  assert(arrival.enemies[0].life==760);
 }
 std::cout<<"2000 bullet slots, recycling, 15000 retired enemies, checkpoint, held-arrival HP/contact, feedback order and signed EX 100-slot release: PASS\n";
}
