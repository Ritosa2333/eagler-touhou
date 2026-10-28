#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(child_bullet_keeps_packed_seed_low_byte_and_parent_skip_count) {
 for(u32 packed:{0x880a0606u,0x080a0606u}){
  StageSim sim;StageEnemy e;EclContext ctx;sim.commands.sim=&sim;sim.current=&e;sim.interactive=true;
  auto op=[&](std::initializer_list<u32> args){std::vector<u8> code;th13test::emit(code,0,609,0,255,0,args);ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());CHECK_EQ(sim.commands.execute(ctx,609),0);};
  op({0,3,0,0x2000,packed,10,float_to_bits(.5f),float_to_bits(2.f)});
  op({0,4,0,0x4000,1,0,float_to_bits(-3.1415927f),float_to_bits(3.1415927f)});
  op({0,6,1,2,1,u32(-999999),float_to_bits(-999999.f),float_to_bits(-999999.f)});
  op({0,7,0,0x100,180,0,float_to_bits(-999999.f),float_to_bits(-999999.f)});
  StageBullet parent;for(int i=0;i<18;++i)parent.ex[i]=e.et[0].ex[i];
  parent.ex_cursor=3;parent.x=0;parent.y=100;sim.process_bullet_queue(parent,false);
  CHECK(sim.error().empty());CHECK_EQ(sim.bullets.size(),size_t(10));
  for(const auto& child:sim.bullets){CHECK_EQ(child.sprite,10u);CHECK_EQ(child.subtype,6u);CHECK_EQ(child.state,2);CHECK_EQ(child.ex_cursor,8);CHECK_EQ(child.behavior_flags,0x100u);}
  //Negative packed input advances3 entries; positive advances2. The negative
  //parent then consumes no-op entry6 and offscreen entry7 too.
  CHECK_EQ(parent.ex_cursor,packed&0x80000000u?8:5);
  CHECK_EQ(parent.state,packed&0x80000000u?3:1);
 }
}
int main(){return th13test::run_all("child bullet packed seed");}
