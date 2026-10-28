// H-LASER-SPLIT: native433870/4342e0/431bf0/4327c0/4315d0.
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;
    StageEnemy enemy;
    Rig(float length=96) {
        StageEt et;et.reset();et.sprite=8;et.subtype=6;
        et.spawn[0]=et.spawn[1]=length;et.laser_width=16;
        et.laser_timing[0]=20;et.laser_timing[1]=8;
        et.laser_timing[2]=140;et.laser_timing[3]=10;
        sim.emit_infinite_laser(et,enemy,1);
        sim.lasers[0].width=16;
    }
    void box(float x) {sim.split_laser(0,{x,0,0},{32,32,0},0,false,false,true);}
};
TEST(laser_split_preserves_parent_and_moves_tail_to_original_limit) {
    Rig r;r.box(48);
    CHECK_EQ(r.sim.lasers.size(),2u);CHECK_EQ(r.sim.lasers[0].length,32.0f);
    const auto child=r.sim.lasers[1];
    CHECK(child.kind==StageLaser::Kind::Line);CHECK_EQ(child.origin[0],64.0f);
    CHECK_EQ(child.length,32.0f);CHECK_EQ(child.line_limit,32.0f);
    CHECK_EQ(child.length_speed,8.0f);CHECK_EQ(child.id,65538);
    CHECK_EQ(child.cull_timer,30);CHECK_EQ(child.start_offset,0.0f);
    CHECK_EQ(r.sim.anm_effects.size(),2u);
    CHECK_EQ(r.sim.anm_effects[0].x,40.0f);CHECK_EQ(r.sim.anm_effects[1].x,56.0f);
    r.sim.advance_lasers();
    CHECK_EQ(r.sim.lasers[0].length,34.0f); // parent's growth continues
    CHECK_EQ(r.sim.lasers[1].origin[0],72.0f);
    CHECK_EQ(r.sim.lasers[1].length,24.0f);
    CHECK_EQ(r.sim.lasers[1].start_offset,8.0f);
    for(int i=0;i<3;++i)r.sim.advance_lasers();
    CHECK_EQ(r.sim.lasers.size(),1u);
}
TEST(laser_box_and_radius_keep_distinct_endpoint_rules) {
    Rig box(16);box.box(0);CHECK_EQ(box.sim.lasers[0].length,0.0f);
    CHECK_EQ(box.sim.anm_effects.size(),1u);
    Rig circle(16);circle.sim.cancel_lasers_radius({0,0,0},16);
    CHECK_EQ(circle.sim.lasers[0].length,16.0f);CHECK(circle.sim.anm_effects.empty());
    Rig b;b.box(48);Rig c;c.sim.cancel_lasers_radius({48,0,0},16);
    CHECK_EQ(b.sim.lasers.back().length,32.0f);
    CHECK_EQ(c.sim.lasers.back().length,16.0f);
    CHECK_EQ(c.sim.lasers.back().sound,0);CHECK_EQ(b.sim.lasers.back().sound,-1);
}
TEST(laser_split_protection_and_newborn_rectangle_gate) {
    Rig r;r.sim.lasers[0].cancel_protection=10;r.box(48);
    CHECK_EQ(r.sim.lasers.size(),1u);CHECK_EQ(r.sim.lasers[0].length,96.0f);
    r.sim.lasers[0].cancel_protection=0;
    r.sim.cancel_lasers_box({48,0,0},{32,32,0},0);
    CHECK_EQ(r.sim.lasers.size(),1u); // 4300f0 requires +7d
    r.sim.advance_lasers();r.sim.cancel_lasers_box({48,0,0},{32,32,0},0);
    CHECK_EQ(r.sim.lasers.size(),2u);
}
TEST(laser_update_saves_next_before_hit_callback_and_does_not_invalidate_parent) {
    Rig r;r.sim.lasers[0].phase=2;r.sim.player_x=48;r.sim.player_y=0;
    r.sim.laser_collision=[](const StageLaser& l){return l.kind==StageLaser::Kind::Infinite?1:0;};
    r.sim.advance_lasers();
    CHECK_EQ(r.sim.lasers.size(),2u);
    CHECK_EQ(r.sim.lasers[0].time,1);CHECK_EQ(r.sim.lasers[1].time,0);
    CHECK_EQ(r.sim.lasers[1].origin[0],64.0f);
    r.sim.laser_collision={};r.sim.advance_lasers();
    CHECK_EQ(r.sim.lasers[1].time,1);CHECK_EQ(r.sim.lasers[1].origin[0],72.0f);
}
TEST(laser_line_resplit_marks_short_remainders_for_next_traversal) {
    Rig r(160);r.box(0);CHECK_EQ(r.sim.lasers.size(),2u);
    CHECK_EQ(r.sim.lasers[1].length,144.0f);
    r.sim.split_laser(1,{48,0,0},{32,32,0},0,false,false,true);
    CHECK_EQ(r.sim.lasers.size(),3u);
    CHECK_EQ(r.sim.lasers[1].length,16.0f);CHECK(r.sim.lasers[1].pending_remove);
    CHECK_EQ(r.sim.lasers[2].origin[0],64.0f);CHECK_EQ(r.sim.lasers[2].length,96.0f);
    r.sim.advance_lasers();CHECK_EQ(r.sim.lasers.size(),2u);
}
TEST(laser_collision_uses_origin_interval_and_line_tip_margins) {
    Rig r;auto& l=r.sim.lasers[0];
    CHECK_EQ(l.player_contact(-1,0,1,1),1);
    CHECK_EQ(l.player_contact(-1.01f,0,1,1),2);
    CHECK_EQ(l.player_contact(97,0,1,1),1);
    CHECK_EQ(l.player_contact(97.01f,0,1,1),2);
    CHECK_EQ(l.player_contact(48,5,1,1),1);
    CHECK_EQ(l.player_contact(48,5.01f,1,1),2);
    CHECK_EQ(l.player_contact(48,21,1,1),0);
    l.kind=StageLaser::Kind::Line;
    CHECK_EQ(l.player_contact(0,0,1,1),2);
    CHECK_EQ(l.player_contact(48,0,1,1),1);
    CHECK_EQ(l.player_contact(96,0,1,1),2);
}
}
int main(){return th13test::run_all("laser-split");}
