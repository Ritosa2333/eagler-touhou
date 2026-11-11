#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(dialogue_cancels_projectiles_after_opening_without_destroying_the_fade) {
 struct Host:StageDialogHost {
  StageSim* sim=nullptr;int calls=0;int entry=0;
  void open_dialog(i32 n)override{entry=n;++calls;CHECK_EQ(sim->bullets[0].state,1);}
  bool dialog_wait_released()const override{return true;}
  bool dialog_active()const override{return false;}
 }host;
 for(i32 entry:{0,-1,-3}){
  StageSim sim;StageEnemy enemy;EclContext c;host.sim=&sim;sim.dialog_host=&host;sim.commands.sim=&sim;sim.current=&enemy;
  StageBullet bullet;bullet.x=10;bullet.y=100;bullet.vel[0]=2;bullet.state_age=50;sim.bullets.push_back(bullet);
  bullet.state=2;sim.bullets.push_back(bullet);bullet.state=3;sim.bullets.push_back(bullet);
  std::vector<u8> code;th13test::emit(code,0,518,0,255,0,{u32(entry)});c.instruction=reinterpret_cast<const EclInstruction*>(code.data());
  CHECK_EQ(sim.commands.execute(c,518),0);CHECK_EQ(host.entry,entry);
  for(int i=0;i<2;++i){CHECK(sim.bullets[i].alive);CHECK_EQ(sim.bullets[i].state,3);CHECK_EQ(sim.bullets[i].animation_interrupt,1);CHECK_EQ(sim.bullets[i].state_age,0);CHECK(!sim.bullets[i].collision_ready);}
  CHECK_EQ(sim.bullets[2].state_age,50);
  sim.advance_bullets();CHECK_EQ(sim.bullets[0].x,11.f);CHECK_EQ(sim.bullets[0].state_age,1);
 }
}
int main(){return th13test::run_all("dialogue projectile cancellation");}
