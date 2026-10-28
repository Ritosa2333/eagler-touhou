#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;StageEnemy* enemy;EclContext ctx;
    Rig(){
        std::vector<u8> code;th13test::emit(code,0,23,0,255,0,{20000});
        th13test::emit(code,0,10,0,255,0,{});
        auto bytes=th13test::build_scpt({{"main",code}});
        sim.program.attach(bytes.data(),u32(bytes.size()));sim.commands.sim=&sim;sim.gvars.sim=&sim;
        enemy=sim.spawn_enemy("main",0,128,40,nullptr,false);
        sim.current=enemy;enemy->tick_gate=false;enemy->flags|=0x1000000u;
    }
    int op(u16 id,std::initializer_list<u32> args){
        std::vector<u8> code;th13test::emit(code,0,id,0,255,0,args);
        ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        return sim.commands.execute(ctx,id);
    }
};
TEST(boss_marker_uses_original_initial_hp_and_independent_remaining_star_count) {
    Rig r;CHECK_EQ(r.enemy->spell_base_life,40);
    CHECK_EQ(r.op(511,{11300}),0);CHECK_EQ(r.op(512,{0}),0);
    CHECK_EQ(r.op(527,{0,float_to_bits(1500),0xffffa080u}),0);
    CHECK_EQ(float_to_bits(r.sim.boss_markers[0][0].ratio),float_to_bits(1500.f/11300.f));
    CHECK_EQ(u32(r.sim.boss_markers[0][0].color),0xffffa080u);
    CHECK_EQ(r.op(540,{3}),0);CHECK_EQ(r.sim.boss_remaining_stars,3);
    CHECK_EQ(r.op(631,{3}),0);CHECK(r.sim.boss_leaving);
    CHECK_EQ(r.op(512,{0}),0);CHECK(!r.sim.boss_leaving);
    r.enemy->life=7000;CHECK_EQ(r.enemy->spell_base_life,11300);
}
TEST(boss_countdown_matches_native_phase_timer_boundaries) {
    const i32 rows[][3]={{0,35,0},{1,34,98},{59,34,1},{60,34,0},{61,33,98},{1801,4,98},{2041,0,98},{2099,0,1}};
    for(const auto& row:rows){Rig r;
        r.enemy->interrupts[0].life=0;r.enemy->interrupts[0].timeout=2100;
        r.enemy->age=float(row[0]);CHECK(r.sim.step_frame());
        CHECK_EQ(r.sim.boss_timer_seconds,row[1]);CHECK_EQ(r.sim.boss_timer_centiseconds,row[2]);
    }
    Rig r;r.enemy->interrupts[0].life=0;r.enemy->interrupts[0].timeout=6001;
    r.enemy->age=0;CHECK(r.sim.step_frame());
    CHECK_EQ(r.sim.boss_timer_seconds,99);CHECK_EQ(r.sim.boss_timer_centiseconds,99);
}
TEST(nonboss_timeout_does_not_overwrite_boss_timer) {
    Rig r;r.enemy->flags&=~0x1000000u;
    r.enemy->interrupts[0].life=0;r.enemy->interrupts[0].timeout=600;
    r.sim.boss_timer_seconds=35;r.sim.boss_timer_centiseconds=98;
    CHECK(r.sim.step_frame());CHECK_EQ(r.sim.boss_timer_seconds,35);CHECK_EQ(r.sim.boss_timer_centiseconds,98);
}
TEST(health_interrupt_does_not_overwrite_timer_before_switching_phase) {
    Rig r;r.enemy->life=10;r.enemy->invulnerable=0;
    r.enemy->interrupts[0].life=5;r.enemy->interrupts[0].timeout=2100;
    r.enemy->interrupts[0].health_label="main";
    r.sim.boss_timer_seconds=17;r.sim.boss_timer_centiseconds=48;
    r.sim.damage(*r.enemy,5);
    CHECK_EQ(r.sim.boss_timer_seconds,17);CHECK_EQ(r.sim.boss_timer_centiseconds,48);
}
}
int main(){return th13test::run_all("ecl-boss-hud-state");}
