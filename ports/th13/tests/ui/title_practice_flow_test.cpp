// Practice state-machine regression: original 0x44cd10 -> 0x450fb0 ->
// 0x451450 -> 0x451cb0. Synthetic ANM tests behavior, not visual fidelity.
#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/TitleScene.hpp"
#include "game/PostGameFlow.hpp"

#include <memory>
#include <vector>

using namespace th13;
using namespace th13::test;

namespace {
std::vector<u8> practice_title_fixture() {
    auto scripts = trivial_scripts(243);
    auto children = [&](i32 root, std::initializer_list<i32> ids) {
        scripts[u32(root)].clear();
        for (i32 id : ids) scripts[u32(root)].push_back(spawn(id));
        scripts[u32(root)].push_back(wait());
        scripts[u32(root)].push_back(stop());
    };
    scripts[0].clear();
    for (i32 id = 3; id <= 22; ++id) scripts[0].push_back(spawn(id));
    scripts[0].push_back(wait());
    scripts[0].push_back(stop());
    children(89, {92, 93, 91, 90});
    children(94, {97, 96, 100});
    children(95, {99, 96, 100});
    children(123, {83, 84, 118, 119, 120, 121, 113, 114, 115, 116});
    children(129, {125, 126, 127, 128, 135, 136, 137, 138, 131, 132, 133, 134});
    return synthetic_anm(8, scripts);
}

struct PracticeHost final : TitleScenePlatform {
    SessionState& progress;
    std::array<u8, 16> played{};
    std::vector<i32> sounds;
    std::vector<HudTextDraw> texts;
    i32 launches = 0, launched_stage = -1, launched_character = -1, launched_rank = -1;
    i32 launched_practice = -1;
    u32 launched_mode = ~0u;
    explicit PracticeHost(SessionState& p) : progress(p) {}
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool title_exit(i32) override { return true; }
    bool fade_out_title_music() override { return true; }
    bool sound(i32 id) override { sounds.push_back(id); return true; }
    bool begin_transition(u32&) override { return false; }  // scene owns the effect bank
    bool start_game(i32 stage) override {
        ++launches;
        launched_stage = stage;
        launched_character = progress.character;
        launched_rank = progress.difficulty;
        launched_practice = progress.practice_mode;
        launched_mode = progress.run_mode_flags & 0x30u;
        return true;
    }
    bool hud_text(const HudTextDraw& text) override { texts.push_back(text); return true; }
    bool practice_stage_played(i32 c, i32 d, i32 s) override {
        return c >= 0 && c < 4 && d >= 0 && d < 4 && s >= 1 && s <= 6 &&
               (played[u32(c * 4 + d)] & (1u << (s - 1)));
    }
};

struct PracticeHarness {
    GameRandom random;
    AnmEnvironment environment;
    AnmManager manager{random, environment};
    SessionState progress;
    TitleSelectionSettings settings;
    FrameScheduler scheduler;
    PracticeHost host{progress};
    std::array<FrameCallback, 2> updates{};
    std::unique_ptr<TitleScene> scene;
    PracticeHarness() {
        for (i32 i = 0; i < 2; ++i) {
            updates[u32(i)].owner = &manager;
            updates[u32(i)].enabled = true;
            updates[u32(i)].run = i == 0 ? +[](void* p) {
                return static_cast<AnmManager*>(p)->update(true) ? 1 : 5;
            } : +[](void* p) {
                return static_cast<AnmManager*>(p)->update(false) ? 1 : 5;
            };
            CHECK(scheduler.add(updates[u32(i)], FramePass::Update, i == 0 ? 9 : 34) >= 0);
        }
        CHECK(load_bank(manager, 16, practice_title_fixture()));
        CHECK(load_bank(manager, 17, synthetic_anm(1, trivial_scripts(1))));
        CHECK(load_bank(manager, 2, synthetic_anm(8, trivial_scripts(20))));
        CHECK(load_bank(manager, 8, synthetic_anm(4, trivial_scripts(4))));
        scene = std::make_unique<TitleScene>(progress, settings, manager, host, scheduler);
        scene->state.return_reason = 0;
    }
    void frame(u32 pressed = 0) {
        scene->controls({0, pressed, 0, 0});
        CHECK(scheduler.update() >= 0);
        CHECK(scene->error.empty());
    }
    void advance(i32 count) { for (i32 i = 0; i < count; ++i) frame(); }
    void ready_main() {
        advance(kTitleBootIntroFrames + kTitleMenuReadyDelay + 3);
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::Main));
        CHECK_EQ(scene->state.substate, 2);
    }
    void enter_practice(i32 rank = 1, i32 character = 0) {
        settings.configured_difficulty = rank;
        settings.preferred_character = character;
        ready_main();
        frame(0x20);  // default save skips Extra: 0 -> 2
        CHECK_EQ(scene->state.menu.cursor, 2);
        frame(1);
        advance(32);
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::Difficulty));
        CHECK_EQ(scene->state.substate, 2);
        CHECK_EQ(scene->state.menu.cursor, rank);
        frame(1);
        advance(25);
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::Character));
        CHECK_EQ(scene->state.menu.cursor, character + kTitleCharacterFirst);
        frame(1);
        advance(26);
        CHECK_EQ(i32(scene->state.screen), i32(TitleScreen::Stage));
        CHECK_EQ(scene->state.substate, 2);
        CHECK_EQ(scene->state.menu.count, 6);
        CHECK_EQ(progress.character, character);
        CHECK_EQ(progress.difficulty, rank);
        CHECK_EQ(progress.run_mode_flags & 0x30u, 0x10u);
        CHECK_EQ(progress.practice_mode, 0);  // launch, not menu entry, configures GameInf
        CHECK_EQ(host.launches, 0);
    }
};
}  // namespace

