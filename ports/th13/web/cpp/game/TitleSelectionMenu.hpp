// 难度与角色菜单。[解剖轮2] 难度屏按 difficulty-menu-anatomy.md 重做：
// root script123 (320,178) spawn 83,84,113..121（行条对/辅助/徽记滚动组/
// 光标 119/右半 118），独立建 script101（自驱滑入→固定徽记 (192,178) L26），
// 神霊粒子 = ascii bank2 script18 克隆（全屏随机出生逐帧漂移，脚本经 RNG 槽
// 10010-10012 自算几何）；难度子脚本 23-48 真机不建（root1 名行/数字行方案
// 作废）。角色菜单（窗B 解剖未覆盖）维持 script2/129 现实现待下轮解剖。
#pragma once

#include "TitleAnimations.hpp"
#include "TitleMainMenu.hpp"
#include "SessionState.hpp"
#include "HudDrawData.hpp"

#include <functional>

namespace th13 {

struct TitleMenuServices {
    virtual ~TitleMenuServices() = default;
    virtual bool sound(i32) = 0;
    virtual bool begin_transition(u32& animation) = 0;
    virtual bool start_game(i32 stage) = 0;
    // 0x451cb0: score + character*0x56dc + 0x7fd + (stage+rank*8)*8.
    // Missing progress is locked, not an invented Stage 1 unlock.
    virtual bool practice_stage_played(i32 character, i32 difficulty, i32 stage) {
        (void)character; (void)difficulty; (void)stage;
        return false;
    }
    // [r4-M] 角色 clear-tag 可见门数据源（原版 0x451450 case0→1 一次性判定：
    // score_data + 0x56dc*character + 0x7e4 + difficulty*4 == 0 → 00470030
    // 递归隐藏 script 131+character）。默认 false = 未通关（零进度全隐藏，
    // 与原版无存档行为一致）；真实持久字段由宿主覆盖（MenuExtras 数据源）。
    virtual bool character_cleared(i32 character, i32 difficulty) {
        (void)character;
        (void)difficulty;
        return false;
    }
    // Extra 行解锁门（原版 0x44c570 case0 +0x5c40 bit1；写点 0x416130 通关
    // 落档 0x1b26b+char*2 bit0x10）。默认 false = 无解锁存档禁用，同原版缺档。
    virtual bool extra_unlocked(i32 character) {
        (void)character;
        return false;
    }
};

class TitleDifficultyMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    TitleAnimations visuals;
    TitleSceneResources resources;
    AnmManager& animations;
    i32 frames = -1;  // 屏龄（substate 切换会重置 state.age，独立计时；-1=未入屏）
    bool check(bool);
    bool sound(i32);
    bool build_difficulty();      // [解剖] root123+badge101+徽记滚动（age0 = Z+21）
    void finish_menu_hides();     // [解剖] 主菜单 Z+21 隐藏时间线（非选中隐/选中白化/白条闪）
    void advance_choreography();  // [解剖] 每帧：光标缓动/徽记下滚/粒子/白条与选中衰减
    void spawn_particles(i32 count);  // ascii18 克隆（脚本自算位置漂移，RNG 对拍）
    void retire_difficulty();     // 徽记101退场 + 背景右半层复位（root label1 已在 Esc 帧发）
    void choose_character() noexcept;
    void return_main();

public:
    std::function<bool(i32)> play_sound;
    std::string error;
    TitleDifficultyMenu(TitleState& s, SessionState& p, TitleSelectionSettings& settings,
                        AnmManager& a, TitleSceneResources resources)
        : state(s), progress(p), settings(settings), visuals(s, a, resources.title, resources.ascii),
          resources(resources), animations(a) {}
    bool update(u32 pressed, u32 repeated);
};

class TitleCharacterMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleAnimations visuals;
    TitleSceneResources resources;
    TitleMenuServices& host;
    bool check(bool, const char*);
    bool visual(bool);
    bool character_interrupt(i32 character, i32 label, bool immediate);
    bool exit_portrait(i32 character, i32 label);   // 0x46fca0：immediate 退场
    bool enter_portrait(i32 character, i32 label);  // 0x46fc30：pending 入场
    void return_difficulty();

public:
    std::string error;
    TitleCharacterMenu(TitleState& s, SessionState& p, TitleSelectionSettings& settings,
                       AnmManager& a, TitleMenuServices& host, TitleSceneResources resources)
        : state(s), progress(p), settings(settings), animations(a),
          visuals(s, a, resources.title, resources.ascii), resources(resources), host(host) {}
    bool update(u32 pressed, u32 repeated);
};

// Practice Stage Select: 0x451cb0, six rows, locked rows remain navigable.
class TitleStageMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleAnimations visuals;
    TitleMenuServices& host;
    bool check(bool, const char*);
    bool available(i32 stage);

public:
    std::string error;
    TitleStageMenu(TitleState& s, SessionState& p, TitleSelectionSettings& choice,
                   AnmManager& a, TitleMenuServices& h, TitleSceneResources resources)
        : state(s), progress(p), settings(choice), animations(a),
          visuals(s, a, resources.title, resources.ascii), host(h) {}
    bool update(u32 pressed, u32 repeated);
    bool draw_texts(HudDrawServices&);
};

// 角色菜单行位（root2 子脚本 49-55，推定：1-4 为可选角色，0/5 为 EX/LUNATIC
// 字条位图（用途待锚定），6 为やめる）。
constexpr i32 kTitleCharacterRows = 7;
constexpr i32 kTitleCharacterFirst = 1;   // 霊夢行
constexpr i32 kTitleCharacterLast = 4;    // 妖夢行
inline i32 title_character_at(i32 row) { return row - kTitleCharacterFirst; }

}  // namespace th13
