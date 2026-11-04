// 标题场景状态：Main/Difficulty/Character/Practice Stage/Replay/Options 等
// 共用光标历史。PlayerData/SpellPractice 已接路由骨架（数据仍归宿主）。
// TitleScreen 数值暂沿 th15 世代枚举位；th13 supervisor（0x44e600 对应物）锚定后回填。
#pragma once

#include "MenuCursor.hpp"
#include "Timer.hpp"

namespace th13 {

// th13 TitleScreen 数值（supervisor 0x44c570 case 表逐项锚定）：
// Main=1、Quit/StartGame=2（同值冲突是有意保留的现状，TitleFrame/ApplicationState
// 以 screen==Quit 区分）、Option=3（case 3 → 0x44dd00，MenuExtras 实证）、
// 难度=5 对应物 450fb0（th13 沿 th15 位 6，待回填）、
// 角色=7（同上）、PlayerData=10 = case a → 0x453170（记录浏览；数据服务归宿主）、
// Replay=11 = case b → 0x4525b0（p58 反编译实证）、
// MusicRoom=13 = case d → 0x4554f0（MenuExtras 实证）、
// ResultName=14 = case 0xe → 0x454050（通关成绩名前录入/结算；Ending 结束
// dc670=3 分支进入）、ResultReplaySlot=15 = case 0xf → 0x4549d0（回放槽保存；
// 0x454050 state3 在 DAT_004be7c8==0 时进入）、Manual=16 = case 10
// → 0x455330（MenuExtras 实证；真实 help 查看器归宿主，wrapper≠完成）、
// SpellPractice=17 = case 0x11/0x12/0x13 → 0x4560a0/0x456650/0x4576c0
//（三级菜单；解锁/启动数据归宿主）。
enum class TitleScreen : i32 {
    Initialize = 0,
    Main = 1,
    Quit = 2,
    StartGame = 2,
    Option = 3,
    KeyConfig = 4,  // 0x44c570 case 4 -> 0x44fcc0 (Pad Config)
    Difficulty = 6,
    Character = 7,
    Stage = 8,  // 0x44c570 case8 -> 0x451cb0 (Practice Stage Select)
    PlayerData = 10,  // 0x44c570 case a -> 0x453170
    Replay = 11,
    MusicRoom = 13,
    ResultName = 14,        // 0x44c570 case 0xe -> 0x454050（成绩名前录入）
    ResultReplaySlot = 15,  // 0x44c570 case 0xf -> 0x4549d0（回放槽保存）
    Manual = 16,
    SpellPractice = 17,  // 0x44c570 case 0x11/0x12/0x13
};

struct TitleSelectionSettings {
    i32 configured_difficulty = 1, saved_difficulty = 0, preferred_character = 0;
    i32 practice_stage = 0;  // DAT_004bb4c0, zero-based row in the six-stage menu.
};

struct TitleState {
    MenuCursor menu;
    TitleScreen screen = TitleScreen::Main, previous_screen = TitleScreen::Initialize;
    i32 substate = 0;
    Timer age{0, 0, 0, 0, 0};
    u32 flags = 2;
    std::array<u32, 248> handles{};
    u32 version_bar = 0;  // 版本条（bank17 script0；原 portrait 字段误名，蓝图 §1.9）
    i32 return_reason = 1;
    i32 confirmed_entry = -1;  // Z 确认的主菜单项（转屏后隐藏时间线仍需定位该项）
    u32 start_effect = 0;
    // [窗B 粒子节奏] 神霊粒子（ascii bank2 script18 克隆）的全局出生钟：
    // oracle 0x46f4a0 头插实证 f491 突发 16 枚、此后每 3 帧 1 枚持续到角色屏/
    // 转场（窗B f749 仍插；窗B 共 102 枚 = 16 + 86）。难度屏建面时归 0，
    // 角色屏接力递增（跨屏相位不断）。
    u32 particle_clock = 0;
    // [R3-M] Replay 屏（0x4525b0）状态：扁平选中序持久化（DAT_004dc674 语义，
    // 自 case0 恢复 row=%25 / page=/25、case3 启动拍写回）；网页导入完成槽位
    // 提示（宿主 import 成功 → 菜单 substate2 消费并选中）；ascii.anm s19 粒子
    // 根句柄（title.anm s104 徽记走 handles[104]，析构经 handles 表）。
    i32 replay_index = 0;
    i32 replay_import_hint = -1;
    u32 replay_ascii_root = 0;
    // [通关结算] 0x44c570 case 0xe/0xf 屏内状态：
    //   result_name        = 0x454050 +0x599c 名字缓冲（8 字编辑，原版定长
    //                        char[9]，移植用 std::string）；
    //   result_name_cursor = +0x59b4 字符网格光标（0..90；88=SPACE/89=DEL/
    //                        90=END，91 项 = 7 行 × 13 列）；
    //   result_slot        = +0x28 回放槽序（0 基；文件名/宿主槽号 = +1，
    //                        1..25，对应 "th13_%.2d.rpy"）。
    std::string result_name;
    i32 result_name_cursor = 90;
    i32 result_slot = 0;
    // 0x4549d0 case2 Z 拍：当前待保存 replay 的名字（原版从 score+0x1b260
    // 拷进 +0x599c 的默认名，也是 00454f90 state3 头部行的名字来源）。
    std::string result_slot_name;
    // 0x454050 建面用的角色/难度（be7b8/be7c4 快照）与 +0x690 背景根。
    // 角色名牌 = script143+character，难度条 = script147+difficulty
    //（difficulty=4 用 script151 EXTRA）。result_rank = 本次入榜行 0..7，
    // -1 = 未入榜（原版 +0x59ac=1 锁定态）。
    i32 result_character = 0;
    i32 result_difficulty = 0;
    i32 result_rank = 0;
    u32 result_background = 0;

    TitleState() { age.set(0); }

    void change_screen(TitleScreen next) noexcept {
        previous_screen = screen;
        screen = next;
        substate = 0;
        age.set(0);
    }

    void change_substate(i32 value) noexcept {
        substate = value;
        age.set(0);
    }
};

}  // namespace th13