TEST(practice_default_save_has_six_locked_navigable_stages) {
    PracticeHarness h;
    h.enter_practice();
    CHECK(h.scene->state.handles[109] != 0);
    CHECK(h.scene->state.handles[112] != 0);
    for (i32 row = 0; row < 6; ++row) {
        CHECK_EQ(h.scene->state.menu.cursor, row);
        h.frame(1);
        CHECK_EQ(h.scene->state.substate, 2);
        CHECK_EQ(h.host.sounds.back(), 16);
        CHECK_EQ(h.host.launches, 0);
        h.frame(0x20);
    }
    CHECK_EQ(h.scene->state.menu.cursor, 0);
    h.host.texts.clear();
    CHECK(h.scheduler.draw() >= 0);
    CHECK_EQ(h.host.texts.size(), size_t(6));
    if (h.host.texts.size() == 6) {
        CHECK_EQ(h.host.texts[0].text, std::string("Stage 1  ---------"));
        CHECK_EQ(h.host.texts[0].style.color, 0xffdfdfdfu);
        CHECK_EQ(h.host.texts[1].style.color, 0xff808080u);
        CHECK_EQ(h.host.texts[5].position.x, 296.0f);
        CHECK_EQ(h.host.texts[5].position.y, 306.0f);
    }
}

TEST(practice_stage1_launch_inherits_selected_character_rank_and_mode) {
    PracticeHarness h;
    h.enter_practice(2, 2);
    h.host.played[2 * 4 + 2] = 1;
    h.frame(1);
    h.advance(42);
    CHECK_EQ(h.host.launches, 1);
    CHECK_EQ(h.host.launched_stage, 1);
    CHECK_EQ(h.host.launched_character, 2);
    CHECK_EQ(h.host.launched_rank, 2);
    CHECK_EQ(h.host.launched_mode, 0x10u);
    CHECK_EQ(h.host.launched_practice, 1);
    CHECK_EQ(h.progress.starting_stage, 1);
}

TEST(practice_stage6_gate_is_independent_per_character_and_rank) {
    PracticeHarness h;
    h.enter_practice(3, 1);
    for (i32 row = 0; row < 5; ++row) h.frame(0x20);
    h.host.played[0 * 4 + 3] = 0x20;  // wrong character
    h.host.played[1 * 4 + 2] = 0x20;  // wrong difficulty
    h.frame(1);
    CHECK_EQ(h.scene->state.substate, 2);
    h.host.played[1 * 4 + 3] = 0x20;
    h.frame(1);
    h.advance(42);
    CHECK_EQ(h.host.launches, 1);
    CHECK_EQ(h.host.launched_stage, 6);
    CHECK_EQ(h.progress.starting_stage, 6);
    CHECK_EQ(h.host.launched_character, 1);
    CHECK_EQ(h.host.launched_rank, 3);
    CHECK_EQ(h.host.launched_mode, 0x10u);
    CHECK_EQ(h.host.launched_practice, 1);
}

