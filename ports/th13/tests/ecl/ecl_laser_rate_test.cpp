#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(infinite_laser_scales_motion_and_preserves_fractional_phase_time) {
 StageSim sim;sim.set_time_rate(.25f);
 StageLaser laser;laser.length=17;laser.target_length=96;laser.length_speed=1;
 laser.width=37;laser.target_width=37;laser.rotation=.03f;
 laser.velocity[0]=.3f;laser.velocity[1]=-.2f;laser.origin[1]=200;
 laser.timing[0]=2;laser.timing[1]=7;laser.timing[2]=3;laser.timing[3]=9;
 sim.lasers.push_back(laser);
 for(int n=0;n<8;++n)sim.advance_lasers();
 CHECK_EQ(sim.lasers[0].phase,3);CHECK_EQ(sim.lasers[0].time,2);
 CHECK_EQ(sim.lasers[0].time_fraction,2.f);CHECK_EQ(sim.lasers[0].length,19.f);
 sim.advance_lasers();CHECK_EQ(sim.lasers[0].phase,4);
 CHECK_EQ(sim.lasers[0].time,0);CHECK_EQ(sim.lasers[0].time_fraction,.25f);
 CHECK_EQ(sim.lasers[0].width,37.f); // transition does not fall through
 sim.advance_lasers();CHECK_EQ(sim.lasers[0].width,float(37.*.25/7.));
}
TEST(line_laser_acceleration_and_wait_use_fractional_rate) {
 StageSim sim;sim.set_time_rate(.5f);StageLaser laser;laser.kind=StageLaser::Kind::Line;
 laser.origin[1]=200;laser.target_length=128;laser.length_speed=1;laser.velocity[0]=1;
 laser.ex[0].op=4;laser.ex[0].args[0]=.25f;laser.ex[0].args[1]=0;laser.ex[0].args[4]=2;
 laser.ex[1].op=0x80000000u;laser.ex[1].args[4]=3;
 sim.lasers.push_back(laser);
 for(int n=0;n<4;++n)sim.advance_lasers();
 CHECK_EQ(sim.lasers[0].acceleration_time,2);CHECK_EQ(sim.lasers[0].length_speed,1.5f);
 CHECK_EQ(sim.lasers[0].behavior_flags,4u);
 sim.advance_lasers();CHECK_EQ(sim.lasers[0].behavior_flags,0x80000000u);
 CHECK_EQ(sim.lasers[0].wait_remaining,2);CHECK_EQ(sim.lasers[0].wait_fraction,2.5f);
 for(int n=0;n<5;++n)sim.advance_lasers();
 CHECK_EQ(sim.lasers[0].behavior_flags,0u);
}
int main(){return th13test::run_all("laser-rate");}
