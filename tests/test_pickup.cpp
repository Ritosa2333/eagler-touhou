#include "pickup_core.hpp"
#include "player_shots.hpp"
#include <iostream>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Pickup contract failed check="+std::to_string(checks));}
int main(){try{
 for(unsigned rank=0;rank<=4;++rank)for(int y=0;y<384;++y){soew::PickupCore p;soew::PointnumCore n;n.rank=rank;unsigned lives=2,bombs=3;p.collect(1,200,y,lives,bombs,n);unsigned value=y<=64?5120:2800-unsigned(y)*7;check(p.frame_score==value&&p.points_collected==1);check(p.score_for_rank(rank)==(value<<(rank>=2?rank-1:0)));check(n.slots[0].points==value&&n.slots[0].left==188);}
 {soew::PickupCore p;soew::PointnumCore n;unsigned lives=2,bombs=3;p.collect(0,200,200,lives,bombs,n);check(p.power==2&&p.level()==0&&p.frame_score==1&&p.collect_skill==1);p.collect(0,200,200,lives,bombs,n);p.collect(0,200,200,lives,bombs,n);check(p.power==4&&p.level()==1);}
 for(unsigned power:{1u,69u,70u,79u,80u}){soew::PickupCore p;p.power=power;soew::PointnumCore n;unsigned l=2,b=3;p.collect(3,200,200,l,b,n);check(p.power==(power<70?power+10:80));check(p.overflow==(power<70?0u:5u));check(p.frame_score==(power<70?1u:5u));}
 {soew::PickupCore p;p.power=80;p.overflow=41;p.collect_skill=31;soew::PointnumCore n;unsigned l=2,b=5;p.collect(0,200,200,l,b,n);check(p.overflow==42&&p.frame_score==1280&&p.item_skill==1&&p.collect_skill==1);p.collect(2,200,200,l,b,n);check(p.frame_score==7833&&p.collect_skill==17);}
 {soew::PickupCore p;soew::PointnumCore n;unsigned l=4,b=4;p.collect(4,200,200,l,b,n);check(l==5&&p.frame_score==0&&p.sounds==std::vector<unsigned>{8,13});p.collect(2,200,200,l,b,n);check(b==5&&p.frame_score==1000);}
 {soew::PointnumCore n;n.add(100,200,5120);auto g=n.glyphs(5120);check(g==std::array<unsigned,7>{5,1,2,0,0,12,12});n.rank=4;g=n.glyphs(1);check(g==std::array<unsigned,7>{12,12,12,1,0,10,8});for(unsigned f=0;f<7;++f){n.update(0);check(n.slots[0].top[0]==200);}n.update(0);check(n.slots[0].top[0]==199);for(unsigned f=8;f<26;++f)n.update(0);check(n.slots[0].flag==2);n.begin_frame(1);check(n.slots[0].flag==0);}
 for(unsigned type=0;type<3;++type){soew::PlayerShots s;soew::RandomRing rng;rng.bytes[0]=3;s.level=1;s.shot_type=type;s.random=&rng;s.emit(0,100,300);check(s.slots[0].vx==(type==2?0:soew::enemy_vector(193,192)[0]));check(rng.cursor==(type==2?0:1));}
 {soew::PlayerShots s;s.level=10;bool refused=false;try{s.emit(0,100,300);}catch(const std::exception&){refused=true;}check(refused&&s.fired==0);}
 for(unsigned power=1;power<=80;++power)for(unsigned overflow=0;overflow<=42;++overflow)for(unsigned type:{0u,3u}){soew::PickupCore p;p.power=power;p.overflow=overflow;soew::PointnumCore n;unsigned l=2,b=3;p.collect(type,200,200,l,b,n);check(p.power<=80&&p.overflow<=42&&p.level()<=9);if(type==0&&power<80)check(p.power==power+1&&p.overflow==0&&p.frame_score==1);if(type==0&&power==80)check(p.overflow==(overflow<42?overflow+1:42));if(type==3&&power<70)check(p.power==power+10&&p.overflow==0);if(type==3&&power>=70)check(p.power==80&&p.overflow==(overflow<38?overflow+5:42));}
 {soew::PlayerShots s;soew::RandomRing r;s.level=1;s.shot_type=0;s.random=&r;for(auto &shot:s.slots)shot.flag=1;check(!s.emit(0,100,300)&&r.cursor==0);}
 std::cout<<"pickup_point_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
