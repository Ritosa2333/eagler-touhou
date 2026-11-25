#include "encounter_scene.hpp"
#include <iostream>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("midboss1 scene failed="+std::to_string(checks));}
soew::EncounterScene setup(const char*root,unsigned shot,unsigned rank){auto s=soew::EncounterScene::load(root,0,int(shot),int(rank),2,3);s.combat_enabled=s.survival_enabled=s.pickup_enabled=true;s.enable_effects();s.enable_bombing();s.enable_midboss();return s;}
int main(int argc,char**argv){try{if(argc<2)throw std::runtime_error("need assets");std::vector<std::uint32_t> pixels(640*400),repeat(640*400);
 for(unsigned shot=0;shot<3;++shot)for(unsigned rank=0;rank<4;++rank){auto s=setup(argv[1],shot,rank);unsigned started=0,finished=0;bool seen=false;
  for(unsigned f=0;f<6400;++f){s.life.invincibility=200;s.tick(0);check(!s.runtime.blocked);if(s.midboss.active&&!seen){seen=true;started=unsigned(s.ticks);check(s.midboss.phase_frame==1&&s.midboss.top[0]==-16&&s.midboss.top[1]==-16);}
   if(seen&&s.midboss.phase_frame>=18&&s.midboss.active&&!s.midboss.dying){check(s.midboss.damage_taken==0);check(s.shots.target_x==216&&s.shots.target_y==64);}
   if(f%13==0){s.render(pixels);s.render(repeat);check(pixels==repeat);}
   if(seen&&!s.midboss.active){finished=unsigned(s.ticks);break;}
  }
  check(started==3710&&finished==5951);check(seen&&finished>started&&s.midboss.score_awarded==0&&s.midboss.shot_groups>0);check(s.runtime.dispatch.suppressed>0&&s.track_tiles.size()>0);check(s.runtime.scroll.progress>=184&&s.midboss.target_x==-1);s.render(pixels);
  std::cout<<"midboss1_timeout shot="<<shot<<" rank="<<rank<<" start="<<started<<" end="<<finished<<" groups="<<s.midboss.shot_groups<<" suppressed="<<s.runtime.dispatch.suppressed<<" tiles="<<s.track_tiles.size()<<'\n';
 }
 // True scenario damage callback: normal shots and Bomb affect shared HP/score.
 for(unsigned shot=0;shot<3;++shot){auto s=setup(argv[1],shot,1);s.runtime.scroll.progress=117;s.midboss.active=true;s.midboss.initialized=true;s.midboss.phase_frame=17;s.midboss.top.fill(16);s.midboss.damage_taken=300;
  // End-of-tick logical scroll0 ->399, so wrapped VRAM top15 satisfies kill gate.
  s.runtime.scroll.line=0;s.runtime.scroll.cycle=0;
  auto &p=s.shots.slots[0];p.flag=1;p.type=0;p.pattern=52;p.position[1]={3200,800};p.vy=-192;
  s.tick(soew::Bomb);check(s.midboss.dying&&s.midboss.damage_taken>300&&s.midboss.score_awarded==10000);check(s.damage.kills==0&&s.items.spawned==0);check(s.life.bombs==2&&s.score_core.score+s.score_core.delta>=10000);
  for(unsigned i=0;i<64;++i){s.life.invincibility=200;s.tick(0);s.render(pixels);}s.tick(0);check(!s.midboss.active&&s.midboss.phase_frame==0&&s.midboss.target_x==-1);
 }
 std::cout<<"midboss1_scene_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
