// H-SINE-MOVEMENT:473d4c..473ecb, actual Stage5 GirlD01 motion.
#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include "game/StageMath.hpp"
using namespace th13;
namespace {
TEST(sine_modes_retain_original_weight_words_at_boundaries_and_midpoint) {
    constexpr u32 expected[4][4]={
        {1003348694u,1044373731u,1060439283u,1065352885u},
        {933593088u,1016142432u,1050015258u,1065247802u},
        {1003348428u,1044150994u,1056964608u,1065247806u},
        {941989888u,1024446816u,1056964608u,1065352554u}};
    constexpr int frames[]={1,30,125,249};
    for(int mode=18;mode<=21;++mode)for(int i=0;i<4;++i)
        CHECK_EQ(float_to_bits(ease_interp(mode,float(frames[i]),250)),expected[mode-18][i]);
}
TEST(stage5_girld01_second_move_uses_sine_acceleration_not_linear) {
    StageSim sim;CHECK(sim.load("artifacts/reference/raw",
        "artifacts/verification/stage5-hit-20261027/girld01-events.jsonl","st05.ecl"));
    auto* enemy=sim.spawn_enemy("GirlD01",0,-64,1500,nullptr,false);
    CHECK(enemy);if(!enemy)return;
    bool saw=false;
    for(int i=0;i<130;++i) {
        CHECK(sim.step_frame());CHECK(sim.error().empty());
        if(enemy->vel.postime.mode==21&&enemy->vel.postime.elapsed==1) {
            saw=true;CHECK_EQ(enemy->position[1],191.99f);
            CHECK_EQ(enemy->vel.postime.target[1],128.f);
            break;
        }
    }
    CHECK(saw);
}
}
int main(){return th13test::run_all("sine-movement");}
