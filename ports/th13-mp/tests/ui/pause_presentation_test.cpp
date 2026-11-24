#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/AnmRenderer.hpp"
#include "game/FrameScheduler.hpp"
#include "game/RunPause.hpp"
#include "game/StagePresentation.hpp"

#include <fstream>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> asset(const char* name) {
    std::ifstream file(std::string("artifacts/reference/raw/") + name, std::ios::binary);
    return std::vector<u8>{std::istreambuf_iterator<char>(file), {}};
}

struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState pipeline_state;
    touhou::graphics::PipelineState& pipeline() override { return pipeline_state; }
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

struct PauseHost final : RunPausePlatform {
    bool sound(i32) override { return true; }
    bool suspend_music() override { return true; }
    bool resume_music() override { return true; }
    int restarts = 0, titles = 0, continues = 0;
    bool restart_run() override {
        ++restarts;
        return true;
    }
    bool return_to_title() override {
        ++titles;
        return true;
    }
    bool continue_run() override {
        ++continues;
        return true;
    }
};

struct RealPause {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    FrameScheduler scheduler;
    PauseHost host;
    std::unique_ptr<RunPause> pause;
    bool ready = false;

    RealPause() {
        const auto text = asset("text.anm");
        const auto front = asset("front.anm");
        CHECK(!text.empty());
        CHECK(!front.empty());
        if (text.empty() || front.empty()) return;
        CHECK(manager.load(0, text.data(), u32(text.size())));
        CHECK(manager.load(3, front.data(), u32(front.size())));
        progress.rate = 1;
        pause = std::make_unique<RunPause>(progress, manager, environment, host, scheduler, 3, 0);
        ready = pause->prepare();
        CHECK(ready);
    }

    void frame(u32 pressed = 0) {
        if (!pause) return;
        pause->controls({pressed, 0, false});
        CHECK(scheduler.update() >= 0);
        CHECK(manager.update());
    }
    void advance(int frames, u32 pressed = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed);
    }
    AnmVm* root() { return pause ? manager.registry.find(pause->state.menu_animation) : nullptr; }
    AnmVm* backdrop() { return pause ? manager.registry.find(pause->state.snapshot_animation) : nullptr; }
    AnmVm* child(i32 script) {
        auto* menu = root();
        return menu ? manager.registry.find_child_script(*menu, script, 0) : nullptr;
    }
};

void check_children(RealPause& h, std::initializer_list<i32> scripts) {
    for (i32 script : scripts) {
        auto* vm = h.child(script);
        CHECK(vm != nullptr);
        if (vm) CHECK_EQ(vm->source_script, script);
    }
}

}

TEST(real_pause_overlay_builds_text75_backdrop_and_front132_tree) {
    RealPause h;
    if (!h.ready) return;
    CHECK(h.pause->open(PauseEntrance::Pause));
    CHECK(h.pause->active());
    CHECK_EQ(h.progress.scene_flags & 0x10u, 0x10u);
    CHECK(h.environment.paused);

    auto* backdrop = h.backdrop();
    CHECK(backdrop != nullptr);
    if (!backdrop) return;
    CHECK(backdrop->resource == h.manager.resource(0));
    CHECK(backdrop->resource != h.manager.resource(3));
    CHECK_EQ(backdrop->source_script, 75);
    for (i32 script : {76, 77, 78, 79, 80})
        CHECK(h.manager.registry.find_child_script(*backdrop, script, 0) != nullptr);

    auto* menu = h.root();
    CHECK(menu != nullptr);
    if (!menu) return;
    CHECK(menu->resource == h.manager.resource(3));
    CHECK_EQ(menu->source_script, 132);
    check_children(h, {120, 121, 122, 154, 123});
    check_children(h, {117, 118});
    CHECK(h.child(129) == nullptr);
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    h.frame(0x20);
    CHECK_EQ(h.pause->state.menu.cursor, 1);
    h.frame(1);
    h.advance(21);
    CHECK_EQ(h.pause->state.phase, 9);
    check_children(h, {129, 130, 131});
    auto* yes = h.child(130);
    auto* no = h.child(131);
    CHECK(yes != nullptr);
    CHECK(no != nullptr);
    if (yes) CHECK_EQ(yes->source_script, 130);
    if (no) CHECK_EQ(no->source_script, 131);
    h.frame(0x102);
    CHECK_EQ(h.pause->state.phase, 10);
    h.advance(21);
    CHECK_EQ(h.pause->state.phase, 6);
    CHECK_EQ(h.host.titles, 0);
    CHECK(h.pause->error.empty());
}

