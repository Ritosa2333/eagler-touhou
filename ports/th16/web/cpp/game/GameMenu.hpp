// th16 游戏内菜单——原版 GameMenu（DAT_004a6ef4，0x3f4 字节；0x43e5a0 创建 → 0x43e350 构造）：
// 暂停、续关（Game Over）与练习 / 符卡练习结算共用一个状态机，另带回放存档槽选择、名字输入与操作说明。
//
// 锚点：
//   0x43e3f0  登记更新链 10（0x43e720 → 0x43e5f0）与绘制链 0x4a（0x43ef10 → 0x43edc0）。更新链 10 排在 GameManager（0xf）
//             之前：暂停时 0x42d7b0 因 flags & 0x10 返回 3 截断其后全部链，菜单与 UI 链 VM（链 9）照常推进
//   0x43e5f0  模式 0：Esc（DAT_004a50bc & 0x100）或失焦位（DAT_004c1800 & 0x10）、非回放、非换关、GameManager 计时 > 0x1d
//             → 0x43f0f0 打开暂停；模式 1..3 → 0x43f980 状态机；最后两只计时按全局游戏速度推进
//   0x43f0f0  打开暂停：播放时长结算 0x42dbc0、模式 1、GameManager flags |= 0x10、front.anm 0x9c（练习 0x9e）菜单 VM 并中断 3、
//             声音循环暂停、音效 0xe、BGM "Pause"、截屏 0x43ef20（text.anm 0x34 UI VM）、游戏速度存为 1.0、
//             声音线程状态存为 0、对话 0x42b610 与 HUD 0x42c580 隐藏
//   0x43f350  续关画面（残机 < 0 的复活第 0x1e 帧，0x442560 调用）：模式 2 状态 2、保存 BGM 名与播放位置、"th128_08" 曲
//   0x43f500  练习 / 符卡练习结算（0x42e150）：模式 3 状态 3（符卡练习 5）、text.anm 0x34 截屏、flags |= 4
//   0x43f980  状态机，见 GameMenu::State；0x43e150 切模式（上一模式、状态 0、两计时复位）；0x43e200 切状态（主计时复位）
//   0x43f6a0  恢复游戏：清 flags & 0x10、恢复游戏速度 / 对话 / HUD / 声音线程状态
//   0x43f7e0  续关首帧排名：普通模式 0x43e250 入榜（→ 名字输入）、练习更新练习最高分
//   0x43e250  排名插入（ScoreFile::insert_rank）
//   0x43edc0  绘制链：截屏框 VM（text.anm 0x34）的子 VM 0x39 颜色 → 画面合成 VM2（暂停时场地的蒙色）；AsciiManager 开阴影后
//             按模式出字——暂停 / 结算：回放槽列表 0x43e9b0 或回放名字输入 0x43eb50；续关：排名 0x43ec10（状态 0xf）、
//             回放列表 / 名字输入，否则（非操作说明）"Credit %d" 剩余续关数
//
// 操作说明（0x42ea30 / 0x42eab0，help.anm + help_NN.png，由装载线程异步装入）在 HelpScreen 中还原。
#pragma once
#include <functional>
#include <string>
#include <vector>

#include "AnmVm.hpp"
#include "MenuCursor.hpp"
#include "Music.hpp"
#include "Sound.hpp"
#include "Types.hpp"

