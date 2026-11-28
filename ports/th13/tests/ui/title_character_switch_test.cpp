#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/AnmEnvironment.hpp"
#include "game/AnmManager.hpp"
#include "game/FrameScheduler.hpp"
#include "game/Rng.hpp"
#include "game/SessionState.hpp"
#include "game/TitleSelectionMenu.hpp"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {

struct SwitchHost final : TitleMenuServices {
    bool sound(i32) override { return true; }
    bool begin_transition(u32&) override { return true; }
    bool start_game(i32) override { return true; }
    bool character_cleared(i32, i32) override { return false; }
};

bool read_repo_file(const std::string& relative, std::vector<u8>& out) {
    for (const std::string& base : {".", "../../../..", "../../../../.."}) {
        FILE* file = std::fopen((base + "/" + relative).c_str(), "rb");
        if (!file) continue;
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size_t(size < 0 ? 0 : size));
        const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
        std::fclose(file);
        if (got == out.size()) return true;
        return false;
    }
    return false;
}

struct PortraitFrame {
    i32 script = -1;
    u32 handle = 0;
    bool visible = false;
    u32 alpha = 0;
    i32 layer = -1;
    bool in_bucket = false;
    i32 pending = 0;
    float x = 0.0f;
};

struct SwitchHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings choice;
    FrameScheduler scheduler;
    SwitchHost host;
    std::unique_ptr<TitleCharacterMenu> menu;
    TitleState state;
    std::array<FrameCallback, 2> updates{};

    SwitchHarness() {
        updates[0].owner = updates[1].owner = &manager;
        updates[0].enabled = updates[1].enabled = true;
        updates[0].run = [](void* p) { return static_cast<AnmManager*>(p)->update(true) ? 1 : 5; };
        updates[1].run = [](void* p) { return static_cast<AnmManager*>(p)->update(false) ? 1 : 5; };
        CHECK(scheduler.add(updates[0], FramePass::Update, 9) >= 0);
        CHECK(scheduler.add(updates[1], FramePass::Update, 34) >= 0);
        std::vector<u8> title;
        CHECK(read_repo_file("artifacts/reference/raw/title.anm", title));
        CHECK(manager.load(16, title.data(), u32(title.size())));
        menu = std::make_unique<TitleCharacterMenu>(state, progress, choice, manager, host, TitleSceneResources{});
    }

    void enter_character_screen(i32 character) {
        state.return_reason = 0;
        state.change_screen(TitleScreen::Character);
        state.menu.push();
        state.menu.wrapping = true;
        state.menu.count = kTitleCharacterRows;
        state.menu.select(kTitleCharacterFirst + character);
        progress.difficulty = 2;
        progress.character = character;
    }

    void frame(u32 pressed = 0, u32 repeated = 0) {
        CHECK(menu->update(pressed, repeated));
        state.age.tick(&progress.rate);
        CHECK(scheduler.update() >= 0);
        CHECK(scheduler.draw() >= 0);
    }

    void advance(int frames, u32 pressed = 0, u32 repeated = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed, repeated);
    }

    AnmVm* portrait(i32 character) {
        AnmVm* root = manager.registry.find(state.handles[129]);
        return root ? manager.registry.find_child_script(*root, 125 + character, 0) : nullptr;
    }

    AnmVm* clear_tag(i32 character) {
        AnmVm* root = manager.registry.find(state.handles[129]);
        return root ? manager.registry.find_child_script(*root, 131 + character, 0) : nullptr;
    }

    static bool drawn(const AnmVm* vm) {
        return vm && vm->visible && (vm->visual.flags & 3u) == 3u && (vm->visual.color >> 24) != 0;
    }

    PortraitFrame observe(i32 character) {
        PortraitFrame row;
        AnmVm* vm = portrait(character);
        if (!vm) return row;
        row.script = vm->source_script;
        row.handle = manager.registry.handle(*vm);
        row.visible = vm->visible;
        row.alpha = vm->visual.color >> 24;
        row.layer = vm->visual.layer;
        row.pending = vm->pending_interrupt;
        row.x = vm->visual.translation.x;
        const auto& bucket = manager.registry.layer(u32(vm->visual.layer));
        for (const AnmVm* item : bucket)
            if (item == vm) row.in_bucket = true;
        return row;
    }

    i32 drawn_portraits() {
        i32 count = 0;
        for (i32 c = 0; c < 4; c++)
            if (drawn(portrait(c))) count++;
        return count;
    }
};

constexpr int kExitFrames = 12;

}

