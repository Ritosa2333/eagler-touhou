// 暂停菜单（th15 PauseMenu 裁剪：继续/重开/回标题 三项；Game Over 幕禁用继续；
// 成绩登录与录像保存延后）。菜单延迟与光标历史属于游戏态，
// ANM 树归标题/HUD 共用的可移植管理器，平台工作显式化。
#pragma once

#include "PauseState.hpp"
#include "AnmManager.hpp"

namespace th13 {

// 0043e504/0043e4f0/0043e5a0/0043fb7d 的原版立即数；不是 th15 的脚本号。
constexpr i32 kPauseMenuScript = 132;
constexpr i32 kPracticePauseMenuScript = 133;
constexpr i32 kPracticeEndMenuScript = 134;
constexpr i32 kGameOverMenuScript = 142;
// 此脚本来自常驻 text.anm（DAT_004dcc54），不是 front.anm 的数字75！
constexpr i32 kPauseBackdropScript = 75;

struct PauseMenuServices {
    virtual ~PauseMenuServices() = default;
    virtual bool sound(i32) = 0;
    // 原版槽位：0=继续、1=回标题、2=录像、3=设置、4=重试。
    virtual bool finish_selection(i32 entry) = 0;
    virtual bool save_replay_slot(i32, const std::string&) { return false; }
    virtual bool change_option(i32, i32) { return false; }
    virtual bool record_result_name(const std::string&) { return false; }
};

class PauseMenu {
    PauseState& state;
    AnmManager& animations;
    PauseMenuServices& host;
    bool check(bool, const char*);
    bool interrupt(u32, i32, bool immediate = false);
    bool child(i32, i32);
    bool navigate(u32, u32, i32);
    bool close();
    bool configure_entries();
    i32 entry_script(i32) const;

public:
    std::string error;
    PauseMenu(PauseState& s, AnmManager& a, PauseMenuServices& h) : state(s), animations(a), host(h) {}
    bool update(u32 pressed, u32 repeated);
};

}  // namespace th13
