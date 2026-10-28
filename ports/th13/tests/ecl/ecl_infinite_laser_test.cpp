// H-INFINITE-LASER: retail 41e77a/418300/433260/433470/42fcd0.
// Boundary expectations come from literal original EXE probes (graphics,
// audio, allocation and player collision are outside this scalar test).
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"

using namespace th13;
namespace {
struct Rig {
    StageSim sim;
    StageEnemy enemy;
    EclContext context;
    Rig() { sim.commands.sim = &sim; sim.current = &enemy; }
    int run(u16 op, std::initializer_list<u32> args, u16 mask = 0) {
        std::vector<u8> bytes;
        th13test::emit(bytes, 0, op, mask, 255, 0, args);
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context, op);
    }
    void configure(i32 warning=20, i32 opening=8, i32 hold=140, i32 closing=10) {
        CHECK_EQ(run(600, {0}), 0);
        CHECK_EQ(run(700, {0,float_to_bits(480),float_to_bits(480),0,float_to_bits(16)}), 0);
        CHECK_EQ(run(701, {0,u32(warning),u32(opening),u32(hold),u32(closing),9}), 0);
    }
    void step(int count) { for (int i=0;i<count;++i) sim.advance_lasers(); }
};

TEST(infinite_laser_ids_are_labels_and_generation_is_birth_identity) {
    Rig r; r.configure();
    CHECK_EQ(r.run(703, {0,257}), 0);
    CHECK_EQ(r.run(703, {0,257}), 0);
    CHECK_EQ(r.run(703, {0,0}), 0);
    CHECK_EQ(r.sim.lasers.size(), 3u);
    CHECK_EQ(r.sim.lasers[0].generation, 0x10001u);
    CHECK_EQ(r.sim.lasers[1].generation, 0x10002u);
    CHECK_EQ(r.run(708, {257,float_to_bits(8)}), 0);
    CHECK_EQ(r.run(708, {0,float_to_bits(10)}), 0);
    CHECK_EQ(r.sim.lasers[0].angle, 8.0f); // raw write; wrapping occurs at update
    CHECK_EQ(r.sim.lasers[1].angle, 0.0f);
    CHECK_EQ(r.sim.lasers[2].angle, 0.0f);
    CHECK(r.sim.find_laser(0)==nullptr);
    // Invalid variable source must not be read if the global ID is absent.
    CHECK_EQ(r.run(708, {999,float_to_bits(-12345)}, 2), 0);
    CHECK(r.context.error.empty());
    CHECK_EQ(r.run(708, {257,float_to_bits(-12345)}, 2), -2);
}

TEST(infinite_laser_capacity_does_not_wrap_ids_or_overwrite_survivors) {
    Rig r; r.configure();
    for (u32 i=0;i<257;++i) CHECK_EQ(r.run(703, {0,i+1}), 0);
    CHECK_EQ(r.sim.lasers.size(), 256u);
    CHECK_EQ(r.sim.laser_serial, 0x10100u);
    CHECK_EQ(r.sim.lasers.front().id, 1);
    CHECK_EQ(r.sim.lasers.back().id, 256);
    r.sim.lasers.front().phase=1;
    CHECK_EQ(r.run(703, {0,999}), 0); // pending retirement still counts
    CHECK_EQ(r.sim.lasers.size(), 256u);
    r.step(1);
    CHECK_EQ(r.run(703, {0,999}), 0);
    CHECK_EQ(r.sim.lasers.size(), 256u);
    CHECK_EQ(r.sim.lasers.front().id, 2);
    CHECK_EQ(r.sim.lasers.back().id, 999);
    CHECK_EQ(r.sim.lasers.back().generation, 0x10101u);
}