TEST(real_practice_pause_uses_front133_entries) {
    RealPause h;
    if (!h.ready) return;
    h.progress.practice_mode = 1;
    CHECK(h.pause->open(PauseEntrance::Pause));
    auto* menu = h.root();
    CHECK(menu != nullptr);
    if (!menu) return;
    CHECK_EQ(menu->source_script, 133);
    check_children(h, {124, 125, 126});
    CHECK(h.child(120) == nullptr);
    CHECK(h.child(154) == nullptr);
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    CHECK(h.pause->state.practice);
    h.frame(0x20);
    CHECK_EQ(h.pause->state.menu.cursor, 1);
    h.frame(1);
    CHECK_EQ(h.pause->state.phase, 16);
    h.advance(21);
    CHECK(!h.pause->active());
    CHECK_EQ(h.host.titles, 1);
    CHECK(h.pause->error.empty());
}

TEST(real_practice_end_uses_front134_entries) {
    RealPause h;
    if (!h.ready) return;
    h.progress.practice_mode = 1;
    CHECK(h.pause->open(PauseEntrance::PracticeEnd));
    auto* menu = h.root();
    CHECK(menu != nullptr);
    if (!menu) return;
    CHECK_EQ(menu->source_script, 134);
    check_children(h, {127, 128});
    CHECK(h.child(120) == nullptr);
    CHECK(h.child(121) == nullptr);
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    CHECK_EQ(h.pause->state.menu.cursor, 1);
    h.frame(0x100);
    CHECK(h.pause->active());
    CHECK_EQ(h.pause->state.menu.cursor, 1);
    h.frame(1);
    CHECK_EQ(h.pause->state.phase, 16);
    h.advance(21);
    CHECK(!h.pause->active());
    CHECK_EQ(h.host.titles, 1);
    CHECK_EQ(h.host.continues, 0);
    CHECK(h.pause->error.empty());
}

TEST(real_game_over_uses_front142_entries_without_cursor_children) {
    RealPause h;
    if (!h.ready) return;
    h.progress.continue_stock = 1;
    CHECK(h.pause->open(PauseEntrance::GameOver));
    auto* menu = h.root();
    CHECK(menu != nullptr);
    if (!menu) return;
    CHECK_EQ(menu->source_script, 142);
    check_children(h, {137, 138, 139, 140, 141});
    check_children(h, {135, 136});
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    CHECK_EQ(h.pause->state.menu.cursor, 0);
    h.frame(1);
    h.advance(15);
    CHECK(!h.pause->active());
    CHECK_EQ(h.host.continues, 1);
    CHECK(!h.environment.paused);
    CHECK(h.pause->error.empty());
}

TEST(real_extra_results_use_front147_entries) {
    RealPause h;
    if (!h.ready) return;
    CHECK(h.pause->open(PauseEntrance::ExtraEnd));
    auto* menu = h.root();
    CHECK(menu != nullptr);
    if (!menu) return;
    CHECK_EQ(menu->source_script, 147);
    check_children(h, {144, 145, 146});
    check_children(h, {135});
    CHECK(h.child(137) == nullptr);
    CHECK(h.child(141) == nullptr);
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    CHECK_EQ(h.pause->state.menu.count, 3);
    CHECK_EQ(h.pause->state.menu.cursor, 0);
    h.frame(1);
    h.advance(15);
    CHECK(!h.pause->active());
    CHECK_EQ(h.host.titles, 1);
    CHECK(h.environment.paused);
    CHECK(h.pause->error.empty());
}

