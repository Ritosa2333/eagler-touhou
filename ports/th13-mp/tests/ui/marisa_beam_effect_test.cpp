#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StagePresentation.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32, const void*, u32) override {}
    void primitives(touhou::graphics::Topology, u32, const void*, u32) override {}
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

TEST(beam_hit_creates_player_script_with_native_motion) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager animations(random, environment);
    Graphics graphics;
    AnmRenderer renderer(graphics);
    const auto bytes = synthetic_anm(1, trivial_scripts(16));
    CHECK(load_bank(animations, 6, bytes));
    StagePresentation presentation(animations, renderer, {4, 5, 6}, 0);
    CHECK(presentation.beam_hit_effect({7, 100, 0}, {0, -64, 0}, -1.5f));
    CHECK_EQ(animations.registry.ordered_count(false), 1u);
    auto* vm = animations.registry.find(animations.registry.ordered_handle(false, 0));
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->source_script, 15);
    CHECK_EQ(vm->visual.translation.x, 7.f);
    CHECK_EQ(vm->visual.translation.y, 100.f);
    CHECK_EQ(vm->variables.rotation.z, -1.5f);
    CHECK(vm->visual.flags & 4u);
    CHECK_EQ(vm->interpolators.position.duration, 20);
    CHECK_EQ(vm->interpolators.position.mode, 4);
    CHECK_EQ(vm->interpolators.position.start[1], 0.f);
    CHECK_EQ(vm->interpolators.position.end[1], -64.f);
    CHECK_EQ(vm->interpolators.position.timer.current, 0);
}

TEST(youmu_slash_resolves_created_player_vm_before_collision) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager animations(random, environment);
    Graphics graphics;
    AnmRenderer renderer(graphics);
    auto scripts = trivial_scripts(21);
    scripts[19] = {AnmIns{401, 0, {0, 0, i32(float_to_bits(1.f))}}, wait(), stop()};
    const auto bytes = synthetic_anm(1, scripts);
    CHECK(load_bank(animations, 6, bytes));
    StagePresentation presentation(animations, renderer, {4, 5, 6}, 0);
    StageBattle battle;
    battle.sim.set_time_rate(.5f);
    const auto spawn = battle.youmu_bomb_.step(0, battle.sim.random);
    CHECK(spawn.has_value());
    if (!spawn) return;
    battle.youmu_slash_spawns_.push_back(*spawn);
    CHECK(presentation.sync_youmu_slash(battle));
    CHECK(battle.youmu_bomb_.active(0));
    CHECK_EQ(battle.youmu_bomb_.angle(0), 1.f);
    CHECK_EQ(battle.youmu_bomb_.rect_scale(0), 0.f);
    CHECK_EQ(battle.youmu_bomb_.scale(0), .5f);
    CHECK_EQ(battle.youmu_bomb_.rect(0).h, 0.f);
}

TEST(youmu_charge_effect_is_created_interrupted_and_retired_synchronously) {
    GameRandom random; AnmEnvironment environment;
    AnmManager animations(random, environment); Graphics graphics;
    AnmRenderer renderer(graphics);
    CHECK(load_bank(animations, 6, synthetic_anm(1, trivial_scripts(19))));
    StagePresentation presentation(animations, renderer, {4, 5, 6}, 0);
    CHECK(presentation.youmu_charge_effect(0, {3, 200, 0}));
    const u32 handle = animations.registry.ordered_handle(false, 0);
    auto* vm = animations.registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->source_script, 18);
    CHECK(presentation.youmu_charge_effect(3, {9, 210, 0}));
    CHECK_EQ(vm->visual.translation.x, 9.f);
    CHECK_EQ(vm->visual.translation.y, 210.f);
    CHECK(presentation.youmu_charge_effect(1, {}));
    CHECK_EQ(vm->pending_interrupt, 2);
    CHECK(presentation.youmu_charge_effect(2, {}));
    CHECK(vm->visual.render_flags & 0x20u);
}

TEST(shot_birth_initializes_before_its_separate_manager_update) {
    GameRandom random; AnmEnvironment environment;
    AnmManager animations(random, environment); Graphics graphics;
    AnmRenderer renderer(graphics);
    auto scripts = trivial_scripts(13);
    // pl01 script11's time1 detached spawn is the original f2341 mechanism.
    // Its child's time0 random read must occur in that same frame, after the
    // parent's separate time0 initialization and time1 manager update.
    scripts[11] = {{300, -1, {0}}, {0, 1, {}}, {504, 1, {12}}, wait(), stop()};
    scripts[12] = {{100, 0, {10000, 10022}, 2}, wait(), stop()};
    CHECK(load_bank(animations, 6, synthetic_anm(1, scripts)));
    StagePresentation presentation(animations, renderer, {4, 5, 6}, 0);
    PlayerShot shot; shot.alive = true; shot.script = 11; shot.generation = 19;
    const auto before = random.index();
    CHECK(presentation.initialize_player_shot(shot, 0));
    CHECK_EQ(random.index(), before);
    // 0x445c40: the birth path itself allocates, template-copies, primes one
    // tick and tail-inserts (0x46f3f0), so the VM is registered immediately.
    CHECK_EQ(animations.registry.count(), 1u);
    // Repeated observation of the already initialized shot must not tick it.
    CHECK(presentation.initialize_player_shot(shot, 0));
    CHECK_EQ(random.index(), before);
    CHECK_EQ(animations.registry.count(), 1u);
    CHECK(load_bank(animations, 4, synthetic_anm(1, trivial_scripts(1))));
    CHECK(load_bank(animations, 5, synthetic_anm(1, trivial_scripts(1))));
    StageBattle battle;
    battle.player_phase = StageBattle::PlayerPhase::Dying;
    battle.shots.push_back(shot);
    // 0x4464d0 semantics: sync only writes the live pose; the chain tick that
    // advances the script (and spawns the script-12 child) is the main-pool
    // update, which runs after sync in the production frame order.
    CHECK(presentation.sync(battle, 0));
    CHECK_EQ(random.index(), before);
    CHECK(animations.update(false));
    CHECK_EQ(random.index(), before + 2);
    CHECK_EQ(animations.registry.count(), 2u);
    auto* child = animations.registry.find(animations.registry.ordered_handle(false, 1));
    CHECK(child != nullptr);
    if (child) CHECK_EQ(child->source_script, 12);
}
}

int main() { return th13test::run_all("Marisa beam hit effect"); }
