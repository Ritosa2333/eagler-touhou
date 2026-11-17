#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/AnmVm.hpp"
using namespace th13;
TEST(ecl_328_mutates_live_alpha_channel_with_original_byte_conversion) {
    StageSim sim;StageEnemy enemy;AnmVm vm;EclContext ctx;
    sim.commands.sim=&sim;sim.current=&enemy;vm.visual.color=0x7f112233;
    sim.enemy_animation=[&](StageEnemy& e,i32 slot){CHECK(&e==&enemy);CHECK_EQ(slot,2);return &vm;};
    std::vector<u8> code;th13test::emit(code,0,328,0,255,0,{2,30,4,300});
    ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx,328),0);
    CHECK_EQ(vm.interpolators.alpha.duration,30);CHECK_EQ(vm.interpolators.alpha.mode,4);
    CHECK_EQ(vm.interpolators.alpha.start[0],127);CHECK_EQ(vm.interpolators.alpha.end[0],44);
    CHECK_EQ(vm.interpolators.alpha.timer.current,0);
    CHECK_EQ(vm.visual.color,0x7f112233u); // no implicit animation tick
}
TEST(ecl_328_missing_slot_does_not_read_remaining_arguments) {
    StageSim sim;StageEnemy enemy;EclContext ctx;sim.commands.sim=&sim;sim.current=&enemy;
    sim.enemy_animation=[](StageEnemy&,i32)->AnmVm*{return nullptr;};
    std::vector<u8> code;th13test::emit(code,0,328,8,255,0,{0,30,4,u32(-12345)});
    ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx,328),0);CHECK(ctx.error.empty());
}
int main(){return th13test::run_all("ecl-anm-alpha");}