TEST(practice_cancel_chain_then_ordinary_start_has_no_mode_or_cursor_leak) {
    PracticeHarness h;
    h.enter_practice(2, 1);
    h.frame(0x20);  // remember Stage 2 row
    h.frame(0x100);
    h.advance(15);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Character));
    CHECK_EQ(h.scene->state.menu.cursor, 2);
    CHECK_EQ(h.scene->state.menu.depth, 2);
    CHECK_EQ(h.settings.practice_stage, 1);
    h.frame(0x100);
    h.advance(15);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Difficulty));
    CHECK_EQ(h.scene->state.menu.cursor, 2);
    CHECK_EQ(h.scene->state.menu.depth, 1);
    h.frame(0x100);
    h.advance(25);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK_EQ(h.scene->state.substate, 2);
    CHECK_EQ(h.scene->state.menu.cursor, 2);
    CHECK_EQ(h.scene->state.menu.depth, 0);
    CHECK_EQ(h.progress.run_mode_flags & 0x30u, 0u);
    CHECK_EQ(h.progress.practice_mode, 0);
    CHECK(h.scene->state.version_bar != 0);
    h.frame(0x10);  // Practice -> Start, skipping Extra again
    CHECK_EQ(h.scene->state.menu.cursor, 0);
    h.frame(1);
    h.advance(32);
    h.frame(1);
    h.advance(25);
    h.frame(1);
    h.advance(45);
    CHECK_EQ(h.host.launches, 1);
    CHECK_EQ(h.host.launched_stage, 1);
    CHECK_EQ(h.host.launched_mode, 0u);
    CHECK_EQ(h.host.launched_practice, 0);
    CHECK_EQ(h.host.launched_character, 1);
    CHECK_EQ(h.host.launched_rank, 2);
}

TEST(practice_can_be_cancelled_repeatedly_without_accumulating_disabled_entries) {
    PracticeHarness h;
    h.ready_main();
    for (i32 i = 0; i < 20; ++i) {
        if (h.scene->state.menu.cursor == 0) h.frame(0x20);
        h.frame(1);
        h.advance(32);
        CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Difficulty));
        h.frame(0x100);
        h.advance(25);
        CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
        CHECK_EQ(h.scene->state.substate, 2);
        CHECK_EQ(h.scene->state.menu.disabled_count, 1);
        CHECK_EQ(h.progress.run_mode_flags & 0x30u, 0u);
    }
}

TEST(practice_record_eligibility_excludes_replay_practice_spell_and_extra) {
    SessionState state;
    state.difficulty = 1;
    CHECK(records_practice_stage(state, false));
    CHECK(!records_practice_stage(state, true));
    for (u32 mode : {0x10u, 0x20u, 0x30u, 0x40u}) {
        state.run_mode_flags = mode;
        CHECK(!records_practice_stage(state, false));
    }
    state.run_mode_flags = 0;
    state.practice_mode = 1;
    CHECK(!records_practice_stage(state, false));
    state.practice_mode = 0;
    state.difficulty = 4;
    CHECK(!records_practice_stage(state, false));
    state.difficulty = 1;
    state.stage = 7;
    CHECK(!records_practice_stage(state, false));
}

TEST(practice_launched_flags_end_stage_in_settlement_without_advancing) {
    PracticeHarness h;
    h.enter_practice(1, 0);
    h.host.played[1] = 1;
    h.frame(1);
    h.advance(42);
    struct ResultsHost final : PostGamePlatform {
        std::vector<i32> destinations;
        void request_destination(i32 raw) override { destinations.push_back(raw); }
        void bgm(i32) override {}
    } results;
    HudPlayerState player;
    HudScoreState score;
    PostGameFlow flow(h.progress, player, score, results);
    flow.on_stage_clear();
    CHECK_EQ(i32(flow.state()), i32(OverlayState::PracticeEntry));
    CHECK(flow.frozen());
    CHECK(results.destinations.empty());
    CHECK_EQ(h.progress.stage, 1);
    CHECK_EQ(h.progress.stage_cleared_bits, 0u);
}

