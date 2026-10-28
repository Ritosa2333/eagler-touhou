#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
namespace {
struct Rig {
    StageSim sim;
    StageEnemy enemy;
    EclContext context;
    Rig() { sim.commands.sim = &sim; sim.current = &enemy; sim.interactive = true; }
    int run(u16 op, std::initializer_list<u32> args) {
        std::vector<u8> bytes;
        th13test::emit(bytes, 0, op, 0, 255, 0, args);
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context, op);
    }
};
TEST(copied_emitter_retains_its_own_append_cursor) {
    // 0041d631..0041d6a9: emitter+positions copied, enemy+3d70 untouched.
    for (u32 cursor : {0u, 3u, 17u}) {
        Rig r;
        r.enemy.et[0].ex[0].op = 2;
        r.enemy.et[0].ex_next = 1;
        r.enemy.et[1].ex_next = cursor;
        CHECK_EQ(r.run(614, {1, 0}), 0);
        CHECK_EQ(r.enemy.et[1].ex[0].op, 2u);
        CHECK_EQ(r.enemy.et[1].ex_next, cursor);
        CHECK_EQ(r.enemy.et[0].ex_next, 1u);
    }
}
TEST(stage5_copied_emitter_overwrites_delay_before_firing) {
    // GirlD01_at: source appends delay, copies into slot1, then each slot
    // appends angular acceleration. Slot1's independent cursor is still0.
    Rig r;
    CHECK_EQ(r.run(600, {0}), 0);
    CHECK_EQ(r.run(611, {0, 1, 2, 1, u32(-999999), 0, 0}), 0);
    CHECK_EQ(r.run(614, {1, 0}), 0);
    for (u32 slot : {0u, 1u})
        CHECK_EQ(r.run(611, {slot, 0, 8, 120, u32(-999999),
                            float_to_bits(.006666667f), float_to_bits(-.017453292f)}), 0);
    CHECK_EQ(r.enemy.et[0].ex[0].op, 2u);
    CHECK_EQ(r.enemy.et[0].ex[1].op, 8u);
    CHECK_EQ(r.enemy.et[1].ex[0].op, 8u);
    CHECK_EQ(r.enemy.et[1].ex[1].op, 0u);
    const float position[] = {0, 100, 0};
    r.sim.emit_et(r.enemy.et[0], 0, 0, position, 0);
    r.sim.emit_et(r.enemy.et[1], 0, 1, position, 0);
    CHECK_EQ(r.sim.bullets.size(), size_t(2));
    CHECK_EQ(r.sim.bullets[0].state, 2);
    CHECK_EQ(r.sim.bullets[1].state, 1);
    CHECK_EQ(r.sim.bullets[1].behavior_flags, 8u);
}
}
int main() { return th13test::run_all("emitter copy cursor"); }
