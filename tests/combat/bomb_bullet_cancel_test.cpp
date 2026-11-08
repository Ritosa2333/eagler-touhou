// Bomb-family state4 bullet cancellation (0x4119c0 / 0x4114f0 / 0x411770).
// The original helpers keep the bullet alive, call its cancel callback, apply a
// rate-scaled half-step, optionally spawn a kind-9 item and set state 4.
// 0x4114f0 (ECL 615/616) has no protection gate; 0x411770 (respawn/Reimu) and
// 0x4119c0 (Marisa/Sanae/Youmu rectangles) do.
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Items final : StageItemHost {
    struct Spawn { float x, y, z; i32 kind; };
    std::vector<Spawn> spawns;
    void spawn_item(float x, float y, float z, i32 kind, float, float) override {
        spawns.push_back({x, y, z, kind});
    }
    void spawn_value_item(i32, float, float, float, float) override {}
    void on_et_clear(float, float, float) override {}
};
StageBullet make_bullet(float x, float y, float vx = 0.0f, float vy = 0.0f,
                        u16 state = 1) {
    StageBullet b;
    b.x = x; b.y = y; b.vel[0] = vx; b.vel[1] = vy;
    b.hit_size[0] = b.hit_size[1] = 4.0f;
    b.sprite = 0; b.subtype = 4;  // bullet_cancel_script(0,4) = 139
    b.state = state;
    return b;
}
}  // namespace

TEST(radius_4114f0_state4_callback_half_step_and_kind9_before_move) {
    StageSim sim; Items items; sim.items = &items; sim.set_time_rate(0.5f);
    int effects = 0;
    sim.stationary_cancel_effect = [&](i32 script, const Vec3& p) {
        ++effects;
        CHECK_EQ(script, 139);
        CHECK_EQ(p.x, 10.0f);
        CHECK_EQ(p.y, 200.0f);
        return true;
    };
    sim.bullets.push_back(make_bullet(10.0f, 200.0f, 2.0f, -3.0f));
    sim.cancel_bullets_radius({0.0f, 200.0f, 0.0f}, 64.0f, true);
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets.size(), size_t(1));
    const auto& b = sim.bullets[0];
    CHECK(b.alive);
    CHECK_EQ(b.state, 4u);
    CHECK_EQ(b.x, 10.5f);
    CHECK_EQ(b.y, 199.25f);
    CHECK_EQ(b.delay, 1);
    CHECK_EQ(b.animation_interrupt, 1);
    CHECK_EQ(effects, 1);
    CHECK_EQ(items.spawns.size(), size_t(1));
    CHECK_EQ(items.spawns[0].kind, 9);
    CHECK_EQ(items.spawns[0].x, 10.0f);  // 4114f0 creates the item before the half-step
    CHECK_EQ(items.spawns[0].y, 200.0f);
}

TEST(radius_4114f0_ignores_protection_and_skips_non_live_states) {
    StageSim sim;
    sim.bullets.push_back(make_bullet(0.0f, 200.0f));
    sim.bullets.back().cancel_protection = 5;
    sim.bullets.push_back(make_bullet(0.0f, 200.0f, 0.0f, 0.0f, 3));
    sim.bullets.push_back(make_bullet(0.0f, 200.0f, 0.0f, 0.0f, 2));
    sim.cancel_bullets_radius({0.0f, 200.0f, 0.0f}, 64.0f, true);
    CHECK_EQ(sim.bullets[0].state, 4u);  // 4114f0 has no protection gate
    CHECK_EQ(sim.bullets[1].state, 3u);  // state3 is not cancellable here
    CHECK_EQ(sim.bullets[2].state, 4u);  // state2 is cancellable
}

TEST(respawn_411770_respects_protection_and_spawns_no_kind9) {
    StageSim sim; Items items; sim.items = &items;
    sim.bullets.push_back(make_bullet(0.0f, 200.0f));
    sim.bullets.back().cancel_protection = 1;
    sim.bullets.push_back(make_bullet(0.0f, 200.0f));
    CHECK(sim.cancel_respawn_bullets({0.0f, 200.0f, 0.0f}, 640.0f));
    CHECK_EQ(sim.bullets[0].state, 1u);
    CHECK_EQ(sim.bullets[1].state, 4u);
    CHECK(items.spawns.empty());
}

TEST(box_4119c0_state4_half_step_and_kind9_after_move) {
    StageSim sim; Items items; sim.items = &items; sim.set_time_rate(1.0f);
    int effects = 0;
    sim.stationary_cancel_effect = [&](i32, const Vec3&) { ++effects; return true; };
    sim.bullets.push_back(make_bullet(10.0f, 200.0f, 2.0f, 0.0f));
    sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {64.0f, 64.0f, 0.0f}, 0.0f, 0.0f, true,
                           true);
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets[0].state, 4u);
    CHECK_EQ(sim.bullets[0].x, 11.0f);
    CHECK_EQ(effects, 1);
    CHECK_EQ(items.spawns.size(), size_t(1));
    CHECK_EQ(items.spawns[0].x, 11.0f);  // 4119c0 creates the item after the half-step
}

