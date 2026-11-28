#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"
#include <cstdio>

#include "game/TitleMainMenu.hpp"
#include "game/TitleOptionsMenu.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

std::vector<u8> option_title_fixture() {
    auto scripts = trivial_scripts(130, 72);
    scripts[1].clear();
    for (i32 child = 23; child <= 48; child++)
        scripts[1].push_back(spawn(child));
    scripts[1].push_back(wait());
    scripts[1].push_back(stop());
    for (i32 child = 23; child <= 48; child++) {
        const i32 row = child <= 27 ? child - 23 : child <= 32 ? child - 28 : child <= 40 ? 0 : 1;
        auto& script = scripts[u32(child)];
        script.clear();
        script.push_back(sprite(0));
        script.push_back(wait());
        for (const i32 id : {3, 7 + row, 17 + row}) {
            script.push_back(label(id));
            script.push_back(wait());
        }
        script.push_back(stop());
    }
    return synthetic_anm(72, scripts);
}

struct RecordingHost final : TitleOptionPlatform {
    TitleOptionVolumes persisted;
    i32 applies = 0, last_bgm = -1, last_effect = -1;
    bool persist_ok = false, key_config_ok = false;
    i32 key_config_opens = 0;
    void apply_volumes(i32 bgm, i32 effect) override {
        ++applies;
        last_bgm = bgm;
        last_effect = effect;
    }
    bool persist_options(const TitleOptionVolumes& volumes) override {
        if (!persist_ok) return false;
        persisted = volumes;
        return true;
    }
    bool open_key_config() override {
        ++key_config_opens;
        return key_config_ok;
    }
};

struct OptionHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    TitleState state;
    TitleOptionVolumes volumes;
    RecordingHost host;
    TitleSceneResources resources;
    std::unique_ptr<TitleOptionsMenu> menu;
    std::vector<i32> sounds;

    OptionHarness() {
        CHECK(load_bank(manager, 16, option_title_fixture()));
        resources.title = 16;
        resources.ascii = 2;
        state.screen = TitleScreen(kTitleScreenOption);
        menu = std::make_unique<TitleOptionsMenu>(state, manager, host, volumes, resources);
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
    AnmVm* child(i32 script) {
        auto* root = manager.registry.find(state.handles[1]);
        return root ? manager.registry.find_child_script(*root, script, 0) : nullptr;
    }
};

}

TEST(option_open_builds_root_and_applies_volumes) {
    OptionHarness h;
    h.volumes.bgm = 42;
    h.volumes.effect = 7;
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 1);
    CHECK(h.state.handles[1] != 0);
    CHECK_EQ(h.host.applies, 1);
    CHECK_EQ(h.host.last_bgm, 42);
    CHECK_EQ(h.host.last_effect, 7);
}

TEST(option_digits_sprite_and_leading_zero_visibility) {
    OptionHarness h;
    h.volumes.bgm = 42;
    h.volumes.effect = 100;
    CHECK(h.frame());
    auto* root = h.manager.registry.find(h.state.handles[1]);
    CHECK(root != nullptr);
    const struct {
        i32 script;
        bool visible;
    } expects[] = {
        {33, false}, {34, true}, {35, true}, {37, false}, {38, true}, {39, true},
        {41, true},  {42, true}, {43, true}, {45, true},  {46, true}, {47, true},
    };
    CHECK(h.frame());
    for (const auto& e : expects) {
        auto* vm = h.child(e.script);
        CHECK(vm != nullptr);
        if (!vm) continue;
        CHECK_EQ((vm->visual.flags & 3u) == 3u, e.visible);
    }
    CHECK(h.menu->error.empty());
}

TEST(option_cursor_move_never_reveals_leading_zero_digits) {
    OptionHarness h;
    h.volumes.bgm = 42;
    h.volumes.effect = 7;
    CHECK(h.frame());
    h.advance(8);
    for (i32 frame_index = 0; frame_index < 12; ++frame_index) {
        if (frame_index == 2)
            h.frame(0x20);
        else
            h.frame();
        for (const i32 script : {33, 37, 41, 45}) {
            auto* vm = h.child(script);
            CHECK(vm != nullptr);
            if (!vm) continue;
            CHECK((vm->visual.flags & 3u) != 3u);
        }
        auto* se_tens = h.child(42);
        CHECK(se_tens != nullptr);
        if (se_tens) CHECK((se_tens->visual.flags & 3u) != 3u);
    }
    CHECK(h.menu->error.empty());
}

