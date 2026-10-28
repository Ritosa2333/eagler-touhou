#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
using namespace th13;
u32 word(float f){u32 w;std::memcpy(&w,&f,4);return w;}
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
int main(){return th13test::run_all("bullet speed precision");}
