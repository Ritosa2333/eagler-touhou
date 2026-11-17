#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
using namespace th13;

namespace {
TEST(bullet_queue_protection_copies_full_integer_operand) {
    StageSim sim;
    StageBullet bullet;
    bullet.ex[0].op = 0x80;
    bullet.ex[0].args[4] = float(16777217);
    bullet.ex[0].integer_arguments[0] = 16777217;
    sim.process_bullet_queue(bullet, true);
    CHECK(sim.error().empty());
    CHECK_EQ(bullet.ex_cursor, 1);
    CHECK_EQ(bullet.cancel_protection, 16777217);
}

TEST(bullet_protection_decrements_only_during_active_behavior_pass) {
    StageSim sim;
    sim.interactive = true;
    StageBullet bullet;
    bullet.cancel_protection = 2;
    bullet.behavior_flags = 0x80000000u;
    bullet.wait_time = 10;
    bullet.wait_timer = 9;
    sim.bullets.push_back(bullet);
    sim.advance_bullets();
    CHECK(sim.error().empty());
    CHECK_EQ(sim.bullets.size(), size_t(1));
    CHECK_EQ(sim.bullets[0].cancel_protection, 1);
    sim.bullets[0].behavior_flags = 0;
    sim.advance_bullets();
    CHECK_EQ(sim.bullets[0].cancel_protection, 1);
}
}
int main() { return th13test::run_all("bullet contact protection"); }
