// 标题主菜单（th13 真机流程重写，oracle 标题窗实证见 docs/plans/title-anm-blueprint.md
// §oracle 实测章）：boot 无「按任意键」提示幕，开场 sig.anm logo 幕 180 帧后
// 标题展开，菜单 intro 180 帧插值、g361 建菜单、g372 起可交互（全局帧锚点，
// oracle/title_oracle.py docstring）。
// 主菜单 10 项（root script0 @ (70,280)，子 3-22：灰 3-12 层 26 / 白 13-22 层 25）；
// 字条内容按 title01.png 位图辨认：開始/Extra開始/練習開始/スペル練習/リプレイ/
// プレイヤーデータ/音楽室/オプション/マニュアル/やめる（人类辨认待复核）。
// 1 号项 Extra Start 无存档数据被禁用（真机 scoreth13.dat 缺省 → 光标跳过、
// 字条黑化 alpha160，oracle 顶点色 0xA0000000 实证）。
#pragma once

#include "TitleState.hpp"
#include "SessionState.hpp"
#include "AnmManager.hpp"

#include <functional>

namespace th13 {

// th13 主菜单条目（顺序 = title01.png 字条行序，位图辨认；语义标注见上）。
// 链路：StartGame(0) → 难度 → 角色 → 开局；Practice(2) → 难度 → 角色 → 六关选择；
// Replay(4) → Replay 屏（TitleReplayMenu）；
// Quit(9) 出退出请求；其余入口 enter() 播确认音后不切屏（真机有画面，蓝图
// 「未实现」清单登记）。
enum class TitleMainEntry : i32 {
    StartGame = 0,
    ExtraStart = 1,   // 无存档禁用（oracle：↓ 一次 0→2 跳过）
    PracticeStart = 2,
    SpellPractice = 3,
    Replay = 4,
    PlayerData = 5,
    MusicRoom = 6,
    Option = 7,
    Manual = 8,
    Quit = 9,
};
constexpr i32 kTitleMainEntries = 10;

// 真机 boot 时序锚（oracle 标题窗，全局帧；标题任务使能 = g181 为 age 0）：
constexpr i32 kTitleBootIntroFrames = 180;  // g181→g361 菜单 intro 插值
constexpr i32 kTitleMenuReadyDelay = 11;    // g361 建菜单 → g372 可交互

// 标题脚本表（脚本号 = artifacts/reference/dumps/anm-list.txt title.anm 段，
// 蓝图 §一；层/锚点/标签语义见蓝图各节。bank 表 Application.cpp kBanks）。
struct TitleSceneResources {
    i32 title = 16;    // title.anm（23 entry / 242 脚本）
    i32 version = 17;  // title_v.anm：仅版本条 script0（bank17 无立绘，蓝图 §1.9）
    i32 ascii = 2;     // ascii.anm（boot 无提示幕，本轮不建 ascii VM）
    i32 help = 19;     // help.anm（析构 retire_resource 用，蓝图 §五-12）
    i32 main_root = 0;            // 主菜单根 (70,280)，spawn 3-22
    i32 difficulty_root = 1;      // 难度根 (70,312)，spawn 23-48（5 名行+数字行）
    i32 character_root = 2;       // 角色根 (70,312)，spawn 49-82（7 名行+数字行）
    i32 char_menu_root = 129;     // 角色菜单根（立绘 125-128+武器 131-138+选择框）
    i32 char_menu_root_ex = 130;  // EX 武器分支根（129/130 分支条件待锚定，蓝图 §1.8）
    // [解剖轮2] 难度屏构成（difficulty-menu-anatomy.md）：root script123
    // (320,178) spawn 83,84,113..121（行条/辅助/徽记/光标 119/右半 118）；
    // 独立建 script101（自带滑入→固定徽记 (192,178) L26）；神霊粒子 = ascii
    // bank2 script18 克隆（RNG 槽 10010-10012 自算位置/漂移）；难度子脚本
    // 23-48 真机不建（解剖实证，root1 方案作废——旧 rank_row_* 字段一并废除）。
    i32 difficulty_tree = 123;    // 难度屏根（子 83,84,113-121）
    i32 difficulty_tree_extra = 124;  // [0x450fb0 case0] Extra 单行屏根（0x7c；pvVar7=(be7c4>=4)+0x7b）
    i32 difficulty_badge = 101;   // 徽记对 101/102：dump L24277/L24293，自带 (-256,8)→(32,8) 20f 滑入，不加平移
    i32 difficulty_cursor = 119;  // root123 子脚本119 = 暗 NORMAL 行（rank00 sprite96）；
                                  // 原版无独立光标 VM，行标签 7/17+cursor 驱动全部行。
    i32 difficulty_particle = 18; // 神霊粒子模板（ascii bank2 script18）
    i32 background_root = 89;     // 背景根（spawn 92,93,91,90；内建 110f/170f 时序）
    i32 logo_root = 94;           // LOGO 首入场根（110f 延迟翻转，oracle g291 实证）
    i32 logo_restore_root = 95;   // LOGO 复位根（自子菜单/对局返回，时机待 oracle）
    i32 version_bar = 0;          // title_v script0 @(256,224) 层 26（30f 延迟淡入）
    i32 portrait_base = 125;      // 立绘 125+角色号（label 7/12/17/22+角色号）
    i32 weapon_base = 135;        // 通常武器图标 135+角色号（同上标签族）
    i32 weapon_base_ex = 139;     // EX 武器图标 139+角色号
    i32 clear_tag_base = 131;  // [r4-M 锚定] 角色 clear 标签 131+c（0x451450：
                               // score_data+0x56dc*c+0x7e4+diff*4==0 → 00470030
                               // 递归清子树可见位；旧名 weapon_selector_base）
    // [R3-M] Replay 屏（0x4525b0）：s104 徽记（dump title.txt L7193：sprite82、
    // ins_304(26)、(-256,8)→(32,8) 20f 滑入、label1 滑出）；ascii.anm s19 粒子根
    //（dump ascii.txt L1075：持续 spawn s18）。
    i32 replay_badge = 104;
    i32 replay_particle_root = 19;
};

class TitleMainMenu {
    TitleState& state;
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleSceneResources resources;
    bool fail(const std::string&);
    bool create(i32);
    bool create_at(i32, const Vec3& translation);
    bool present(u32&);
    bool interrupt(i32, i32, bool);
    bool child_interrupt(i32, i32, bool);
    bool sound(i32);
    // [轮7] 选择交叉驱动（绘制侧 poke 序列）
    i32 selection_step = 0, selection_item = 0, selection_previous = 0;
    bool selection_active = false, selection_move = false;
    bool version_bar_pending = false;  // [f388] ver 延 1 帧创建（oracle f389 首绘）
    i32 pending_frame_gap = 0;
    i32 menu_age = -1;  // [wf360] 单调屏龄（substate 切换重置 state.age 的规避）
    void set_item_alpha(i32 item, i32 alpha, bool white = true);  // [轮7] 逐项 poke（white 白条/灰条）
    void rearm_grey(i32 item);  // [f371] 灰条入场重挂（旧隐+新尾插，oracle 批序实证）
    void boot_fade(i32 frame);                   // [wf360] 15 帧线性入场
    void adopt_background_layers();  // [窗A] 背景半张层值按 oracle 实测（§8.3）
    void fade_menu_alphas(i32 step);  // [解剖轮2] Z+10 起灰条透明度衰减 poke
    bool open_menu();     // g361 菜单展开（root0+版本条+全项入场+禁用黑化）
    bool open_menu_version_bar();  // [f389] ver 延 1 帧创建体（挂起消费）
    bool select_entry();  // interrupt(0,7+cursor) 选中入场
    bool show_menu();     // 全项入场（真机 boot 全员可见，oracle 顶点色实证）
    void dim_disabled();  // 禁用项黑化 alpha160（oracle 0xA0000000）
    void enter(i32 entry);
    bool extra_gate();  // Extra 行解锁门（宿主存档；0x44c570 case0 +0x5c40 bit1）

public:
    // [轮9] 交叉序列推进（绘制回调逐帧驱动——update 侧 poke 被 tick 插值
    // 覆写，wasi trace 实证；TitleFrame draw 回调经 TitleScene 接线调用）。
    void advance_selection();
    std::function<bool(i32)> play_sound;
    // Extra 行解锁门数据源（原版 0x44c570 case0 读 +0x5c40 bit1；写点 =
    // 0x416130 通关落档链置 0x1b26b+char*2 bit0x10。宿主 FileStore 持久位，
    // 未接 = 缺省禁用，与原版无存档一致）。
    std::function<bool(i32)> extra_unlocked;
    std::string error;
    TitleMainMenu(TitleState& state, SessionState& progress, TitleSelectionSettings& settings,
                  AnmManager& animations, TitleSceneResources resources)
        : state(state), progress(progress), settings(settings), animations(animations),
          resources(resources) {}
    bool update(u32 pressed, u32 repeated);
};

}  // namespace th13
