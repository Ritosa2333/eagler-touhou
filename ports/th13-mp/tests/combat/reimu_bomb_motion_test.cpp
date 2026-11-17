#include "../harness/test_framework.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
TEST(reimu_orb_orbit_launch_and_homing_match_native_component_words) {
 struct Expected {int slot,frame;u32 words[7];};
 const Expected expected[]={
 {0,0,{3263818629u,1135545549u,0,1037465424u,1065764991u,3263818629u,1135545549u}},
 {0,89,{3272485765u,1135250637u,0,1078529996u,1119671796u,3215437312u,3239953024u}},
 {0,90,{3272571617u,1134927544u,1092560227u,3218733072u,1119671796u,3215437312u,3239953056u}},
 {0,187,{1077097267u,1124758979u,1065353216u,1074665465u,1119671796u,3206293752u,1061830144u}},
 {3,120,{3269892506u,1137151836u,1092531225u,3222857027u,1119864816u,3236408864u,3235444160u}},
 {7,170,{3265341686u,1129046999u,1093032225u,3212975183u,1120122176u,1085213248u,3238883488u}}};
 for(int slot:{0,3,7}) {
  PlayerBombOrb orb;orb.angle=float(double(slot)*.7853982);StageEnemy target;
  for(int frame=0;frame<200;++frame) {
   target.position[0]=float(-20+frame*.1);target.position[1]=float(100+frame*.2);
   orb.step(frame,slot,float(-70+frame*.25),float(350-frame*.1),&target);
   for(const auto& e:expected)if(e.slot==slot&&e.frame==frame) {
    const float values[]={orb.x,orb.y,orb.speed,orb.angle,orb.radius,orb.velocity_x,orb.velocity_y};
    for(int i=0;i<7;++i)CHECK_EQ(float_to_bits(values[i]),e.words[i]);
   }
  }
 }
}
TEST(reimu_late_orbit_continues_radial_growth_after_age90) {
 PlayerBombOrb orb;for(int age=0;age<159;++age)orb.step(age,7,0,300,nullptr);
 CHECK(orb.radius>97.f);CHECK(orb.radius<98.f);
 const float radius=orb.radius;orb.step(159,7,0,300,nullptr);CHECK(orb.radius>radius);
 const float launched=orb.radius;orb.step(160,7,0,300,nullptr);CHECK_EQ(orb.radius,launched);
}
}
int main(){return th13test::run_all("reimu-bomb-motion");}