TEST(box_4119c0_respects_protection_world_rect_and_screen_field) {
    // Protected bullets are skipped.
    {
        StageSim sim;
        sim.bullets.push_back(make_bullet(0.0f, 200.0f));
        sim.bullets.back().cancel_protection = 1;
        sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {64.0f, 64.0f, 0.0f}, 0.0f, 0.0f,
                               true, true);
        CHECK_EQ(sim.bullets[0].state, 1u);
    }
    // World rectangle: |dx| == half width is included; +1 is outside.
    {
        StageSim sim;
        sim.bullets.push_back(make_bullet(32.0f, 200.0f));
        sim.bullets.push_back(make_bullet(33.0f, 200.0f));
        sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {64.0f, 64.0f, 0.0f}, 0.0f, 0.0f,
                               true, true);
        CHECK_EQ(sim.bullets[0].state, 4u);
        CHECK_EQ(sim.bullets[1].state, 1u);
    }
    // Screen field x in [-192,192], y in [0,448]; boundary included, +1 outside.
    {
        StageSim sim;
        sim.bullets.push_back(make_bullet(192.0f, 200.0f));
        sim.bullets.push_back(make_bullet(193.0f, 200.0f));
        sim.bullets.push_back(make_bullet(0.0f, 0.0f));
        sim.bullets.push_back(make_bullet(0.0f, -1.0f));
        sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {1024.0f, 1024.0f, 0.0f}, 0.0f, 0.0f,
                               true, true);
        CHECK_EQ(sim.bullets[0].state, 4u);
        CHECK_EQ(sim.bullets[1].state, 1u);
        CHECK_EQ(sim.bullets[2].state, 4u);
        CHECK_EQ(sim.bullets[3].state, 1u);
    }
    // state3/state4 nodes are not re-cancelled.
    {
        StageSim sim;
        sim.bullets.push_back(make_bullet(0.0f, 200.0f, 0.0f, 0.0f, 3));
        sim.bullets.push_back(make_bullet(0.0f, 200.0f, 0.0f, 0.0f, 4));
        sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {64.0f, 64.0f, 0.0f}, 0.0f, 0.0f,
                               true, true);
        CHECK_EQ(sim.bullets[0].state, 3u);
        CHECK_EQ(sim.bullets[1].state, 4u);
    }
}

TEST(box_4119c0_uses_each_bullets_scaled_radius) {
    // Original Sanae stage5 f3693: slot1978 and 1997 touch rays 3 and 4
    // with distances 3.9497 and 1.1513 outside their 320x32 rectangles.
    // The original gives both bullet types radius 4 (4119c0 +1324*+b90).
    struct Case { Vec3 center; float angle, x, y; };
    const Case cases[] = {
        {{-6.43389892578125f, 126.90037536621094f, 0.f}, -0.6283185482025146f,
         -116.40731811523438f, 231.4599609375f},
        {{-67.54704284667969f, 126.90481567382812f, 0.f}, 0.6283185482025146f,
         -64.23521423339844f, 108.1108169555664f},
    };
    for (const auto& c : cases) {
        StageSim sim;
        sim.bullets.push_back(make_bullet(c.x, c.y));
        sim.bullets.push_back(make_bullet(c.x, c.y));
        sim.bullets.back().hit_scale = 0.f;
        sim.cancel_bullets_box(c.center, {320.f, 32.f, 0.f}, c.angle, 0.f,
                               false, true, true);
        CHECK_EQ(sim.bullets[0].state, 4u);
        CHECK_EQ(sim.bullets[1].state, 1u);
    }
    // Screen clipping uses the same radius, including scaled values.
    StageSim screen;
    screen.bullets.push_back(make_bullet(194.f, 200.f));
    screen.bullets.push_back(make_bullet(194.f, 200.f));
    screen.bullets.back().hit_scale = 0.25f;
    screen.cancel_bullets_box({0.f, 200.f, 0.f}, {1024.f, 1024.f, 0.f}, 0.f,
                              0.f, false, true, true);
    CHECK_EQ(screen.bullets[0].state, 4u);
    CHECK_EQ(screen.bullets[1].state, 1u);
}

TEST(kind9_item_box_is_strict_at_playfield_edges) {
    // 0x412320: x+32 > -192, x-32 < 192, y+32 > 0, y-32 < 448.
    struct Case { float x, y; bool spawn; };
    const Case cases[] = {{-224.0f, 200.0f, false}, {-223.99f, 200.0f, true},
                          {0.0f, -32.0f, false},   {0.0f, -31.99f, true}};
    for (const auto& c : cases) {
        StageSim sim; Items items; sim.items = &items;
        sim.bullets.push_back(make_bullet(c.x, c.y));
        sim.cancel_bullets_radius({c.x, c.y, 0.0f}, 64.0f, true);
        CHECK_EQ(sim.bullets[0].state, 4u);
        CHECK_EQ(items.spawns.size(), c.spawn ? size_t(1) : size_t(0));
    }
}

TEST(state4_cancel_is_idempotent_and_keeps_the_node) {
    StageSim sim; Items items; sim.items = &items;
    sim.bullets.push_back(make_bullet(0.0f, 200.0f, 2.0f, 0.0f));
    sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {64.0f, 64.0f, 0.0f}, 0.0f, 0.0f, true,
                           true);
    const float x = sim.bullets[0].x;
    sim.cancel_bullets_box({0.0f, 200.0f, 0.0f}, {64.0f, 64.0f, 0.0f}, 0.0f, 0.0f, true,
                           true);
    CHECK_EQ(sim.bullets.size(), size_t(1));
    CHECK(sim.bullets[0].alive);
    CHECK_EQ(sim.bullets[0].x, x);
    CHECK_EQ(items.spawns.size(), size_t(1));
}
int main() { return th13test::run_all("bomb-bullet-cancel"); }