TEST(infinite_laser_phase_clock_matches_original_boundary_frames) {
    Rig r; r.configure(); CHECK_EQ(r.run(703, {0,1}), 0);
    auto& l=r.sim.lasers.front();
    CHECK_EQ(l.time,0); CHECK_EQ(l.previous_time,-1); CHECK_EQ(l.width,2.0f);
    r.step(20); CHECK_EQ(l.phase,3); CHECK_EQ(l.time,20);
    r.step(1); CHECK_EQ(l.phase,4); CHECK_EQ(l.time,1); CHECK_EQ(l.width,2.0f);
    CHECK(l.collision_enabled()); CHECK_EQ(l.collision_width(),1.0f);
    r.step(7); CHECK_EQ(l.phase,4); CHECK_EQ(l.time,8); CHECK_EQ(l.width,14.0f);
    r.step(1); CHECK_EQ(l.phase,2); CHECK_EQ(l.time,1); CHECK_EQ(l.width,16.0f);
    r.step(139); CHECK_EQ(l.phase,2); CHECK_EQ(l.time,140);
    r.step(1); CHECK_EQ(l.phase,5); CHECK_EQ(l.time,1); CHECK_EQ(l.width,16.0f);
    CHECK(!l.collision_enabled());
    r.step(9); CHECK_EQ(l.phase,5); CHECK_EQ(l.time,10);
    CHECK_EQ(float_to_bits(l.width),float_to_bits(1.6f));
    r.step(1); CHECK(r.sim.lasers.empty());
}

TEST(infinite_laser_zero_durations_preserve_warning_transition) {
    Rig r; r.configure(0,0,0,0); CHECK_EQ(r.run(703,{0,1}),0);
    r.step(1); CHECK_EQ(r.sim.lasers.front().phase,4);
    CHECK_EQ(r.sim.lasers.front().width,2.0f);
    r.step(1); CHECK(r.sim.lasers.empty());
}

TEST(infinite_laser_growth_follow_and_queue_are_independent_of_width) {
    Rig r; r.configure();
    r.enemy.position[0]=96;r.enemy.position[1]=144;r.enemy.position[2]=3;
    CHECK_EQ(r.run(603,{0,float_to_bits(2),float_to_bits(-4)}),0);
    CHECK_EQ(r.run(700,{0,float_to_bits(17),float_to_bits(96),0,float_to_bits(37)}),0);
    CHECK_EQ(r.run(605,{0,float_to_bits(8),0}),0);
    CHECK_EQ(r.run(609,{0,0,0,0x100000,0,0,0,0}),0);
    CHECK_EQ(r.run(608,{0,18,u32(-1)}),0);
    CHECK_EQ(r.run(703,{0,1}),0);
    auto& l=r.sim.lasers.front();
    CHECK_EQ(l.origin[0],98.0f);CHECK_EQ(l.origin[1],140.0f);CHECK_EQ(l.origin[2],3.0f);
    CHECK_EQ(l.sound,18);CHECK_EQ(l.transform_sound,-1);
    CHECK_EQ(l.length,17.0f);CHECK_EQ(l.target_length,96.0f);
    CHECK_EQ(r.run(600,{0}),0); // existing laser owns a snapshot
    r.step(1);CHECK_EQ(l.length,25.0f);CHECK_EQ(l.width,2.0f);
    CHECK_EQ(l.ex_cursor,1);CHECK_EQ(l.anm_flags & 0x1e0u,0u);
    auto other=std::make_unique<StageEnemy>();other->script_id=10;
    other->position[0]=23;other->position[1]=41;other->position[2]=7;
    r.sim.boss_slot[1]=10;r.sim.enemies.push_back(std::move(other));
    r.step(1);CHECK_EQ(l.origin[0],98.0f); // slot1 is not followed
    r.sim.boss_slot[0]=10;r.step(1);CHECK_EQ(l.origin[0],23.0f);CHECK_EQ(l.origin[1],41.0f);
    r.sim.enemies.front()->dead=true;r.step(1);CHECK_EQ(l.origin[0],23.0f);
    CHECK_EQ(r.sim.lasers.size(),1u); // emitter/boss lifetime does not own laser
    r.step(6);CHECK_EQ(l.length,96.0f);CHECK_EQ(l.width,2.0f);
}

