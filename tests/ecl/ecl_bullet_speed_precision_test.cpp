#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
using namespace th13;
u32 word(float f){u32 w;std::memcpy(&w,&f,4);return w;}
float value(u32 w){float f;std::memcpy(&f,&w,4);return f;}
TEST(accelerated_bullet_rounds_squared_speed_before_sqrt) {
 StageSim sim;sim.bullet_animation=[](StageBullet&,bool){return true;};
 StageBullet b;b.x=10;b.y=200;b.speed=.7f;b.vel[0]=.7f;
 b.ex[0].op=0x200000;b.ex[0].args[0]=.1f;b.ex[0].args[1]=-999999;b.ex[0].args[4]=120;
 b.ex[1].op=4;b.ex[1].args[0]=.01f;b.ex[1].args[1]=1.7f;b.ex[1].args[4]=120;
 sim.bullets.push_back(b);
 for(int f=0;f<=150;++f){sim.advance_bullets();CHECK(sim.error().empty());
  //Original40db10 with explicit x87 FPCW037f; default Machine FPCW0
  //is a different component environment and not the reference for these words.
  if(f==127)CHECK_EQ(word(sim.bullets[0].speed),1039481920u);
  if(f==133)CHECK_EQ(word(sim.bullets[0].speed),1042618191u);
  if(f==150)CHECK_EQ(word(sim.bullets[0].speed),1050697578u);
 }
}
TEST(fan_spoke_and_offset_stores_match_original_sanae_stage5_birth) {
 StageSim sim;sim.interactive=true;
 StageEt et;et.aim=10;et.ways=3;et.count=3;
 et.angle1=value(3216375925u);et.angle2=value(1016003125u);
 et.speed1=value(1050253722u);et.speed2=.25f;
 const float origin[]={0,200,0};sim.emit_et(et,7,0,origin,0);
 CHECK_EQ(sim.bullets.size(),size_t(9));
 if(sim.bullets.size()!=9)return;
 // 40e840 raw-angle probe, mode10,way2,idx1, frame252.
 CHECK_EQ(word(sim.bullets[7].angle),1060151598u);
 CHECK_EQ(word(sim.bullets[7].vel[0]),1044738578u);
 CHECK_EQ(word(sim.bullets[7].vel[1]),1042477581u);
}
TEST(queue_cancel_retains_entered_active_movement_until_next_update) {
 StageSim sim;
 StageBullet b;b.x=5;b.y=200;b.vel[0]=2;b.speed=2;b.state=1;
 b.ex[0].op=0x400;sim.bullets.push_back(b);
 sim.advance_bullets();CHECK_EQ(sim.bullets.size(),size_t(1));
 CHECK_EQ(sim.bullets[0].state,3);CHECK_EQ(sim.bullets[0].x,7.f);
 sim.advance_bullets();CHECK_EQ(sim.bullets[0].x,8.f);
}
int main(){return th13test::run_all("bullet speed precision");}
