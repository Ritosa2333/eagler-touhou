#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
#include "game/BulletAppearance.hpp"
#include <vector>
using namespace th13;

TEST(retired_bullets_return_to_free_head_in_active_update_order) {
    StageSim sim; sim.interactive = true;
    StageEt et; et.ways = 1; et.count = 3; et.sprite = 0; et.speed1 = et.speed2 = 0;
    const float pos[] = {0, 200, 0};
    sim.emit_et(et, 1, 0, pos, 0);
    CHECK_EQ(sim.bullets.size(), size_t(3));
    CHECK_EQ(sim.bullets[0].pool_slot, 1999);
    CHECK_EQ(sim.bullets[1].pool_slot, 1998);
    CHECK_EQ(sim.bullets[2].pool_slot, 1997);
    sim.bullet_animation = [](StageBullet& bullet, bool birth) {
        if (!birth && (bullet.pool_slot == 1999 || bullet.pool_slot == 1997)) bullet.alive = false;
        return true;
    };
    sim.advance_bullets();
    CHECK_EQ(sim.bullets.size(), size_t(1));
    CHECK_EQ(sim.bullets[0].pool_slot, 1998);
    et.count = 2;
    sim.emit_et(et, 1, 0, pos, 0);
    CHECK_EQ(sim.bullets[1].pool_slot, 1999);
    CHECK_EQ(sim.bullets[2].pool_slot, 1997);
}

TEST(cancel_scans_physical_slots_while_updates_stay_active_list_order) {
    StageSim sim; sim.interactive = true;
    StageEt et; et.ways = 1; et.count = 3; et.sprite = 0; et.speed1 = et.speed2 = 0;
    const float pos[] = {0, 200, 0};
    sim.emit_et(et, 1, 0, pos, 0);
    for (int i = 0; i < 3; ++i) sim.bullets[size_t(i)].subtype = u32(i);
    std::vector<i32> update_order, cancel_order;
    sim.bullet_animation = [&](StageBullet& b, bool birth) {
        if (!birth) update_order.push_back(b.pool_slot);
        return true;
    };
    sim.stationary_cancel_effect = [&](i32 script, const Vec3&) {
        cancel_order.push_back(script); return true;
    };
    sim.advance_bullets();
    CHECK(update_order == std::vector<i32>({1997, 1998, 1999}));
    sim.cancel_bullets_radius({0, 200, 0}, 64, false, true);
    CHECK(cancel_order == std::vector<i32>({bullet_cancel_script(0, 2),
                                           bullet_cancel_script(0, 1),
                                           bullet_cancel_script(0, 0)}));
}

TEST(protected_birth_returns_same_slot_to_the_free_head) {
    StageSim sim; sim.interactive = true;
    StageEt et; et.ways = 1; et.count = 1; et.sprite = 0; et.speed1 = et.speed2 = 0;
    const float pos[] = {0, 200, 0};
    sim.player_x = 0; sim.player_y = 200; sim.active_bullet_protect_radius2 = 16;
    sim.emit_et(et, 1, 0, pos, 0);
    CHECK(sim.bullets.empty());
    sim.active_bullet_protect_radius2 = 0;
    sim.emit_et(et, 1, 0, pos, 0);
    CHECK_EQ(sim.bullets.size(), size_t(1));
    CHECK_EQ(sim.bullets[0].pool_slot, 1999);
}
TEST(offscreen_retirement_returns_the_node_before_next_birth) {
    StageSim sim; sim.interactive = true;
    StageEt et; et.ways = 1; et.count = 1; et.sprite = 0; et.speed1 = et.speed2 = 0;
    const float outside[] = {300, 200, 0}; const float inside[] = {0, 200, 0};
    sim.emit_et(et, 1, 0, outside, 0);
    CHECK_EQ(sim.bullets[0].pool_slot, 1999);
    sim.advance_bullets();
    CHECK(sim.bullets.empty());
    sim.emit_et(et, 1, 0, inside, 0);
    CHECK_EQ(sim.bullets[0].pool_slot, 1999);
}
TEST(pool_exhaustion_rejects_birth_until_a_real_node_retires) {
    StageSim sim; sim.interactive = true;
    StageEt et; et.ways = 1; et.count = 2001; et.sprite = 0; et.speed1 = et.speed2 = 0;
    const float pos[] = {0, 200, 0};
    sim.emit_et(et, 1, 0, pos, 0);
    CHECK_EQ(sim.bullets.size(), size_t(2000));
    sim.bullet_animation = [](StageBullet& bullet, bool birth) {
        if (!birth && bullet.pool_slot == 1234) bullet.alive = false;
        return true;
    };
    sim.advance_bullets();
    CHECK_EQ(sim.bullets.size(), size_t(1999));
    et.count = 1; sim.emit_et(et, 1, 0, pos, 0);
    CHECK_EQ(sim.bullets.size(), size_t(2000));
    CHECK_EQ(sim.bullets.back().pool_slot, 1234);
}
int main() { return th13test::run_all("bullet pool ordering"); }
