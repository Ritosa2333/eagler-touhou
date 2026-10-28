#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;

TEST(enemy_position_read_uses_requested_live_enemy_and_float_destinations) {
    StageSim sim;StageEnemy caller;EclContext ctx;
    sim.current=&caller;sim.commands.sim=&sim;
    auto first=std::make_unique<StageEnemy>();first->script_id=17;
    first->position[0]=12.25f;first->position[1]=87.5f;
    auto second=std::make_unique<StageEnemy>();second->script_id=18;
    second->position[0]=-5.f;second->position[1]=128.f;
    sim.enemies.push_back(std::move(first));sim.enemies.push_back(std::move(second));
    std::vector<u8> code;
    th13test::emit(code,0,801,3,255,0,{th13test::bits_of(0.f),th13test::bits_of(4.f),17});
    ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx,801),0);
    float x,y;std::memcpy(&x,ctx.stack.data(),4);std::memcpy(&y,ctx.stack.data()+4,4);
    CHECK_EQ(x,12.25f);CHECK_EQ(y,87.5f);
    // 417f40's original nonzero-ID miss returns the final examined live enemy.
    code.clear();th13test::emit(code,0,801,3,255,0,{th13test::bits_of(0.f),th13test::bits_of(4.f),99});
    ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(ctx,801),0);
    std::memcpy(&x,ctx.stack.data(),4);std::memcpy(&y,ctx.stack.data()+4,4);
    CHECK_EQ(x,-5.f);CHECK_EQ(y,128.f);
}

int main(){return th13test::run_all("ecl-enemy-position-read");}
