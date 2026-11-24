#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
using namespace th13;
u32 word(float f) {
    u32 w;
    std::memcpy(&w, &f, 4);
    return w;
}
float value(u32 w) {
    float f;
    std::memcpy(&f, &w, 4);
    return f;
}

static const u32 kMode4[28] = {3225368607u, 3224427407u, 3223486208u, 3222545008u, 3221603808u, 3220099745u,
                               3218217346u, 3216334946u, 3214452548u, 3212303432u, 3208538632u, 3204773836u,
                               3197569816u, 3180370528u, 1042119616u, 1052877408u, 1058685808u, 1062450604u,
                               1065784310u, 1067666710u, 1069549110u, 1071431506u, 1073313906u, 1074469065u,
                               1075410265u, 1076351465u, 1077292663u, 1078233863u};
static const u32 kMode5[28] = {3225788037u, 3224846838u, 3223905638u, 3222964438u, 3222023238u, 3220938606u,
                               3219056207u, 3217173807u, 3215291408u, 3213409008u, 3210216356u, 3206451556u,
                               3200925256u, 3190731680u, 1029461376u, 1049521960u, 1057008084u, 1060772884u,
                               1064537684u, 1066827850u, 1068710246u, 1070592646u, 1072475046u, 1074049635u,
                               1074990835u, 1075932033u, 1076873233u, 1077814433u};
static const u32 kMode3[28] = {3225419776u, 3224478576u, 3223537377u, 3222596177u, 3221654978u, 3220202084u,
                               3218319684u, 3216437285u, 3214554886u, 3212508108u, 3208743312u, 3204978512u,
                               3197979168u, 3182007968u, 1041300912u, 1052468056u, 1058481128u, 1062245928u,
                               1065681972u, 1067564372u, 1069446768u, 1071329168u, 1073211568u, 1074417896u,
                               1075359096u, 1076300296u, 1077241494u, 1078182694u};

TEST(mode4_ring_mid_and_spoke_f32_drops) {
    StageSim sim;
    sim.interactive = true;
    StageEt et;
    et.ways = 1;
    et.count = 28;
    et.aim = 4;
    et.angle1 = -3.2f;
    et.angle2 = 0.11f;
    const float origin[] = {0, 200, 0};
    sim.emit_et(et, 0, 0, origin, 0.1f);
    CHECK_EQ(sim.bullets.size(), size_t(28));
    if (sim.bullets.size() != 28) return;
    for (int idx = 0; idx < 28; ++idx)
        CHECK_EQ(word(sim.bullets[idx].angle), kMode4[idx]);
}
TEST(mode5_ring_base_zero_mid_and_spoke_f32_drops) {
    StageSim sim;
    sim.interactive = true;
    StageEt et;
    et.ways = 1;
    et.count = 28;
    et.aim = 5;
    et.angle1 = -3.2f;
    et.angle2 = 0.11f;
    const float origin[] = {0, 200, 0};
    sim.emit_et(et, 0, 0, origin, 0.0f);
    CHECK_EQ(sim.bullets.size(), size_t(28));
    if (sim.bullets.size() != 28) return;
    for (int idx = 0; idx < 28; ++idx)
        CHECK_EQ(word(sim.bullets[idx].angle), kMode5[idx]);
}
TEST(mode3_fan_unaffected_by_halfstep_fix) {
    StageSim sim;
    sim.interactive = true;
    StageEt et;
    et.ways = 1;
    et.count = 28;
    et.aim = 3;
    et.angle1 = -3.0f;
    et.angle2 = 0.11f;
    const float origin[] = {0, 200, 0};
    sim.emit_et(et, 0, 0, origin, 0.0f);
    CHECK_EQ(sim.bullets.size(), size_t(28));
    if (sim.bullets.size() != 28) return;
    for (int idx = 0; idx < 28; ++idx)
        CHECK_EQ(word(sim.bullets[idx].angle), kMode3[idx]);
}
int main() { return th13test::run_all("halfstep ring precision"); }