TEST(infinite_laser_absolute_origin_and_radial_offset) {
    Rig r;r.configure();r.enemy.position[0]=100;r.enemy.position[1]=200;
    auto& et=r.enemy.et[0];et.absolute[0]=12;et.absolute[1]=34;et.absolute[2]=1;
    et.offset[0]=5;et.offset[1]=-4;et.offset[2]=9;et.spin=8;
    CHECK_EQ(r.run(703,{0,1}),0);
    CHECK_EQ(r.sim.lasers.front().origin[0],25.0f);
    CHECK_EQ(r.sim.lasers.front().origin[1],30.0f);
    CHECK_EQ(r.sim.lasers.front().origin[2],0.0f);
}
TEST(infinite_laser_cancel_removes_protection_and_spawns_original_effect_segments) {
    for (i32 length : {0,16,17,32,33,480}) {
        Rig r;r.configure();r.enemy.et[0].sprite=8;r.enemy.et[0].subtype=6;
        r.enemy.et[0].spawn[0]=r.enemy.et[0].spawn[1]=float(length);
        CHECK_EQ(r.run(703,{0,1}),0);
        r.sim.lasers.front().cancel_protection=20;
        CHECK_EQ(r.run(545,{}),0);
        CHECK_EQ(r.sim.lasers.front().cancel_protection,0);
        CHECK_EQ(r.sim.lasers.front().phase,1);
        const size_t expected=length<=16?0:length<=32?1:length==33?2:13;
        CHECK_EQ(r.sim.anm_effects.size(),expected);
        for(size_t n=0;n<r.sim.anm_effects.size();++n) {
            const auto& effect=r.sim.anm_effects[n];
            CHECK_EQ(effect.file,0);CHECK_EQ(effect.script,175);
            CHECK_EQ(effect.x,8.0f+16.0f*float(n));CHECK_EQ(effect.y,0.0f);
        }
        r.step(1);CHECK(r.sim.lasers.empty());
    }
}
TEST(infinite_laser_clear_preserves_protection_and_skips_already_canceled) {
    Rig r;r.configure();r.enemy.et[0].sprite=8;r.enemy.et[0].subtype=6;
    r.enemy.et[0].spawn[0]=r.enemy.et[0].spawn[1]=33;
    CHECK_EQ(r.run(703,{0,1}),0);
    r.sim.lasers.front().cancel_protection=20;
    CHECK_EQ(r.run(613,{}),0);
    CHECK_EQ(r.sim.lasers.front().phase,1);
    CHECK_EQ(r.sim.lasers.front().cancel_protection,20);
    CHECK_EQ(r.sim.anm_effects.size(),2u);
    CHECK_EQ(r.run(613,{}),0);
    CHECK_EQ(r.sim.anm_effects.size(),2u);
    CHECK_EQ(r.enemy.et[0].laser_width,16.0f);
}

