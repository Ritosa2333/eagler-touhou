// th16 标题场景——原版 Title 对象（DAT_004a6f20，0x44aee0 创建；更新 0x44af80 优先级 6）。
//
// 对象偏移：
//   +0x10 title.anm 槽  +0x14 title_v.anm 槽
//   +0x18 画面（0x44a560）  +0x1c 上一画面  +0x20 画面内子状态（0x44a5e0）
//   +0x24 MenuCursor（0xd8 字节）
//   +0x2ac Timer（0x44af80 末尾每帧 +1；0x44a560/0x44a5e0 清零）
//   +0x2c0 VM id 表，按脚本号索引（+0x2c0 + script*4）：0 logo，0x61/0x65 装饰，0x68 难度底，0x69 机体底，
//          0x6a 季节底，0x71 关卡底，0x7e/0x7f 难度面板，0x96/0x98 机体面板，0x97/0x99 季节面板，
//          0x11f（+0x73c）= ascii.anm 0x13（落叶粒子发射器，0x44fe20 难度画面生成、0x44ad20 析构时 interrupt 1），0x14c = title_v script0
//
// 画面（+0x18）：1 主菜单 0x44b5f0；5 难度 0x44fe20；6 机体 0x4502c0；7 子季节 0x450af0；8 关卡 0x450ef0；
//   2 = 标题结束（0x44af80 case 2 置场景请求）；3 Option 0x44c570；4 Key Config（自 Option）；
//   0xa Player Data / 0xb Replay / 0xd Music Room / 0x10 Manual /
//   0x11 Spell Practice / 0x12 List / 0x13 Shot / 0x14 Start 已接（0x4560b0 名 VM + SCORE 高分）。
#pragma once
#include "AnmManager.hpp"
#include "GameGlobals.hpp"
#include "GameMenu.hpp"
#include "Input.hpp"
#include "MenuCursor.hpp"
#include "Music.hpp"
#include "ScoreFile.hpp"
#include "TextPaint.hpp"
#include "Types.hpp"

#include <functional>
#include <string>
#include <vector>

namespace th16 {

struct HelpScreen;  // GameMenu.hpp：标题 Manual / 暂停 Manual 共用
struct ScoreFile;
class AsciiManager;

class TitleScene {
public:
    enum Screen : i32 {
        kNone = 0, kMainMenu = 1, kFinished = 2, kOption = 3, kKeyConfig = 4, kDifficulty = 5, kShot = 6,
        kSeason = 7, kStage = 8, kScore = 0xa, kReplay = 0xb, kMusicRoom = 0xd,
        kNameEntry = 0xe, kNameConfirm = 0xf, kManual = 0x10,
        kSpellPractice = 0x11, kSpellList = 0x12, kSpellShot = 0x13, kSpellStart = 0x14,
    };
    static constexpr i32 kVmTable = 0x150;
    static constexpr i32 kLogo = 0;
    static constexpr i32 kOptionPanel = 1;            // +0x2c4
    static constexpr i32 kTitleV = 0x14c;
    static constexpr i32 kLeafEmitter = 0x11f;        // +0x73c
    static constexpr i32 kLeafEmitterScript = 0x13;   // ascii.anm：每帧 ins_502 生成脚本 0x12 落叶（mode 8）
    static constexpr i32 kSceneGame = 7;

    struct Event { const char* kind; i32 value; };

