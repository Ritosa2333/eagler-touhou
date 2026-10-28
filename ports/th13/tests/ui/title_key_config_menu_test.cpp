// キー配置屏（FUN_0044fcc0）状态机单测：
//   case0 建 title.anm root2（spawn 49-82）/ 抄五键 / 数字 sprite；
//   >6 帧可交互；↑↓ 7 行环绕（音10）；
//   原始手柄位图首个按下位学习（音7），重复冲突与其它行交换，按住幂等；
//   无手柄门控（pad_present=false）不写绑定；
//   行5 Default 回滚为宿主当前值（音7）；行6 X/Esc 放弃（音9）/
//   Z 保存（persist + 音9）；>9 帧回 Option 并 pop 光标栈；重入重新装载。
#include "ui_test_fixtures.hpp"
#include <cstdio>
#include "../harness/test_framework.hpp"

#include "game/TitleKeyConfigMenu.hpp"

#include <array>
#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> keyconfig_title_fixture() {
    auto scripts = trivial_scripts(90, 72);
    // title.anm root2：spawn 49-82（7 行灰/白 + 5×2 灰/白数字）。
    scripts[2].clear();
    for (i32 child = 49; child <= 82; child++) scripts[2].push_back(spawn(child));
    scripts[2].push_back(wait());
    scripts[2].push_back(stop());
    return synthetic_anm(72, scripts);
}

struct KeyConfigHost final : TitleKeyConfigPlatform {
    bool present = true;
    u32 buttons = 0;
    std::array<i16, 5> stored{0, 1, 2, 3, 4};
    bool persist_ok = true;
    i32 persists = 0;
    std::array<i16, 5> persisted{};
    bool pad_present() const override { return present; }
    u32 pad_buttons() const override { return buttons; }
    std::array<i16, 5> pad_bindings() const override { return stored; }
    bool persist_pad_bindings(const std::array<i16, 5>& values) override {
        ++persists;
        persisted = values;
        return persist_ok;
    }
};

struct KeyConfigHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    TitleState state;
    KeyConfigHost host;
    TitleSceneResources resources;
    std::unique_ptr<TitleKeyConfigMenu> menu;
    std::vector<i32> sounds;

    KeyConfigHarness() {
        CHECK(load_bank(manager, 16, keyconfig_title_fixture()));
        resources.title = 16;
        resources.ascii = 2;
        state.screen = TitleScreen::KeyConfig;
        menu = std::make_unique<TitleKeyConfigMenu>(state, manager, host, resources);
        menu->play_sound = [this](i32 id) {
            sounds.push_back(id);
            return true;
        };
    }

    bool frame(u32 pressed = 0, u32 repeated = 0) {
        const bool ok = menu->update(pressed, repeated);
        manager.update(true);
        manager.update(false);
        return ok;
    }
    void advance(i32 frames, u32 pressed = 0, u32 repeated = 0) {
        for (i32 i = 0; i < frames; i++) CHECK(frame(pressed, repeated));
    }
    void reach_interactive() {
        CHECK(frame());       // open -> substate1
        advance(6);
        CHECK_EQ(state.substate, 1);
        CHECK(frame());       // 第 7 帧 -> substate2
        CHECK_EQ(state.substate, 2);
    }
    AnmVm* child(i32 script) {
        auto* root = manager.registry.find(state.handles[2]);
        return root ? manager.registry.find_child_script(*root, script, 0) : nullptr;
    }
    i32 sprite_of(i32 script) {
        auto* vm = child(script);
        return vm ? vm->visual.sprite : -1;
    }
};

}  // namespace

TEST(keyconfig_open_copies_bindings_and_builds_real_vm) {
    KeyConfigHarness h;
    h.host.stored = {3, 5, 7, 9, 11};
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 1);
    CHECK(h.state.handles[2] != 0);
    CHECK_EQ(h.state.menu.depth, 1);
    CHECK_EQ(h.menu->row(), 0);
    CHECK_EQ(int(h.menu->bindings()[0]), 3);
    CHECK_EQ(int(h.menu->bindings()[4]), 11);
    // 次帧 root2 已 spawn：数字 sprite = 50 + 十/个位（00450020 灰白同写）。
    CHECK(h.frame());
    CHECK_EQ(h.sprite_of(63), 50);  // row0 十位 0
    CHECK_EQ(h.sprite_of(64), 53);  // row0 个位 3
    CHECK_EQ(h.sprite_of(73), 50);  // 白组同写
    CHECK_EQ(h.sprite_of(74), 53);
    CHECK(h.menu->error.empty());
}

TEST(keyconfig_cursor_wraps_over_seven_rows) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.frame(0x10);  // ↑ 0 -> 6
    CHECK_EQ(h.menu->row(), 6);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    h.frame(0x20);  // ↓ 6 -> 0
    CHECK_EQ(h.menu->row(), 0);
    h.frame(0x20);  // ↓ 0 -> 1
    CHECK_EQ(h.menu->row(), 1);
}

TEST(keyconfig_learn_first_pad_button_swaps_duplicates) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 3;  // 按钮 3 当前属于行3
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 3);
    CHECK_EQ(int(h.menu->bindings()[3]), 0);  // 行3 得到行0 旧值（交换）
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
    CHECK_EQ(h.sprite_of(64), 53);  // row0 个位 3
    CHECK_EQ(h.sprite_of(70), 50);  // row3 个位 0
    // 按住同一位不再产生变化/音效（原版等值 no-op，无额外 release 状态）。
    const size_t sounds = h.sounds.size();
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 3);
    CHECK_EQ(h.sounds.size(), sounds);
    CHECK_EQ(h.host.persists, 0);  // 未按 End 不落盘
}

