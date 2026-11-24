#include "spark_core.hpp"
#include "item_core.hpp"
#include "score_core.hpp"
#include <iostream>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("Effect core failed check="+std::to_string(checks));}
int main(){try{
 {soew::IRand r(1234),copy(1234);soew::SparkCore s;s.initialize(r);for(unsigned i=0;i<64;++i){check(s.slots[i].angle==std::uint8_t(copy.next()));check(s.slots[i].base_speed==(copy.next()&63));check(s.slots[i].default_sprite==(i%2==0));}check(copy.state==r.state);s.add(100,100,128,16,true,0);check(s.generated==16&&s.ring==16);check(s.slots[0].velocity==std::array<int,2>{128,0});s.update(0);check(s.slots[0].position[0][0]==1728&&s.slots[0].velocity[1]==1);s.begin_frame(1);check(s.slots[0].position[1][0]==1728);s.add(100,408,20,64,true,1);check(s.ring==16&&s.slots[16].position[1][1]==128);}
 {soew::IRand r(1);soew::SparkCore s;s.initialize(r,true);check(s.max_age==24);for(unsigned i=0;i<64;++i)check(s.slots[i].default_sprite==(i%8==0));s.slots[0].flag=1;s.slots[0].age=24;s.slots[0].position[0]={1600,1600};s.update(0);check(s.slots[0].flag==2);s.begin_frame(1);check(s.slots[0].flag==0);}
 {soew::ItemCore items;soew::RandomRing rng;rng.bytes[0]=7;items.initialize(rng);check(items.ring_index==7&&rng.cursor==1);items.semirandom(100,100,rng,2);check(items.spawned==1&&items.slots[0].type==1&&rng.cursor==2);items.semirandom(100,100,rng,2);items.semirandom(100,100,rng,2);check(rng.cursor==2&&items.spawned==1);items.semirandom(100,100,rng,2);check(items.spawned==2&&rng.cursor==3);}
 {soew::ItemCore i;soew::RandomRing r;r.bytes[0]=18;i.semirandom(100,100,r,2);check(i.slots[0].type==soew::ItemCore::Bomb);for(auto &s:i.slots)s.flag=1;const auto c=r.cursor;i.cycle=0;i.semirandom(100,100,r,2);check(r.cursor==std::uint8_t(c+1)&&i.ring_index==2);}
 {soew::ItemCore i;i.bigpower_override=1;check(i.add(100,100,0,2)&&i.slots[0].type==3&&i.bigpower_override==0);check(i.add(100,100,4,5)&&i.slots[1].type==2);i.update(0,300,300,false,[](auto,auto,auto){return true;});check(i.slots[0].top[0]==1552&&i.slots[0].vy==-47);}
 {soew::ItemCore i;soew::RandomRing r;const auto c=r.cursor;i.miss_add(200,300,208,false,r);check(i.spawned==5&&r.cursor>c);check(i.slots[0].type==3&&i.slots[0].vy==-56&&i.slots[4].vx==2);}
 {soew::ItemCore i;soew::RandomRing r;i.miss_add(100,300,100,true,r);check(i.spawned==5&&r.cursor==0);for(auto &s:i.slots)if(s.flag)check(s.type==3&&s.vx==2);}
 {soew::ScoreCore s;s.delta=0xffff;s.add_hit_lowword(6);check(s.delta==5);unsigned lives=2,bombs=3;s.update(lives,bombs);check(s.score==1&&s.delta==4);s.commit();check(s.score==5&&s.delta==0);s.add(100000);while(s.delta)s.update(lives,bombs);check(lives==3&&s.extends==1);}
 {soew::ScoreCore s;s.delta=800000;unsigned lives=5,bombs=5;while(s.delta)s.update(lives,bombs,true);check(s.score==800000&&lives==5&&bombs==5&&s.extends==5&&s.sound8==5);}
 // Source drop-cycle uses the old wrapping byte, including255->0.
 {soew::ItemCore i;soew::RandomRing r;i.cycle=255;i.semirandom(100,100,r,2);check(i.cycle==0&&r.cursor==1);i.semirandom(100,100,r,2);check(i.cycle==1&&r.cursor==2);}
 // Exhaustion consumes the semirandom selection draw but death-item spawning
 // consumes NO random numbers when no free slot exists.
 {soew::ItemCore i;soew::RandomRing r;for(auto &s:i.slots)s.flag=1;i.miss_add(100,300,208,false,r);check(r.cursor==0);i.bigpower_override=1;i.semirandom(100,100,r,2);check(i.bigpower_override==0&&r.cursor==0);}
 // Refused pickup leaves the object active and cannot change its type.
 {soew::ItemCore i;i.add(210,310,0,2);unsigned callbacks=0;i.update(0,208,300,false,[&](auto type,auto,auto){++callbacks;check(type==0);return false;});check(callbacks==1&&i.slots[0].flag==1);}
 // Score commit deliberately does not perform an extend check by itself.
 {soew::ScoreCore s;s.add(100000);s.commit();unsigned lives=2,bombs=3;s.update(lives,bombs);check(lives==2&&s.extends==0);s.add(1);s.update(lives,bombs);check(lives==3&&s.extends==1);}
 // Generating a non-sprite spark uses stored defaults without advancing IRand
 // or the gameplay ring; overwrite follows the low6-bit ring index.
 {soew::IRand r(1234);soew::SparkCore s;s.initialize(r);const auto before=r.state;s.slots[0].angle=64;s.slots[0].base_speed=7;s.add(100,100,20,1,false,0);check(s.slots[0].velocity==std::array<int,2>{0,27}&&r.state==before);s.add(100,100,20,63,false,0);check(s.ring==0);s.add(100,100,32,1,true,0);check(s.ring==1&&s.slots[0].velocity==std::array<int,2>{32,0});}
 std::cout<<"effect_core_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