TEST(character_switch_exits_old_portrait_immediately) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    const u32 old_handle = h.observe(0).handle;

    std::vector<PortraitFrame> old_log, new_log;
    h.frame(0x80);
    old_log.push_back(h.observe(0));
    new_log.push_back(h.observe(1));
    for (int i = 0; i < 24; i++) {
        h.frame();
        old_log.push_back(h.observe(0));
        new_log.push_back(h.observe(1));
    }
    CHECK_EQ(old_log[0].layer, 22);
    CHECK_EQ(old_log[0].script, 125);
    CHECK(old_log[0].in_bucket);
    CHECK(old_log[2].alpha < 255);
    CHECK(new_log[2].alpha > 0);
    CHECK_EQ(old_log[kExitFrames].alpha, 0u);
    CHECK_EQ(old_log[24].alpha, 0u);
    CHECK_EQ(new_log[24].alpha, 255u);
    CHECK_EQ(new_log[24].layer, 23);
    CHECK(!SwitchHarness::drawn(h.portrait(0)));
    CHECK(SwitchHarness::drawn(h.portrait(1)));
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK_EQ(old_log[24].handle, old_handle);
    CHECK_EQ(old_log[24].pending, 0);
    CHECK_EQ(new_log[24].pending, 0);
}

TEST(character_switch_left_exits_to_right) {
    SwitchHarness h;
    h.enter_character_screen(1);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    std::vector<PortraitFrame> old_log;
    h.frame(0x40);
    old_log.push_back(h.observe(1));
    for (int i = 0; i < 24; i++) {
        h.frame();
        old_log.push_back(h.observe(1));
    }
    CHECK_EQ(old_log[0].layer, 22);
    CHECK(old_log[2].alpha < 255);
    CHECK_EQ(old_log[kExitFrames].alpha, 0u);
    CHECK_EQ(old_log[24].alpha, 0u);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
    CHECK_EQ(h.drawn_portraits(), 1);
}

TEST(character_switch_sequence_and_fast_alternation) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    const i32 expected_after_press_right[3] = {1, 2, 3};
    for (i32 step = 0; step < 3; step++) {
        h.frame(0x80);
        h.advance(25);
        CHECK_EQ(h.state.menu.cursor, kTitleCharacterFirst + expected_after_press_right[step]);
        CHECK_EQ(h.drawn_portraits(), 1);
        CHECK(SwitchHarness::drawn(h.portrait(expected_after_press_right[step])));
    }
    for (i32 back = 2; back >= 0; back--) {
        h.frame(0x40);
        h.advance(25);
        CHECK_EQ(h.drawn_portraits(), 1);
        CHECK(SwitchHarness::drawn(h.portrait(back)));
    }
    CHECK_EQ(h.state.menu.cursor, kTitleCharacterFirst);
    const u32 fast[4] = {0x80, 0x40, 0x80, 0x40};
    for (u32 press : fast) {
        h.frame(press);
        h.frame();
    }
    for (i32 c = 0; c < 4; c++) {
        PortraitFrame row = h.observe(c);
        CHECK_EQ(row.pending, 0);
    }
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
    CHECK(h.menu->error.empty());
}

TEST(character_four_slots_wrap_and_remain_synchronized) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.state.menu.count, 4);
    h.frame(0x40);
    h.advance(25);
    CHECK_EQ(h.state.menu.cursor, 3);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(SwitchHarness::drawn(h.portrait(3)));
    h.frame(0x80);
    h.advance(25);
    CHECK_EQ(h.state.menu.cursor, 0);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
}

TEST(character_switch_row_boundary_wraps) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    h.frame(0x40);
    h.advance(25);
    CHECK_EQ(h.state.menu.cursor, 3);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK_EQ(h.observe(0).alpha, 0u);
    CHECK(SwitchHarness::drawn(h.portrait(3)));
    h.frame(0x80);
    h.advance(25);
    CHECK_EQ(h.state.menu.cursor, kTitleCharacterFirst);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
}

TEST(character_switch_keeps_uncleared_tags_hidden) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    for (i32 step = 0; step < 3; step++) {
        h.frame(0x80);
        h.advance(25);
        for (i32 c = 0; c < 4; c++) {
            AnmVm* tag = h.clear_tag(c);
            CHECK(tag != nullptr);
            CHECK(!SwitchHarness::drawn(tag));
            CHECK(tag->visible);
            CHECK_EQ(tag->visual.flags & 2u, 0u);
            CHECK_EQ(tag->pending_interrupt, 0);
        }
    }
    CHECK_EQ(h.drawn_portraits(), 1);
}

TEST(character_switch_negative_pending_broadcast_leaves_residue) {
    SwitchHarness h;
    h.enter_character_screen(0);
    h.advance(25);
    CHECK_EQ(h.drawn_portraits(), 1);
    CHECK(h.manager.interrupt(h.state.handles[129], 17 + 0, false));
    CHECK(h.manager.interrupt(h.state.handles[129], 7 + 1, false));
    h.advance(25);
    CHECK(SwitchHarness::drawn(h.portrait(0)));
    CHECK(SwitchHarness::drawn(h.portrait(1)));
    CHECK_EQ(h.drawn_portraits(), 2);
}

int main() { return th13test::run_all("角色切换残留"); }