TEST(real_overlay_trees_are_collected_after_continue_close_and_title) {
    RealPause h;
    if (!h.ready) return;
    const u32 baseline = h.manager.registry.count();
    CHECK_EQ(baseline, 0u);

    CHECK(h.pause->open(PauseEntrance::Pause));
    const u32 menu_handle = h.pause->state.menu_animation;
    const u32 backdrop_handle = h.pause->state.snapshot_animation;
    CHECK(menu_handle != 0);
    CHECK(backdrop_handle != 0);
    std::vector<u32> children;
    for (i32 script : {120, 121, 122, 154, 123, 117, 118}) {
        auto* vm = h.child(script);
        CHECK(vm != nullptr);
        if (vm) children.push_back(h.manager.registry.handle(*vm));
    }
    CHECK_EQ(children.size(), 7u);
    h.advance(12);
    h.frame(1);
    h.advance(15);
    CHECK(!h.pause->active());
    CHECK_EQ(h.pause->state.menu_animation, 0u);
    CHECK_EQ(h.pause->state.snapshot_animation, 0u);
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
    CHECK(h.manager.registry.find(menu_handle) == nullptr);
    CHECK(h.manager.registry.find(backdrop_handle) == nullptr);
    for (u32 handle : children)
        CHECK(h.manager.registry.find(handle) == nullptr);

    CHECK(h.pause->open(PauseEntrance::Pause));
    const u32 menu_escape = h.pause->state.menu_animation;
    h.advance(12);
    h.frame(0x100);
    h.advance(15);
    CHECK(!h.pause->active());
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
    CHECK(h.manager.registry.find(menu_escape) == nullptr);

    CHECK(h.pause->open(PauseEntrance::Pause));
    const u32 menu_title = h.pause->state.menu_animation;
    h.advance(12);
    h.frame(0x20);
    h.frame(1);
    h.advance(21);
    h.frame(0x10);
    h.frame(1);
    h.advance(21);
    h.advance(13);
    CHECK_EQ(h.host.titles, 1);
    CHECK(!h.pause->active());
    CHECK(h.environment.paused);
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
    CHECK(h.manager.registry.find(menu_title) == nullptr);
    CHECK(h.pause->error.empty());
}

TEST(focus_ring_script22_freeze_is_owned_by_stage_presentation_not_run_pause) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    const auto effect = asset("effect.anm");
    const auto front = asset("front.anm");
    const auto text = asset("text.anm");
    const auto ascii = asset("ascii.anm");
    CHECK(!effect.empty());
    CHECK(!front.empty());
    CHECK(!text.empty());
    CHECK(!ascii.empty());
    if (effect.empty() || front.empty() || text.empty() || ascii.empty()) return;
    CHECK(manager.load(0, text.data(), u32(text.size())));
    CHECK(manager.load(2, ascii.data(), u32(ascii.size())));
    CHECK(manager.sprite_fallback(2));
    CHECK(manager.load(3, front.data(), u32(front.size())));
    CHECK(manager.load(8, effect.data(), u32(effect.size())));

    Graphics graphics;
    AnmRenderer renderer(graphics);
    StagePresentation presentation(manager, renderer, {4, 5, 6, 8}, 0);
    const u32 focus = presentation.player_effect(22, {0, 300, 0}, 12);
    CHECK(focus != 0);
    auto* focus_root = manager.registry.find(focus);
    CHECK(focus_root != nullptr);
    if (!focus_root) return;
    CHECK_EQ(focus_root->source_script, 22);
    CHECK(focus_root->resource == manager.resource(8));
    CHECK(focus_root->pre_tick != nullptr);
    auto* focus_child = manager.registry.find_child_script(*focus_root, 21, 0);
    CHECK(focus_child != nullptr);
    if (!focus_child) return;
    CHECK_EQ(focus_child->source_script, 21);
    CHECK(focus_child->pre_tick != nullptr);

    const u32 bare = manager.create(8, 22, 12, 0, {40, 300, 0});
    CHECK(bare != 0);
    auto* bare_root = manager.registry.find(bare);
    CHECK(bare_root != nullptr);
    if (!bare_root) return;
    bare_root->visual.render_flags = (bare_root->visual.render_flags & ~0xc000u) | 0x8000u;
    const u32 gated = manager.create(8, 22, 12, 0, {80, 300, 0});
    CHECK(gated != 0);
    auto* gated_root = manager.registry.find(gated);
    CHECK(gated_root != nullptr);
    if (!gated_root) return;
    CHECK_EQ(gated_root->visual.render_flags & 0xc000u, 0x4000u);
    CHECK(bare_root->pre_tick == nullptr);
    auto* bare_child = manager.registry.find_child_script(*bare_root, 21, 0);
    CHECK(bare_child != nullptr);
    if (!bare_child) return;
    CHECK(bare_child->pre_tick == nullptr);

    for (int i = 0; i < 5; i++)
        CHECK(manager.update());
    const i32 root_time = focus_root->timer.current;
    const i32 child_time = focus_child->timer.current;
    const i32 root_age = focus_root->age.current;
    const i32 bare_time = bare_root->timer.current;
    const i32 gated_time = gated_root->timer.current;
    const float root_rotation = focus_root->variables.rotation.z;

    SessionState progress;
    progress.rate = 1;
    FrameScheduler scheduler;
    PauseHost host;
    auto pause = std::make_unique<RunPause>(progress, manager, environment, host, scheduler, 3, 0);
    CHECK(pause->prepare());
    CHECK(pause->open(PauseEntrance::Pause));
    CHECK(environment.paused);
    auto* menu = manager.registry.find(pause->state.menu_animation);
    CHECK(menu != nullptr);
    const i32 menu_age = menu ? menu->age.current : 0;
    for (int i = 0; i < 12; i++) {
        pause->controls({0, 0, false});
        CHECK(scheduler.update() >= 0);
        CHECK(manager.update());
    }
    CHECK_EQ(pause->state.phase, 6);
    CHECK_EQ(focus_root->timer.current, root_time);
    CHECK_EQ(focus_child->timer.current, child_time);
    CHECK_EQ(focus_root->age.current, root_age);
    CHECK_EQ(focus_root->variables.rotation.z, root_rotation);
    CHECK(bare_root->timer.current > bare_time);
    CHECK_EQ(gated_root->timer.current, gated_time);
    if (menu) CHECK(menu->age.current > menu_age);
    CHECK(manager.registry.find(focus) != nullptr);
    CHECK(manager.registry.find(bare) != nullptr);

    pause->controls({1, 0, false});
    CHECK(scheduler.update() >= 0);
    CHECK(manager.update());
    for (int i = 0; i < 15; i++) {
        pause->controls({0, 0, false});
        CHECK(scheduler.update() >= 0);
        CHECK(manager.update());
    }
    CHECK(!pause->active());
    CHECK(!environment.paused);
    CHECK(focus_root->timer.current > root_time);
    CHECK(focus_child->timer.current > child_time);
    CHECK(pause->error.empty());
}

