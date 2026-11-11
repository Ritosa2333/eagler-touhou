#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Dialog:StageDialogHost {
 bool active=true;
 void open_dialog(i32)override{}
 bool dialog_active()const override{return active;}
};
TEST(boss_and_dialog_wait_freeze_script_time_until_release) {
 for(u16 op:{519,520}){
  StageSim sim;StageEnemy caller;Dialog dialog;EclContext ctx;
  sim.commands.sim=&sim;sim.current=&caller;sim.dialog_host=&dialog;
  auto boss=std::make_unique<StageEnemy>();boss->script_id=7;
  sim.enemies.push_back(std::move(boss));sim.boss_slot[0]=7;
  std::vector<u8> code;th13test::emit(code,0,op,0,255,0,{});
  th13test::emit(code,60,0,0,255,0,{});th13test::emit(code,61,1,0,255,0,{});
  CHECK(ctx.bind_code(code.data(),u32(code.size()),0));ctx.commands=&sim.commands;
  for(int frame=0;frame<8;++frame){
   if(frame==5){dialog.active=false;sim.boss_slot[0]=-1;}
   CHECK_EQ(ctx.step(.5f),0);
   CHECK_EQ(ctx.time,frame<5?0.f:float(frame-4)*.5f);
   CHECK_EQ(ctx.instruction_offset,frame<5?0:16);
  }
  // Pending61-timestamp work was not silently consumed while waiting.
  CHECK_EQ(ctx.routine,0);
 }
}
}
int main(){return th13test::run_all("ecl-wait-clock");}
