// Option -> キー配置 -> Option 主接线单测：
//   TitleScreen::KeyConfig=4 分派、Option 行2 Z 经宿主 open_key_config 切屏、
//   TitleScene::connect_key_config 注入手柄位图/五键/落盘、End 保存后回 Option
//   并恢复 5 行光标（不残留 key config 的 7 行/栈）、再退出回主菜单。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

#include <array>
#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct RoundTripHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    bool key_config_open = false;
    explicit RoundTripHost(AnmManager& manager) : manager(manager) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = manager.create(8, 0, -1, 0);
        return animation != 0;
    }
    bool start_game(i32 stage) override {
        start_stage = stage;
        return true;
    }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool title_exit(i32) override { return true; }
    void apply_volumes(i32, i32) override {}
    bool persist_options(const TitleOptionVolumes&) override { return true; }
    bool open_key_config() override {
        key_config_open = true;
        return true;
    }
};

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
        scripts[u32(3 + item)].push_back(label(17 + item));
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);  // Option 行条/数字
    spawn_range(2, 49, 82);  // キー配置 7 行 + 数字
    scripts[89] = {spawn(92), spawn(93), spawn(91), spawn(90), wait(), stop()};
    scripts[94] = {spawn(97), spawn(96), spawn(100), wait(), stop()};
    scripts[95] = {spawn(99), spawn(96), spawn(100), wait(), stop()};
    scripts[123].clear();
    for (i32 child : {83, 84, 118, 119, 120, 121, 113, 114, 115, 116})
        scripts[123].push_back(spawn(child));
    scripts[123].push_back(wait());
    scripts[123].push_back(stop());
    scripts[129].clear();
    for (i32 child : {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134})
        scripts[129].push_back(spawn(child));
    scripts[129].push_back(wait());
    scripts[129].push_back(stop());
    return synthetic_anm(72, scripts);
}

struct RoundTripHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    RoundTripHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;
    u32 raw_buttons = 0;
    bool pad_present = true;
    std::array<i16, 5> stored{0, 1, 2, 3, 4};
    std::array<i16, 5> persisted{};
    i32 persist_calls = 0;
    bool persist_ok = true;

    RoundTripHarness() {
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
        scene->connect_key_config(
            [this] { return pad_present && raw_buttons != 0; },
            [this] { return raw_buttons; },
            [this] { return stored; },
            [this](const std::array<i16, 5>& values) {
                ++persist_calls;
                persisted = values;
                return persist_ok;
            });
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        scene->controls({0, pressed, repeated, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; i++) frame(pressed, repeated);
    }
    void reach_main_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    void reach_key_config() {
        reach_main_menu();
        for (int i = 0; i < 6; i++) frame(0x20);  // 主菜单 0 -> 7（Option）
        CHECK_EQ(scene->state.menu.cursor, 7);
        frame(0x01);
        advance(25);  // 主菜单确认序 -> Option
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Option));
        advance(8);   // Option case1 >6 -> 可交互
        frame(0x20);  // 行0 -> 1
        frame(0x20);  // 行1 -> 2（キー配置）
        CHECK_EQ(scene->option_menu().row(), 2);
        frame(0x01);  // Z -> substate4
        CHECK_EQ(scene->state.substate, 4);
        advance(11);  // >9 帧 leave -> KeyConfig；次帧 case0 开面
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::KeyConfig));
        CHECK(scene->state.handles[2] != 0);
    }
};

}  // namespace

TEST(option_enters_key_config_and_returns_with_five_row_cursor) {
    RoundTripHarness h;
    h.reach_key_config();
    CHECK(h.host.key_config_open);
    CHECK_EQ(h.scene->state.menu.depth, 2);  // Option 栈 + KeyConfig 栈
    CHECK_EQ(h.scene->state.menu.count, 7);
    // 可交互：>6 帧。
    h.advance(7);
    CHECK_EQ(h.scene->state.substate, 2);
    // 学习按钮3（行3 当前值0，交换）。
    h.raw_buttons = 1u << 3;
    h.frame();
    CHECK_EQ(int(h.scene->key_config_menu().bindings()[0]), 3);
    CHECK_EQ(int(h.scene->key_config_menu().bindings()[3]), 0);
    h.raw_buttons = 0;
    // 行6 End -> 保存退出。
    for (int i = 0; i < 6; i++) h.frame(0x20);
    CHECK_EQ(h.scene->key_config_menu().row(), 6);
    h.frame(0x01);
    h.advance(10);
    CHECK_EQ(h.persist_calls, 1);
    CHECK_EQ(int(h.persisted[0]), 3);
    CHECK_EQ(int(h.scene->state.screen), int(TitleScreen::Option));
    CHECK_EQ(h.scene->state.menu.depth, 1);  // KeyConfig 栈已 pop，剩 Option
    // Option 重入：count 恢复 5、row 归 0、root1 重建。
    h.advance(9);
    CHECK_EQ(h.scene->state.substate, 2);
    CHECK_EQ(h.scene->state.menu.count, 5);
    CHECK_EQ(h.scene->option_menu().row(), 0);
    CHECK(h.scene->state.handles[1] != 0);
    CHECK(h.scene->option_menu().error.empty());
    // Option 行4 退出回主菜单（验证与 key config 往返后的栈完整）。
    h.frame(0x100);  // X -> 行4
    CHECK_EQ(h.scene->option_menu().row(), 4);
    h.frame(0x01);
    h.advance(12);
    CHECK_EQ(int(h.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(h.scene->state.menu.cursor, 7);
    CHECK_EQ(h.scene->state.menu.count, kTitleMainEntries);
    CHECK(h.scene->error.empty());
}

TEST(key_config_cancel_returns_to_option_without_persist) {
    RoundTripHarness h;
    h.reach_key_config();
    h.advance(7);
    h.raw_buttons = 1u << 5;
    h.frame();
    CHECK_EQ(int(h.scene->key_config_menu().bindings()[0]), 5);
    h.raw_buttons = 0;
    for (int i = 0; i < 6; i++) h.frame(0x20);
    h.frame(0x100);  // Esc 取消
    h.advance(10);
    CHECK_EQ(h.persist_calls, 0);
    CHECK_EQ(int(h.scene->state.screen), int(TitleScreen::Option));
    CHECK_EQ(h.scene->state.menu.depth, 1);
    CHECK_EQ(int(h.scene->key_config_menu().bindings()[0]), 0);  // 回滚展示值
}

int main() { return th13test::run_all("Option 与キー配置往返"); }