TEST(option_entry_timing_and_cursor_move) {
    OptionHarness h;
    CHECK(h.frame());
    h.advance(6);
    CHECK_EQ(h.state.substate, 1);
    CHECK(h.frame());
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.frame(0x20));
    CHECK_EQ(h.menu->row(), 1);
    CHECK(!h.sounds.empty() && h.sounds.back() == 10);
    h.advance(1, 0, 0x10);
    CHECK_EQ(h.menu->row(), 0);
    h.advance(1, 0x10);
    CHECK_EQ(h.menu->row(), 4);
}

TEST(option_cancel_jumps_to_quit_row) {
    OptionHarness h;
    CHECK(h.frame());
    h.advance(8);
    CHECK_EQ(h.state.substate, 2);
    CHECK(h.frame(0x102));
    CHECK_EQ(h.menu->row(), 4);
    CHECK(!h.sounds.empty() && h.sounds.back() == 9);
    CHECK_EQ(h.state.substate, 2);
}

TEST(option_volume_adjust_and_clamp) {
    OptionHarness h;
    h.volumes.bgm = 100;
    CHECK(h.frame());
    h.advance(8);
    CHECK(h.frame(0x40));
    CHECK_EQ(h.volumes.bgm, 95);
    CHECK(h.sounds.empty());
    CHECK_EQ(h.host.last_bgm, 95);
    h.volumes.bgm = 3;
    CHECK(h.frame(0x40));
    CHECK_EQ(h.volumes.bgm, 0);
    CHECK(h.frame(0x80));
    CHECK_EQ(h.volumes.bgm, 5);
    h.volumes.bgm = 98;
    CHECK(h.frame(0x80));
    CHECK_EQ(h.volumes.bgm, 100);
    h.frame(0x20);
    h.volumes.effect = 80;
    CHECK(h.frame(0x40));
    CHECK_EQ(h.volumes.effect, 75);
    CHECK_EQ(h.host.last_effect, 75);
}

TEST(option_reset_defaults_and_quit_exit) {
    OptionHarness h;
    h.volumes.bgm = 17;
    h.volumes.effect = 33;
    CHECK(h.frame());
    h.advance(8);
    h.frame(0x20);
    h.frame(0x20);
    h.frame(0x20);
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.volumes.bgm, 100);
    CHECK_EQ(h.volumes.effect, 80);
    CHECK(!h.sounds.empty() && h.sounds.back() == 7);
    h.frame(0x20);
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.state.substate, 4);
    CHECK_EQ(int(h.state.screen), kTitleScreenOption);
    h.advance(9);
    CHECK_EQ(h.state.substate, 4);
    CHECK(h.frame());
    CHECK_EQ(int(h.state.screen), int(TitleScreen::Main));
    CHECK_EQ(h.state.handles[1], 0);
    CHECK(!h.host.persist_ok);
    CHECK(!h.menu->error.empty());
}

TEST(option_persist_success_path_and_key_config_gate) {
    OptionHarness h;
    h.host.persist_ok = true;
    CHECK(h.frame());
    h.advance(8);
    h.frame(0x102);
    CHECK(h.frame(0x80001));
    h.advance(10);
    CHECK_EQ(h.host.persisted.bgm, 100);
    CHECK_EQ(h.host.persisted.effect, 80);
    CHECK(h.menu->error.empty());
    h.state.screen = TitleScreen(kTitleScreenOption);
    h.state.substate = 0;
    CHECK(h.frame());
    h.advance(8);
    h.frame(0x20);
    h.frame(0x20);
    CHECK_EQ(h.menu->row(), 2);
    CHECK(h.frame(0x80001));
    CHECK_EQ(h.state.substate, 4);
    h.advance(9);
    CHECK(!h.frame());
    CHECK(!h.menu->error.empty());
    h.host.key_config_ok = true;
}

int main() {
    std::printf("# title_option_menu_test\n");
    return th13test::run_all("title_option_menu_test");
}
