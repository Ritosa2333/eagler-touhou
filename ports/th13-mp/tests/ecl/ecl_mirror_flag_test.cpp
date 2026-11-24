#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"

#include <memory>

using namespace th13;

namespace {

constexpr u32 kMirror = 0x100000u;

struct Rig {
    std::unique_ptr<StageSim> sim = std::make_unique<StageSim>();
    EclContext context;
    Rig() {
        sim->commands.sim = sim.get();
        sim->gvars.sim = sim.get();
    }
    void mirror_on(StageEnemy& enemy, i32 value) {
        sim->current = &enemy;
        std::vector<u8> code;
        th13test::emit(code, 0, 558, 0, 255, 0, {u32(value)});
        context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim->commands.execute(context, 558), 0);
    }
    const StageEnemy* spawn(u16 op, StageEnemy& parent, float x, float y) {
        sim->current = &parent;
        std::vector<u8> code;
        th13test::emit(code, 0, op, 0, 255, 0,
                       {8, 0x6c696843, 0x64, th13test::bits_of(x), th13test::bits_of(y), 400, 1000, 1});
        context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        if (sim->commands.execute(context, op) != 0) return nullptr;
        return sim->enemies.empty() ? nullptr : sim->enemies.back().get();
    }
};

}

TEST(op558_sets_bit_for_odd_values_and_clears_for_even_values) {
    Rig r;
    StageEnemy enemy;
    enemy.flags = 0x20020u;
    CHECK((enemy.flags & kMirror) == 0);
    r.mirror_on(enemy, 1);
    CHECK((enemy.flags & kMirror) != 0);
    r.mirror_on(enemy, 1);
    CHECK((enemy.flags & kMirror) != 0);
    r.mirror_on(enemy, 3);
    CHECK((enemy.flags & kMirror) != 0);
    r.mirror_on(enemy, 0);
    CHECK((enemy.flags & kMirror) == 0);
    r.mirror_on(enemy, 1);
    CHECK((enemy.flags & kMirror) != 0);
    r.mirror_on(enemy, 2);
    CHECK((enemy.flags & kMirror) == 0);
    CHECK((enemy.flags & ~kMirror) == 0x20020u);
}

TEST(op300_family_positions_mirror_only_against_mirrored_parent) {
    Rig r;
    std::vector<u8> child, main;
    th13test::emit(child, 0, 23, 0, 255, 0, {600});
    th13test::emit(main, 0, 23, 0, 255, 0, {600});
    auto data = th13test::build_scpt({{"Child", child}, {"main", main}});
    CHECK_EQ(r.sim->program.attach(data.data(), u32(data.size())), 0);
    auto* plain_parent = r.sim->spawn_enemy("main", 0, 0, 100, nullptr, false);
    CHECK(plain_parent != nullptr);
    if (!plain_parent) return;
    const auto* a = r.spawn(300, *plain_parent, -224.f, 128.f);
    CHECK(a != nullptr);
    if (a) {
        CHECK_EQ(th13test::bits_of(a->position[0]), th13test::bits_of(-224.f));
        CHECK_EQ(th13test::bits_of(a->position[1]), th13test::bits_of(128.f));
        CHECK((a->flags & kMirror) == 0);
    }
    const auto* b = r.spawn(304, *plain_parent, 224.f, 128.f);
    CHECK(b != nullptr);
    if (b) {
        CHECK_EQ(th13test::bits_of(b->position[0]), th13test::bits_of(224.f));
        CHECK((b->flags & kMirror) != 0);
    }
    auto* mirrored_parent = r.sim->spawn_enemy("main", 0, 0, 100, nullptr, false);
    CHECK(mirrored_parent != nullptr);
    if (!mirrored_parent) return;
    mirrored_parent->flags |= kMirror;
    const auto* c = r.spawn(300, *mirrored_parent, -224.f, 128.f);
    CHECK(c != nullptr);
    if (c) {
        CHECK_EQ(th13test::bits_of(c->position[0]), th13test::bits_of(224.f));
        CHECK((c->flags & kMirror) != 0);
    }
    const auto* d = r.spawn(304, *mirrored_parent, 224.f, 128.f);
    CHECK(d != nullptr);
    if (d) {
        CHECK_EQ(th13test::bits_of(d->position[0]), th13test::bits_of(-224.f));
        CHECK((d->flags & kMirror) == 0);
    }
}

int main() { return th13test::run_all("ecl-mirror-flag"); }
