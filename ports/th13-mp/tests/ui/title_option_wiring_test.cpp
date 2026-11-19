// Option 屏主接线单测（Title owner 接线，非 MenuExtras 屏内逻辑——那由
// title_option_menu_test 7/7 覆盖）：
//   TitleScreen::Option=3 枚举、TitleMainMenu entry 7 Z 切屏、TitleScene
//   update_title_menu 屏值 3 分支 → TitleOptionsMenu 驱动、TitleScenePlatform
//   聚合 TitleOptionPlatform（apply_volumes/persist_options 经宿主回调）、
//   行4 退出持久化 + 回主菜单 + 光标栈还原（entry 7）。
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"

#include "game/TitleScene.hpp"
#include "game/TitleMainMenu.hpp"
#include "game/FrameScheduler.hpp"

#include <memory>
#include <utility>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct OptionHost final : TitleScenePlatform {
    AnmManager& manager;
    i32 start_stage = -1;
    std::vector<std::pair<i32, i32>> applied;      // apply_volumes 记录
    std::vector<std::pair<i32, i32>> persisted;    // persist_options 记录
    bool persist_ok = true;
    explicit OptionHost(AnmManager& manager) : manager(manager) {}
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
    // TitleOptionPlatform
    void apply_volumes(i32 bgm, i32 effect) override { applied.push_back({bgm, effect}); }
    bool persist_options(const TitleOptionVolumes& v) override {
        persisted.push_back({v.bgm, v.effect});
        return persist_ok;
    }
    bool open_key_config() override { return false; }
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
        scripts[u32(3 + item)].push_back(wait());
        scripts[u32(3 + item)].push_back(stop());
    }
    spawn_range(1, 23, 48);   // root1：Option 行条 23-27/28-32 + 数字 33-48
    spawn_range(2, 49, 82);
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
    // 72 sprite：Option 数字 select_sprite(50+d / 61+d) 上界 61+9。
    return synthetic_anm(72, scripts);
}
}  // namespace

// 安全末元素访问（CHECK 失败不中断执行，空向量 back()/[] 是 UB）。
std::pair<i32, i32> last_of(const std::vector<std::pair<i32, i32>>& v) {
    return v.empty() ? std::make_pair(-1, -1) : v.back();
}

struct OptionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings selection;
    FrameScheduler scheduler;
    OptionHost host{manager};
    std::array<FrameCallback, 2> animation_updates{};
    std::unique_ptr<TitleScene> scene;

    OptionHarness() {
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
        CHECK(scheduler.draw() >= 0);
    }
    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++) frame(pressed, repeated);
    }
    void reach_main_menu() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(int(scene->state.screen), int(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
};

// entry 7（オプション）Z → 屏值 3 → open（root1 + 音量 apply 100/80）→
// >6 帧可交互；行1 → 编辑 SE +5 → apply(100,85)；X 跳行4 → Z 退出：
// persist(100,85) + 回主菜单 + 光标还原 7 + count 10。
TEST(option_entry_wires_screen_and_platform_roundtrip) {
    OptionHarness harness;
    harness.reach_main_menu();
    for (int i = 0; i < 6; i++) harness.frame(0x20);  // 0→2→…→7（每次跳禁用1）
    CHECK_EQ(harness.scene->state.menu.cursor, 7);
    harness.frame(0x01);   // Z：主菜单确认序（substate4）
    harness.advance(25);   // age>19 → enter(7) → TitleScreen::Option
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Option));
    CHECK_EQ(int(harness.scene->state.screen), 3);  // 0x44c570 case 3 屏值
    harness.advance(8);  // case1 >6 → 可交互
    CHECK_EQ(harness.scene->state.substate, 2);
    CHECK(harness.scene->state.handles[1] != 0);  // root1 已建
    CHECK_EQ(last_of(harness.host.applied).first, 100);   // 默认 BGM 100
    CHECK_EQ(last_of(harness.host.applied).second, 80);   // 默认 SE 80（0x5064）
    CHECK_EQ(harness.scene->state.menu.count, 5);       // Option 5 行
    CHECK_EQ(harness.scene->option_menu().row(), 0);      // 开屏光标归 0
    // ↓ 到行1（SE音量）→ → 步进 +5（钳 0..100，无音效）→ apply(100,85)。
    // 行光标 = TitleOptionsMenu::row_（内部态，非 state.menu.cursor）。
    harness.frame(0x20);
    CHECK_EQ(harness.scene->option_menu().row(), 1);
    harness.frame(0x80);
    CHECK_EQ(last_of(harness.host.applied).second, 85);
    CHECK_EQ(last_of(harness.host.applied).first, 100);
    // X（行≠4）→ 跳行4；Z → substate4 → >9 帧离屏：persist + 回主菜单。
    harness.frame(0x100);
    CHECK_EQ(harness.scene->option_menu().row(), 4);
    harness.frame(0x01);
    CHECK_EQ(harness.scene->state.substate, 4);
    harness.advance(12);
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.scene->state.menu.cursor, 7);  // 光标栈 pop 还原 entry 7
    CHECK_EQ(harness.scene->state.menu.count, kTitleMainEntries);
    CHECK_EQ(harness.host.persisted.size(), 1u);
    CHECK_EQ(last_of(harness.host.persisted).first, 100);
    CHECK_EQ(last_of(harness.host.persisted).second, 85);
    CHECK(harness.scene->error.empty());
}

// persist 失败：真实 false 记录 TitleOptionsMenu::error，仍回主菜单不阻断。
TEST(option_leave_with_failed_persistence_reports_error) {
    OptionHarness harness;
    harness.host.persist_ok = false;
    harness.reach_main_menu();
    for (int i = 0; i < 6; i++) harness.frame(0x20);
    harness.frame(0x01);
    harness.advance(25);   // 进 Option
    harness.advance(8);    // 可交互
    harness.frame(0x100);  // X 跳行4
    harness.frame(0x01);
    harness.advance(12);   // >9 帧离屏
    CHECK_EQ(int(harness.scene->state.screen), int(TitleScreen::Main));
    CHECK_EQ(harness.host.persisted.size(), 1u);
    CHECK(!harness.scene->option_menu().error.empty());  // 不假成功：记录 TitleOptionsMenu::error
}

int main() { return th13test::run_all("Option 屏接线"); }