namespace th16 {

class AnmManager;
struct AnmFile;
struct GameGlobals;
class GameManager;
class Gui;
class AsciiManager;
class Player;
class ScreenCompositor;
struct InputState;
struct ScoreFile;

// 回放头里菜单列表要显示的那部分（原版回放头 0x4483b0：+0x00 名字 / +0x0a 标志 bit1 符卡练习 / +0x0c 时刻（time32）/
// +0x84 机体 / +0x88 机体副号 / +0x8c 难度 / +0x90 关卡 / +0x98 符卡号 / +0x9c 子季节）
struct ReplaySummary {
    static constexpr i32 kAllStages = 8;  // 关卡名表 0x4918f0[8] = "All"
    char name[9] = "        ";
    i32 timestamp = 0;           // 1970 起算的秒数（UTC）；0x43e9b0 用 gmtime 拆成 年/月/日
    bool spell_practice = false;
    i32 shot = 0;
    i32 shot_aux = 0;
    i32 difficulty = 0;
    i32 stage = 0;
    i32 spell_id = 0;
    i32 season = 0;
    float slow_rate = 0.0f;      // 用户块慢放率（标题列表 %2.1f%%）
};

// 回放文件存取（0x447ef0 读槽头 / 0x448400 保存 / 0x447fb0 释放）：由宿主接入文件系统；为空时视为全部槽位为空
struct ReplayStore {
    virtual ~ReplayStore() = default;
    virtual bool slot_exists(int slot) = 0;                 // th16_%.2d.rpy 是否存在
    virtual ReplaySummary prepare_save(i32 stage) = 0;    // 写本局元数据，供名字输入和存档共用
    virtual void save(int slot, const char* name) = 0;      // 以当前记录写入该槽
    // 槽列表一行要显示的头信息；返回 false 表示该槽为空（画 "--------" 占位行）
    virtual bool slot_summary(int slot, ReplaySummary& out) { (void)slot; (void)out; return false; }
};

// 操作说明画面——原版 DAT_004a6dd8（0x42e6a0 构造，0x1bc 字节；更新链 0xb、绘制链 0x48）。
// help.anm（槽 0x13）与各页 help_NN.png 由装载线程异步装入；这里由宿主回调同步提供 ANM 文件。
struct HelpScreen {
    enum Phase : i32 { Loading = 0, Running = 1, Closing = 2 };
    enum Step : i32 { BuildMenu = 0, Menu = 1, PageReady = 3, Page = 4, TurnPage = 5 };

    Phase phase = Loading;      // +0x0c
    Timer timer;                // +0x10
    MenuCursor cursor;          // +0x24（9 页）
    u32 item_vms[9] = {};       // +0xfc：目录项 VM
    u32 page_vm = 0;            // +0x120：当前页 VM（脚本 9）
    bool closed = false;        // +0x124：关闭完成（GameMenu 据此回到主菜单）
    float horizontal_offset = 0; // +0x128
    AnmFile* anm = nullptr;     // +0x12c：help.anm
    Step step = BuildMenu;      // +0x134
    std::vector<i32> sfx_queue;
    std::function<bool(i32)> load_page;  // 0x42e7c0: help_NN.png

