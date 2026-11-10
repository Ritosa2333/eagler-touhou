#include "../../web/cpp/game/EnemyCallbacks.hpp"
struct BalanceRuntimeHost final:EnemyRuntimeHost {
 EnemyVisualHost* animations()override{return nullptr;}
 int after_script(EnemyRuntime&,float)override{return 0;}int collide_and_damage(EnemyRuntime&,float)override{return 0;}
 bool update_distortion(EnemyState&,float)override{return true;}
};
void balance_rules(RetailAssets& files){
 {
  World challenge(files,2,0,{0,1,0},true);auto& b=*challenge.battle;auto& p=*b.pilots[1];
  require(b.session.bombs==0&&p.session.bombs==0&&p.player->life.base_bombs==0,"Challenge base Bomb stock must be zero");
  p.session.bombs=4;p.input.held=2;b.bomb_enemies.available=true;require(p.tick()&&p.session.bomb_state==0&&p.session.bombs==4&&!(p.keys.held&2)&&!p.allow_bomb(),"Challenge must reject Bomb input even with earned stock");
  p.player->life.state=2;p.player->life.age.set(30);p.session.bomb_pieces=4;require(p.tick()&&p.session.bombs==0&&p.session.bomb_pieces==4,"Challenge respawn must reset its base stock without clearing personal fragments");
 }

 for(unsigned count:{2u,3u}){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  for(unsigned s=0;s<count;++s)require(b.pilots[s]->session.bombs==2&&b.pilots[s]->player->life.base_bombs==2,"Co-op initial Bomb stock must be two per pilot");
  auto& remote=*b.pilots[1];remote.session.bombs=7;remote.session.bomb_pieces=4;remote.player->life.state=2;remote.player->life.age.set(30);require(remote.tick(),b.error);
  require(remote.session.bombs==2&&remote.session.bomb_pieces==4&&b.session.bombs==2,"Respawn must reset only its owner's base Bomb stock and retain fragments");
  remote.player->life.state=1;remote.player->life.invulnerability.set(280);if(count==3)b.pilots[2]->player->life.invulnerability.set(90);
  b.bomb_enemies.available=true;require(b.pilots[0]->begin_bomb(),b.error);
  require(remote.player->life.invulnerability.current==280&&(count!=3||b.pilots[2]->player->life.invulnerability.current==180),"Teammate Bomb protection must extend short timers without shortening respawn grace");
  require(b.session.bombs==1&&remote.session.bombs==2,"Bomb use must debit only its owner");
  remote.player->life.state=4;remote.player->life.age.set(8);const auto deaths=w.progress.chapter_deaths,counters=b.enemy_world.counters[0];require(remote.tick(),b.error);
  require(w.progress.chapter_deaths==deaths+1&&w.progress.stage_deaths[0]==1&&w.progress.stage_deaths[2]==1&&b.enemy_world.counters[0]==counters+1,"Remote committed death must update shared chapter, stage and ECL counters once");
  require(remote.tick()&&w.progress.chapter_deaths==deaths+1,"Miss counted more than once during respawn");
  b.player->life.state=4;b.player->life.age.set(8);require(b.pilots[0]->tick(),b.error);
  BattleFrame frame;require(b.prepare_frame(frame),b.error);
  require(w.progress.chapter_deaths==deaths+2&&b.enemy_world.counters[0]==counters+2&&b.enemy_world.counter3==2,"Primary miss must retain remote miss counters and sum individual deaths");
 }
 for(unsigned count:{2u,3u})for(unsigned dying=0;dying<count;++dying){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.player->life.state=s==dying?2:1;p.player->frame.input_age.set(1);require(p.player->damage.circle({0,200,0},48,0,20,20)>0,"Damage allocation failed");}
  BattleFrame frame;require(b.prepare_frame(frame),b.error);
  require(b.enemy_world.player_damage_state==1,"Primary death still controls aggregate damage state");
  BalanceRuntimeHost host;EnemyRuntime enemy(w.assets.scripts,b.enemy_world,w.environment.game_rng,w.visual,host);
  auto& e=enemy.state;e.id=70;e.flags=128|0x4000000;e.hitbox={24,24};e.motion.position={0,200,0};e.life=10000;e.damage_multiplier=1;
  const i32 total=i32(count-1)*20+4;require(b.enemy_damage->update(enemy,1)==0&&e.life==10000-total,"A dying pilot must reduce only its own damage, regardless of seat");
  e.id=71;e.life=10000;e.flags=128|0x4000000|0x40800000;e.multiplayer_damage_remainder=0;
  require(b.enemy_damage->update(enemy,1)==0&&e.life==10000-total*(count==2?9:8)/12,"Boss scaling was not applied after per-pilot damage and native protections");
  e.id=72;e.life=10000;e.multiplayer_damage_remainder=0;e.damage_multiplier=.5f;b.enemy_world.game_state=1;b.spell.flags=33;
  require(b.enemy_damage->update(enemy,1)==0&&e.life==10000,"Native spell startup protection must precede co-op scaling");
 }
 {
  World w(files,3,0,{0,1,3});auto& b=*w.battle;EnemyState boss;boss.flags=0x800000;
  i32 sum=0;for(unsigned n=0;n<12;++n)sum+=b.mp_boss_damage(boss,1);require(sum==8,"Tiny Boss hits lost fractional co-op damage");
  b.pilots[2]->player->life.state=2;require(b.mp_viable_count()==3,"Temporary death must not change Boss participant count");
  b.pilots[2]->session.extra_lives=-1;require(b.mp_viable_count()==2&&b.mp_boss_damage(boss,12)==9,"Exhausted pilot must leave Boss scaling before ghost transition");
  b.pilots[1]->ghost=true;require(b.mp_viable_count()==1&&b.mp_boss_damage(boss,12)==12,"Last viable pilot must receive native Boss damage");
  EnemyState ordinary;require(b.mp_boss_damage(ordinary,12)==12,"Ordinary enemy damage must remain native");
 }
 for(int primary:{0,3}){
  World w(files,2,0,{primary,1,0});auto& b=*w.battle;EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
  auto* boss=b.enemies->at(0);require(boss&&boss->commands.host,"Retail ECL owner missing");boss->state.flags|=0x800000;b.enemy_world.boss_ids[0]=boss->state.id;b.enemy_world.refresh_boss();
  b.pilots[1]->session.bomb_state=1;SpellStartRequest spell;spell.id=0;spell.duration=1800;spell.name="Co-op bomb context";
  require(boss->commands.host->begin_spell(spell)&&bool(b.spell.flags&32),"Remote damaging Bomb must protect the new spell even with Reisen as primary");
  BattleFrame frame;frame.player_enabled=frame.bomb_enabled=frame.enemies_enabled=frame.lasers_enabled=frame.bullets_enabled=frame.items_enabled=false;
  require(b.step(frame)&&bool(b.spell.flags&32),"Spell startup Bomb state cleared while a remote Bomb remained active");
  b.pilots[1]->session.bomb_state=0;b.session.bomb_state=primary==3?1:0;
  require(b.step(frame)&&!(b.spell.flags&32),"Spell startup protection must end when the final damaging Bomb ends");
  require(b.spell_card.abort(),b.spell_card.error);
  if(primary==3){
   b.session.bomb_state=0;require(boss->commands.host->begin_spell(spell)&&!(b.spell.flags&32),"Shield-free spell started protected");b.bomb_enemies.available=true;
   require(b.pilots[0]->begin_bomb()&&!(b.spell.flags&32),"Reisen activation must not protect the Boss during spell startup");
   require(b.step(frame)&&!(b.spell.flags&32),"Reisen shield enabled spell startup damage protection");b.session.bomb_state=0;
   require(b.step(frame)&&!(b.spell.flags&32),"Reisen startup protection did not end with its own shield");require(b.spell_card.abort(),b.spell_card.error);
   b.session.bomb_state=1;
  }
  if(primary==3){require(b.mp_bomb_active()&&!b.mp_bomb_active(true),"Reisen's personal shield was treated as a damaging Bomb");require(boss->commands.host->begin_spell(spell)&&!(b.spell.flags&32),"Reisen-only shield must not protect spell startup");b.player->hit();require(b.step(frame)&&!(b.spell.flags&32),"Reisen shield hit must not protect the Boss");b.player->life.state=1;b.session.bomb_state=0;require(b.step(frame)&&!(b.spell.flags&32),"Shield-hit startup protection outlived its owner");require(b.spell_card.abort(),b.spell_card.error);}
 }
 {
  World w(files,3,0,{0,1,3});auto& b=*w.battle;BulletShooter shot;shot.position={100,300,0};shot.count=shot.rows=1;shot.speed=2;
  require(b.bullet_scene->manager.emit(shot,0),b.bullet_scene->manager.error);auto& bullet=*b.bullet_scene->manager.state(b.bullet_scene->manager.first_active());bullet.step_limit=1;bullet.motion.velocity={2,0,0};
  PlayerCollision players[3];players[0].position={-100,400};players[1].position=players[2].position={100,300};for(auto& p:players)p.radius=3;
  scale_bullets_near_players(b.bullet_scene->manager,players,3,.5f,false);
  require(bullet.motion.velocity.x==1,"Remote special-bullet rule must apply once even when two pilots overlap");
 }
}