TEST(laser_position_velocity_speed_width_and_rotation_are_distinct_fields) {
    Rig r;r.configure(20,8,140,10);CHECK_EQ(r.run(703,{0,7}),0);
    CHECK_EQ(r.run(704,{7,float_to_bits(10),float_to_bits(20)}),0);
    CHECK_EQ(r.run(705,{7,float_to_bits(2),float_to_bits(-3)}),0);
    CHECK_EQ(r.run(706,{7,float_to_bits(9)}),0);
    CHECK_EQ(r.run(707,{7,float_to_bits(24)}),0);
    CHECK_EQ(r.run(709,{7,float_to_bits(.25f)}),0);
    auto& laser=r.sim.lasers.front();
    CHECK_EQ(laser.target_width,16.f);CHECK_EQ(laser.width,24.f);
    CHECK_EQ(laser.length_speed,9.f);CHECK_EQ(laser.velocity[0],2.f);
    CHECK_EQ(laser.velocity[1],-3.f);CHECK_EQ(laser.velocity[2],0.f);
    r.step(1);
    CHECK_EQ(laser.origin[0],12.f);CHECK_EQ(laser.origin[1],17.f);
    CHECK_EQ(laser.angle,.25f);CHECK_EQ(laser.width,24.f);
    // Native 4182b0's raw descriptor offsets have different meanings on lines.
    laser.kind=StageLaser::Kind::Line;laser.velocity[0]=7;
    CHECK_EQ(r.run(705,{7,float_to_bits(.75f),float_to_bits(64)}),0);
    CHECK_EQ(laser.line_descriptor_angle,.75f);CHECK_EQ(laser.target_length,64.f);
    CHECK_EQ(laser.line_initial_length,0.f);CHECK_EQ(laser.velocity[0],7.f);
    CHECK_EQ(r.run(709,{7,float_to_bits(32)}),0);
    CHECK_EQ(laser.target_width,32.f);CHECK_EQ(laser.width,24.f);
}

TEST(laser_control_argument_reads_match_native_missing_id_branches) {
    for(u16 op:{704,705}) {
        Rig r;
        CHECK_EQ(r.run(op,{999,float_to_bits(-12345),0},2),-2);
        CHECK(!r.context.error.empty());
    }
    for(u16 op:{706,707,709,714}) {
        Rig r;r.configure();
        const u32 ref=op==714?u32(-12345):float_to_bits(-12345);
        CHECK_EQ(r.run(op,{999,ref},2),0);
        CHECK(r.context.error.empty());
        CHECK_EQ(r.run(703,{0,7}),0);
        CHECK_EQ(r.run(op,{7,ref},2),-2);
    }
}

TEST(laser_cancel_id_clears_all_duplicates_and_preserves_unrelated_lasers) {
    Rig r;r.configure();r.enemy.et[0].sprite=8;r.enemy.et[0].subtype=6;
    r.enemy.et[0].spawn[0]=r.enemy.et[0].spawn[1]=33;
    for(u32 id:{7,8,7,0})CHECK_EQ(r.run(703,{0,id}),0);
    for(auto& laser:r.sim.lasers)laser.cancel_protection=20;
    CHECK_EQ(r.run(710,{7}),0);
    CHECK_EQ(r.sim.lasers.size(),4u); // removal belongs to the laser manager
    CHECK_EQ(r.sim.lasers[0].id,0);CHECK_EQ(r.sim.lasers[2].id,0);
    CHECK_EQ(r.sim.lasers[0].phase,1);CHECK_EQ(r.sim.lasers[2].phase,1);
    CHECK_EQ(r.sim.lasers[1].phase,3);CHECK_EQ(r.sim.lasers[3].phase,3);
    CHECK_EQ(r.sim.lasers[0].cancel_protection,20);
    CHECK_EQ(r.sim.anm_effects.size(),4u);
    CHECK_EQ(r.run(710,{7}),0);CHECK_EQ(r.run(710,{0}),0);
    CHECK_EQ(r.sim.anm_effects.size(),4u);
    r.step(1);CHECK_EQ(r.sim.lasers.size(),2u);
}

