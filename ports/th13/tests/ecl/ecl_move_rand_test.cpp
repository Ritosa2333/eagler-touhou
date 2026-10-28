#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(move_rand_boundaries_keep_native_draw_count_and_direction) {
    // Native41b260/412 with duration40, mode4, speed2.5, limits(0,128,280,192).
    struct Case { u16 seed;float x,y,player_x;u32 angle;u16 next;u32 calls; };
    const Case cases[]={
        {0,-200,128,100,3206921315u,49095,2},
        {0,-70,79,-100,1056479893u,4755,3},
        {0,-70,128,100,1076493448u,4755,3},
        {2,70,128,70,1076493649u,4627,3},
        {60189,0,80,0,1075849694u,33610,3},
        {60189,0,176,0,1075849694u,33610,3},
    };
    for (const auto& c:cases) {
        StageSim sim;StageEnemy e;EclContext ctx;
        sim.current=&e;sim.commands.sim=&sim;sim.random.reseed_restore(c.seed);
        sim.player_x=c.player_x;sim.player_y=400;
        e.position[0]=c.x;e.position[1]=c.y;
        e.limit_center[1]=128;e.limit_half[0]=280;e.limit_half[1]=192;
        std::vector<u8> code;th13test::emit(code,0,412,0,255,0,{40,4,th13test::bits_of(2.5f)});
        ctx.instruction=reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(ctx,412),0);
        CHECK_EQ(th13test::bits_of(e.vel.polar.from_a),c.angle);
        CHECK_EQ(sim.random.seed(),c.next);CHECK_EQ(sim.random.index(),c.calls);
    }
}
TEST(random16_returns_the_post_rotation_seed) {
    // Direct472c20 observations; its return and stored seed are identical.
    struct Case {u16 seed,next;};
    const Case cases[]={{0,50036},{1,50040},{2,50044},{100,50180},
                        {37781,33098},{60189,24424},{65535,4592}};
    for (const auto& c:cases) {
        GameRandom random(c.seed);
        CHECK_EQ(random.step16(),c.next);
        CHECK_EQ(random.seed(),c.next);CHECK_EQ(random.index(),1u);
    }
}
int main(){return th13test::run_all("random-movement");}