    i32 screen = kNone;          // +0x18
    i32 prev_screen = kNone;     // +0x1c
    i32 sub = 0;                 // +0x20
    MenuCursor cursor;           // +0x24
    i32 prev_cursor = 0;         // +0x28
    i32 timer = 0;               // +0x2b0
    u32 vm[kVmTable] = {};       // +0x2c0
    AnmFile* title_file = nullptr;   // +0x10
    AnmFile* title_v_file = nullptr; // +0x14
    AnmFile* ascii_file = nullptr;   // AsciiManager 槽 2
    i32 idle_frames = 0;             // DAT_004a5bf0：主菜单无输入帧计数
    i32 pending_demo = -1;           // 本帧触发的 demo 下标；GameSession 装文件后清
    // 标题 Replay 画面 0x451750
    MenuCursor page_cursor;          // +0x1d4：页（0..2），槽 = page*0x19 + cursor
    // 0x451560 扫描：槽 0..24 = th16_01..25.rpy，槽 25..74 = th16_ud????.rpy（目录枚举序）
    static constexpr i32 kReplayListSlots = 75;
    struct ReplayListEntry {
        ReplaySummary summary;
        char ud_name[5] = {};          // 头对象 +0x223：文件名 "ud" 后 4 字节（User 页行首）
        i32 stage_score[8] = {};       // 0x451d50 sub4：下一关段头 +0x34 或用户块 +0x14
        i32 stage_continues[8] = {};   // 同上 +0x3c / +0x94
        std::string path;
    };
    bool replay_slot_live[100] = {}; // +0x5b50：槽是否有文件
    i32 replay_stage_mask[100] = {}; // 每槽已有关卡位图（bit0=stage1…）；确认后用于禁用项
    bool replay_spell_practice[100] = {};
    ReplayListEntry replay_entries[kReplayListSlots];
    i32 replay_choice = -1;          // +0x5b48：选中的绝对槽
    i32 replay_stage_choice = 0;     // +0x5b4c：开局关卡下标（1-based 存到 DAT_004a5790）
    u32 replay_ui_flags = 0;         // +0x5ce8：bit2 离开中 / bit3 扫描完成
    std::string replay_directory;    // 宿主：th16_XX.rpy 所在目录
    std::string pending_replay_path; // 确认播放：GameSession case 0xd 装载
    HelpScreen* help = nullptr;      // DAT_004a6dd8：标题 Manual 0x4545a0
    // 画面 6 Music Room 0x4546f0：musiccmt.txt 每次进入画面重新解析（0x402440 读档）
    static constexpr i32 kMusicTracks = 32;      // +0x804 路径 / +0x1004 标题 / +0x1844 注释 的容量
    static constexpr i32 kMusicTrackVms = 20;    // +0x780..+0x7cc（其后即注释 VM 表）
    struct MusicTrack {
        char path[0x40] = {};        // +0x804 + i×0x40："bgm/th16_01"（'@' 之后）
        char title[0x42] = {};       // +0x1004 + i×0x42
        char comment[8][0x42] = {};  // +0x1844 + (i×8 + k)×0x42
    };
    std::string music_cmt;           // musiccmt.txt 原文
    MusicTrack music[kMusicTracks];
    i32 music_count = 0;             // +0x7f4
    i32 music_scroll = 0;            // +0x5a44 窗口首项（可见 10 条）
    u32 music_track_vms[kMusicTrackVms] = {};  // +0x780：title.anm 0xbc+i
    u32 music_comment_vms[8] = {};   // +0x7d0：text.anm 0x13+k
    i32 music_comment_line = 0;      // +0x7f8：注释逐行刷入进度
    i32 music_comment_track = 0;     // +0x7fc：注释所属曲目
    i32 music_notice = 0;            // +0x800：未听过曲目的提示已显示（再按确认才播放）
    // 0x44af80 +0x5ce8 bit0 / +0x5ce4：进标题 10 帧后重播 th16_01
    bool title_bgm_pending = false;
    bool first_entry = false;        // +0x5ce8 bit1：首次进标题（DAT_004a6f1c == 0），主菜单播完整开场
    i32 title_bgm_delay = 0;
    MenuCursor spell_shot_cursor;    // +0x5cec：符卡练习机体副光标
    i32 spell_group = 0;             // +0x5dc4：选中的分组（确认后带入列表）
    i32 spell_list_index = 0;        // +0x5dc8：列表确认的槽（带入 0x13）
    i32 spell_entry_index = 0;       // +0x5dcc：条目页确认的下标（带入 0x14 / 开战）
    i32 spell_slot_ids[5] = {-2, -2, -2, -2, -2};  // +0x5dd0：当前列表槽对应符卡 id
    // +0x740：文字行 VM——符卡练习名行（前 5，title.anm 0xd2+i）/ Player Data 符卡页 10 行（text.anm 脚本 3..12）
    u32 row_text_vms[10] = {};
    ScoreFile* scores = nullptr;     // 0x456060 解锁判定
    MusicManager* bgm = nullptr;     // DAT_004d9e10 乐曲命令
    SoundEffects* sound = nullptr;   // 0x45e150 音效请求；0x44dc70 写 DAT_004df4a0（音效主音量）
    const float* game_speed = nullptr;   // DAT_004a5788（0x43c470 淡出秒数除数）
    const u32* config_flags = nullptr;   // DAT_004c1800（bit13：退出改为停在场景 2）
    u32* loading_vm = nullptr;           // AsciiManager+0x19248：0x41a360 进标题时中断 1 并清掉
    // 0x44a850：任一机体在 Easy..Lunatic 有未续关通关（主菜单 Extra 项可选）
    bool extra_unlocked() const { return scores != nullptr && scores->extra_unlocked(); }
    bool shot_extra_unlocked(i32 shot) const { return scores != nullptr && scores->shot_extra_unlocked(shot); }        // 0x44a800
    bool difficulty_cleared_by_all(i32 d) const { return scores != nullptr && scores->difficulty_cleared_by_all(d); } // 0x44a8e0
    std::vector<TextIntoTextureRequest>* text_paints = nullptr;  // 0x46d990 名行入队
    // Player Data：机体沿用 +0x24 主光标（count 4，←→），难度 +0xfc（count 5，↑↓），
    // 翻页 +0x1d4（0 = 排名页，1.. = 该难度符卡每页 10 条）
    MenuCursor score_difficulty_cursor;
    static constexpr i32 kScoreShotLabel = 0xa7;        // title.anm 0xa7 + 机体（+0x55c）
    static constexpr i32 kScoreDifficultyLabel = 0xaf;  // title.anm 0xaf + 难度（+0x57c）
    // 0x402130 宿主键盘快照（DIK 下标，bit7 = 按下）；空 = 窗口无焦点（原版清零并返回 0）
    const u8* keyboard_state = nullptr;
    // Player Data 隐藏键序：DAT_004dfd60 本帧 / DAT_004dfe60 上帧键盘、DAT_004dfa44 进度、DAT_004dfa40 计时（进程全局）
    u8 secret_keys[256] = {};
    u8 secret_prev_keys[256] = {};
    i32 secret_progress = 0;
    i32 secret_timer = 0;
    MenuCursor score_page_cursor;
    // Key Config 工作表 +0x5b34..+0x5b3e（6×i16 手柄按钮号）：进入时自 GameGlobals::pad_buttons 抄入，
    // Quit 时写回；槽序 0 Shot / 1 Bomb / 2 Release / 3 Slow / 4 Pause / 5 保留（0x44f710 对调时参与）
    static constexpr int kKeyTableCount = 6;
    i16 key_table[kKeyTableCount] = {};
    // 0x401c30 手柄按钮快照（bit i = 按钮 i 按下，i < 0x1f）；Key Config 每帧取首个按下的按钮号
    const u32* pad_buttons = nullptr;
    char name_entry[9] = "        "; // +0x5a48：通关名字输入缓冲
    i32 name_cursor = 0;             // +0x5a54：名字光标
    i32 name_no_rank = 0;            // +0x5a58：1=未入榜
    MenuCursor name_pick;            // +0x5a5c：字符表光标（count 0x5b，wrap）
    i32 name_pick_prev = 0;          // +0x5a60
    i32 name_rank = -1;              // 0x43e250 插入名次（同时写入 cursor；<0 未入榜）
    // 画面 0xf（0x453c10）：通关后存回放
    bool save_slot_live[25] = {};    // +0x5b50：th16_01..25.rpy 头是否读到
    ReplaySummary save_slots[25];    // 槽头摘要（0x447ef0）
    i32 save_slot_choice = 0;        // +0x5b48：确认的槽（0 起）
    std::function<bool(int slot, ReplaySummary& out)> read_replay_slot;   // 0x447ef0
    std::function<bool(int slot, const char* name)> write_replay_slot;    // 0x448400
    std::function<ReplaySummary()> current_replay_summary;                // DAT_004a6f08 本局回放头
    std::function<void()> on_replay_full_run;                             // 0x4483b0(1)：重取时间戳、关卡记 8
    GameGlobals* globals = nullptr;
    std::vector<Event> events;       // SFX / 画面 / 场景请求 / 过场效果（供 harness 断言）
    // GameSession 注入：0x45d150 淡入淡出 / 0x41a390 Loading + 0x418ba0 过场
    std::function<void(i32 kind, i32 duration, u32 color)> on_screen_fade;
    std::function<void()> on_transition_effect;
    std::function<bool(i32)> load_help_page;
    std::function<AnmFile*()> load_help_anm;  // 0x42e760：help.anm

