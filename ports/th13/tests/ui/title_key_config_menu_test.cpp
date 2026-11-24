#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"
#include <cstdio>

#include "game/TitleKeyConfigMenu.hpp"

#include <array>
#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> keyconfig_title_fixture() {
    auto scripts = trivial_scripts(90, 72);
    scripts[2].clear();
    for (i32 child = 49; child <= 82; child++)
        scripts[2].push_back(spawn(child));
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
        for (i32 i = 0; i < frames; i++)
            CHECK(frame(pressed, repeated));
    }
    void reach_interactive() {
        CHECK(frame());
        advance(6);
        CHECK_EQ(state.substate, 1);
        CHECK(frame());
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

}

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
    CHECK(h.frame());
    CHECK_EQ(h.sprite_of(63), 50);
    CHECK_EQ(h.sprite_of(64), 53);
    CHECK_EQ(h.sprite_of(73), 50);
    CHECK_EQ(h.sprite_of(74), 53);
    CHECK(h.menu->error.empty());
}

TEST(keyconfig_cursor_wraps_over_seven_rows) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.frame(0x10);
    CHECK_EQ(h.menu->row(), 6);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    h.frame(0x20);
    CHECK_EQ(h.menu->row(), 0);
    h.frame(0x20);
    CHECK_EQ(h.menu->row(), 1);
}

TEST(keyconfig_learn_first_pad_button_swaps_duplicates) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 3;
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 3);
    CHECK_EQ(int(h.menu->bindings()[3]), 0);
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
    CHECK_EQ(h.sprite_of(64), 53);
    CHECK_EQ(h.sprite_of(70), 50);
    const size_t sounds = h.sounds.size();
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 3);
    CHECK_EQ(h.sounds.size(), sounds);
    CHECK_EQ(h.host.persists, 0);
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
    for (i32 i = 0; i < 5; i++)
        h.frame(0x20);
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
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 2);
    h.host.buttons = 0;
    for (i32 i = 0; i < 5; i++)
        h.frame(0x20);
    CHECK(h.frame(0x80001));
    CHECK_EQ(int(h.menu->bindings()[0]), 0);
    CHECK_EQ(int(h.menu->bindings()[2]), 2);
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
    CHECK_EQ(h.state.substate, 2);
}

TEST(keyconfig_repeated_actions_do_not_confirm_or_cancel) {
    KeyConfigHarness h;
    h.reach_interactive();
    h.host.buttons = 1u << 7;
    CHECK(h.frame());
    h.host.buttons = 0;
    for (i32 row = 0; row < 5; row++)
        CHECK(h.frame(0x20));
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
    CHECK(h.frame());
    h.host.buttons = 0;
    for (i32 i = 0; i < 6; i++)
        h.frame(0x20);
    CHECK_EQ(h.menu->row(), 6);
    CHECK(h.frame(0x100));
    CHECK_EQ(int(h.menu->bindings()[0]), 0);
    CHECK(!h.sounds.empty() && h.sounds.back() == 9);
    CHECK_EQ(h.host.persists, 0);
    CHECK_EQ(h.state.substate, 4);
    h.advance(9);
    CHECK_EQ(h.state.substate, 4);
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
    CHECK(h.frame());
    h.host.buttons = 0;
    for (i32 i = 0; i < 6; i++)
        h.frame(0x20);
    CHECK(h.frame(0x01));
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
    for (i32 i = 0; i < 6; i++)
        h.frame(0x20);
    CHECK(h.frame(0x01));
    CHECK(!h.menu->error.empty());
    CHECK_EQ(h.host.persists, 1);
    h.advance(10);
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Option));
}

TEST(keyconfig_reentry_reloads_saved_bindings) {
    KeyConfigHarness h;
    h.host.stored = {6, 5, 4, 3, 2};
    CHECK(h.frame());
    CHECK_EQ(int(h.menu->bindings()[0]), 6);
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
