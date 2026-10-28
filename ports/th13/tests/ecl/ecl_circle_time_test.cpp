#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
 StageSim sim;StageEnemy* e;
 Rig(){std::vector<u8>s;th13test::emit(s,0,23,0,255,0,{1000});th13test::emit(s,0,10,0,255,0,{});auto b=th13test::build_scpt({{"main",s}},{});sim.program.attach(b.data(),u32(b.size()));sim.commands.sim=&sim;sim.gvars.sim=&sim;e=sim.spawn_enemy("main",0,192,100,nullptr,false);e->tick_gate=false;sim.current=e;}
 int cmd(u16 op,std::initializer_list<u32>a){std::vector<u8>b;th13test::emit(b,0,op,0,255,0,a);EclContext c;c.instruction=reinterpret_cast<const EclInstruction*>(b.data());return sim.commands.execute(c,op);}
};
TEST(circle_time_interpolates_radial_speed_instead_of_instantly_shrinking) {
 Rig r;CHECK_EQ(r.cmd(408,{0,0,float_to_bits(64),0}),0);
 CHECK_EQ(r.cmd(409,{120,0,float_to_bits(.017453f),float_to_bits(-999999),float_to_bits(-2)}),0);
 CHECK_EQ(r.e->vel.growth,0.f);CHECK_EQ(r.e->vel.radius,64.f);
 CHECK(r.sim.step_frame());CHECK_EQ(r.e->vel.radius,64.f-2.f/120.f);
 for(int i=1;i<30;++i)CHECK(r.sim.step_frame());
 CHECK_EQ(r.e->vel.radius,63.5f);CHECK_EQ(r.e->vel.growth,-.5f);
 CHECK(r.e->position[0]>60); // old immediate -2 collapses to x~6 here
 for(int i=30;i<120;++i)CHECK(r.sim.step_frame());
 CHECK_EQ(r.e->vel.radius,62.f);CHECK_EQ(r.e->vel.growth,-2.f);
 CHECK(r.sim.step_frame());CHECK_EQ(r.e->vel.radius,60.f);
}
TEST(relative_circle_time_has_independent_radial_channel) {
 Rig r;CHECK_EQ(r.cmd(410,{0,0,float_to_bits(40),float_to_bits(1)}),0);
 CHECK_EQ(r.cmd(411,{60,0,0,float_to_bits(80),float_to_bits(-.5f)}),0);
 for(int i=0;i<30;++i)CHECK(r.sim.step_frame());
 CHECK_EQ(r.e->pos.radius,60.25f);CHECK_EQ(r.e->pos.growth,.25f);
 CHECK_EQ(r.e->vel.radial.duration,0);CHECK_EQ(r.e->position[0],60.25f);
}
TEST(zero_duration_cancels_both_channels_without_changing_circle_state) {
 Rig r;CHECK_EQ(r.cmd(408,{0,0,float_to_bits(64),0}),0);
 CHECK_EQ(r.cmd(409,{120,0,0,float_to_bits(80),float_to_bits(-2)}),0);
 CHECK_EQ(r.cmd(409,{0,0,float_to_bits(.2f),float_to_bits(10),float_to_bits(4)}),0);
 CHECK_EQ(r.e->vel.polar.duration,0.f);CHECK_EQ(r.e->vel.radial.duration,0);
 CHECK_EQ(r.e->vel.radius,64.f);CHECK_EQ(r.e->vel.growth,0.f);
}
TEST(real_stage5_phantom_retains_radius_during_opening) {
 StageSim sim;CHECK(sim.load("artifacts/reference/raw","artifacts/verification/circle-time-20261027/phantom.jsonl","st05.ecl"));StageSpawnArguments a;a.floats={0,64,.017453f,0};
 auto*e=sim.spawn_enemy("Phantom01",0,192,80,nullptr,false,false,&a);CHECK(e);if(!e)return;
 for(int i=0;i<31;++i)CHECK(sim.step_frame());
 CHECK_EQ(e->vel.radius,63.5f);CHECK_EQ(e->vel.growth,-.5f);CHECK(e->position[0]>60);
}
}
int main(){return th13test::run_all("circle-time");}