    void activate(AnmManager& anm, AnmFile* title, AnmFile* title_v, AnmFile* ascii, GameGlobals& g) {
        close_help(anm);
        auto fade = on_screen_fade;
        auto transition = on_transition_effect;
        auto help_loader = load_help_anm;
        auto replay_dir = replay_directory;
        auto read_slot = std::move(read_replay_slot);
        auto write_slot = std::move(write_replay_slot);
        auto current_summary = std::move(current_replay_summary);
        auto full_run = std::move(on_replay_full_run);
        *this = TitleScene{};
        on_replay_full_run = std::move(full_run);
        read_replay_slot = std::move(read_slot);
        write_replay_slot = std::move(write_slot);
        current_replay_summary = std::move(current_summary);
        title_file = title;
        title_v_file = title_v;
        ascii_file = ascii;
        globals = &g;
        screen = kNone;
        on_screen_fade = std::move(fade);
        on_transition_effect = std::move(transition);
        load_help_anm = std::move(help_loader);
        replay_directory = std::move(replay_dir);
    }
    bool live() const { return title_file != nullptr; }

    // 0x44af80：每帧由标题更新链（优先级 6）调用
    void update(AnmManager& anm, const InputState& input);
    // 0x44b530：标题绘制链分派（关卡练习分 / Player Data / Replay / 符卡 SCORE…）
    void draw(AsciiManager& ascii);
    // 0x456d50：符卡练习 0x12/0x13/0x14 的 SCORE 行（AsciiManager 入队，每帧重打）
    void draw_spell_hud(AsciiManager& ascii);
    // 0x4513c0：关卡选择画面练习最高分行
    void draw_stage_practice_scores(AsciiManager& ascii);
    // 0x453030：Player Data 排行榜行（page0）
    void draw_player_data(AsciiManager& ascii);
    // 0x451d50：Replay 列表 Ascii（page×25 槽）
    void draw_replay_list(AsciiManager& ascii);
    // 0x4538b0：画面 0xe 本机体×难度 10 条排名 + 入榜行名字输入
    void draw_name_entry_ranks(AsciiManager& ascii);
    // 0x4541b0：画面 0xf 回放槽列表（sub 2）/ 选中槽滑入 + 名字输入（sub 3）
    void draw_name_confirm(AsciiManager& ascii);
    // 0x4560b0：按难度填 +0x5dd0、刷 +0x740 名 VM（Nothing / No.%3d / ？？？）
    void refresh_spell_slot_names(AnmManager& anm, i32 list_index);   // 0x4560b0
    void move_spell_shot(AnmManager& anm, const InputState& input);    // 0x455790
    void highlight_spell_entry(AnmManager& anm, i32 entry);            // 0x4569a0
    void launch_spell_practice(i32 entry, i32 season);                 // 0x456a20 / 0x455d50 case 3
    void enter_shot_select();                                          // 0x44fe20 LAB_004501a0
    void enter_season_select(AnmManager& anm, i32 panel);              // 0x4502c0 LAB_004509e1
    void enter_practice_stage(AnmManager& anm, i32 panel);             // 0x450af0 LAB_00450d8f
    void enter_spell_list(AnmManager& anm);                            // 0x4553d0 LAB_004556b1
    void enter_spell_entries(AnmManager& anm);                         // 0x455900 LAB_00455bd4
    // 主菜单空闲 demo 门控（0x708 / bit6 / 下标 %3）；供单测与 update 共用
    void tick_idle_demo(const InputState& input);
    void update_help_after_anm(AnmManager& anm, const InputState& input);
    void close_help(AnmManager& anm);  // 释放标题 Manual 的 HelpScreen
    void parse_music_cmt(const char* text, size_t len) { music_cmt.assign(text ? text : "", text ? len : 0); }

private:
    // 0x44a560 / 0x44a5e0
    void set_screen(i32 s) { prev_screen = screen; screen = s; sub = 0; timer = 0; }
    void set_sub(i32 s) { sub = s; timer = 0; }
    // 0x45e150：标题各画面的音效请求直接进 SoundManager 队列（声像 0），下一帧 0x45e330 出声；事件表供 harness 断言
    void sfx(i32 id) {
        events.push_back({"sfx", id});
        if (sound != nullptr) sound->play(id);
    }

