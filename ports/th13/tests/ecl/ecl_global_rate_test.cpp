#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/SessionState.hpp"
using namespace th13;
namespace {
void attach(StageSim& sim,const std::vector<u8>& script) {
    const auto data=th13test::build_scpt({{"main",script}},{});
    CHECK_EQ(sim.program.attach(data.data(),u32(data.size())),0);
    sim.commands.sim=&sim;sim.gvars.sim=&sim;
}
TEST(global_rate_ecl_keeps_the_call_snapshot_but_updates_the_enemy_tail) {
    StageSim sim;SessionState session;sim.session=&session;
    std::vector<u8> script;
    th13test::emit(script,0,547,0,255,0,{float_to_bits(.5f)});
    th13test::emit(script,1,547,0,255,0,{float_to_bits(.25f)});
    th13test::emit(script,2,547,0,255,0,{float_to_bits(1.f)});
    th13test::emit(script,100,1,0,255,0,{});
    attach(sim,script);
    // Independent4772a0/41f242 samples, including ECL547's own frame.
    const float times[]={1,1.5f,1.75f,2,2.25f,3.25f,4.25f,5.25f,6.25f};
    const float rates[]={.5f,.25f,.25f,.25f,1,1,1,1,1};
    auto* e=sim.spawn_enemy("main",0,100,100,nullptr,false);CHECK(e);if(!e)return;
    float age=.5f;
    for(int i=0;i<9;++i) {
        if(i){e->tick_gate=false;CHECK(sim.step_frame());age+=rates[i];}
        CHECK_EQ(e->threads->main.time,times[i]);
        CHECK_EQ(session.rate,rates[i]);CHECK_EQ(e->age,age);
    }
}
TEST(global_rate_fractional_enemy_movement_and_invulnerability) {
    StageSim sim;sim.set_time_rate(.25f);
    std::vector<u8> script;th13test::emit(script,0,23,0,255,0,{100});
    th13test::emit(script,0,10,0,255,0,{});
    attach(sim,script);
    auto* e=sim.spawn_enemy("main",0,100,100,nullptr,false);CHECK(e);if(!e)return;
    e->vel.speed=4;e->vel.angle=0;e->tick_gate=false;
    e->invulnerable=3;e->invulnerable_clock.set(3);
    for(int i=0;i<4;++i){CHECK(sim.step_frame());CHECK_EQ(e->position[0],float(i+1));}
    CHECK_EQ(e->invulnerable,2);CHECK_EQ(e->invulnerable_clock.fractional,2.f);
    e->vel.postime.start[0]=4;e->vel.postime.start[1]=100;
    e->vel.postime.target[0]=12;e->vel.postime.target[1]=100;
    e->vel.postime.duration=2;e->vel.postime.elapsed=0;
    for(int i=0;i<8;++i){CHECK(sim.step_frame());CHECK_EQ(e->position[0],float(5+i));}
    CHECK_EQ(e->vel.postime.duration,0.f);
}
TEST(enemy_body_invulnerability_uses_its_own_fractional_clock) {
    StageSim sim; sim.set_time_rate(.25f);
    std::vector<u8> script;
    th13test::emit(script,0,541,0,255,0,{3});
    th13test::emit(script,0,23,0,255,0,{100});
    th13test::emit(script,0,10,0,255,0,{});
    attach(sim,script);
    auto* e=sim.spawn_enemy("main",0,100,100,nullptr,false);CHECK(e);if(!e)return;
    CHECK_EQ(e->body_invulnerable,2);
    CHECK_EQ(e->body_invulnerable_clock.fractional,2.75f);
    CHECK_EQ(e->invulnerable,1);
    e->tick_gate=false;
    for(int i=0;i<3;++i)CHECK(sim.step_frame());
    CHECK_EQ(e->body_invulnerable,2);
    CHECK_EQ(e->body_invulnerable_clock.fractional,2.f);
    CHECK_EQ(e->invulnerable,1);
}
TEST(global_rate_does_not_quantize_or_clamp_ecl_arguments) {
    StageSim sim;StageEnemy e;EclContext context;sim.commands.sim=&sim;sim.current=&e;
    float notified=0;sim.time_rate_changed=[&](float value){notified=value;};
    for(float rate:{0.f,-.5f,.25f,.5f,1.f,2.f}) {
        std::vector<u8> code;th13test::emit(code,0,547,0,255,0,{float_to_bits(rate)});
        context.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(context,547),0);
        CHECK_EQ(sim.time_rate(),rate);CHECK_EQ(notified,rate);
    }
}
}
int main(){return th13test::run_all("global-rate");}
