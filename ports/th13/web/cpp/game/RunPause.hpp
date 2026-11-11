// 对局暂停覆盖层（th15 RunPause 裁剪组合：PauseState/PauseFrame/PauseMenu
// + 激活序列内联；成绩登录、录像保存、说明书、选项延后，ui-base.md §四）。
// pause 是 run 内覆盖层：不动 destination，重开/回标题由平台请求。
#pragma once

#include "PauseFrame.hpp"
#include "SessionState.hpp"
#include "AnmManager.hpp"
#include "TitlePlayerDataMenu.hpp"

namespace th13 {
struct HudDrawServices;

// GameOver = 0x43f570 mode0（0x43fa90/142）；BGameOver = 0x43f570 mode≠0
// （0x440960 态 0x18/147，三项：标题/录像/重试，无续关）。
enum class PauseEntrance { Pause, GameOver, PracticeEnd, ExtraEnd, BGameOver };

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
    // 0x43f8f0 排名门（mode0）：合格 = FUN_0043d6b0 把新分当场插入榜块并经
    // 0x415030(rank) 把 0 基入榜行写进覆盖层 +0x38；不合格 -1。返回行号并把
    // 插入后的 10 行榜拷进 rows（新行名字为空——END 才写名，0x43d6b0 写 8 空格）。
    virtual i32 result_ranking(TitlePlayerDataScore rows[10]) const { return -1; }
    virtual bool can_save_replay() const { return false; }
    // 0x43e3f0:0x43e588-0x43e592：Esc 暂停激活末尾对活动对话对象调
    // 0x429d00（9 槽 VM+0x18 清 bit1，不销毁）；0x43e700:0x43e749 恢复时
    // 0x429f10 置回。宿主实现应覆盖对话立绘/名牌/正文/气泡全部 VM。
    virtual bool dialogue_visible(bool) { return true; }
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
