#include "native-world.hpp"
#include "../../web/cpp/game/EnemyCallbacks.hpp"

struct ShieldRuntimeHost final:EnemyRuntimeHost {
 EnemyVisualHost* animations()override{return nullptr;}
 int after_script(EnemyRuntime&,float)override{return 0;}
 int collide_and_damage(EnemyRuntime&,float)override{return 0;}
 bool update_distortion(EnemyState&,float)override{return true;}
};

int main(int argc,char** argv){
 require(argc==2,"Pass a legally supplied Japanese th15 1.00b archive");
 RetailAssets files(argv[1]);
 for(bool legacy:{false,true})for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat){
  std::array<int,3> selection{};selection[seat]=3;
  World w(files,count,0,selection);auto& b=*w.battle;auto& reisen=*b.pilots[seat];
  b.mp_options.replay_legacy_stage_flow=legacy;
  EnemySpawnRequest main;main.routine="main";require(b.spawn(main),b.error);
  auto* boss=b.enemies->at(0);require(boss&&boss->commands.host,"Retail Boss missing");
  boss->state.flags|=0x800000;b.enemy_world.boss_ids[0]=boss->state.id;b.enemy_world.refresh_boss();
  SpellStartRequest spell;spell.id=0;spell.duration=1800;spell.name="Reisen co-op shield";
  require(boss->commands.host->begin_spell(spell),b.error);b.bomb_enemies.available=true;
  require(reisen.begin_bomb(),b.error);
  for(unsigned tick=0;tick<35;++tick)require(reisen.tick_bomb(),b.error);
  BattleFrame frame;
  require(b.prepare_frame(frame)&&b.enemy_world.game_state==int(legacy)&&bool(b.spell.flags&32)==legacy,"Persistent Reisen shield did not follow its recorded rules");

  ShieldRuntimeHost host;EnemyRuntime enemy(w.assets.scripts,b.enemy_world,w.environment.game_rng,w.visual,host);
  auto& e=enemy.state;e.id=90;e.life=10000;e.flags=128|0x4000000|0x40800000|(legacy?0:0x10000000);
  e.damage_multiplier=0;e.pending_damage=120;
  require(b.enemy_damage->update(enemy,1)==0&&e.life==10000-(legacy?0:count==2?90:80),"Shield damage context did not follow its recorded rules");
  require(!(e.flags&0x20000001u),"Shield made the enemy inactive/invincible");

  require(reisen.player->life.hit()&&reisen.tick_bomb()&&b.prepare_frame(frame),b.error);
  require(static_cast<BombReisen&>(*reisen.bomb).charges==2&&reisen.player->life.state==1&&reisen.player->life.invulnerability.current==30,"Personal shield no longer absorbs a hit");
  require(bool(b.spell.flags&32)==legacy&&b.enemy_world.game_state==int(legacy),"Shield break did not follow its recorded spell startup rules");

  const unsigned other=(seat+1)%count;b.pilots[other]->session.bomb_state=1;
  require(b.prepare_frame(frame)&&b.enemy_world.game_state==1,"Damaging teammate Bomb lost its enemy context");
  e.flags=128|0x4000000|0x40800000;e.pending_damage=120;const auto life=e.life;
  require(b.enemy_damage->update(enemy,1)==0&&e.life==life,"Actual damaging Bomb ignored the enemy's authored zero damage multiplier");
  require(b.spell_card.abort()&&boss->commands.host->begin_spell(spell)&&bool(b.spell.flags&32),"A damaging Bomb must still protect spell startup");
  require(b.prepare_frame(frame)&&bool(b.spell.flags&32),"Reisen shield cleared another pilot's damaging Bomb protection");
  b.pilots[other]->session.bomb_state=0;
  require(b.prepare_frame(frame)&&bool(b.spell.flags&32)==legacy&&b.enemy_world.game_state==int(legacy),"Remaining Reisen shield did not follow its recorded rules");
 }
 std::puts("PASS: 2P/3P Reisen shields in every seat preserve rule 20 replay behavior and rule 22 Boss damage, hit absorption and damaging Bomb protection");
}
