#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
#include <cstring>
#include <vector>
using namespace th13;

namespace {
u32 bits(float value) {
    u32 out;
    std::memcpy(&out, &value, 4);
    return out;
}
void run_scale(float from, float to, i32 duration, i32 mode, const std::vector<u32>& expected,
               bool final_visible_flag) {
    StageSim sim;
    sim.player_x = 30;
    sim.player_y = 40;
    StageBullet bullet;
    bullet.alive = true;
    bullet.state = 1;
    bullet.ex[0].op = 0x400000;
    bullet.ex[0].args[0] = from;
    bullet.ex[0].args[1] = to;
    bullet.ex[0].integer_arguments[0] = duration;
    bullet.ex[0].integer_arguments[1] = mode;
    sim.process_bullet_queue(bullet, false);
    CHECK(sim.error().empty());
    CHECK_EQ(bullet.behavior_flags, 0x400000u);
    CHECK((bullet.flags & 0x40u) != 0);
    sim.bullets.push_back(bullet);
    for (u32 frame = 0; frame < expected.size(); ++frame) {
        sim.advance_bullets();
        CHECK(sim.error().empty());
        CHECK_EQ(sim.bullets.size(), size_t(1));
        if (sim.bullets.empty()) return;
        const auto& b = sim.bullets[0];
        CHECK_EQ(bits(b.hit_scale), expected[frame]);
        if (frame + 1 == expected.size()) {
            CHECK_EQ(b.behavior_flags, 0u);
            CHECK_EQ((b.flags & 0x40u) != 0, final_visible_flag);
        }
    }
}
TEST(scale_tween_linear_30_frames) {
    run_scale(.5f, 2.0f, 30, 0,
              {0x3f0ccccdu, 0x3f19999au, 0x3f266666u, 0x3f333333u, 0x3f400000u, 0x3f4ccccdu, 0x3f59999au, 0x3f666667u,
               0x3f733334u, 0x3f800000u, 0x3f866666u, 0x3f8ccccdu, 0x3f933333u, 0x3f99999au, 0x3fa00000u, 0x3fa66667u,
               0x3faccccdu, 0x3fb33334u, 0x3fb9999au, 0x3fc00000u, 0x3fc66666u, 0x3fcccccdu, 0x3fd33333u, 0x3fd9999au,
               0x3fe00000u, 0x3fe66666u, 0x3fecccccu, 0x3ff33333u, 0x3ff99999u, 0x40000000u, 0x40000000u},
              true);
}
TEST(scale_tween_mode_1_20_frames) {
    run_scale(1.0f, .25f, 20, 1,
              {0x3f7f851fu, 0x3f7e147bu, 0x3f7bae14u, 0x3f7851ebu, 0x3f740000u, 0x3f6eb852u, 0x3f687ae1u,
               0x3f6147aeu, 0x3f591eb8u, 0x3f500000u, 0x3f45eb85u, 0x3f3ae148u, 0x3f2ee148u, 0x3f21eb85u,
               0x3f140000u, 0x3f051eb8u, 0x3eea8f5cu, 0x3ec8f5c4u, 0x3ea570a4u, 0x3e800000u, 0x3e800000u},
              true);
}
TEST(scale_tween_mode_18_10_frames) {
    run_scale(1.0f, .5f, 10, 18,
              {0x3f6bf9f4u, 0x3f587222u, 0x3f45e3a3u, 0x3f34c374u, 0x3f257d86u, 0x3f187221u, 0x3f0df380u, 0x3f0643c8u,
               0x3f01936eu, 0x3f000000u, 0x3f000000u},
              true);
}
TEST(scale_tween_one_frame_to_unity_clears_visible_flag) {
    run_scale(2.0f, 1.0f, 1, 0, {0x3f800000u, 0x3f800000u}, false);
}
}
int main() { return th13test::run_all("ecl-bullet-scale"); }
