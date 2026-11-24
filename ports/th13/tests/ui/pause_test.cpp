#include "../harness/test_framework.hpp"
#include "ui_test_fixtures.hpp"

#include "game/FrameScheduler.hpp"
#include "game/RunPause.hpp"

using namespace th13;
using namespace th13::test;

namespace {
struct PauseHost final : RunPausePlatform {
    bool sound(i32) override { return true; }
    bool suspend_music() override { return true; }
    int restarts = 0, titles = 0, continues = 0;
    std::vector<int> dialogue_visibility;
    bool dialogue_visible(bool visible) override {
        dialogue_visibility.push_back(visible ? 1 : 0);
        return true;
    }
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

struct PauseHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    FrameScheduler scheduler;
    PauseHost host;
    std::unique_ptr<RunPause> pause;

    PauseHarness() : host() {
        CHECK(load_bank(manager, 0, synthetic_anm(8, trivial_scripts(76))));
        CHECK(load_bank(manager, 3, synthetic_anm(8, trivial_scripts(155))));
        progress.rate = 1;
        pause = std::make_unique<RunPause>(progress, manager, environment, host, scheduler, 3);
        CHECK(pause->prepare());
    }

    void frame(u32 pressed = 0, bool lost_focus = false) {
        pause->controls({pressed, 0, lost_focus});
        CHECK(scheduler.update() >= 0);
    }

    void advance(int frames, u32 pressed = 0) {
        for (int i = 0; i < frames; i++)
            frame(pressed);
    }
};
}

TEST(pause_open_and_continue) {
    PauseHarness harness;
    CHECK(!harness.pause->active());
    CHECK(harness.pause->open(PauseEntrance::Pause));
    CHECK(harness.pause->active());
    CHECK_EQ(harness.progress.scene_flags & 0x10u, 0x10u);
    harness.advance(12);
    CHECK_EQ(harness.pause->state.phase, 6);
    CHECK_EQ(harness.pause->state.menu.cursor, 0);
    CHECK_EQ(harness.pause->state.menu.count, 5);
    harness.frame(1);
    harness.advance(15);
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.restarts, 0);
    CHECK_EQ(harness.host.titles, 0);
    CHECK_EQ(harness.progress.scene_flags & 0x10u, 0u);
    CHECK(harness.pause->error.empty());
}

TEST(pause_esc_resumes_like_continue) {
    PauseHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.frame(0x100);
    harness.advance(15);
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.restarts, 0);
}

TEST(pause_cursor_navigates_five_entries) {
    PauseHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.menu.cursor, 4);
    harness.frame(0x10);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(0x10);
    CHECK_EQ(harness.pause->state.menu.cursor, 0);
    harness.frame(0x10);
    CHECK_EQ(harness.pause->state.menu.cursor, 4);
}

TEST(pause_restart_and_title_destinations) {
    PauseHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.frame(0x20);
    harness.frame(1);
    harness.advance(21);
    CHECK_EQ(harness.pause->state.phase, 9);
    CHECK_EQ(harness.pause->state.menu.count, 2);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(0x10);
    harness.frame(1);
    harness.advance(21);
    harness.advance(13);
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.titles, 1);

    CHECK(harness.pause->open(PauseEntrance::Pause));
    harness.advance(12);
    harness.frame(0x20);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.menu.cursor, 4);
    harness.frame(1);
    harness.advance(21);
    harness.frame(0x10);
    harness.frame(1);
    harness.advance(21);
    harness.advance(13);
    CHECK_EQ(harness.host.restarts, 1);
    CHECK_EQ(harness.host.titles, 1);
    CHECK(harness.pause->error.empty());
}

TEST(game_over_disables_continue) {
    PauseHarness harness;
    harness.progress.continue_stock = 0;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    CHECK_EQ(int(harness.pause->state.screen), int(PauseScreen::GameOver));
    harness.advance(12);
    CHECK_EQ(harness.pause->state.menu.count, 5);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    for (int i = 0; i < 6; i++)
        harness.frame(0x10);
    const i32 cursor = harness.pause->state.menu.cursor;
    CHECK(cursor == 1 || cursor == 4);
    harness.frame(1);
    harness.advance(15);
    CHECK_EQ(harness.host.restarts + harness.host.titles, 1);
}

TEST(game_over_allows_continue_when_stock_positive) {
    PauseHarness harness;
    harness.progress.continue_stock = 1;
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    CHECK_EQ(int(harness.pause->state.screen), int(PauseScreen::GameOver));
    harness.advance(12);
    CHECK_EQ(harness.pause->state.menu.count, 5);
    CHECK_EQ(harness.pause->state.menu.cursor, 0);
    harness.frame(1);
    harness.advance(15);
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.continues, 1);
    CHECK_EQ(harness.host.restarts, 0);
    CHECK_EQ(harness.host.titles, 0);
    CHECK_EQ(harness.progress.scene_flags & 0x10u, 0u);
    CHECK(!harness.environment.paused);
    CHECK(harness.pause->error.empty());
}

TEST(practice_end_disables_continue_and_requests_destination) {
    PauseHarness harness;
    CHECK(harness.pause->open(PauseEntrance::PracticeEnd));
    CHECK_EQ(int(harness.pause->state.screen), int(PauseScreen::PracticeEnd));
    harness.advance(12);
    CHECK_EQ(harness.pause->state.menu.count, 5);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(0x100);
    CHECK(harness.pause->active());
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.menu.cursor, 4);
    harness.frame(0x20);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(1);
    harness.advance(21);
    CHECK_EQ(harness.pause->state.phase, 9);
    CHECK_EQ(harness.pause->state.menu.cursor, 1);
    harness.frame(0x10);
    CHECK_EQ(harness.pause->state.menu.cursor, 0);
    harness.frame(1);
    harness.advance(21);
    harness.advance(13);
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.titles, 1);
    CHECK_EQ(harness.host.restarts, 0);
    CHECK(harness.pause->error.empty());
}

TEST(pause_hides_and_restores_dialogue_visibility) {
    PauseHarness harness;
    CHECK(harness.pause->open(PauseEntrance::Pause));
    CHECK(!harness.host.dialogue_visibility.empty());
    CHECK_EQ(harness.host.dialogue_visibility.back(), 0);
    harness.advance(12);
    harness.frame(0x100);
    harness.advance(15);
    CHECK(!harness.pause->active());
    CHECK_EQ(harness.host.dialogue_visibility.back(), 1);
    const size_t calls = harness.host.dialogue_visibility.size();
    CHECK(harness.pause->open(PauseEntrance::GameOver));
    CHECK_EQ(harness.host.dialogue_visibility.size(), calls);
}

int main() { return th13test::run_all("暂停覆盖层"); }
