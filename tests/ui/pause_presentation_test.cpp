// 暂停覆盖层真实资源测试（新增独立文件；不改既有 pause_test.cpp、产品代码）。
// 覆盖三条此前合成 bank 无法证明的数据面：
// 1) open 用真实 text.anm script75 建背景，用真实 front.anm 132/133/134/142/147
//    建菜单根，并真正出生 120-123/154、124-126、127/128、137-141、144-146；
//    cursor 129/130/131 只在 root 收到 label14 后出生（132 的 Yes/No 路径）。
// 2) 继续 / Esc / 回标题关闭后 overlay 树经 0x20 回收，句柄与 registry 计数归零。
// 3) focus ring script22/子21 的 timer 冻结归属：StagePresentation 的
//    create_player_effect 安装 pre_tick，RunPause 只切 environment.paused。
//    用同 manager 直建、无 pre_tick 的 script22 作反证，不越权改产品。
// 负控：错误脚本（front2/text0）不得产出暂停子 VM；错误 bank 身份（132 不在
// text.anm）必须让 open 失败，而不是靠“句柄非零”放行。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

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

// 真实资源夹具：text.anm 进 bank0、front.anm 进 bank3，与 RunPause 生产槽一致。
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
        for (int i = 0; i < frames; i++) frame(pressed);
    }
    AnmVm* root() { return pause ? manager.registry.find(pause->state.menu_animation) : nullptr; }
    AnmVm* backdrop() {
        return pause ? manager.registry.find(pause->state.snapshot_animation) : nullptr;
    }
    AnmVm* child(i32 script) {
        auto* menu = root();
        return menu ? manager.registry.find_child_script(*menu, script, 0) : nullptr;
    }
};

// 所有暂停菜单项的真实子脚本都存在，且 source_script 与请求脚本一致。
void check_children(RealPause& h, std::initializer_list<i32> scripts) {
    for (i32 script : scripts) {
        auto* vm = h.child(script);
        CHECK(vm != nullptr);
        if (vm) CHECK_EQ(vm->source_script, script);
    }
}

}  // namespace

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
    // 背景来自常驻 text.anm（bank0）script75，不是 front.anm 的同号脚本。
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
    check_children(h, {117, 118});  // 真实根 132 的行条装饰子体
    // cursor 129/130/131 在 label14 之后才出生；open 只 interrupt(3)。
    CHECK(h.child(129) == nullptr);
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    // 回标题项确认 → phase8 的 interrupt(root,14) 才出生真实 cursor 子体。
    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    h.frame(0x20);  // ↓ → タイトルへ
    CHECK_EQ(h.pause->state.menu.cursor, 1);
    h.frame(1);  // 确认 → 20 帧闪烁
    h.advance(21);
    CHECK_EQ(h.pause->state.phase, 9);
    check_children(h, {129, 130, 131});
    auto* yes = h.child(130);
    auto* no = h.child(131);
    CHECK(yes != nullptr);
    CHECK(no != nullptr);
    if (yes) CHECK_EQ(yes->source_script, 130);
    if (no) CHECK_EQ(no->source_script, 131);
    // 默认 No（131）取消后回到菜单，不触发回标题。
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
    h.frame(0x20);  // ↓ → タイトルへ
    CHECK_EQ(h.pause->state.menu.cursor, 1);
    h.frame(1);
    // practice 分支不进入 Yes/No（无 cursor 130/131），12 帧后直接执行。
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
    CHECK_EQ(h.pause->state.menu.cursor, 1);  // 继续项被禁用，光标落タイトルへ
    // 结束局 Esc 不恢复；菜单仍由真实 134 树承载。
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
    // 142 的真实脚本没有 cursor 130/131；GameOver 也不走 Yes/No。
    CHECK(h.child(130) == nullptr);
    CHECK(h.child(131) == nullptr);

    h.advance(12);
    CHECK_EQ(h.pause->state.phase, 6);
    CHECK_EQ(h.pause->state.menu.cursor, 0);
    h.frame(1);  // 继续
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
    h.frame(1);  // タイトルへ
    h.advance(15);
    CHECK(!h.pause->active());
    CHECK_EQ(h.host.titles, 1);
    // 回标题由宿主切场景；暂停位保持（原版冻结语义，不由本测试改）。
    CHECK(h.environment.paused);
    CHECK(h.pause->error.empty());
}

