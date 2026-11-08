#include "../harness/test_framework.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
void entry(StageBullet& b, int index, u32 op, i32 argument) {
    b.ex[index].op = op;
    b.ex[index].args[4] = float(argument);
    b.ex[index].integer_arguments[0] = argument;
}
TEST(bullet_queue_forward_jump_skips_entries_without_incrementing_target) {
    StageSim sim; StageBullet b;
    entry(b, 0, 0x10000, 2); entry(b, 1, 0x80000000, 99);
    entry(b, 2, 0x80000000, 7);
    sim.process_bullet_queue(b, false);
    // Direct retail0040f030: cursor3, wait timer6, behavior80000000.
    CHECK(sim.error().empty()); CHECK_EQ(b.ex_cursor, 3);
    CHECK_EQ(b.wait_timer, 6); CHECK_EQ(b.behavior_flags, 0x80000000u);
}
TEST(bullet_queue_backward_jump_restarts_until_next_blocking_behavior) {
    StageSim sim; StageBullet b;
    entry(b, 0, 0x80000000, 7);
    entry(b, 1, 0x10000, 0); b.ex_cursor = 1;
    sim.process_bullet_queue(b, false);
    CHECK(sim.error().empty()); CHECK_EQ(b.ex_cursor, 1);
    CHECK_EQ(b.wait_timer, 6); CHECK_EQ(b.behavior_flags, 0x80000000u);
    b.wait_timer = 2;
    sim.process_bullet_queue(b, false);
    CHECK_EQ(b.wait_timer, 2); // active wait blocks the jump itself
    b.behavior_flags = 0;
    sim.process_bullet_queue(b, false);
    CHECK_EQ(b.wait_timer, 6); CHECK_EQ(b.ex_cursor, 1);
}
TEST(bullet_queue_jump_to_end_retires_only_the_queue) {
    StageSim sim; StageBullet b;
    entry(b, 0, 0x10000, 18);
    sim.process_bullet_queue(b, false);
    CHECK(sim.error().empty()); CHECK_EQ(b.ex_cursor, 18);
    CHECK_EQ(b.behavior_flags, 0u); CHECK(b.alive);
}
}
int main() { return th13test::run_all("bullet queue jump"); }