TEST(line_laser_ecl702_growth_acceleration_and_completion_match_native) {
    Rig r;CHECK_EQ(r.run(600,{0}),0);
    CHECK_EQ(r.run(602,{0,20,7}),0);
    CHECK_EQ(r.run(604,{0,float_to_bits(1.5707964f),0}),0);
    CHECK_EQ(r.run(605,{0,float_to_bits(1),0}),0);
    CHECK_EQ(r.run(700,{0,0,float_to_bits(128),0,float_to_bits(14)}),0);
    CHECK_EQ(r.run(609,{0,0,0,4,120,u32(-999999),float_to_bits(.033333335f),float_to_bits(-999999)}),0);
    CHECK_EQ(r.run(702,{0}),0);CHECK_EQ(r.sim.lasers.size(),1u);
    auto& laser=r.sim.lasers.front();
    CHECK(laser.kind==StageLaser::Kind::Line);CHECK_EQ(laser.id,0x10001);
    CHECK_EQ(laser.width,14.f);CHECK_EQ(laser.length,0.f);
    CHECK_EQ(laser.cull_timer,30);CHECK_EQ(laser.ex_cursor,0);
    r.step(60);
    CHECK_EQ(float_to_bits(laser.length),1123155960u);
    CHECK_EQ(float_to_bits(laser.length_speed),1077936120u);
    CHECK_EQ(float_to_bits(laser.velocity[0]),3020738022u);
    CHECK_EQ(laser.origin[1],0.f);CHECK_EQ(laser.behavior_flags,4u);
    r.step(61);
    CHECK_EQ(laser.length,128.f);CHECK_EQ(laser.behavior_flags,0u);
    CHECK_EQ(float_to_bits(laser.origin[1]),1131203780u);
    CHECK_EQ(float_to_bits(laser.length_speed),1084227576u);
}

TEST(line_laser_travel_limit_retires_after_tail_reaches_limit) {
    Rig r;r.configure();CHECK_EQ(r.run(700,{0,float_to_bits(128),float_to_bits(128),float_to_bits(256),float_to_bits(14)}),0);
    CHECK_EQ(r.run(605,{0,float_to_bits(8),0}),0);
    CHECK_EQ(r.run(604,{0,float_to_bits(1.5707964f),0}),0);
    CHECK_EQ(r.run(702,{0}),0);r.step(30);
    CHECK_EQ(r.sim.lasers.size(),1u);CHECK_EQ(r.sim.lasers.front().length,16.f);
    r.step(2);CHECK(r.sim.lasers.empty());
}

// Retail41eaf3 ->42fed0(2) is711. 713 goes to LaserBeamInf (type3),
// and its old curve expectation tested the wrong dispatch. Keep32 samples
// here by supplying the actual701 count rather than assuming a constant.
TEST(curve_laser_711_creates_time_segmented_head_and_tail_samples) {
    Rig r;CHECK_EQ(r.run(600,{0}),0);CHECK_EQ(r.run(602,{0,20,7}),0);
    CHECK_EQ(r.run(604,{0,float_to_bits(0),0}),0);
    CHECK_EQ(r.run(605,{0,float_to_bits(4),0}),0);
    CHECK_EQ(r.run(700,{0,float_to_bits(0),float_to_bits(128),0,float_to_bits(14)}),0);
    CHECK_EQ(r.run(701,{0,32,u32(-1),u32(-1),u32(-1),0}),0);
    r.enemy.et[0].ex[0].op=4;r.enemy.et[0].ex[0].args[0]=-.09f;r.enemy.et[0].ex[0].args[1]=-999999.f;
    r.enemy.et[0].ex[0].args[4]=40;r.enemy.et[0].ex[0].args[5]=0;
    CHECK_EQ(r.run(711,{0}),0);CHECK_EQ(r.sim.lasers.size(),1u);
    if(r.sim.lasers.empty())return;
    auto& curve=r.sim.lasers.front();CHECK(curve.kind==StageLaser::Kind::Curve);
    CHECK_EQ(curve.curve_path.segments.size(),1u);CHECK_EQ(curve.curve_samples.size(),32u);
    r.step(1);CHECK_EQ(curve.curve_path.segments.size(),3u);
    r.step(1);
    CHECK(curve.curve_samples[0].position.x!=curve.curve_samples.back().position.x||
          curve.curve_samples[0].position.y!=curve.curve_samples.back().position.y);
    CHECK(r.sim.error().empty());
}
}
int main(){return th13test::run_all("infinite-laser-scalars");}
