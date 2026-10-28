// 对局暂停覆盖层（th15 RunPause 裁剪组合：PauseState/PauseFrame/PauseMenu
// + 激活序列内联；成绩登录、录像保存、说明书、选项延后，ui-base.md §四）。
// pause 是 run 内覆盖层：不动 destination，重开/回标题由平台请求。
#pragma once

#include "PauseFrame.hpp"
#include "SessionState.hpp"
#include "AnmManager.hpp"

namespace th13 {
struct HudDrawServices;

enum class PauseEntrance { Pause, GameOver, PracticeEnd, ExtraEnd };

struct RunPausePlatform {
    virtual ~RunPausePlatform() = default;
    virtual bool sound(i32) = 0;
    virtual bool suspend_music() = 0;
    virtual bool resume_music() { return true; }
    virtual bool restart_run() = 0;        // pending_destination = 13
    virtual bool return_to_title() = 0;    // pending_destination = 4
    virtual bool continue_run() { return false; }
    virtual bool save_replay() { return false; }
    virtual bool open_pause_options() { return false; }
    virtual bool can_open_pause_options() const { return false; }
    virtual bool save_replay_slot(i32, const std::string&) { return false; }
    virtual bool change_pause_option(i32, i32) { return false; }
    virtual i32 pause_option_value(i32) const { return 0; }
    virtual bool record_result_name(const std::string&) { return false; }
    virtual bool result_needs_name() const { return false; }
    virtual bool can_save_replay() const { return false; }
};

class RunPause final : private PauseMenuServices {
    struct Services;
    std::unique_ptr<Services> services;
    SessionState& progress;
    AnmManager& animations;
    AnmEnvironment& environment;
    RunPausePlatform& platform;
    i32 front, text;
    bool fail(const std::string&);
    // PauseMenuServices
    bool sound(i32) override;
    bool finish_selection(i32 entry) override;
    bool save_replay_slot(i32 slot, const std::string& name) override;
    bool change_option(i32 row, i32 delta) override;
    bool record_result_name(const std::string& name) override;

public:
    PauseState state;
    PauseMenu menu;
    PauseFrame frame;
    std::string error;
    RunPause(SessionState&, AnmManager&, AnmEnvironment&, RunPausePlatform&, FrameScheduler&,
             i32 front_bank, i32 text_bank = 0);
    ~RunPause();
    RunPause(const RunPause&) = delete;
    RunPause& operator=(const RunPause&) = delete;
    bool prepare();
    bool open(PauseEntrance);
    void controls(const PauseFrameInput& input) noexcept { frame.controls(input); }
    void disable() noexcept { frame.enable(false); }
    bool active() const noexcept { return state.screen != PauseScreen::Inactive; }
    bool draw_overlay(HudDrawServices& sink);
};

}  // namespace th13
