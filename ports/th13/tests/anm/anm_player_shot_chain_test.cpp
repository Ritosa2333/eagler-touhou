#include "anm_test_fixtures.hpp"
#include "game/AnmRenderer.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/ZunGraphics.hpp"

#include <memory>

using namespace th13;

namespace {

i32 bits(float value) {
    i32 result;
    std::memcpy(&result, &value, 4);
    return result;
}

struct NullGraphics final : ZunGraphics {
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

struct Fixture {
    GameRandom random{7};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    StagePresentation presentation{manager, renderer, {4, 5, 6}, 0};
    u32 parent = 0;
    bool ready = false;

    Fixture() {
        std::vector<u8> parent_script, child_script, shot_script;
        th13test::push_insn(parent_script, 500, 2, 0, {1});
        th13test::push_insn(parent_script, 3, 0);
        th13test::push_terminator(parent_script);
        th13test::push_insn(child_script, 117, 0, 3, {bits(10010.f), bits(10011.f), bits(0.5f)});
        th13test::push_insn(child_script, 3, 0);
        th13test::push_terminator(child_script);
        th13test::push_insn(shot_script, 3, 0);
        th13test::push_terminator(shot_script);
        const auto bullet = th13test::build_simple_anm({parent_script, child_script});
        const auto player = th13test::build_simple_anm({shot_script});
        if (!manager.load(5, bullet.data(), u32(bullet.size()))) return;
        if (!manager.load(6, player.data(), u32(player.size()))) return;
        parent = manager.create(5, 0, -1, 0);
        if (!parent) return;
        ready = true;
    }

    bool chain() { return manager.update(false); }

    u32 spawn_shot(u32 index, u32 generation) {
        PlayerShot shot;
        shot.alive = true;
        shot.script = 0;
        shot.generation = generation;
        if (!presentation.initialize_player_shot(shot, index)) return 0;
        const u32 count = manager.registry.ordered_count(false);
        return count ? manager.registry.ordered_handle(false, count - 1) : 0;
    }

    AnmVm* child() {
        for (u32 h = 1; h < 0x10000u; ++h)
            if (auto* vm = manager.registry.find(h))
                if (vm->source_script == 1) return vm;
        return nullptr;
    }
};

}

TEST(player_shot_vm_is_tail_inserted_main_chain_member) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    const u32 first = fixture.spawn_shot(0, 11);
    const u32 second = fixture.spawn_shot(1, 22);
    CHECK(first != 0);
    CHECK(second != 0);
    CHECK_EQ(fixture.manager.registry.ordered_handle(false, 0), fixture.parent);
    CHECK_EQ(fixture.manager.registry.ordered_handle(false, 1), first);
    CHECK_EQ(fixture.manager.registry.ordered_handle(false, 2), second);
    auto* a = fixture.manager.registry.find(first);
    auto* b = fixture.manager.registry.find(second);
    CHECK(a && b);
    if (!a || !b) return;
    CHECK_EQ(a->age.current, 1);
    CHECK_EQ(b->age.current, 1);
    CHECK(fixture.chain());
    CHECK_EQ(a->age.current, 2);
    CHECK_EQ(b->age.current, 2);
}

TEST(birth_path_writes_the_shot_pose_into_the_registered_vm) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    PlayerShot shot;
    shot.alive = true;
    shot.script = 0;
    shot.generation = 51;
    shot.x = 123.5f;
    shot.y = 45.25f;
    CHECK(fixture.presentation.initialize_player_shot(shot, 0));
    const u32 count = fixture.manager.registry.ordered_count(false);
    const u32 handle = count ? fixture.manager.registry.ordered_handle(false, count - 1) : 0;
    auto* vm = fixture.manager.registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_NEAR(vm->visual.translation.x, 123.5, 1e-6);
    CHECK_NEAR(vm->visual.translation.y, 45.25, 1e-6);
    CHECK_NEAR(vm->visual.translation.z, 0.0, 1e-6);
}

TEST(tail_inserted_player_shot_lets_child_tick_same_frame) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    CHECK(fixture.spawn_shot(0, 31) != 0);
    for (int frame = 0; frame < 3 && !fixture.child(); ++frame)
        CHECK(fixture.chain());
    AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->age.current, 2);
    CHECK(fixture.manager.registry.find(fixture.parent) != nullptr);
}

TEST(tail_parent_without_follower_misses_same_frame_child_tick) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    for (int frame = 0; frame < 3 && !fixture.child(); ++frame)
        CHECK(fixture.chain());
    AnmVm* child = fixture.child();
    CHECK(child != nullptr);
    if (!child) return;
    CHECK_EQ(child->age.current, 1);
}

TEST(retired_player_shot_is_collected_without_chain_tick) {
    Fixture fixture;
    CHECK(fixture.ready);
    if (!fixture.ready) return;
    const u32 handle = fixture.spawn_shot(0, 41);
    CHECK(handle != 0);
    AnmVm* vm = fixture.manager.registry.find(handle);
    CHECK(vm != nullptr);
    if (!vm) return;
    CHECK_EQ(vm->age.current, 1);
    u32 retired = handle;
    CHECK(fixture.manager.retire(retired));
    CHECK_EQ(retired, 0u);
    CHECK_EQ(vm->visual.render_flags & 0x20u, 0x20u);
    CHECK(fixture.chain());
    CHECK_EQ(vm->age.current, 1);
    CHECK(fixture.manager.registry.find(handle) == nullptr);
}

TEST(presentation_installs_and_clears_the_main_chain_hook) {
    GameRandom random{3};
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    NullGraphics graphics;
    AnmRenderer renderer{graphics};
    {
        auto presentation =
            std::make_unique<StagePresentation>(manager, renderer, StagePresentation::Banks{4, 5, 6}, 0);
        CHECK(manager.after_main_update != nullptr);
        CHECK(manager.update(false));
        CHECK(presentation->error().empty());
        presentation->release();
        CHECK(manager.update(false));
        CHECK(presentation->error().empty());
        presentation.reset();
    }
    CHECK(manager.after_main_update == nullptr);
}

int main() { return th13test::run_all("player shot main chain membership"); }