    // 0x44a790：vm[slot] 子树里脚本号 == script 的 VM id（无则 0）
    u32 child_id(AnmManager& anm, i32 slot, i32 script);
    // 0x44a700 / 0x44a680（logo 子项）
    void item_interrupt_now(AnmManager& anm, i32 script, i32 value);
    void item_interrupt(AnmManager& anm, i32 script, i32 value);
    // 0x44a650：interrupt(vm[slot],1) 并清零
    void dismiss(AnmManager& anm, i32 slot);
    // 0x46f340：清 flags_lo bit1（隐藏）并递归子树
    static void hide_medal(AnmManager& anm, u32 id);
    void spawn_into(AnmManager& anm, i32 slot, AnmFile& file, i32 script);

    void update_main_menu(AnmManager& anm, const InputState& input);     // 0x44b5f0
    void refresh_main_items(AnmManager& anm);
    void update_difficulty(AnmManager& anm, const InputState& input);    // 0x44fe20
    void update_shot(AnmManager& anm, const InputState& input);          // 0x4502c0
    void update_season(AnmManager& anm, const InputState& input);        // 0x450af0
    void update_stage(AnmManager& anm, const InputState& input);         // 0x450ef0
    void update_option(AnmManager& anm, const InputState& input);        // 0x44c570
    void update_replay(AnmManager& anm, const InputState& input);        // 0x451750
    void update_manual(AnmManager& anm);                                // 0x4545a0
    void update_music(AnmManager& anm, const InputState& input);         // 0x4546f0
    void parse_music_tracks();                                           // 0x455330 / 0x455370 逐行切分
    void layout_music_items(AnmManager& anm);                            // 光标移动后重排曲目 VM
    void paint_music_comment(AnmManager& anm);                           // 偶数帧刷一行注释
    void leave_music(AnmManager& anm);                                   // LAB_00455228：收起并 → 子状态 3
    void play_bgm_op(i32 op);                                            // 0x45ed00(op, 0, "dummy")
    void bgm_default();                                                  // 0x43c370(0, "th16_01")
    void bgm_name_entry();                                               // 0x43c370(0, "th128_08")
    void bgm_fade();                                                     // 0x43c470(0.05)
    void tick_title_bgm();                                               // 0x44af80 LAB_0044b0c6
    void update_spell_practice(AnmManager& anm, const InputState& input); // 0x4553d0
    void update_spell_list(AnmManager& anm, const InputState& input);    // 0x455900（分组内符卡列表 → 0x13）
    void update_spell_shot(AnmManager& anm, const InputState& input);    // 0x456a20（条目/机体；非 Extra→0x14）
    void update_spell_start(AnmManager& anm, const InputState& input);   // 0x455d50（最终确认开战）
    void update_score(AnmManager& anm, const InputState& input);         // 0x452330 Player Data
    void refresh_score_spell_page(AnmManager& anm);                       // 0x452c30
    void dismiss_score_page_rows(AnmManager& anm);
    void update_score_secret(const InputState& input);                    // 0x452330 机体 3 × 难度 4 段
    void update_name_entry(AnmManager& anm, const InputState& input);    // 0x4532f0 通关名字输入
    void update_name_confirm(AnmManager& anm, const InputState& input);  // 0x453c10 通关存回放
    void begin_name_input();                                             // 名字缓冲 ← last_name，字符表光标初值
    void navigate_name_pick(const InputState& input);                    // 13 列字符表上下左右
    // 确认键落一个字符 / 空格 / 退格；返回 false = 退格时已在行首（原版直接 return，不播 7）
    bool apply_name_pick();
    void draw_name_input(AsciiManager& ascii, float x, float y);         // 名字 + "_" + 字符表（0x4538b0 / 0x4541b0 共用段）
    void print_rank_row(AsciiManager& ascii, float y, i32 rank, i32 shot, i32 diff);  // 0x453030 / 0x4538b0 行格式
    void print_replay_row(AsciiManager& ascii, float y, i32 slot, const ReplaySummary& s, const char* stage);
    void print_replay_list_row(AsciiManager& ascii, float y, i32 abs);    // 0x451d50 一行（No. 页 / User 页）
    void update_key_config(AnmManager& anm, const InputState& input);    // 0x44e930
    void update_stub(AnmManager& anm, const InputState& input);          // 画面 9 / 0xc：原版仅每帧 0x43c440 停 BGM
    void request_game(i32 stage, i32 table);                             // DAT_004c17c4 = 7 等
    void transition_effect();                                            // 0x41a390 + 0x418ba0
    void refresh_option_items(AnmManager& anm);                          // 0x44c8c0：面板子项 0x1e/0x1f
    void apply_option_volume(AnmManager& anm);                           // 0x44dc70：音量数字 ANM（缺 VM 则跳过）
    void load_key_table();                                               // 0x44e930 入口 / Default：pad_buttons → key_table
    void commit_key_table();                                             // 0x44e930 Quit：key_table → pad_buttons（=配置块 +4）
    void assign_key_button(AnmManager& anm, i32 slot, i32 button);       // 0x44f710：写入并对调冲突槽
    void paint_key_table(AnmManager& anm);                               // 0x44ec60：按钮号 /10 %10 → 面板子脚本 0x3f..
    void refresh_key_config_items(AnmManager& anm);                      // 0x44f810：光标前 0x1e / 后 0x1f
    void panel_item_interrupt_now(AnmManager& anm, i32 script, i32 value);
    void scan_replay_slots();                                            // 同步替代 0x402fb0 扫描线程
    i32 replay_abs_slot() const { return page_cursor.index * 0x19 + cursor.index; }
};

}  // namespace th16
