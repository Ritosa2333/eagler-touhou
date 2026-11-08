#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include <cstdlib>
using namespace th13;
TEST(random_velocity_operands_are_evaluated_once_and_trace_observes_the_same_values) {
    for(u16 op:{404,406,428,430}) for(bool mirrored:{false,true}) {
        StageSim sim;StageEnemy enemy;EclContext ctx;
        sim.commands.sim=&sim;sim.gvars.sim=&sim;sim.current=&enemy;ctx.variables=&sim.gvars;
        sim.random.reseed_restore(51548);
        enemy.flags=mirrored?0x100000u:0u;
        const char* trace=std::getenv("TH13_MOVEMENT_EVENT_TEST");
        if(trace)CHECK(sim.sink.open(trace));
        std::vector<u8> code;th13test::emit(code,0,op,1,255,0,{float_to_bits(-9998.f),float_to_bits(.4f)});
        ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(ctx,op),0);
        const auto& mover=op==404||op==428?enemy.vel:enemy.pos;
        // Actual4772a0 ->41b260 ->478ee0 ->420d00 from seed51548.
        CHECK_EQ(float_to_bits(mover.angle),mirrored&&(op==404||op==406)?1041519904u:1077922441u);
        CHECK_EQ(float_to_bits(mover.speed),1053609165u);
        CHECK_EQ(sim.random.seed(),25616);CHECK_EQ(sim.random.index(),2u);
        sim.sink.close();CHECK(sim.sink.ok());
        if(trace)CHECK_EQ(sim.sink.emitted(),1u);
    }
}
int main(){return th13test::run_all("movement-rng");}