TEST(real_overlay_trees_are_collected_after_continue_close_and_title) {
    RealPause h;
    if (!h.ready) return;
    const u32 baseline = h.manager.registry.count();
    CHECK_EQ(baseline, 0u);

    // 继续：retire(menu/snapshot) 后 0x20 树在下一次 manager.update 回收。
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
    for (u32 handle : children) CHECK(h.manager.registry.find(handle) == nullptr);

    // Esc 关闭同样回收。
    CHECK(h.pause->open(PauseEntrance::Pause));
    const u32 menu_escape = h.pause->state.menu_animation;
    h.advance(12);
    h.frame(0x100);
    h.advance(15);
    CHECK(!h.pause->active());
    CHECK(h.manager.update());
    CHECK_EQ(h.manager.registry.count(), baseline);
    CHECK(h.manager.registry.find(menu_escape) == nullptr);

    // 回标题：overlay 回收，但 run 仍冻结等宿主换场景。
    CHECK(h.pause->open(PauseEntrance::Pause));
    const u32 menu_title = h.pause->state.menu_animation;
    h.advance(12);
    h.frame(0x20);
    h.frame(1);
    h.advance(21);
    h.frame(0x10);  // Yes
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
    // 生产同构：ascii bank 兼任负值精灵回落（Application.cpp:266），effect 的
    // 模板在装载期消费 ins_300(-1)，缺回落会整体装载失败。
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
    // 停机回调由 StagePresentation::create_player_effect 安装（实际所有者）。
    CHECK(focus_root->pre_tick != nullptr);
    auto* focus_child = manager.registry.find_child_script(*focus_root, 21, 0);
    CHECK(focus_child != nullptr);
    if (!focus_child) return;
    CHECK_EQ(focus_child->source_script, 21);
    CHECK(focus_child->pre_tick != nullptr);

    // 负控：同 manager 直建 script22，不经过 create_player_effect → 无 pre_tick。
    const u32 bare = manager.create(8, 22, 12, 0, {40, 300, 0});
    CHECK(bare != 0);
    auto* bare_root = manager.registry.find(bare);
    CHECK(bare_root != nullptr);
    if (!bare_root) return;
    CHECK(bare_root->pre_tick == nullptr);
    auto* bare_child = manager.registry.find_child_script(*bare_root, 21, 0);
    CHECK(bare_child != nullptr);
    if (!bare_child) return;
    CHECK(bare_child->pre_tick == nullptr);

    for (int i = 0; i < 5; i++) CHECK(manager.update());
    const i32 root_time = focus_root->timer.current;
    const i32 child_time = focus_child->timer.current;
    const i32 root_age = focus_root->age.current;
    const i32 bare_time = bare_root->timer.current;
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
    // 12 帧覆盖入场延迟（age>9）到 phase6，再发确认键。
    for (int i = 0; i < 12; i++) {
        pause->controls({0, 0, false});
        CHECK(scheduler.update() >= 0);
        CHECK(manager.update());
    }
    CHECK_EQ(pause->state.phase, 6);
    // RunPause 只置 environment.paused；focus 的冻结来自 StagePresentation pre_tick。
    CHECK_EQ(focus_root->timer.current, root_time);
    CHECK_EQ(focus_child->timer.current, child_time);
    CHECK_EQ(focus_root->age.current, root_age);
    CHECK_EQ(focus_root->variables.rotation.z, root_rotation);
    // 反证：无 pre_tick 的同脚本 VM 不被暂停位冻结。
    CHECK(bare_root->timer.current > bare_time);
    // 覆盖层副链本身继续走（0xc000 已清），暂停不是全局停表。
    if (menu) CHECK(menu->age.current > menu_age);
    CHECK(manager.registry.find(focus) != nullptr);  // 冻结 ≠ 回收
    CHECK(manager.registry.find(bare) != nullptr);

    // 继续：finish_selection(0) 清暂停位后 focus/子恢复推进。
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

    // 错误脚本：front2/text0 不 spawn，也不得产出 120/76 子 VM。
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

    // 正确脚本必须产出同一断言：证明上面的检查不是“句柄非零”。
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

    // 错误 bank 身份：front 槽喂 text.anm、text 槽喂 front.anm。两资源都在，
    // prepare 会过；text.anm 没有 script132，open 必须在建菜单时失败。
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
    // 失败发生在挂起音频/暂停位之前：环境不得被半激活，背景也须回收。
    CHECK(!environment.paused);
    CHECK(manager.update());
    CHECK_EQ(manager.registry.count(), before_failed_open);
}

int main() { return th13test::run_all("暂停覆盖层真实资源"); }