    void update(AnmManager& anm_manager, const InputState& input, float game_speed);   // 0x42eab0
    void retire_animations(AnmManager& anm_manager) const;

private:
    void spawn_items(AnmManager& anm_manager);
    void highlight(AnmManager& anm_manager);
    bool start_page_load(AnmManager& anm_manager);
    void return_to_directory(AnmManager& anm_manager);
};

struct GameMenuContext {
    AnmManager* animations = nullptr;
    GameGlobals* globals = nullptr;
    GameManager* game = nullptr;
    Gui* gui = nullptr;
    Player* player = nullptr;
    ScoreFile* scores = nullptr;
    ReplayStore* replays = nullptr;
    const InputState* input = nullptr;     // DAT_004a50b0 组（pressed / repeat）
    AnmFile* text_anm = nullptr;           // DAT_004c17f8：截屏框 VM 用 text.anm 0x34
    float* game_speed = nullptr;           // DAT_004a5788
    i32* sound_thread_state = nullptr;     // DAT_004d9d90
    const u32* config_flags = nullptr;     // DAT_004c1800
    MusicManager* music = nullptr;         // 0x45ed00 乐曲命令：暂停 6 / 恢复 7 / 续关换 th128_08 / 续播保存曲
    SoundRequests* sfx_sink = nullptr;     // 帧级音效表（为空时登记到 sfx_queue 由宿主取走）
    SoundEffects* sound = nullptr;         // 0x42e4d0 暂停 / 0x440bd0 恢复 / 0x45e330 排空
    // 0x42e760 装载线程：help.anm 进槽 0x13；返回 nullptr 表示仍在装载（操作说明保持等待）
    std::function<bool(i32)> load_help_page;
    std::function<AnmFile*()> load_help_anm;
    // 0x42dbc0 播放时长结算 / 0x45b130 取时刻重置起点 DAT_004a5c00（宿主墙钟；harness 无墙钟时为空操作）
    std::function<void()> accumulate_play_time;
    std::function<void()> restart_play_clock;
};

class GameMenu {
public:
    enum Mode : i32 { Closed = 0, Pause = 1, Continue = 2, PracticeResult = 3 };   // +0x1ec
    enum State : i32 {                                                             // +0x1f4
        PauseIntro = 0,          // 暂停首帧（第 10 帧进主菜单）
        PracticeIntro = 1,       // 练习结算首帧
        ContinueIntro = 2,       // 续关首帧
        PracticeResultIntro = 3, // 练习结算首帧（与 2 同一处理）
        ResultIntro4 = 4,        // 原版保留项（无写入点）
        SpellPracticeIntro = 5,  // 符卡练习结算首帧
        MainMenu = 6,
        ConfirmRestart = 7,      // "やり直す" 确认
        ConfirmDone = 8,         // 确认后回到主菜单 / 执行
        ConfirmQuit = 9,         // 返回标题确认
        ReplayPrompt = 10,       // 读取回放槽列表
        ReplaySlot = 0xb,        // 回放槽选择
        ReplayName = 0xc,        // 回放名字输入
        HelpPages = 0xe,         // 操作说明
        RankName = 0xf,          // 排名名字输入
        Execute = 0x10,          // 第 0xc 帧执行所选项并关闭
    };
    // 主菜单项（front.anm 0x9c / 0x9e / 0xa0 / 0xa1 的子脚本组）
    enum Item : i32 { Resume = 0, ToTitle = 1, SaveReplay = 2, Manual = 3, Restart = 4 };
    static constexpr u32 kFlagReplaySave = 0x1;    // +0x3ec bit0：回放槽列表打开
    static constexpr u32 kFlagNameEntry = 0x2;     // bit1：名字输入中
    static constexpr u32 kFlagSpellPractice = 0x4; // bit2：符卡练习结算（0x43f500 置）
    static constexpr int kReplaySlots = 25;
    static constexpr int kNameCharCount = 0x5b;    // 88 字符 + 空格 / 退格 / 结束
    static constexpr int kNameSpace = 0x58, kNameBackspace = 0x59, kNameEnd = 0x5a;

    Timer timer;                 // +0x0c
    Timer timer2;                // +0x20
    MenuCursor cursor;           // +0x34（+0x104 = cursor.wrap，菜单打开时置 1；+0x108 = cursor.disabled_count，切模式时清零）
    i32 cursor_previous = 0;     // +0x38
    MenuCursor name_cursor;      // +0x10c（+0x1dc = name_cursor.wrap）
    i32 name_cursor_previous = 0;// +0x110
    u32 menu_vm = 0;             // +0x1e4
    u32 screenshot_vm = 0;       // +0x1e8
    Mode mode = Closed;          // +0x1ec
    Mode previous_mode = Closed; // +0x1f0
    State state = PauseIntro;    // +0x1f4
    i32 name_length = 0;         // +0x1f8
    i32 practice = 0;            // +0x1fc：续关画面 0 / 练习结算 1
    i32 no_rank = 0;             // +0x200：续关时未入榜
    i32 kind = 0;                // +0x204：主菜单种类 0 暂停 / 1 练习结算
    i32 saved_sound_state = 0;   // +0x208
    bool replay_slot_used[kReplaySlots] = {};  // +0x20c：各槽回放头是否存在
    char name[12] = {};          // +0x2d4
    float saved_game_speed = 0;  // +0x2e0（0x43e350 memset 为 0）
    std::string saved_bgm;       // +0x2ec
    double saved_bgm_position = 0.0;   // +0x2e4：0x471bd0 取得的播放时刻（秒）
    u32 flags = 0;               // +0x3ec
    AnmFile* front = nullptr;    // +0x3f0
    HelpScreen* help = nullptr;  // DAT_004a6dd8
    std::vector<i32> sfx_queue;                 // 0x45e150

