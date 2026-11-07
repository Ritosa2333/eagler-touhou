#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(offscreen_retention_counts_down_without_blocking_other_ex_entries) {
    StageSim sim;sim.interactive=true;StageEt et;et.reset();et.sprite=8;et.subtype=6;
    et.ex[0].op=0x100;et.ex[0].args[4]=3;et.ex[0].args[5]=0;
    et.ex[1].op=0x100000;et.ex[1].args[4]=1;
    const float pos[3]={300,100,0};sim.emit_et(et,1,0,pos,0,0);
    CHECK(sim.error().empty());CHECK_EQ(sim.bullets.size(),1u);
    if(sim.bullets.size()!=1)return;
    CHECK_EQ(sim.bullets[0].ex_cursor,2);CHECK_EQ(sim.bullets[0].offscreen_timer,3);
    sim.advance_bullets();CHECK_EQ(sim.bullets.size(),1u);CHECK_EQ(sim.bullets[0].offscreen_timer,2);
    sim.advance_bullets();CHECK_EQ(sim.bullets.size(),1u);CHECK_EQ(sim.bullets[0].offscreen_timer,1);
    sim.advance_bullets();CHECK(sim.bullets.empty());
}
int main(){return th13test::run_all("offscreen-retention");}
