#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
#include <deque>
using namespace th13;

namespace {
struct Rig {
    StageSim sim;
    StageEnemy owner;
    EclContext context;
    // ECL 640 retains an instruction string, exactly like a loaded program.
    std::deque<std::vector<u8>> instructions;
    Rig() {
        std::vector<u8> child, root;
        // Constructor-time use proves that arguments arrived before ECL ran.
        th13test::emit(child, 0, 500, 3, 255, 0,
                      {float_to_bits(-9981.f), float_to_bits(-9980.f)});
        th13test::emit(child, 0, 510, 1, 255, 0, {u32(-9985)});
        th13test::emit(child, 0, 23, 0, 255, 0, {600});
        th13test::emit(child, 0, 10, 0, 255, 0, {});
        th13test::emit(root, 0, 23, 0, 255, 0, {600});
        th13test::emit(root, 0, 10, 0, 255, 0, {});
        const auto data = th13test::build_scpt({{"Child", child}, {"main", root}});
        sim.program.attach(data.data(), u32(data.size()));
        sim.commands.sim = &sim;
        sim.gvars.sim = &sim;
        sim.current = &owner;
        sim.interactive = true;
        sim.difficulty_rank = 4;
        owner.script_id = 77;
        owner.position[0] = 12;
        owner.position[1] = 45;
        owner.position[2] = 3;
        context.random = &sim.random;
    }
    int op(u16 opcode, std::initializer_list<u32> args) {
        instructions.emplace_back();
        auto& bytes = instructions.back();
        th13test::emit(bytes, 0, opcode, 0, 255, 0, args);
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context, opcode);
    }
    int bind_name(i32 emitter, i32 index, const char* name) {
        const u32 size = (u32(std::strlen(name)) + 4) & ~3u;
        std::vector<u8> padded(size);
        std::memcpy(padded.data(), name, std::strlen(name));
        instructions.emplace_back();
        auto& bytes = instructions.back();
        th13test::emit(bytes, 0, 640, 0, 255, 0,
                      {u32(emitter), u32(index), size}, padded.data(), size);
        context.instruction = reinterpret_cast<const EclInstruction*>(bytes.data());
        return sim.commands.execute(context, 640);
    }
    int descriptor(i32 index) {
        return op(610, {0, u32(index), 0, 0x1000000,
                       16777217u, u32(-16777217), 0x7fffffffu, 0x80000000u,
                       float_to_bits(31.f), float_to_bits(47.f),
                       float_to_bits(-2.5f), float_to_bits(.25f)});
    }
};

TEST(bullet_subroutine_uses_original_arguments_during_constructor) {
    Rig r;
    CHECK_EQ(r.op(600, {0}), 0);
    // Name can precede descriptor writes; these touch disjoint native fields.
    CHECK_EQ(r.bind_name(0, 0, "Child"), 0);
    CHECK_EQ(r.descriptor(0), 0);
    CHECK_EQ(r.op(605, {0, 0, 0}), 0);
    CHECK_EQ(r.op(601, {0}), 0);
    CHECK(r.sim.error().empty());
    CHECK_EQ(r.sim.enemies.size(), size_t(1));
    if (r.sim.enemies.empty()) return;
    const auto& child = *r.sim.enemies.front();
    CHECK_EQ(child.routine, std::string("Child"));
    CHECK_EQ(child.position[0], 12.f);
    CHECK_EQ(child.position[1], 45.f);
    // 40e840 gives the bullet z=0.1; 40f030 copies that live bullet value,
    // rather than restoring the firing enemy's z=3.
    CHECK_EQ(child.position[2], .1f);
    CHECK_EQ(child.trace_id, 0);
    CHECK_EQ(child.life, 10000.f);
    CHECK(child.flags & 0x80000000u);
    CHECK_EQ(child.threads->main.difficulty, 4);
    CHECK_EQ(child.i_slots[0], 16777217);
    CHECK_EQ(child.i_slots[1], -16777217);
    CHECK_EQ(child.i_slots[2], INT32_MAX);
    CHECK_EQ(child.i_slots[3], INT32_MIN);
    CHECK_EQ(child.f_slots[2], -2.5f);
    CHECK_EQ(child.f_slots[3], .25f);
    CHECK_EQ(child.drop_main, 16777217);
    CHECK_EQ(child.hurtbox[0], 31.f);
    CHECK_EQ(child.hurtbox[1], 47.f);
    CHECK_EQ(r.sim.current, &r.owner);
    CHECK_EQ(r.sim.bullets.size(), size_t(1));
    if (!r.sim.bullets.empty()) CHECK_EQ(r.sim.bullets.front().ex_cursor, 1);
}

TEST(bullet_subroutine_survives_emitter_copy_reset_and_delayed_execution) {
    Rig r;
    CHECK_EQ(r.op(600, {0}), 0);
    CHECK_EQ(r.op(609, {0, 0, 0, 0x80000000u, 2, 0, 0, 0}), 0);
    CHECK_EQ(r.descriptor(1), 0);
    const u32 cursor = r.owner.et[0].ex_next;
    CHECK_EQ(r.bind_name(0, 1, "Child"), 0);
    CHECK_EQ(r.owner.et[0].ex_next, cursor);
    CHECK_EQ(r.op(605, {0, 0, 0}), 0);
    CHECK_EQ(r.op(614, {1, 0}), 0);
    CHECK_EQ(r.op(600, {0}), 0);
    CHECK_EQ(r.owner.et[0].ex[1].subroutine, nullptr);
    CHECK_EQ(r.op(601, {1}), 0);
    CHECK_EQ(r.sim.enemies.size(), size_t(0));
    // The live bullet owns a copied descriptor, not a reference to this slot.
    CHECK_EQ(r.op(600, {1}), 0);
    // 40db10 consumes the following entry in the SAME update that clears
    // the wait bit. Native wait=2 emits on update index2, not index3.
    for (int frame = 0; frame < 2; ++frame) {
        r.sim.advance_bullets();
        CHECK(r.sim.error().empty());
        CHECK_EQ(r.sim.enemies.size(), size_t(0));
    }
    r.sim.advance_bullets();
    CHECK(r.sim.error().empty());
    CHECK_EQ(r.sim.enemies.size(), size_t(1));
    if (!r.sim.enemies.empty()) CHECK_EQ(r.sim.enemies.front()->drop_main, 16777217);
    r.sim.advance_bullets();
    CHECK_EQ(r.sim.enemies.size(), size_t(1));
}

TEST(bullet_subroutine_missing_name_is_an_explicit_runtime_error) {
    Rig r;
    CHECK_EQ(r.op(600, {0}), 0);
    CHECK_EQ(r.descriptor(0), 0);
    CHECK_EQ(r.op(601, {0}), -2);
    CHECK(!r.sim.error().empty());
    CHECK_EQ(r.sim.enemies.size(), size_t(0));
}
}

int main() { return th13test::run_all("bullet-subroutine"); }
