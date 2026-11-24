#include "midboss1_core.hpp"
#include <iostream>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("midboss1 contract failed="+std::to_string(checks));}
int main(){try{
 soew::Midboss1Core::Hooks h;std::vector<soew::EnemyBulletRequest> groups;std::vector<soew::Midboss1Core::Spark> sparks;unsigned score=0,tiles=0,queries=0;
 h.bullet=[&](auto r){groups.push_back(r);};h.spark=[&](auto r){sparks.push_back(r);};h.tile=[&](int x,int y,unsigned image){check(x>=192&&x<=240&&y%16==0&&image>=60&&image<=63);++tiles;};h.score=[&](unsigned v){score+=v;};h.damage=[&](int x,int y,int w,int height){check(x==192&&y==16&&w==64&&height==96);++queries;return 0u;};
 soew::Midboss1Core b;b.trigger(115);check(!b.active);b.trigger(116);check(b.active);
 for(unsigned f=1;f<=128;++f){unsigned p=(f-1)&1;b.begin_frame(p);b.update(p,0,116,h);check(b.phase_frame==f);if(f==1)check(!b.visible&&b.top[p]==-16);if(f==17)check(b.visible&&b.top[p]==16);if(f>=18)check(b.top[p]==16);}
 check(groups.size()==3&&groups[0].group==11&&groups[0].speed==30&&groups[1].group==18&&groups[2].group==18);check(groups[0].left==220&&groups[0].top==104);check(queries==111&&tiles==444);
 // Damage threshold is strictly >300, and its kill gate uses wrapped VRAM Y.
 {soew::Midboss1Core c;c.active=true;c.initialized=true;c.phase_frame=17;c.left.fill(192);c.top.fill(16);unsigned value=300;auto k=h;k.damage=[&](int,int,int,int){return value;};c.update(0,0,116,k);check(c.damage_taken==300&&!c.dying&&c.hit_flash);value=1;c.update(0,288,116,k);check(c.damage_taken==301&&!c.dying);c.update(0,289,116,k);check(c.damage_taken==302&&!c.dying);c.update(0,399,116,k);check(c.dying&&score==10000&&!c.visible);
  for(unsigned f=0;f<64;++f){const auto p=f&1;c.begin_frame(p);c.update(p,0,120,k);if(f<63)check(c.visible&&c.death_pattern==10+f/8&&c.top[p]==16+int((f+1)/4));}
  check(c.phase_frame==0&&!c.present&&c.target_x==-1&&sparks.back().sprite&&sparks.back().count==48);c.begin_frame(0);check(!c.active);}
 // Timeout applies ONLY on a zero-damage query and grants no score.
 {soew::Midboss1Core c;c.active=true;c.phase_frame=17;c.top.fill(16);auto k=h;unsigned value=1;k.damage=[&](int,int,int,int){return value;};c.update(0,0,184,k);check(!c.dying);value=0;c.update(0,0,184,k);check(c.dying&&score==10000);}
 std::cout<<"midboss1_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
