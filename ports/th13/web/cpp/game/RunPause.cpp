#include "RunPause.hpp"
#include "GameHud.hpp"
#include "TitleResultView.hpp"

#include <algorithm>
#include <cstdio>
#include <ctime>

namespace th13 {

namespace {
constexpr const char* kRankStageNames[10] = {"test   ", "Stage 1", "Stage 2", "Stage 3", "Stage 4",
                                             "Stage 5", "Stage 6", "Extra  ", "Clear  ", "ExClear"};

std::string rank_row_text(const TitlePlayerDataScore& row, i32 index) {
    char text[128];
    const char* name = row.name.empty() ? "        " : row.name.c_str();
    if (row.timestamp == 0 || row.stage < 0 || row.stage >= 10) {
        std::snprintf(text, sizeof text, "%2d %s %9d%d --/--/-- Stage -", index + 1, name, row.score, row.continues);
        return text;
    }
    const std::time_t clock = std::time_t(row.timestamp);
    const std::tm* date = std::localtime(&clock);
    if (!date) {
        std::snprintf(text, sizeof text, "%2d %s %9d%d --/--/-- Stage -", index + 1, name, row.score, row.continues);
        return text;
    }
    std::snprintf(text, sizeof text, "%2d %s %9d%d %.2d/%.2d/%.2d %s", index + 1, name, row.score, row.continues,
                  date->tm_year % 100, date->tm_mon + 1, date->tm_mday, kRankStageNames[row.stage]);
    return text;
}
}

struct RunPause::Services final : PauseMenuServices {
    RunPause& owner;
    explicit Services(RunPause& p) : owner(p) {}
    bool sound(i32 id) override { return owner.sound(id); }
    bool finish_selection(i32 entry) override { return owner.finish_selection(entry); }
    bool save_replay_slot(i32 slot, const std::string& name) override { return owner.save_replay_slot(slot, name); }
    bool change_option(i32 row, i32 delta) override { return owner.change_option(row, delta); }
    bool record_result_name(const std::string& name) override { return owner.record_result_name(name); }
    std::string last_result_name() const override { return owner.platform.last_result_name(); }
};

RunPause::RunPause(SessionState& p, AnmManager& a, AnmEnvironment& e, RunPausePlatform& h, FrameScheduler& f, i32 bank,
                   i32 text_bank)
    : services(std::make_unique<Services>(*this)), progress(p), animations(a), environment(e), platform(h), front(bank),
      text(text_bank), menu(state, a, *services), frame(state, p, menu, f) {}

RunPause::~RunPause() { frame.enable(false); }

bool RunPause::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Run pause unavailable" : why;
    return false;
}

bool RunPause::sound(i32 id) { return platform.sound(id); }
bool RunPause::save_replay_slot(i32 slot, const std::string& name) {
    return platform.save_replay_slot(slot, name) || fail("Replay save failed");
}
bool RunPause::change_option(i32 row, i32 delta) {
    return platform.change_pause_option(row, delta) || fail("Pause option update failed");
}
bool RunPause::record_result_name(const std::string& name) {
    return platform.record_result_name(name) || fail("Result record failed");
}

