#include "RunPause.hpp"
#include "GameHud.hpp"

namespace th13 {

struct RunPause::Services final : PauseMenuServices {
    RunPause& owner;
    explicit Services(RunPause& p) : owner(p) {}
    bool sound(i32 id) override { return owner.sound(id); }
    bool finish_selection(i32 entry) override { return owner.finish_selection(entry); }
    bool save_replay_slot(i32 slot, const std::string& name) override { return owner.save_replay_slot(slot, name); }
    bool change_option(i32 row, i32 delta) override { return owner.change_option(row, delta); }
    bool record_result_name(const std::string& name) override { return owner.record_result_name(name); }
};

RunPause::RunPause(SessionState& p, AnmManager& a, AnmEnvironment& e, RunPausePlatform& h,
                   FrameScheduler& f, i32 bank, i32 text_bank)
    : services(std::make_unique<Services>(*this)), progress(p), animations(a), environment(e),
      platform(h), front(bank), text(text_bank), menu(state, a, *services), frame(state, p, menu, f) {}

RunPause::~RunPause() { frame.enable(false); }

bool RunPause::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Run pause unavailable" : why;
    return false;
}

bool RunPause::sound(i32 id) { return platform.sound(id); }
bool RunPause::save_replay_slot(i32 slot, const std::string& name) {
    return platform.save_replay_slot(slot,name) || fail("Replay save failed");
}
bool RunPause::change_option(i32 row, i32 delta) {
    return platform.change_pause_option(row,delta) || fail("Pause option update failed");
}
bool RunPause::record_result_name(const std::string& name) {
    return platform.record_result_name(name) || fail("Result record failed");
}

bool RunPause::draw_overlay(HudDrawServices& sink) {
    HudTextStyle style; style.font=4; style.color=0xffffffffu; style.shadow=true;
    std::string text;
    if (state.phase == 17) text="REPLAY SLOT  " + std::to_string(state.replay_slot) + " / 25";
    else if (state.phase == 18) text="NAME  " + state.replay_name + "   [" +
        (state.name_cursor == 90 ? std::string("END") : state.name_cursor == 89 ? std::string("DEL") :
         state.name_cursor == 88 ? std::string("SPACE") : std::string(1,kResultNameCharacters[state.name_cursor])) + "]";
    else if (state.phase == 19) text="OPTIONS  " + std::string(state.option_row ? "SE " : "BGM ") +
        std::to_string(platform.pause_option_value(state.option_row));
    else return true;
    if (sink.hud_text({{150,180,0},style,text})) return true;
    return fail("Pause overlay text failed");
}

bool RunPause::finish_selection(i32 entry) {
    // 43e780 phase4 only calls43e700 (resume) for entry0. Title/restart
    // keep the run frozen until the host switches scenes, including the
    // remaining callbacks in this update.
    if (entry == 2) return platform.save_replay() || fail("Replay save request failed");
    if (entry == 3) return platform.open_pause_options() || fail("Pause options request failed");
    if (!animations.retire(state.menu_animation) ||
        !animations.retire(state.snapshot_animation)) return fail(animations.error);
    state.menu_animation = 0;
    switch (entry) {
    case 0:
        if (state.screen == PauseScreen::GameOver && !platform.continue_run())
            return fail("Continue request failed");
        progress.rate = state.saved_rate;
        progress.scene_flags &= ~0x10u;
        environment.paused = false;
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
    // 激活序列（th15 PauseActivation 裁剪）：菜单视觉 → 音频挂起 → 表现冻结。
    state.select(entrance == PauseEntrance::GameOver ? PauseScreen::GameOver
                 : entrance == PauseEntrance::PracticeEnd ? PauseScreen::PracticeEnd
                 : entrance == PauseEntrance::ExtraEnd ? PauseScreen::ExtraEnd
                 : PauseScreen::Pause);
    progress.scene_flags |= 0x10;
    state.front_bank = front;
    state.practice = progress.practice_mode != 0;
    state.can_continue = progress.continue_stock > 0;
    state.can_save_replay = platform.can_save_replay() && progress.continues == 0;
    state.can_open_options = platform.can_open_pause_options();
    state.result_needs_name = (entrance == PauseEntrance::ExtraEnd || entrance == PauseEntrance::GameOver)
                              && platform.result_needs_name();
    if (entrance == PauseEntrance::GameOver || entrance == PauseEntrance::PracticeEnd)
        state.select_phase(0);
    if (!animations.retire(state.menu_animation) ||
        !animations.retire(state.snapshot_animation)) return fail(animations.error);
    // overlay 菜单 = create(ordering 4) + 清 0xc000 渲染位（th15 create_overlay 同义）。
    state.snapshot_animation = animations.create(text, kPauseBackdropScript, -1, 4);
    if (!state.snapshot_animation) return fail(animations.error);
    const i32 script = entrance == PauseEntrance::GameOver ? kGameOverMenuScript
                       : entrance == PauseEntrance::ExtraEnd ? 147
                       : entrance == PauseEntrance::PracticeEnd ? kPracticeEndMenuScript
                       : state.practice ? kPracticePauseMenuScript : kPauseMenuScript;
    state.menu_animation = animations.create(front, script, -1, 4);
    if (auto* vm = animations.registry.find(state.menu_animation))
        vm->visual.render_flags &= ~0xc000u;
    if (!state.menu_animation) return fail(animations.error);
    state.saved_rate = progress.rate;
    progress.rate = 1;
    environment.paused = true;
    if (!platform.suspend_music()) return fail("Pause music suspend failed");
    return animations.interrupt(state.menu_animation, 3) || fail(animations.error);
}

}  // namespace th13
