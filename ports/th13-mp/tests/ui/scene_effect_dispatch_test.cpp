#include "../ecl/ecl_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/AnmEnvironment.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmVisualState.hpp"
#include "game/EclContext.hpp"
#include "game/StageSim.hpp"

#include <cmath>
#include <cstring>
#include <vector>

using namespace th13;
using th13test::TestFileReader;

namespace {
u32 bits(float f) {
    u32 x;
    std::memcpy(&x, &f, 4);
    return x;
}
}

TEST(scene_effect_dispatch_passes_enemy_position) {
    StageSim sim;
    sim.commands.sim = &sim;
    StageEnemy enemy;
    enemy.position[0] = -64.f;
    enemy.position[1] = 32.f;
    enemy.position[2] = 0.f;
    bool called = false;
    i32 seen_kind = -1;
    Vec3 seen_position{};
    sim.scene_effect = [&](i32 kind, const Vec3& position) {
        called = true;
        seen_kind = kind;
        seen_position = position;
        return true;
    };
    sim.current = &enemy;
    EclContext context;
    std::vector<u8> code;
    th13test::emit(code, 0, 334, 0, 255, 0, {1});
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context, 334), 0);
    CHECK(called);
    CHECK_EQ(seen_kind, 1);
    CHECK_EQ(bits(seen_position.x), bits(-64.f));
    CHECK_EQ(bits(seen_position.y), bits(32.f));
    CHECK_EQ(bits(seen_position.z), bits(0.f));
}

TEST(scene_effect_kind1_trail_init_tick_retire) {
    GameRandom random{0};
    AnmEnvironment env;
    AnmManager manager{random, env};
    TestFileReader reader;
    std::vector<u8> bytes;
    CHECK(reader.read("artifacts/reference/raw/ascii.anm", bytes));
    CHECK(manager.load(2, bytes.data(), u32(bytes.size())));
    CHECK(manager.sprite_fallback(2));
    CHECK(reader.read("artifacts/reference/raw/effect.anm", bytes));
    const bool loaded = manager.load(8, bytes.data(), u32(bytes.size()));
    if (!loaded) std::printf("load8 error=%s\n", manager.error.c_str());
    CHECK(loaded);
    const Vec3 spawn{-64.f, 32.f, 0.f};
    const u32 handle = manager.create(8, 0, -1, 0, spawn);
    auto* vm = manager.registry.find(handle);
    CHECK(vm);
    if (!vm) return;
    const u32 draws_before_spawn = random.index();
    const u16 seed_before = random.seed();
    CHECK(manager.spawn_effect(*vm, 1));
    CHECK(vm->geometry.trail);
    CHECK_EQ(vm->visual.layer, 13);
    CHECK_EQ(bits(vm->visual.translation.x), bits(0.f));
    CHECK_EQ(bits(vm->visual.translation.y), bits(0.f));
    CHECK(vm->visual.flags & 0x20u);
    auto& trail = *vm->geometry.trail;
    CHECK_EQ(trail.segment_count, 8);
    CHECK_EQ(vm->geometry.allocation_bytes, 0x78u);
    CHECK_EQ(trail.age.current, 1);
    CHECK_EQ(trail.age.previous, 0);
    CHECK_EQ(bits(trail.points[0].x), bits(256.f));
    CHECK_EQ(bits(trail.points[0].y), bits(48.f));
    CHECK_EQ(trail.colors[0], 0xffff80ffu);
    CHECK_EQ(trail.colors[1], 0xffdf80ffu);
    CHECK_EQ(trail.colors[4], 0xff7f80ffu);
    CHECK_EQ(trail.colors[5], 0xef5f80ffu);
    CHECK_EQ(trail.colors[7], 0xcf1f80ffu);
    GameRandom probe{seed_before};
    CHECK_EQ(bits(trail.angle), bits(float(probe.signed_unit() * 3.1415927410125732f)));
    CHECK_EQ(random.index(), draws_before_spawn + 2);
    const float angle0 = trail.angle;
    const u16 seed0 = random.seed();
    const u32 draws0 = random.index();
    CHECK_EQ(trail.update(random, 1.f), 0);
    CHECK_EQ(random.index(), draws0 + 4);
    CHECK_EQ(trail.age.current, 2);
    CHECK_EQ(trail.age.previous, 1);
    GameRandom probe2{seed0};
    const float speed = float(probe2.unit() * 5.f + 4.f);
    CHECK_EQ(bits(trail.points[1].x), bits(float(256.f + std::cos(double(angle0)) * double(speed))));
    CHECK_EQ(bits(trail.points[1].y), bits(float(48.f + std::sin(double(angle0)) * double(speed))));
    const float delta = float(probe2.signed_unit() * 3.1415927410125732f / 5.f);
    CHECK_EQ(bits(trail.angle), bits(normalize_angle(float(angle0 + delta))));
    CHECK_EQ(trail.colors[0] >> 24, 239u);
    for (i32 frame = 0; frame < 6; ++frame)
        CHECK_EQ(trail.update(random, 1.f), 0);
    CHECK_EQ(trail.age.current, 8);
    CHECK_EQ(trail.colors[6] >> 24, 207u);
    CHECK_EQ(trail.update(random, 1.f), 1);
}

TEST(scene_effect_callback_contract_kind6_unchanged) {
    StageSim sim;
    sim.commands.sim = &sim;
    StageEnemy enemy;
    sim.current = &enemy;
    bool called = false;
    sim.scene_effect = [&](i32 kind, const Vec3&) {
        called = kind == 6;
        return called;
    };
    EclContext context;
    std::vector<u8> code;
    th13test::emit(code, 0, 334, 0, 255, 0, {6});
    context.instruction = reinterpret_cast<const EclInstruction*>(code.data());
    CHECK_EQ(sim.commands.execute(context, 334), 0);
    CHECK(called);
}

int main() { return th13test::run_all("scene-effect-dispatch"); }