bool RunPause::draw_overlay(HudDrawServices& sink) {
    if (state.screen == PauseScreen::Inactive) return true;
    HudTextStyle style;
    style.shadow = true;
    const auto put = [&](float x, float y, u32 color, const std::string& text) {
        style.color = color;
        return sink.hud_text({{x, y, 0}, style, text}) || fail("Pause overlay text failed");
    };
    if (state.phase == 18) {
        TitlePlayerDataScore rows[10];
        const i32 rank = platform.result_ranking(rows);
        if (state.name_is_result) {
            if (rank < 0) return fail("Result rank unavailable for name entry");
            if (!put(48, 64, 0xffffffffu, "            Score Ranking!!")) return false;
            for (i32 i = 0; i < 10; ++i)
                if (!put(48, 96.f + 18.f * i, i == rank ? 0xffffff00u : 0xff808080u, rank_row_text(rows[i], i)))
                    return false;
            const float name_y = 96.f + 18.f * rank;
            if (!put(75, name_y, 0xffffffffu, state.replay_name)) return false;
            float underscore_x = 75.f + 9.f * float(state.replay_name.size());
            if (state.replay_name.size() >= 8) underscore_x -= 9.f;
            if (!put(underscore_x, name_y, 0xffffff00u, "_")) return false;
        } else {
            const float slot_y = 64.f + 15.f * (state.replay_slot - 1);
            const float age = std::min(10.f, state.age.fractional);
            const float y = 224.f + (slot_y - 224.f) * (10.f - age) / 10.f;
            char slot[8];
            std::snprintf(slot, sizeof slot, "No.%02d", state.replay_slot);
            if (!put(48, y, 0xffffff00u, slot) || !put(102, y, 0xffffffffu, state.replay_name)) return false;
            float underscore_x = 102.f + 9.f * float(state.replay_name.size());
            if (state.replay_name.size() >= 8) underscore_x -= 9.f;
            if (!put(underscore_x, y, 0xffffff00u, "_")) return false;
        }
        for (i32 i = 0; i < 91; ++i) {
            const u8 ch = i < 0x58    ? u8(kResultNameCharacters[i])
                          : i == 0x58 ? u8(0x81)
                          : i == 0x59 ? u8(0x7f)
                                      : u8(0x80);
            const float x = 112.f + 18.f * float(i % 13);
            const float y = 320.f + 16.f * float(i / 13);
            if (!put(x, y, i == state.name_cursor ? 0xffffff00u : 0xff808080u, std::string(1, char(ch)))) return false;
        }
        return true;
    }
    if (state.phase == 17) {
        for (i32 slot = 1; slot <= 25; ++slot) {
            const u32 color = slot == state.replay_slot ? 0xffffff00u : 0xff808080u;
            if (!put(48, 64.f + 15.f * (slot - 1), color,
                     replay_save_slot_row(slot, platform.replay_entry(slot - 1), true)))
                return false;
        }
        return true;
    }
    if (state.phase == 19) {
        const std::string text = "OPTIONS  " + std::string(state.option_row ? "SE " : "BGM ") +
                                 std::to_string(platform.pause_option_value(state.option_row));
        return put(150, 180, 0xffffffffu, text);
    }
    if (state.screen == PauseScreen::GameOver && state.phase == 6)
        return put(184, 448, 0xffffffffu, "Credit " + std::to_string(progress.continue_stock));
    return true;
}

bool RunPause::finish_selection(i32 entry) {
    if (entry == 2) return platform.save_replay() || fail("Replay save request failed");
    if (entry == 3) return platform.open_pause_options() || fail("Pause options request failed");
    if (!animations.retire(state.menu_animation) || !animations.retire(state.snapshot_animation))
        return fail(animations.error);
    animations.clear_captures();
    state.menu_animation = 0;
    switch (entry) {
    case 0:
        if (state.screen == PauseScreen::GameOver && !platform.continue_run()) return fail("Continue request failed");
        progress.rate = state.saved_rate;
        progress.scene_flags &= ~0x10u;
        environment.paused = false;
        if (!platform.dialogue_visible(true)) return fail("Pause dialogue show failed");
        return platform.resume_music() || fail("Pause music resume failed");
    case 1:
        return platform.return_to_title() || fail("Pause title destination failed");
    case 4:
        return platform.restart_run() || fail("Pause restart destination failed");
    default:
        return fail("Pause selection outside original range");
    }
}

bool RunPause::prepare() {
    if (!error.empty() || !frame.error.empty()) return fail(frame.error);
    if (!animations.resource(front)) return fail("Run pause front resource unavailable");
    if (!animations.resource(text)) return fail("Run pause text resource unavailable");
    frame.enable(true);
    return true;
}