TEST(keyconfig_learning_ignores_missing_pad) {
    KeyConfigHarness h;
    h.host.present = false;
    h.reach_interactive();
    h.host.buttons = 1u << 6;
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 0);
    CHECK_EQ(int(h.menu->bindings()[1]), 1);
    CHECK(h.menu->error.empty());
}

TEST(keyconfig_learning_only_on_binding_rows) {
    KeyConfigHarness h;
    h.reach_interactive();
    for (i32 i = 0; i < 5; i++) h.frame(0x20);  // 行0 -> 5（Default）
    CHECK_EQ(h.menu->row(), 5);
    h.host.buttons = 1u << 8;
    const size_t sounds = h.sounds.size();
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 0);
    CHECK_EQ(h.sounds.size(), sounds);
}

TEST(keyconfig_default_row_reverts_to_stored_bindings) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 2;
    CHECK(h.frame());  // 行0 = 2，行2 = 0
    CHECK_EQ(int(h.menu->bindings()[0]), 2);
    h.host.buttons = 0;
    for (i32 i = 0; i < 5; i++) h.frame(0x20);  // 行5
    CHECK(h.frame(0x80001));                     // Z：Default 回滚
    CHECK_EQ(int(h.menu->bindings()[0]), 0);
    CHECK_EQ(int(h.menu->bindings()[2]), 2);
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
    CHECK_EQ(h.state.substate, 2);  // 不离屏
}

TEST(keyconfig_repeated_actions_do_not_confirm_or_cancel) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 7;
    CHECK(h.frame());
    h.host.buttons = 0;
    for (i32 row = 0; row < 5; row++) CHECK(h.frame(0x20));
    const size_t sounds = h.sounds.size();
    CHECK(h.frame(0, 0x01));
    CHECK_EQ(int(h.menu->bindings()[0]), 7);
    CHECK_EQ(h.sounds.size(), sounds);
    CHECK(h.frame(0, 0x20));
    CHECK_EQ(h.menu->row(), 6);
    CHECK(h.frame(0, 0x80001 | 0x102));
    CHECK_EQ(h.state.substate, 2);
    CHECK_EQ(h.host.persists, 0);
}

TEST(keyconfig_escape_on_quit_row_cancels_without_persist) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 4;
    CHECK(h.frame());  // 编辑行0 = 4
    h.host.buttons = 0;
    for (i32 i = 0; i < 6; i++) h.frame(0x20);  // 行6 Quit
    CHECK_EQ(h.menu->row(), 6);
    CHECK(h.frame(0x100));  // Esc：放弃
    CHECK_EQ(int(h.menu->bindings()[0]), 0);  // 回滚
    CHECK(!h.sounds.empty() && h.sounds.back() == 9);
    CHECK_EQ(h.host.persists, 0);
    CHECK_EQ(h.state.substate, 4);
    h.advance(9);
    CHECK_EQ(h.state.substate, 4);  // >9 帧才离屏
    CHECK(h.frame());
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Option));
    CHECK_EQ(h.state.menu.depth, 0);
    CHECK_EQ(h.state.handles[2], 0u);
    CHECK(h.menu->error.empty());
}

TEST(keyconfig_end_row_saves_bindings_and_returns_to_option) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 7;
    CHECK(h.frame());  // 行0 = 7
    h.host.buttons = 0;
    for (i32 i = 0; i < 6; i++) h.frame(0x20);  // 行6 End
    CHECK(h.frame(0x01));  // Z：保存 + 离屏
    CHECK_EQ(h.host.persists, 1);
    CHECK_EQ(int(h.host.persisted[0]), 7);
    CHECK(!h.sounds.empty() && h.sounds.back() == 9);
    CHECK_EQ(h.state.substate, 4);
    h.advance(10);
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Option));
    CHECK_EQ(h.state.menu.depth, 0);
    CHECK_EQ(h.state.handles[2], 0u);
    CHECK(h.menu->error.empty());
}

TEST(keyconfig_persist_failure_is_reported_but_exit_continues) {
    KeyConfigHarness h;
    h.host.persist_ok = false;
    h.reach_interactive();
    for (i32 i = 0; i < 6; i++) h.frame(0x20);
    CHECK(h.frame(0x01));
    CHECK(!h.menu->error.empty());  // 不假成功
    CHECK_EQ(h.host.persists, 1);
    h.advance(10);
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Option));
}

TEST(keyconfig_reentry_reloads_saved_bindings) {
    KeyConfigHarness h;
    h.host.stored = {6, 5, 4, 3, 2};
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 6);
    // 模拟离开后重入：宿主值已更新，scratch 必须重新装载。
    h.host.stored = {9, 8, 7, 6, 5};
    h.state.screen = TitleScreen::KeyConfig;
    h.state.substate = 0;
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 9);
    CHECK_EQ(int(h.menu->bindings()[4]), 5);
    CHECK_EQ(h.state.menu.count, 7);
}

int main() {
    std::printf("# title_key_config_menu_test\n");
    return th13test::run_all("title_key_config_menu_test");
}
