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
  // 0x40f865 在 memset(temp,0,0x380) 之后显式写 temp+0x374=-1
  // （transform_sound）：子发射弹 +0xbcc 恒 -1，0x40f1b5 外观音门
  // （[0xbd0]!=0 && [0xbcc]>=0）不通过。原生 S5 f11265-11269 的
  // 10 颗/帧 sprite=0x6000a 子发射弹 bcc=-1 实证。
  for(const auto& child:sim.bullets){CHECK_EQ(child.sprite,10u);CHECK_EQ(child.subtype,6u);CHECK_EQ(child.state,2);CHECK_EQ(child.ex_cursor,8);CHECK_EQ(child.behavior_flags,0x100u);CHECK_EQ(child.transform_sound,-1);}
  //Negative packed input advances3 entries; positive advances2. The negative
  //parent then consumes no-op entry6 and offscreen entry7 too.
  CHECK_EQ(parent.ex_cursor,packed&0x80000000u?8:5);
  CHECK_EQ(parent.state,packed&0x80000000u?3:1);
 }
}
int main(){return th13test::run_all("child bullet packed seed");}
