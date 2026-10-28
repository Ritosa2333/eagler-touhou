// H-CURVE-PRODUCTION: 41eaf3/434f10/4366c0/4355f0.
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim; StageEnemy enemy; EclContext context;
    Rig(){sim.commands.sim=&sim;sim.current=&enemy;}
    int run(u16 op,std::initializer_list<u32> args) {
        std::vector<u8> bytes;th13test::emit(bytes,0,op,0,255,0,args);
        context.instruction=reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context,op);
    }
    void setup(i32 count) {
        CHECK_EQ(run(600,{0}),0);CHECK_EQ(run(602,{0,0,4}),0);
        CHECK_EQ(run(604,{0,0,0}),0);CHECK_EQ(run(605,{0,float_to_bits(4),0}),0);
        CHECK_EQ(run(700,{0,float_to_bits(-1),float_to_bits(-1),float_to_bits(-1),float_to_bits(14)}),0);
        CHECK_EQ(run(701,{0,u32(count),u32(-1),u32(-1),u32(-1),0}),0);
    }
};
TEST(curve_711_uses_timing_count_and_absolute_radial_origin) {
    for(i32 count:{30,40,64}) {
        Rig r;r.setup(count);r.enemy.position[0]=100;r.enemy.position[1]=200;
        auto& et=r.enemy.et[0];et.offset[0]=3;et.offset[1]=5;
        et.absolute[0]=10;et.absolute[1]=20;et.absolute[2]=1;et.spin=8;
        CHECK_EQ(r.run(711,{0}),0);CHECK_EQ(r.sim.lasers.size(),1u);
        if(r.sim.lasers.empty())continue;
        const auto& laser=r.sim.lasers[0];CHECK_EQ(laser.curve_samples.size(),size_t(count));
        CHECK_EQ(laser.id,i32(laser.generation));CHECK_EQ(laser.origin[0],21.f);
        CHECK_EQ(laser.origin[1],25.f);CHECK_EQ(laser.origin[2],0.f);
        for(const auto& point:laser.curve_samples) {
            CHECK_EQ(point.position.x,21.f);CHECK_EQ(point.position.y,25.f);
            CHECK_EQ(point.speed,4.f);
        }
    }
}
TEST(curve_queue_distinguishes_acceleration_and_angular_segments) {
    for(u32 op:{4u,8u}) {
        Rig r;r.setup(64);auto& ex=r.enemy.et[0].ex[0];
        ex.op=op;ex.flags=1;ex.args[4]=40;ex.args[5]=0;
        ex.args[0]=-.09f;ex.args[1]=op==4?-999999.f:0;
        CHECK_EQ(r.run(711,{0}),0);r.sim.advance_lasers();
        CHECK(r.sim.error().empty());const auto& segments=r.sim.lasers[0].curve_path.segments;
        CHECK_EQ(segments.size(),3u);CHECK_EQ(segments[1].mode,op==4?1:2);
        CHECK_EQ(segments[1].end_time,40.f);CHECK_EQ(segments[2].mode,0);
    }
}
TEST(curve_offscreen_retirement_releases_birth_identity) {
    Rig r;r.setup(30);r.enemy.position[1]=200;int births=0,retired=0;
    r.sim.laser_birth=[&](StageLaser&){++births;return true;};
    r.sim.laser_retire=[&](u32){++retired;};CHECK_EQ(r.run(711,{0}),0);
    for(int i=0;i<29;++i)r.sim.advance_lasers();CHECK_EQ(retired,0);
    for(int i=0;i<100;++i)r.sim.advance_lasers();
    CHECK_EQ(births,1);CHECK_EQ(retired,1);CHECK(r.sim.lasers.empty());
}
TEST(curve_samples_follow_fractional_path_time_during_slowdown) {
    Rig r;r.setup(30);r.enemy.position[1]=200;r.sim.set_time_rate(.25f);
    CHECK_EQ(r.run(711,{0}),0);
    // Original435b80 mode0: origin + (direction*time)*speed, speed4.
    for(int frame=0;frame<12;++frame){
        r.sim.advance_lasers();CHECK_EQ(r.sim.lasers.size(),size_t(1));
        const auto& laser=r.sim.lasers[0];
        CHECK_EQ(laser.curve_samples[0].position.x,float(frame));
        CHECK_EQ(laser.curve_samples[0].position.y,200.f);
        const float tail=frame<4?0.f:float(frame-4);
        CHECK_EQ(laser.curve_samples[1].position.x,tail);
        CHECK_EQ(laser.time_fraction,float(frame+1)*.25f);
    }
}
TEST(real_stage3_card_emits_curve_lasers_through_711) {
    StageSim sim;CHECK(sim.load("artifacts/reference/raw",
        "artifacts/verification/curve-production-20261027/real-script-events.jsonl","st03.ecl"));
    if(!sim.error().empty())return;
    // BossCard3 sets I1 to16 before calling this emission routine on Normal.
    StageSpawnArguments args;args.integers[1]=16;
    sim.spawn_enemy("BossCard3_at",0,128,100,nullptr,false,false,&args);
    CHECK(sim.error().empty());CHECK(!sim.lasers.empty());
    for(const auto& laser:sim.lasers) {
        CHECK(laser.kind==StageLaser::Kind::Curve);CHECK_EQ(laser.curve_samples.size(),64u);
    }
    sim.advance_lasers();CHECK(sim.error().empty());
    for(const auto& laser:sim.lasers)CHECK_EQ(laser.curve_path.segments.size(),5u);
}
}
int main(){return th13test::run_all("curve-laser-production");}
