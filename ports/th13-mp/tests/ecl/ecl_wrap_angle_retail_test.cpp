#include "../harness/test_framework.hpp"
#include "game/StageMath.hpp"
#include "game/StageSim.hpp"
using namespace th13;
u32 word(float f) {
    u32 w;
    std::memcpy(&w, &f, 4);
    return w;
}

TEST(wrap_s5_raw_two_calls_each_34_subtractions) {
    CHECK_EQ(word(wrap_angle(780.3973388671875f)), 0x440db12eu);
    CHECK_EQ(word(wrap_angle(wrap_angle(780.3973388671875f))), 0x43b091f6u);
}

TEST(wrap_34th_subtraction_lands_before_exit) { CHECK_EQ(word(wrap_angle(212.0f)), 0xbfd06df8u); }

TEST(wrap_sub33_and_negative_regions_unchanged) {
    CHECK_EQ(word(wrap_angle(208.9f)), 0x3fc70478u);
    CHECK_EQ(word(wrap_angle(207.0f)), 0xbeb0b9e0u);
    CHECK_EQ(word(wrap_angle(-780.3973388671875f)), 0xc40db12eu);
}

TEST(wrap_identity_within_pi) {
    const float pi_f = 3.1415927f;
    const float cases[] = {0.0f, 1.0f, -1.0f, 0.5f, -2.9f, 3.0f, pi_f, -pi_f};
    for (float v : cases)
        CHECK_EQ(word(wrap_angle(v)), word(v));
}

TEST(emit_et_birth_velocity_uses_raw_angle) {
    StageSim sim;
    sim.interactive = true;
    StageEt et;
    et.reset();
    et.angle1 = 779.20556640625f;
    et.speed1 = 0.1f;
    const float spawn[3] = {-100.64227294921875f, 162.38999938964844f, 0.0f};
    sim.emit_et(et, 1, 0, spawn, 1.191794514656067f);
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets.size(), 1u);
    if (sim.bullets.size() != 1) return;
    CHECK_EQ(word(sim.bullets[0].angle), 0x43b091f6u);
    CHECK_EQ(word(sim.bullets[0].vel[0]), 0x3ce90611u);
    CHECK_EQ(word(sim.bullets[0].vel[1]), 0x3dc456f8u);
}

TEST(emit_et_raw_birth_vel_settles_bit_exact_under_op4_steer) {
    StageSim sim;
    sim.interactive = true;
    StageEt et;
    et.reset();
    et.angle1 = 779.20556640625f;
    et.speed1 = 0.1f;
    et.ex[0].op = 4;
    et.ex[0].args[0] = 0.008333333767950535f;
    et.ex[0].args[1] = -999999.0f;
    et.ex[0].args[4] = 120.0f;
    const float spawn[3] = {-100.64227294921875f, 162.38999938964844f, 0.0f};
    sim.emit_et(et, 1, 0, spawn, 1.191794514656067f);
    CHECK(sim.error().empty());
    for (int f = 0; f < 120; ++f) {
        sim.advance_bullets();
        CHECK(sim.error().empty());
    }
    CHECK_EQ(sim.bullets.size(), 1u);
    if (sim.bullets.size() != 1) return;
    CHECK_EQ(word(sim.bullets[0].vel[0]), 0x3ea068c0u);
    CHECK_EQ(word(sim.bullets[0].vel[1]), 0x3f86f7e7u);
    CHECK_EQ(word(sim.bullets[0].speed), 0x3f8cccd0u);
}

TEST(respawn_cancel_half_step_uses_same_f32_chain_on_all_axes) {
    StageSim sim;
    sim.set_time_rate(0.9f);
    StageBullet b;
    b.x = 1.0f;
    b.y = 10.0f;
    b.z = -3.0f;
    b.vel[0] = 0.7f;
    b.vel[1] = 1.1f;
    b.vel[2] = 2.35f;
    b.state = 1;
    sim.bullets.push_back(b);
    CHECK(sim.cancel_respawn_bullets({1.0f, 10.0f, -3.0f}, 0.0f));
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets.size(), 1u);
    if (sim.bullets.size() != 1) return;
    CHECK_EQ(word(sim.bullets[0].x), 0x3fa851ecu);
    CHECK_EQ(word(sim.bullets[0].y), 0x4127eb85u);
    CHECK_EQ(word(sim.bullets[0].z), 0xbff8a3d8u);
    CHECK_EQ(sim.bullets[0].state, 4u);
}

TEST(respawn_cancel_half_step_rate_one_unchanged) {
    StageSim sim;
    StageBullet b;
    b.x = 1.0f;
    b.y = 10.0f;
    b.z = -3.0f;
    b.vel[0] = 0.7f;
    b.vel[1] = 1.1f;
    b.vel[2] = 2.35f;
    b.state = 1;
    sim.bullets.push_back(b);
    CHECK(sim.cancel_respawn_bullets({1.0f, 10.0f, -3.0f}, 0.0f));
    CHECK(sim.error().empty());
    CHECK_EQ(word(sim.bullets[0].x), word(1.0f + 0.35f));
    CHECK_EQ(word(sim.bullets[0].y), word(10.0f + 0.55f));
    CHECK_EQ(word(sim.bullets[0].z), word(-3.0f + 1.175f));
}
int main() { return th13test::run_all("wrap-angle/emit-et retail bits"); }
