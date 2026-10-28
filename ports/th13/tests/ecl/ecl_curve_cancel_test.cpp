// H-CURVE-CANCEL: 4371b0/4378d0/437b80 retail component captures.
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;
    Rig(){StageEt et;et.reset();et.sound=-1;et.sprite=0;et.subtype=4;et.angle1=0;et.speed1=4;et.laser_width=14;et.laser_timing[0]=16;
        StageEnemy e;e.position[0]=-100;e.position[1]=128;sim.emit_curve_laser(et,e);
        for(int i=0;i<40;++i)sim.advance_lasers();}
};
TEST(curve_rectangle_splits_with_independent_path_clock_and_birth) {
    Rig r;r.sim.split_laser(0,{28,128,0},{8,32,0},0,false,true,true);
    CHECK_EQ(r.sim.lasers.size(),2u);if(r.sim.lasers.size()!=2)return;
    CHECK_EQ(r.sim.lasers[0].curve_samples.size(),6u);
    CHECK_EQ(r.sim.lasers[1].curve_samples.size(),7u);
    CHECK_EQ(r.sim.lasers[1].time,31);CHECK_EQ(r.sim.lasers[1].ex_cursor,99);
    CHECK_EQ(r.sim.lasers[1].curve_samples[0].position.x,24.f);
    CHECK(r.sim.lasers[0].generation!=r.sim.lasers[1].generation);
    CHECK_EQ(r.sim.anm_effects.size(),1u);CHECK_EQ(r.sim.anm_effects[0].x,32.f);
    r.sim.advance_lasers();CHECK_EQ(r.sim.lasers[1].time,32);
    CHECK_EQ(r.sim.lasers[1].curve_samples[0].position.x,24.f);
}
TEST(curve_circle_uses_sample_compaction_without_clock_rewind_or_children) {
    Rig r;r.sim.split_laser(0,{28,128,0},{8,0,0},0,true,false,true);
    CHECK_EQ(r.sim.lasers.size(),1u);auto& l=r.sim.lasers[0];
    CHECK_EQ(l.curve_samples.size(),6u);CHECK_EQ(l.time,40);
    CHECK_EQ(l.curve_samples[0].position.x,16.f);CHECK_EQ(r.sim.anm_effects.size(),2u);
    r.sim.advance_lasers();CHECK_EQ(l.curve_samples[0].position.x,60.f);
}
TEST(curve_full_cancel_effects_sample_every_third_point_even_outside_screen) {
    Rig r;auto& l=r.sim.lasers[0];
    for(auto& point:l.curve_samples)point.position.x+=500;
    CHECK(r.sim.cancel_laser(l));CHECK_EQ(l.phase,1);CHECK_EQ(r.sim.anm_effects.size(),6u);
    CHECK_EQ(r.sim.anm_effects[0].script,171);CHECK_EQ(r.sim.anm_effects[0].x,556.f);
    r.sim.advance_lasers();CHECK(r.sim.lasers.empty());
}
TEST(curve_hit_callback_can_append_without_updating_new_tail_in_same_traversal) {
    Rig r;int calls=0;r.sim.player_x=36;r.sim.player_y=128;
    r.sim.laser_collision=[&](const StageLaser&){return ++calls==1?1:0;};
    r.sim.advance_lasers();CHECK(r.sim.error().empty());CHECK_EQ(r.sim.lasers.size(),2u);
    for(const auto& l:r.sim.lasers)if(l.generation!=0x10001u)CHECK(!l.has_updated);
}
}
int main(){return th13test::run_all("curve-cancel");}
