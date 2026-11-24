#include "GameWorld.hpp"
#include "ChargeAttack.hpp"
#include "ChargeGauge.hpp"
#include "PlayerDamage.hpp"
#include "WorldCheckpoint.hpp"
#include "ReplayInput.hpp"
#include <cassert>
#include <iostream>
int main(){
 // Native second-round9601 still freezes ordinary fire at the C2 tail.
 {th19::ChargeAttack attack;assert(attack.release_skill(2));
  for(int i=0;i<70;++i)attack.tick(true);
  assert(attack.busy_for_charge(true));attack.tick(true);
  // Native clears the C2 busy flag before arming the next charge input.
  assert(attack.frame==71&&!attack.firing()&&!attack.busy_for_charge(true));
  th19::ChargeGauge gauge;gauge.tick(true,attack.busy_for_charge(true),true);
  assert(gauge.input_primed&&gauge.charge==0);attack.tick(true);assert(attack.frame==-1);
  gauge.tick(true,attack.busy_for_charge(true),true);assert(gauge.charge==10);
  assert(attack.release_skill(2));for(int i=0;i<71;++i)attack.tick(false);assert(attack.frame==-1);
 }

 // Player-contact cuts defer child contacts; bomb rectangles and ECL circles
 // register their children for same-tick contacts.
 for(int phase:{0,1,2,3}){
  const bool rectangle=phase!=0,beforeWorldTick=phase==1||phase==3,contactCut=phase==3;
  th19::GameWorld world;world.reward_mode=2;world.battle_frame=10;
  world.player_x=120;world.player_y=25;
  th19::InfiniteLaser laser;laser.id=1;laser.straight=true;laser.phase=2;
  laser.length=laser.straight_target=160;laser.width=8;laser.age=5;laser.motion_frame=beforeWorldTick?11:10;
  world.next_id=2;world.lasers.push_back(laser);
  auto cut=[](float x,float){return x>=48&&x<80;};
  if(rectangle)world.cancel_lasers_region(cut,true,[](float x,float,float){return x>=48&&x<80;},contactCut?1u:0u);
  else world.cancel_lasers_region(cut);
  assert(world.lasers.size()==2);
  world.battle_frame=11;world.finish_laser_graze(true,3);
  assert(world.lasers[1].graze_clock==(contactCut?0u:1u));
  if(contactCut){world.battle_frame=12;world.finish_laser_graze(true,3);assert(world.lasers[1].graze_clock==1);}

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
 // Respawn's forced clear bypasses a curve lock and remains effective on update.
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.id=1;laser.curved=true;laser.phase=2;laser.width=24;laser.cancel_lock=300;
  std::array<th19::BulletChange,24> changes{};
  assert(laser.curve.initialize(0,224,1.570796327f,4,32,changes));
  world.lasers.push_back(laser);
  world.cancel_lasers_circle(0,400,640,true,true);assert(world.lasers[0].active);
  world.cancel_lasers_circle(0,400,640,false,true);assert(!world.lasers[0].active);
  world.tick_wave(false,{},true);
  assert(world.lasers.empty());
 }
 // Native curve+0x85c remains300 past path clock301: Bomb must respect
 // the retained ECL lock while a forced respawn clear can bypass it.
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.curved=true;laser.phase=2;laser.width=24;laser.cancel_lock=300;
  std::array<th19::BulletChange,24> changes{};
  assert(laser.curve.initialize(0,224,0,.1f,32,changes));
  for(int i=0;i<301;++i)laser.tick();
  assert(laser.active&&laser.cancel_lock==300);
  world.lasers.push_back(laser);
  world.cancel_lasers_circle(0,224,640,true,false);assert(world.lasers[0].active);
  world.cancel_lasers_circle(0,224,640,false,true);assert(!world.lasers[0].active);
 }
 // Native12796: newborn curves remain clearable until their first manager visit.
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.curved=true;laser.phase=2;laser.width=24;
  laser.source.changes[0].flags=0x80;laser.source.changes[0].integers[0]=300;
  assert(laser.curve.initialize(117.72f,.32f,0,.1f,26,laser.source.changes));
  world.lasers.push_back(laser);
  assert(world.cancel_lasers_circle(0,128,640,true,false)==26);
  assert(!world.lasers[0].active&&world.bullet_cancels.size()==3);
  laser.tick();assert(laser.cancel_lock==300);
  for(int i=0;i<301;++i)laser.tick();assert(laser.cancel_lock==300);
 }
 // Native525270 keeps the first run in its old controller but allocates
 // later runs with lock0 and a sealed change cursor (no parent lock replay).
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.id=1;laser.curved=true;laser.phase=2;laser.width=24;
  laser.cancel_lock=300;laser.curve_changes_initialized=true;
  laser.source.changes[0].flags=0x80;laser.source.changes[0].integers[0]=300;
  laser.curve.time=30;laser.curve.segments={{0,999999,{0,224,0,1},0,0}};
  for(int i=0;i<21;++i)laser.curve.nodes.push_back(laser.curve.at(30.f-i));
  world.lasers.push_back(laser);
  world.cancel_lasers_circle(20,224,1.1f,false,false);
  assert(world.lasers.size()==2);
  assert(world.lasers[0].cancel_lock==300&&world.lasers[1].cancel_lock==0);
  world.lasers[1].tick();assert(world.lasers[1].cancel_lock==0);
  world.cancel_lasers_circle(0,224,640,true,false);
  assert(world.lasers[0].active&&!world.lasers[1].active);
 }
 // Two respawn circles visit the same retired curve before manager reclaim.
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.curved=true;laser.phase=2;laser.width=24;
  std::array<th19::BulletChange,24> changes{};
  assert(laser.curve.initialize(0,224,0,.1f,26,changes));world.lasers.push_back(laser);
  assert(world.cancel_lasers_circle(0,224,640,false,true)==26);
  assert(!world.lasers[0].active);
  assert(world.cancel_lasers_circle(0,224,640,false,true)==26);
  assert(world.bullet_cancels.size()==6);
  world.tick_wave(false,{},true);assert(world.lasers.empty());
 }
 // Pending native first updates apply the ECL lock before player clears;
 // late registrations defer that first update and remain initially clearable.
 for(bool late:{false,true}){th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.curved=true;laser.phase=2;laser.width=24;
  laser.source.changes[0].flags=0x80;laser.source.changes[0].integers[0]=300;
  assert(laser.curve.initialize(0,224,0,.1f,26,laser.source.changes));
  laser.motion_frame=late?world.battle_frame+1:-1;world.lasers.push_back(laser);
  world.advance_vs_projectiles();
  const int hit=world.cancel_lasers_circle(0,224,640,true,true);
  assert(hit==(late?26:0));assert(world.lasers[0].cancel_lock==(late?0:300));
 }
 // Native path mode2 remains iterative for a zero-change ECL609 segment.
 // Its rounded per-tick coordinates feed curve contacts and barrier direction.
 {th19::CurveLaserPath::Segment segment{40,120,{-138.00999450683594f,92.30998229980469f,1.5707963705062866f,3.9999983310699463f},0,0,true};
  const auto point=th19::CurveLaserPath::sample(segment,120);
  // MinGW32 may retain a final expression in x87 registers; compare the
  // stored f32 fields, matching native SSE and WebAssembly state storage.
  volatile float x=point.x,y=point.y;
  assert(x==-138.00999450683594f&&y==412.3099670410156f);
 }
 // Native524070 emits global-clear feedback every3 nodes; circle cuts use10.
 for(bool global:{false,true}){
  th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.curved=true;laser.phase=2;laser.width=24;
  std::array<th19::BulletChange,24> changes{};
  assert(laser.curve.initialize(0,224,0,.1f,26,changes));
  world.lasers.push_back(laser);
  if(global)world.cancel_all_lasers();else world.cancel_lasers_circle(0,224,640);
  assert(!world.lasers[0].active);
  assert(world.bullet_cancels.size()==(global?9u:3u));
 }
 // A static curve leaves native bounds before early global-clear callbacks.
 // Its old draw cache remains visible but the pending contact sample is not.
 {th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.curved=true;laser.phase=2;laser.width=24;
  laser.curve.time=196;laser.curve.segments={{0,999999,{0,224,0,1},0,0}};
  for(int i=0;i<26;++i)laser.curve.nodes.push_back(laser.curve.at(195.f-i));
  world.lasers.push_back(laser);world.advance_vs_projectiles();world.cancel_all_lasers();
  assert(!world.lasers[0].active&&world.bullet_cancels.empty());
 }
 // Native0x4eeda0/402690: the Aura-slowed preservation timer expires
 // at remaining0.5, whose integer timer component is already zero.
 {th19::BulletChanges changes;auto& a=changes.actions[6];a.active=true;
  a.record.flags=0x100;a.record.integers[0]=2;a.record.integers[1]=0;
  float vx=0,vy=0,x=0,y=224;
  changes.tick(vx,vy,x,y,0,400,.5f);assert(changes.preserves_bounds());
  changes.tick(vx,vy,x,y,0,400,.5f);assert(changes.preserves_bounds());
  changes.tick(vx,vy,x,y,0,400,.5f);assert(!changes.preserves_bounds());
 }
 // Respawn clears use the native current curve sample, not the draw cache.
 // A one-node cut must address index30 and publish its every-tenth feedback;
 // the stale cache would cut index28 and silently lose that feedback.
 for(bool registeredOnly:{false,true}){th19::GameWorld world;world.reward_mode=2;
  th19::InfiniteLaser laser;laser.active=true;laser.phase=2;laser.curved=true;
  laser.age=40;laser.width=24;laser.source.color=4;laser.curve.time=120;
  laser.curve.segments={{0,999999,{133,0,1.57079632679f,4},0,0}};
  for(int i=0;i<33;++i)laser.curve.nodes.push_back(laser.curve.at(119.f-i));
  world.lasers.push_back(laser);world.advance_vs_projectiles();
  world.cancel_lasers_circle(133,364,.01f,false,registeredOnly);
  assert(world.lasers[0].curve.nodes.size()==30);
  assert(world.bullet_cancels.size()==1);
  assert(std::abs(world.bullet_cancels[0].y-364)<.001f);
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
 // Result-state Sanae EX dispatch keeps reserved slots but never constructs
 // flights: two provisional edge draws per reservation, no duration/tangents.
 {
  th19::GameWorld sender,target;sender.reward_mode=2;sender.difficulty=3;
  sender.pending_native_attack_credit=160;
  th19::AnmVm::restore_random(123456,true);
  for(int i=0;i<4;++i)th19::AnmVm::rand_integer(true);
  const auto expected=th19::AnmVm::random_checkpoint(true);
  th19::AnmVm::restore_random(123456,true);
  assert(sender.send_native_extra_credit(target,{},2,14090,true)==2);
  assert(sender.pending_native_attack_credit==0&&sender.native_extra_reserved_count==2);
  assert(sender.native_extra_sequence==2&&sender.native_extra_reservations.empty());
  assert(target.extra_flights.empty()&&th19::AnmVm::random_checkpoint(true)==expected);
  sender.send_native_extra_credit(target,{},2,14091,true);
  assert(sender.native_extra_reserved_count==2&&th19::AnmVm::random_checkpoint(true)==expected);
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
 // Native ECL525 visits hosts retired by an earlier clear until the next
 // enemy-list visit. Two clears dispatch twice; a later-frame clear cannot revive them.
 {th19::GameWorld world;world.reward_mode=2;world.battle_frame=10;
  auto& enemy=world.enemies.emplace_back();enemy.id=1;enemy.sprite=2;enemy.life=10;
  th19::EclProgram program;program.subs.push_back({"Clear",0,32});
  program.bytecode.resize(32);
  for(int offset:{0,16}){program.bytecode[offset+4]=13;program.bytecode[offset+5]=2;
   program.bytecode[offset+6]=16;program.bytecode[offset+10]=255;}
  th19::AnmVm::seed_random(31);
  th19::EclVm first;first.attach_program(program);first.attach_world(&world);
  assert(first.start("Clear"));first.step();
  assert(!first.error()&&world.defeated==2&&world.deaths.size()==2);
  assert(!world.enemies[0].active&&(world.enemies[0].status_flags&0x200));
  const auto rng=th19::AnmVm::random_checkpoint(true);
  th19::EclVm leftRoot,rightRoot;th19::GameWorld right;
  th19::WorldCheckpoint saved(world,leftRoot,right,rightRoot);
  world.enemies[0].clear_retirement_frame=-1;saved.restore(world,leftRoot,right,rightRoot);
  assert(world.enemies[0].clear_retirement_frame==10);
  ++world.battle_frame;th19::EclVm later;later.attach_program(program);later.attach_world(&world);
  assert(later.start("Clear"));later.step();
  assert(!later.error()&&world.defeated==2&&th19::AnmVm::random_checkpoint(true)==rng);
 }
 std::cout<<"2000 bullet slots, recycling, 15000 retired enemies, checkpoint, held-arrival HP/contact, feedback order and signed EX 100-slot release: PASS\n";
}
