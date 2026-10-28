#include "../harness/test_framework.hpp"
#include "game/TitleScene.hpp"

using namespace th13;

namespace {
struct Host final : TitleScenePlatform {
    SpellPracticeCommand received;
    i32 launches = 0;
    bool accept = true;
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    i32 transitions = 0;
    bool begin_transition(u32&) override { ++transitions; return true; }
    bool start_game(i32) override { return false; }
    bool title_exit(i32) override { return true; }
    bool hud_text(const HudTextDraw&) override { return true; }
    bool start_spell_practice(const SpellPracticeCommand& command) override {
        received = command;
        ++launches;
        return accept;
    }
    bool can_start_spell_practice(const SpellPracticeCommand&) const override { return accept; }
    i32 stage_count() const override { return 1; }
    bool stage_available(i32 stage) const override { return stage == 1; }
    i32 spell_count(i32 stage) const override { return stage == 1 ? 1 : 0; }
    bool spell_unlocked(i32 stage, i32 slot) const override { return stage == 1 && slot == 0; }
    i32 spell_id(i32, i32, i32 row) const override { return row == 0 ? 2 : -1; }
    i32 spell_rank(i32, i32, i32 row) const override { return row == 0 ? 2 : -1; }
    i32 character_count() const override { return 4; }
};
}

TEST(selected_spell_is_delivered_to_the_production_launch_service) {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager(random, environment);
    SessionState progress;
    TitleSelectionSettings selection;
    selection.preferred_character = 3;
    FrameScheduler scheduler;
    Host host;
    TitleScene scene(progress, selection, manager, host, scheduler);
    scene.state.screen = TitleScreen::SpellPractice;
    for (i32 i = 0; i < 3; ++i) {
        scene.controls({0, 1, 0, 0});
        CHECK(scheduler.update() >= 0);
    }
    CHECK_EQ(host.launches, 0);
    scene.controls({0, 0, 0, 0});
    for (i32 frame = 1; frame <= 39; ++frame) {
        CHECK(scheduler.update() >= 0);
        CHECK_EQ(host.launches, 0);
        CHECK_EQ(host.transitions, frame < 10 ? 0 : 1);
    }
    CHECK(scheduler.update() >= 0);
    CHECK_EQ(host.launches, 1);
    CHECK_EQ(host.received.character, 3);
    CHECK_EQ(host.received.stage, 1);
    CHECK_EQ(host.received.spell_slot, 0);
    CHECK_EQ(host.received.spell_id, 2);
    CHECK_EQ(host.received.rank, 2);
    CHECK_EQ(i32(scene.state.screen), i32(TitleScreen::StartGame));
    CHECK(!scene.spell_practice_launch_pending());
}

int main() { return th13test::run_all("title spell launch"); }
