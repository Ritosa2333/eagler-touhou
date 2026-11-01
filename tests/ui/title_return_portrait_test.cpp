// 返回主菜单灵梦可见性（H01-1）：
//   0x44cdb9 返回路径重建 89/94；新 89 的 90/91 仍带 ins_6(170) 初延迟，
//   必须立即发 label2（90/91 label2 = ins_409(60,0,255)）才能在 65f 内
//   回到 alpha255。重复进入/返回同样成立。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

#include <memory>

using namespace th13;
using namespace th13::test;

namespace {

AnmIns fade(i32 frames, i32 target) { return AnmIns{409, 0, {frames, 0, target}}; }

std::vector<u8> title_anm_fixture() {
    std::vector<std::vector<AnmIns>> scripts(243);
    for (auto& script : scripts) script = {wait(), stop()};
    auto spawn_range = [&](i32 root, i32 first, i32 last) {
        scripts[u32(root)].clear();
        for (i32 child = first; child <= last; child++)
            scripts[u32(root)].push_back(spawn(child));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    spawn_range(0, 3, 22);
    for (i32 item = 0; item < 10; item++) {
        scripts[u32(3 + item)].clear();
        scripts[u32(3 + item)].push_back(label(7 + item));
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);
    spawn_range(2, 49, 82);
    // 真实 title.anm 89-93 的时序：90/91 先 ins_6(170) 再 60f 淡入；
    // label3 淡出、label2 淡入。返回路径必须用 label2 跳过初延迟。
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), label(3),
                   wait(), label(2), wait(), label(1), stop()};
    scripts[90] = {AnmIns{6, 0, {170}}, AnmIns{400, 0, {0, 0, 0}}, AnmIns{403, 0, {0}},
                   fade(60, 255), wait(), label(3), fade(30, 0), wait(), label(2),
                   fade(60, 255), wait(), label(1), stop()};
    scripts[91] = scripts[90];
    scripts[92] = {wait(), stop()};
    scripts[93] = {wait(), stop()};
    scripts[94] = {spawn(96), spawn(100), wait(), stop()};
    scripts[96] = {wait(), stop()};
    scripts[100] = {wait(), stop()};
    return synthetic_anm(72, scripts);
}

struct Host final : TitleScenePlatform {
    AnmManager& manager;
    explicit Host(AnmManager& manager) : manager(manager) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool title_exit(i32) override { return true; }
};

struct Harness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    Host host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    Harness() {
        animation_updates[0].owner = animation_updates[1].owner = &manager;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
        };
        CHECK(scheduler.add(animation_updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(animation_updates[1], FramePass::Update, 34) >= 0);
        CHECK(load_bank(manager, 16, title_anm_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 19, synthetic_anm(4, trivial_scripts(4))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, selection, manager, host, scheduler,
                                             TitleSceneResources{});
        CHECK(scene->error.empty());
        scene->state.return_reason = 0;
    }
    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }
    u32 background_alpha(i32 script) {
        return root_alpha(scene->state.handles[89], script);
    }
    u32 root_alpha(u32 handle, i32 script) {
        auto* root = manager.registry.find(handle);
        if (!root) return 0xdeadbeef;
        auto* child = manager.registry.find_child_script(*root, script, 0);
        return child ? (child->visual.color >> 24) : 0xdeadbeef;
    }
    void reach_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(scene->state.substate, 2);
    }
    void enter_music_room() {
        for (int i = 0; scene->state.menu.cursor != 6 && i < 12; ++i) frame(0x20);
        CHECK_EQ(scene->state.menu.cursor, 6);
        frame(0x01);
        advance(25);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::MusicRoom));
    }
    void leave_music_room() {
        advance(20);  // case0/1 → case2
        frame(0x100);  // X → case3
        advance(11);   // >9 帧 → Main（返回路径重建 89/94）
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
    }
};

}  // namespace

TEST(return_path_rebuilds_background_and_restores_portrait) {
    Harness h;
    h.reach_menu();
    h.advance(200);  // boot 90/91 170f 延迟 + 60f 淡入
    CHECK_EQ(h.background_alpha(90), 255u);
    CHECK_EQ(h.background_alpha(91), 255u);
    const u32 first_root = h.scene->state.handles[89];
    h.enter_music_room();
    h.leave_music_room();
    CHECK(h.scene->state.handles[89] != 0);
    CHECK(h.scene->state.handles[89] != first_root);
    // 原版 0x44bb80 不回收旧树：旧 90/91 保持 alpha255，65f 窗口内灵梦不消失。
    CHECK(h.manager.registry.find(first_root) != nullptr);
    CHECK_EQ(h.root_alpha(first_root, 90), 255u);
    CHECK_EQ(h.root_alpha(first_root, 91), 255u);
    // 返回路径同时给新树发 label2：65f 后新树 90/91 也回到 alpha255。
    h.advance(65);
    CHECK_EQ(h.background_alpha(90), 255u);
    CHECK_EQ(h.background_alpha(91), 255u);
    // 重复进入/返回：每次新建并恢复；上一轮树仍保留。
    const u32 second_root = h.scene->state.handles[89];
    h.advance(kTitleMenuReadyDelay + 1);
    h.enter_music_room();
    h.leave_music_room();
    CHECK(h.scene->state.handles[89] != second_root);
    CHECK(h.manager.registry.find(first_root) != nullptr);
    CHECK(h.manager.registry.find(second_root) != nullptr);
    CHECK_EQ(h.root_alpha(first_root, 90), 255u);
    CHECK_EQ(h.root_alpha(second_root, 90), 255u);
    h.advance(65);
    CHECK_EQ(h.background_alpha(90), 255u);
    CHECK_EQ(h.background_alpha(91), 255u);
    CHECK(h.scene->error.empty());
}

int main() {
    std::printf("# title_return_portrait_test\n");
    return th13test::run_all("title_return_portrait_test");
}