bool RunPause::open(PauseEntrance entrance) {
    if (!error.empty()) return false;
    PauseState previous_state = state;
    const float previous_rate = progress.rate;
    const u32 previous_scene_flags = progress.scene_flags;
    const bool previous_paused = environment.paused;
    bool activated = false;
    auto rollback = [&]() {
        animations.retire(state.menu_animation);
        animations.retire(state.snapshot_animation);
        animations.clear_captures();
        state = previous_state;
        progress.rate = previous_rate;
        progress.scene_flags = previous_scene_flags;
        environment.paused = previous_paused;
        if (activated && !platform.resume_music()) error = "Pause activation rollback music resume failed";
    };
    state.select(entrance == PauseEntrance::GameOver      ? PauseScreen::GameOver
                 : entrance == PauseEntrance::PracticeEnd ? PauseScreen::PracticeEnd
                 : entrance == PauseEntrance::ExtraEnd    ? PauseScreen::ExtraEnd
                 : entrance == PauseEntrance::BGameOver   ? PauseScreen::BGameOver
                                                          : PauseScreen::Pause);
    progress.scene_flags |= 0x10;
    state.front_bank = front;
    state.practice = progress.practice_mode != 0;
    state.can_continue = progress.continue_stock > 0;
    const bool results_menu = entrance == PauseEntrance::ExtraEnd || entrance == PauseEntrance::BGameOver;
    state.can_save_replay = platform.can_save_replay() && (results_menu || progress.continues == 0);
    state.can_open_options = platform.can_open_pause_options();
    state.result_needs_name =
        (entrance == PauseEntrance::ExtraEnd || entrance == PauseEntrance::GameOver) && platform.result_needs_name();
    if (entrance == PauseEntrance::GameOver || entrance == PauseEntrance::PracticeEnd) state.select_phase(0);
    if (!animations.retire(state.menu_animation) || !animations.retire(state.snapshot_animation)) {
        rollback();
        return fail(animations.error);
    }
    state.snapshot_animation = animations.create(text, kPauseBackdropScript, -1, 4);
    if (!state.snapshot_animation) {
        rollback();
        return fail(animations.error);
    }
    if (auto* backdrop = animations.registry.find(state.snapshot_animation)) animations.queue_capture(*backdrop);
    const i32 script = entrance == PauseEntrance::GameOver      ? kGameOverMenuScript
                       : entrance == PauseEntrance::ExtraEnd    ? kResultsMenuScript
                       : entrance == PauseEntrance::BGameOver   ? kResultsMenuScript
                       : entrance == PauseEntrance::PracticeEnd ? kPracticeEndMenuScript
                       : state.practice                         ? kPracticePauseMenuScript
                                                                : kPauseMenuScript;
    if (!(entrance == PauseEntrance::GameOver && state.result_needs_name)) {
        state.menu_animation = animations.create(front, script, -1, 4);
        if (auto* vm = animations.registry.find(state.menu_animation)) vm->visual.render_flags &= ~0xc000u;
        if (!state.menu_animation) {
            rollback();
            return fail(animations.error);
        }
    }
    state.saved_rate = progress.rate;
    progress.rate = 1;
    environment.paused = true;
    if (!platform.suspend_music()) {
        rollback();
        return fail("Pause music suspend failed");
    }
    activated = true;
    if (entrance == PauseEntrance::Pause && !platform.sound(0x0e)) {
        rollback();
        return fail("Pause entry sound failed");
    }
    if (state.menu_animation && !animations.interrupt(state.menu_animation, 3)) {
        rollback();
        return fail(animations.error);
    }
    if (entrance == PauseEntrance::Pause && !platform.dialogue_visible(false)) {
        rollback();
        platform.dialogue_visible(true);
        return fail("Pause dialogue hide failed");
    }
    return true;
}

}