TEST(real_pause_resource_identity_and_wrong_script_negative_controls) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    const auto text = asset("text.anm");
    const auto front = asset("front.anm");
    CHECK(!text.empty());
    CHECK(!front.empty());
    if (text.empty() || front.empty()) return;
    CHECK(manager.load(0, text.data(), u32(text.size())));
    CHECK(manager.load(3, front.data(), u32(front.size())));

    const u32 wrong_front = manager.create(3, 2, -1, 4);
    const u32 wrong_text = manager.create(0, 0, -1, 4);
    CHECK(wrong_front != 0);
    CHECK(wrong_text != 0);
    auto* wrong_front_vm = manager.registry.find(wrong_front);
    auto* wrong_text_vm = manager.registry.find(wrong_text);
    CHECK(wrong_front_vm != nullptr);
    CHECK(wrong_text_vm != nullptr);
    if (!wrong_front_vm || !wrong_text_vm) return;
    CHECK(manager.registry.children(*wrong_front_vm).empty());
    CHECK(manager.registry.children(*wrong_text_vm).empty());
    CHECK(manager.registry.find_child_script(*wrong_front_vm, 120, 0) == nullptr);
    CHECK(manager.registry.find_child_script(*wrong_text_vm, 76, 0) == nullptr);

    const u32 right_front = manager.create(3, 132, -1, 4);
    const u32 right_text = manager.create(0, 75, -1, 4);
    CHECK(right_front != 0);
    CHECK(right_text != 0);
    auto* right_front_vm = manager.registry.find(right_front);
    auto* right_text_vm = manager.registry.find(right_text);
    CHECK(right_front_vm != nullptr);
    CHECK(right_text_vm != nullptr);
    if (!right_front_vm || !right_text_vm) return;
    CHECK(manager.registry.find_child_script(*right_front_vm, 120, 0) != nullptr);
    CHECK(manager.registry.find_child_script(*right_text_vm, 76, 0) != nullptr);
    const auto before_failed_open = manager.registry.count();

    SessionState progress;
    progress.rate = 1;
    FrameScheduler scheduler;
    PauseHost host;
    RunPause swapped(progress, manager, environment, host, scheduler, 0, 3);
    CHECK(swapped.prepare());
    CHECK(!swapped.open(PauseEntrance::Pause));
    CHECK(!swapped.error.empty());
    CHECK_EQ(swapped.state.menu_animation, 0u);
    CHECK_EQ(swapped.state.snapshot_animation, 0u);
    CHECK(!swapped.active());
    CHECK_EQ(progress.scene_flags & 0x10u, 0u);
    CHECK(!environment.paused);
    CHECK(manager.update());
    CHECK_EQ(manager.registry.count(), before_failed_open);
}

int main() { return th13test::run_all("暂停覆盖层真实资源"); }