    GameMenuContext ctx;

    // 0x43e3f0 末：两只计时复位（登记链后）
    void reset_timers() { timer.reset(); timer2.reset(); }
    // 0x43e5f0
    void update();
    // 0x43edc0（绘制链 0x4a）：截屏框子 VM 0x39 的颜色传给画面合成 VM2；按模式画 Credit / 排名 / 回放槽列表 / 名字输入
    void draw(AsciiManager& text, ScreenCompositor& compositor);
    // 0x43f0f0 / 0x43f350 / 0x43f500 / 0x43f240
    void open_pause();
    void open_continue();
    void open_practice_result();
    void open_return_to_title();
    void accumulate_play_time(); // 0x42dbc0：本局游玩时间计入成绩文件（成绩文件接入前仅保留调用点）
    bool is_open() const noexcept { return mode != Closed; }
    // 0x43e4c0：摘链后释放回放槽信息 / 操作说明对象
    void destroy();

private:
    ReplaySummary replay_save_summary;

    void set_mode(Mode m);       // 0x43e150
    void set_state(State s);     // 0x43e200
    void run_state_machine();    // 0x43f980
    void resume_game();          // 0x43f6a0
    void close_overlay();        // 0x43f740 / 0x43f790
    void execute_selection();    // 0x43f980 case 0x10
    void continue_game();        // case 0x10 模式 2 光标 0
    bool register_rank();        // 0x43f7e0：返回 true 表示进入名字输入
    int score_block() const;     // 成绩文件机体块下标
    void enter_main_menu_after_rank();   // 0x43f980 LAB_004404ae
    void enter_replay_slots();   // case 10
    void begin_name_entry();     // case 0xb 确认 / 0x43f7e0 入榜：名字缓冲与光标初值
    void draw_name_entry(AsciiManager& text, Vec3 pos) const;   // 0x43e730
    void draw_replay_row(AsciiManager& text, Vec3 pos, int slot, const ReplaySummary& summary) const;   // 0x43e8c0
    void draw_replay_slots(AsciiManager& text) const;          // 0x43e9b0
    void draw_replay_name_entry(AsciiManager& text) const;     // 0x43eb50
    void draw_score_ranking(AsciiManager& text) const;         // 0x43ec10
    void capture_screenshot();   // 0x43ef20
    void cursor_interrupt(i32 base);            // 0x46f0b0(menu_vm, cursor + base)
    void child_interrupt(i32 script, i32 value);// 0x46f5a0 + 0x46f0b0
    void hide_menu();            // 0x46f340
    void show_menu();            // 0x46f300
    void close_help();
    bool nav(u32 bit) const;     // 0x4186f0
    void sfx(i32 id) { if (ctx.sfx_sink != nullptr) ctx.sfx_sink->push(id); else sfx_queue.push_back(id); }
    void suspend_sound() { if (ctx.sound != nullptr) ctx.sound->suspend(); }   // 0x42e4d0
    void resume_sound() { if (ctx.sound != nullptr) ctx.sound->resume(); }     // 0x440bd0
    void bgm(i32 kind, i32 track, const char* name) { if (ctx.music != nullptr) ctx.music->enqueue(kind, track, name); }
    // 0x45e330 排空：do … while (队头 kind != 0)——乐曲命令逐步推进，音效尾段每次消费
    void flush_sound() {
        for (int k = 0; k < 64; ++k) {
            if (ctx.music != nullptr) ctx.music->step();
            if (ctx.sound != nullptr) ctx.sound->process();
            if (ctx.music == nullptr || ctx.music->head_kind() == 0) break;
        }
    }
};

}  // namespace th16
