#include "rika_core.hpp"
#include "laser_core.hpp"
#include <iostream>
#include <vector>
#include <source_location>
unsigned checks=0;void check(bool v,const std::source_location where=std::source_location::current()){++checks;if(!v)throw std::runtime_error("Rika contract failed="+std::to_string(checks)+" line="+std::to_string(where.line()));}
soew::RikaCore make(unsigned rank=1){soew::RikaCore b;b.init(rank,{0,0,0,0,0,377});return b;}
int main(){try{
 for(unsigned rank=0;rank<4;++rank){auto b=make(rank);check(b.left[0]==192&&b.state==0&&b.pattern==150&&b.render_y==25);std::vector<soew::EnemyBulletRequest> fire;soew::RikaCore::Hooks h;h.bullet=[&](auto r){fire.push_back(r);};
  for(unsigned frame=0;frame<416;++frame){auto p=frame&1;b.begin_frame(p);b.update(p,frame,h);if(frame==14)check(b.left[p]==192);if(frame==15)check(b.left[p]==191);if(frame==114)check(b.left[p]==92);if(frame==314)check(b.left[p]==292);if(frame==415)check(b.phase_frame==16&&b.left[p]==191);}
  check(fire.size()==15);for(unsigned i=0;i<3;++i)check(fire[i].group==37&&fire[i].angle==(rank>=2?(i+1)*4:0)&&fire[i].speed==56+i*16&&fire[i].left==228);check(fire[3].group==(rank==0?25:11)&&fire[4].group==21);
 }
 // Exact thresholds and attack after current damage:700 switches same frame,
 // 1400 enters high phase,2260 still alive,2261 awards bonus+invincibility.
 {auto b=make();unsigned score=0,inv=0;std::vector<soew::EnemyBulletRequest> fire;auto h=soew::RikaCore::Hooks{};unsigned dealt=700;h.damage=[&](int x,int y,int w,int height){check(x==191&&y==48&&w==64&&height==96);return dealt;};h.score=[&](unsigned v){score+=v;};h.invincibility=[&](unsigned v){inv=v;};h.bullet=[&](auto r){fire.push_back(r);};b.phase_frame=63;b.update(0,32,h);check(b.damage_taken==700&&b.hit_flash&&fire.size()==4&&fire[0].group==32&&fire[0].angle==24&&fire[2].angle==104);b.begin_frame(1);b.left[1]=192;dealt=700;b.update(1,33,h);check(b.damage_taken==1400&&b.state==1&&b.attack_frame==1);h.damage=[&](int,int,int,int){return 860u;};b.update(1,34,h);check(b.damage_taken==2260&&!b.dying);h.damage=[&](int,int,int,int){return 1u;};b.update(1,35,h);check(b.dying&&score==20000&&inv==200&&b.attack_frame==3&&!b.visible);}
 // High phase freezes via state2, two delayed lasers, pellets, then random16s.
 for(unsigned rank=0;rank<4;++rank){auto b=make(rank);b.damage_taken=1400;b.phase_frame=0;std::vector<soew::EnemyBulletRequest> fire;unsigned lasers=0;soew::RikaCore::Hooks h;h.bullet=[&](auto r){fire.push_back(r);};h.laser=[&](int x,int y,int frames,unsigned pattern,unsigned delay){check((x==194||x==238)&&y==112&&frames==160&&pattern==111&&delay==64);++lasers;};
  for(unsigned f=0;f<320;++f){b.begin_frame(f&1);b.update(f&1,f,h);if(f==99)check(lasers==2&&b.attack_frame==100);}
  check(b.state==2&&b.phase_frame==0&&b.pattern==153&&lasers==2);unsigned pellets=0,big=0;for(const auto&r:fire){if(r.kind==soew::EnemyBulletRequest::Bullet16){++big;check(r.pattern==80&&r.group==28&&r.angle==1&&r.top==72);}else{++pellets;check((r.group==14||r.group==15)&&r.speed==58);}}check(pellets==7&&big==(rank==0?6:11));
 }
 // Death uses inherited phase frame, not independent death-frame counter.
 {auto b=make();b.dying=true;b.cached_x=222;b.phase_frame=30;unsigned sparks=0,explosions=0;soew::RikaCore::Hooks h;h.spark=[&](int x,int y,int speed,int count,bool sprite){check(x==254&&y==128&&speed==50&&count==2&&!sprite);++sparks;};h.explosion=[&](int x,int y,unsigned frame){check(x==246&&y==112&&frame==explosions+1);++explosions;};b.update(0,0,h);check(!b.death_zoom&&b.death_pattern==6);b.update(1,1,h);check(b.death_zoom&&b.death_pattern==7);for(unsigned f=2;f<80;++f)b.update(f&1,f,h);check(b.done&&!b.visible&&sparks==80&&explosions==79);}
 // Laser global-frame aligned aging and duration: no Bomb cancellation input.
 {soew::LaserCore l;l.initial_delay=64;check(l.add(194,112,160,111));unsigned hits=0,enter=0,exit=0;for(unsigned f=0;f<300;++f){l.begin_frame(f);bool hit=l.update(f,194,200,false);if(l.slots[0].state==4){++hits;if(!enter)enter=f;}if(l.slots[0].state==5&&!exit)exit=f;if(hit)check(l.slots[0].state==4);}std::cout<<"laser_hits="<<hits<<" enter="<<enter<<" exit="<<exit<<'\n';check(hits==160&&enter==72&&exit==232&&l.slots[0].flag==0&&l.spawned==1&&l.removed==1);}
 {soew::LaserCore l;for(unsigned i=0;i<12;++i)check(l.add(100+int(i),112,2,111));check(!l.add(200,112,2,111)&&!l.add(31,112,2,111)&&!l.add(416,112,2,111));}
 // Inherited counters obey signed16 source subtraction/completion semantics.
 {auto b=make();b.dying=true;b.death_frame=0xffff;b.update(0,0,{});check(!b.done&&b.death_frame==0&&b.death_pattern==6);b.death_frame=0x7fff;b.update(1,1,{});check(!b.done&&b.death_frame==0x8000);}
 {auto b=make();b.damage_taken=1400;b.state=2;b.attack_frame=2;b.pattern=150;b.update(0,0,{});check(b.draw_pattern==150&&b.pattern==150);b.update(1,1,{});check(b.draw_pattern==150&&b.pattern==151);}
 std::cout<<"rika_core_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
