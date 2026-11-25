#include "encounter_scene.hpp"
#include <iostream>
unsigned checks=0;
void check(bool v){++checks;if(!v)throw std::runtime_error("Bomb scene failed check="+std::to_string(checks));}
soew::EncounterScene setup(const char *root,unsigned type){auto s=soew::EncounterScene::load(root,0,int(type),1,2,3);s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;s.enable_effects();s.enable_bombing();return s;}
int main(int argc,char**argv){try{if(argc<2)throw std::runtime_error("need assets root");
 for(unsigned type=0;type<3;++type){auto s=setup(argv[1],type);s.life.bombs=1;
  auto &b=s.bullet_core.bullets[0];b.flag=1;b.position[1]={3200,1600};
  s.tick(soew::Bomb);check(s.bomb.active&&s.life.bombs==0&&s.life.bomb_invincible&&s.bomb.stage_used==1);check(b.flag==2&&s.bullet_core.spawned==0);
  s.paused=true;auto tick=s.ticks;auto frame=s.bomb.frame;s.tick(soew::Bomb);check(s.ticks==tick&&s.bomb.frame==frame);s.paused=false;
  std::vector<std::uint32_t> pixels(640*400),repeat(640*400);
  const unsigned recovery=type==0?136:type==1?164:112;bool recovery_checked=false;
  while(s.bomb.active){
   if(s.bomb.circle_done&&unsigned(s.bomb.frame)==recovery-1){auto &probe=s.bullet_core.bullets[149];probe.flag=1;probe.position[s.runtime.page^1]={1600,1600};}
s.tick(s.ticks<7?soew::Right|soew::Bomb:0);if(s.bomb.circle_done&&unsigned(s.bomb.frame)==recovery){check(s.bullet_core.bullets[149].flag==2);recovery_checked=true;}check(s.bomb.stage_used==1);s.render(pixels);s.render(repeat);check(pixels==repeat);if(s.bomb.active)check(s.life.bomb_invincible&&s.life.stage_misses==0&&s.bullet_core.spawned==0);}
  check(recovery_checked);check(s.bullet_core.removed==2);check(s.ticks==(type==2?162u:190u));check(!s.life.bomb_invincible&&s.life.invincibility==69&&s.player.x>208);
  s.life.register_hit();s.tick(0);check(!s.life.miss_active&&!s.life.hit&&s.life.invincibility==68);
  std::cout<<"bomb_scene_type="<<type<<" ticks="<<s.ticks<<" body_frame="<<s.bomb.frame<<"\n";
 }
 // Exercise the real enemy callback: bomb damage must reach HP and score even
 // with no shot present; an ordinary lethal hit still creates its item drop.
 {auto s=setup(argv[1],0);auto &t=s.script.enemies[0];t.raw[22]=2;t.raw[23]=0;t.raw[24]=1;t.raw[16]=3;t.raw[18]=100;t.raw[19]=t.raw[20]=t.raw[21]=0;s.script.paths[t.path_slot].bytes={0,255};auto &enemy=s.runtime.enemies.slots[0];enemy.raw[0xe]=1;enemy.word(0xc,0);enemy.word(4,100);enemy.word(6,100);
  s.tick(soew::Bomb);check(enemy.word(0x20)==1&&s.damage.kills==0&&s.score_core.score+s.score_core.delta==1);s.tick(0);check(enemy.word(0x20)==1);s.tick(0);check(enemy.word(0x20)==2&&s.damage.kills==1&&s.items.spawned==1&&s.score_core.score+s.score_core.delta==102);}
 // An already latched unprotected hit blocks bomb activation (no deathbomb).
 {auto s=setup(argv[1],1);s.life.register_hit();s.tick(soew::Bomb);check(!s.bomb.active&&s.life.bombs==3&&s.life.miss_frame==1);}
 std::cout<<"bomb_scene_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