TEST(practice_game_return_restores_stage_and_entire_cancel_history_without_scoring) {
    PracticeHarness h;
    h.scene.reset();
    h.progress.character = 3;
    h.progress.difficulty = 2;
    h.progress.stage = h.progress.starting_stage = 4;
    h.progress.run_mode_flags = 0x10;
    h.progress.practice_mode = 1;
    h.progress.high_score = 7654321;
    h.progress.continues = 2;
    h.progress.ending_points = 123;
    h.settings.preferred_character = h.settings.configured_difficulty = 0;
    h.settings.practice_stage = 0;
    h.scene = std::make_unique<TitleScene>(h.progress, h.settings, h.manager, h.host, h.scheduler);
    // Application begin_title translates a real Practice quit (pause Exit
    // reason1 / GameOver-exit reason3, both original scene request 4) to
    // reason4. Retry is the separate original destination0xb path; it never
    // reaches begin_title (0x43c1c0 case0xb restarts the stage in place).
    CHECK(returns_to_practice_selection(h.progress, 1, false));
    CHECK(returns_to_practice_selection(h.progress, 3, false));
    h.scene->state.return_reason = 4;
    h.advance(15);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Stage));
    CHECK_EQ(h.scene->state.substate, 2);
    CHECK_EQ(h.scene->state.menu.cursor, 3);
    CHECK_EQ(h.scene->state.menu.depth, 3);
    CHECK_EQ(h.settings.preferred_character, 3);
    CHECK_EQ(h.settings.configured_difficulty, 2);
    CHECK_EQ(h.settings.practice_stage, 3);
    CHECK_EQ(h.progress.high_score, 7654321);
    CHECK_EQ(h.progress.continues, 2);
    CHECK_EQ(h.progress.ending_points, 123);
    CHECK_EQ(h.host.launches, 0);
    h.frame(0x100);
    h.advance(15);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Character));
    CHECK_EQ(h.scene->state.menu.cursor, 4);
    h.frame(0x100);
    h.advance(15);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Difficulty));
    CHECK_EQ(h.scene->state.menu.cursor, 2);
    h.frame(0x100);
    h.advance(25);
    CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
    CHECK_EQ(h.scene->state.menu.cursor, 2);
    CHECK_EQ(h.progress.run_mode_flags & 0x30u, 0u);
    CHECK_EQ(h.progress.practice_mode, 0);
    CHECK_EQ(h.progress.high_score, 7654321);
}

TEST(practice_return_classifier_preserves_ordinary_replay_and_gameover_paths) {
    SessionState progress;
    progress.run_mode_flags = 0x10;
    progress.practice_mode = 1;
    progress.difficulty = 2;
    progress.starting_stage = 3;
    // Pause Exit (reason1) and GameOver Continue-menu exit (reason3) both issue
    // original scene request 4 (0x43e780:209 / 0x43fa90:549); both re-enter
    // Stage Select while dc670==4 survives.
    CHECK(returns_to_practice_selection(progress, 1, false));
    CHECK(returns_to_practice_selection(progress, 3, false));
    CHECK(!returns_to_practice_selection(progress, 0, false));
    CHECK(!returns_to_practice_selection(progress, 1, true));
    progress.run_mode_flags |= 0x40;
    CHECK(!returns_to_practice_selection(progress, 1, false));
    progress.run_mode_flags = 0;
    CHECK(!returns_to_practice_selection(progress, 1, false));
    progress.run_mode_flags = 0x10;
    progress.practice_mode = 0;
    CHECK(!returns_to_practice_selection(progress, 1, false));
    progress.practice_mode = 1;
    progress.difficulty = 4;
    CHECK(!returns_to_practice_selection(progress, 1, false));
}

TEST(ordinary_and_replay_style_returns_still_enter_main) {
    for (i32 reason : {1, 3}) {
        PracticeHarness h;
        h.scene->state.return_reason = reason;
        h.advance(15);
        CHECK_EQ(i32(h.scene->state.screen), i32(TitleScreen::Main));
        CHECK_EQ(h.scene->state.substate, 2);
        CHECK_EQ(h.scene->state.menu.depth, 0);
        CHECK_EQ(h.host.launches, 0);
    }
}

int main() { return th13test::run_all("Practice title flow"); }
