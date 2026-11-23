// th16 Gui——原版 DAT_004a6dcc（0x2e4 字节；0x427ca0 创建 → 0x4268c0 / 0x426b00 / 0x426c10；更新链 0x20 → 0x427cf0；
// 绘制链 0x33 / 0x30）。两部分：
//   · 对话（Msg）与其带出的 VM、ECL 518 / 519 / 554 入口——Gui.cpp
//   · HUD：残机 / 炸弹格、季节量表、Boss 血条 / 计时数字 / 方位标、符卡星、弹出通知——GuiHud.cpp
//     （0x426d70 建 VM；0x42c280 / 0x42c390 / 0x42c600 刷新；0x42bcf0 弹出；0x427cf0 每帧）
// HUD 的文字（得分 / 擦弹 / 得点值等，绘制链 0x30 → 0x428e70）经 AsciiManager 画出，不在此类。
//
//   0x426c10  装 logo ANM（关卡表 +0x24 → 槽 6）；按机体读 msg 文件（关卡表 +0x14[shot + shot_aux]）到 +0x1cc；
//             +0x154 计时复位；+0x170 = DAT_004a57b0；+0x1d0 / +0x1d8 = -1（过关换关时 0x42cb60 再次调用，Gui 对象本身不重建）
//   0x427970  换关：删 Msg、卸 logo 槽 6、释放 msg 文件（+0x1ac 标志与过关奖励计时保留，下一关首帧 0x42c1b0 才清）
//   0x42bc10  每帧（GameManager 0x42d7b0 调用）：+0x170 显示分向 DAT_004a57b0 逼近，+0x174 为步长；超过最高分时
//             DAT_004a59b8 / DAT_004a59bc 更新并置 DAT_004a5bec bit2
//   0x429ff0  对话开始（ECL 518）：entry ≥ 0 → 删旧 Msg，new Msg(0x1c8) 以 msg[4 + entry×8] 为脚本起点；
//             -1 / -3 为结局入口、-2 为过关（0x42e150）/ 续关（0x43f350）——后续阶段接入
//   0x427cf0  每帧（Msg 部分）：0x42a1d0 返回 0 → Msg+0x4 年龄计时推进；非 0（op 0 结束）→ 0x4264a0 删 VM、释放
//   0x42c240  ECL 554：logo ANM 脚本 0（Boss 登场标题；练习 / 符卡练习模式不建）
//   0x42c480  Boss 名牌 +0xd8：spell_id < 0x29 取关卡表块 1 序号，否则块 0；front.anm 脚本 序号 + 0xa4
//
// Msg（0x429b20 构造 / 0x42a1d0 解释 / 0x4264a0 析构）字段见 Msg 结构体；指令集按 0x42a1d0 的 switch 编号，
// 文字经 0x42bbe0 解密（key 0x77，步长 7 起每字节 +0x10）。文字贴图（0x46d990 AsciiManager 画进 VM 纹理）
// 不改变逻辑状态，这里把解密后的文本与颜色记入 text_requests 供宿主渲染。
#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "AnmRenderer.hpp"
#include "AnmVm.hpp"
#include "Music.hpp"
#include "Sound.hpp"
#include "TextPaint.hpp"
#include "Types.hpp"

namespace th16 {

class AnmManager;
struct AnmGeometry;
class BulletManager;
class EnemyManager;
class LaserManager;
struct GameGlobals;
struct InputState;
class Player;
struct ScoreFile;
struct PlayerWorld;
struct StageEntry;
struct AnmFile;
struct Spellcard;
class AsciiManager;

// 对话脚本（msg 文件）：entry 表 {u32 count; {u32 offset; u32 pad}[count]}，指令 {u16 time; u8 op; u8 size; u8 data[size]}
struct MsgInstruction {
    u16 time;
    u8 opcode;
    u8 size;
    u8 data[1];
};

struct Msg {
    static constexpr u32 kSkippable = 0x1;      // +0x190 bit0：op 10——本段可被长按射击快进
    static constexpr u32 kTextShadow = 0x2;     // bit1：op 26 置、op 7/8/9/32 清——文字 VM 用第二套样式
    static constexpr u32 kStyleShift = 2;       // bit2..5：op 29 文字框样式（front.anm 0xe4 + side + style×2）
    static constexpr u32 kStyleMask = 0x3c;
    static constexpr u32 kShotPressed = 0x40;   // bit6：按过射击 / 跳过键（与 kSkippable 同置才允许快进）

    i32 entry = 0;              // +0x00
    Timer age;                  // +0x04：Gui 每帧推进（0x427cf0）
    Timer timer;                // +0x18：脚本时刻（op 11 等待期间不推进）
    Timer wait;                 // +0x2c：op 11 倒计时
    u32 portrait_vm = 0;        // +0x40：自机 / 对方立绘（op 1）
    u32 face_vms[4]{};          // +0x44..+0x50：Boss 立绘（op 2 / 31，关卡表 Boss 块）
    u32 frame_vm = 0;           // +0x54（th16 无创建路径，析构 / 中断照旧处理）
    u32 text_vms[4]{};          // +0x58 / +0x5c：text.anm 0 两行；+0x60 / +0x64：text.anm 1 两行（右侧 / 阴影）
    u32 name_vm = 0;            // +0x68：Boss 名牌（op 20）
    u32 box_vm = 0;             // +0x6c：文字框（front.anm 0xe4 + 下标，0x42ba30）
    const u8* pc = nullptr;     // +0x158
    Vec3 anchors[4] = {{16, 16, 0}, {16, 16, 0}, {16, 16, 0}, {16, 16, 0}};  // +0x15c..+0x188：op 9 文字 VM 基点
    i32 ecl_release = 0;        // +0x18c：op 12 置 1、每帧 -1；ECL 519 在其为 0 时等待
    u32 flags = 0;              // +0x190
    i32 lines_written = 0;      // +0x194：本段已写入的文字行数
    i32 lines_cleared = 0;      // +0x198：本段已清空文字 VM
    i32 side = 0;               // +0x19c：0 = 左（自机）/ 1 = 右（Boss）
    u32 colors[4]{};            // +0x1a0..+0x1ac：各侧文字颜色（0 = 默认白）
    Vec3 text_pos{384, 640, 0}; // +0x1b0：op 28（参数 ×2）
    float text_width = 320;     // +0x1bc：文字框宽（按最长行 max((len×8 & ~0xf) − 0x1c) × 2）
    i32 unknown_1c0 = 0;        // +0x1c0
    i32 box_index = 0;          // +0x1c4：当前文字框脚本下标（0x42ba30 存，跟随子 VM 0xb4 + 下标）

    // 宿主渲染用：0x46d990 把文字画进 VM 纹理的请求（亦入队 GameSession::text_into_texture）
    struct TextRequest { u32 vm_id; std::string text; u32 color; u32 shadow_color; i32 font; i32 x; i32 y; };
    std::vector<TextRequest> text_requests;

    // 0x42aff8（对话指令 14）：按参数直接取 +0x44 起的第 index 个 VM 槽，不做掩码——index 4 起依次落到 frame_vm、
    // text_vms、name_vm、box_vm（第 5 面对话用了 index 8，即 text_vms[3]）。负下标与 11 以外在本作脚本里不出现，夹到端点槽
    u32& slot_vm(int index) noexcept {
        if (index <= 0) return face_vms[0];
        if (index < 4) return face_vms[index];
        if (index == 4) return frame_vm;
        if (index < 9) return text_vms[index - 5];
        if (index == 9) return name_vm;
        return box_vm;
    }
    const MsgInstruction* ins() const noexcept { return reinterpret_cast<const MsgInstruction*>(pc); }
    i32 iarg(int i) const noexcept;
    float farg(int i) const noexcept;
};

class Gui final : public AnmDrawHooks {
public:
    static constexpr int kLogoSlot = 6;  // 0x426c10：关卡 logo ANM 装入槽 6（oracle banks[6] = stNNlogo.anm）

    AnmFile* front = nullptr;        // +0x2d8：front.anm（槽 5）
    AnmFile* logo = nullptr;         // +0x178：关卡 logo ANM（槽 6）
    std::vector<u8> msg_data;        // +0x1cc：对话脚本文件
    std::unique_ptr<Msg> dialogue;   // +0x1c8
    u32 boss_name_vm = 0;            // +0xd8（0x42c480）
    u32 vm_110 = 0;                  // +0x110：op 35（0x426780）front.anm 0x3c
    u32 boss_title_vm = 0;           // 0x42c240：logo 脚本 0（原版不保存 id）
    u32 clear_bonus_vm = 0;          // +0x120：0x42c070 过关奖励 VM（front.anm 0x78）
    i32 clear_bonus = 0;             // +0x2dc：过关奖励分（关卡 × 1,000,000，0x42e150 再加残机 / 碎片项）
    u32 flags_1ac = 0;               // +0x1ac（bit4 过关结算中、bit8 过关奖励显示中）
    Timer timer_154;                 // +0x154
    Timer clear_timer;               // +0x1b0：过关奖励显示计时（0x42c070 复位；0x42d7b0 < 0x78 时暂停游戏推进）
    i32 displayed_score = 0;         // +0x170：HUD 显示分（0x42bc10 逐帧逼近 DAT_004a57b0）
    i32 score_step = 0;              // +0x174
    i32 boss_timer_display = -1;     // +0x1d0：Boss 剩余秒数（0x425010 每帧写；-1 = 无）
    i32 boss_timer_fraction = 0;     // +0x1d4：百分秒
    i32 boss_timer_shown = -1;       // +0x1d8：上一帧显示的秒数（变化时换数字、≤4 / ≤1 秒警告音）
    const u32* game_flags = nullptr; // GameManager+0x88（bit16 关卡切换中不显示计时）

    // ---- HUD（0x426d70 建；UI 链表常驻，跨关保留；新开局才建面板 / 难度牌）----
    static constexpr int kResourceSlots = 8;     // 残机 / 炸弹各 8 格
    static constexpr int kSpellStarSlots = 10;   // Boss 剩余符卡星（ECL 540 给数量）
    static constexpr int kBossBarSlots = 2;      // EnemyManager Boss 槽 0 / 1 各一套血条
    static constexpr int kBossBarMarkers = 4;    // 血条上的阶段刻度（ECL 527）
    static constexpr int kBossBarVms = 7;        // 环 0xf4、底 0xf5、面 0xf6、刻度 0xf7 ×4
    static constexpr int kNoticeDigits = 8;      // 符卡奖励弹出的 8 位数字

    // front.anm 脚本号（0x426d70 / 0x427cf0 / 0x42bcf0 直接用常量；集中在此便于对照 front.anm）
    enum FrontScript : i32 {
        kHudFrame = 0x00,            // +0x150 底板（UI 链表）
        kLifeSlot0 = 0x1e,           // +0xc[i]：残机格 0x1e..0x25
        kBombSlot0 = 0x26,           // +0x2c[i]：炸弹格 0x26..0x2d
        kNoticeSpellBonus = 0x3d,    // +0xc8：符卡奖励（数字由 ascii.anm 4..0xb 组成）
        kNoticeSpellFailed = 0x3e,
        kNoticeFullPower = 0x3f,     // +0xcc
        kNoticeKind3 = 0x40,
        kNoticeExtend = 0x41,
        kNoticeKind6 = 0x42,         // +0xc8
        kStageOneHint = 0x45,        // 第 1 面首次开局的操作提示（随机体偏移）
        kSpellStar0 = 0x46,          // +0xdc[i]：0x46..0x4f
        kDifficultyLabel0 = 0x51,    // +0x104：难度名牌 0x51 + 难度
        kDifficultyBadge0 = 0x57,    // +0x108：难度徽记 0x57 + 难度
        kNoticeLabel = 0x60,         // +0x128：弹出通知的文字底（0x428e70 在其上画文字）
        kBossIndicator = 0x70,       // +0x9c：场地下沿的 Boss 方位标
        kHudPanel = 0x71,            // +0x10c：右侧面板（子 VM：0x73 量表条、0x74 量表等级、0x75 季节图、0x76 炸弹可用标）
        kGaugeBar = 0x73,
        kGaugeLevel = 0x74,
        kSeasonIcon = 0x75,          // 精灵 0x52 + 子季节
        kBombReadyMark = 0x76,
        kDemoBanner = 0x77,          // flags bit6（演示回放）
        kBossRing = 0xf4,            // Boss 血条：环（rotation.x = -2π × 显示比例）
        kBossRingBase = 0xf5,
        kBossRingFace = 0xf6,
        kBossRingMarker = 0xf7,
    };

    u32 hud_frame_vm = 0;                        // +0x150
    u32 life_vm_ids[kResourceSlots]{};           // +0xc..+0x28
    AnmVm* life_vms[kResourceSlots]{};           // +0x54..+0x70（0x42c280 直接按指针操作）
    u32 bomb_vm_ids[kResourceSlots]{};           // +0x2c..+0x48
    AnmVm* bomb_vms[kResourceSlots]{};           // +0x74..+0x90
    u32 boss_timer_vm_ids[2]{};                  // +0x4c / +0x50：ascii.anm 2 / 3——十位 / 个位
    AnmVm* boss_timer_vms[2]{};                  // +0x94 / +0x98
    u32 boss_indicator_vm = 0;                   // +0x9c
    u32 notice_digit_vms[kNoticeDigits]{};       // +0xa0..+0xbc：ascii.anm 4 + i
    u32 notice_comma_vms[2]{};                   // +0xc0 / +0xc4：ascii.anm 0xc / 0xd（百万位 / 千位逗号）
    u32 notice_vm = 0;                           // +0xc8
    u32 notice_small_vm = 0;                     // +0xcc
    u32 spell_star_vms[kSpellStarSlots]{};       // +0xdc..+0x100
    u32 difficulty_label_vm = 0;                 // +0x104
    u32 difficulty_badge_vm = 0;                 // +0x108
    u32 panel_vm = 0;                            // +0x10c
    i32 gauge_level_shown = 0;                   // +0x114：量表等级 VM 当前是否展开（0x42c600 的中断 2 / 3 切换）
    i32 bomb_ready_shown = 0;                    // +0x118：炸弹可用标当前是否点亮（中断 2 / 3）
    u32 notice_label_vm = 0;                     // +0x128
    i32 notice_label_active = 0;                 // +0x14c：0x428e70 在 notice_label_vm 上画奖励文字
    i32 spell_star_count = 0;                    // +0x188（ECL 540）

    // Boss 血条一套（+0x1dc 起、每槽 0x54 字节）
    struct BossBar {
        float shown = 0;                         // +0x1dc：显示比例，每帧向 target 逼近 0.025
        float target = 0;                        // +0x1e0 = life / life_max
        i32 life = 0;                            // +0x1e4
        float markers[kBossBarMarkers]{};        // +0x1ec / +0x1f4 / +0x1fc / +0x204：阶段刻度比例（ECL 527）
        i32 marker_flags[kBossBarMarkers]{};     // +0x1f0 / …：ECL 527 arg2（原版无读取方）
        u32 vm_ids[kBossBarVms]{};               // +0x20c..+0x224
        i32 created = 0;                         // +0x228
        i32 near_player = 0;                     // +0x22c：自机贴近 Boss 时整套中断 3 淡出、离开中断 2 恢复
    };
    BossBar boss_bars[kBossBarSlots];

    // 宿主绑定
    AnmManager* animations = nullptr;
    const AnmGeometry* geometry = nullptr;   // 0x406a70 / 0x406c40（文字框子 VM 的屏幕位置）
    EnemyManager* enemies = nullptr;         // DAT_004a6dc0（ANM 表 / 全灭 / Boss 槽）
    BulletManager* bullets = nullptr;        // DAT_004a6dac（0x416f40 全屏消弹）
    LaserManager* lasers = nullptr;          // DAT_004a6ee0（0x431af0 对话开始时激光全灭）
    Player* player = nullptr;                // DAT_004a6ef8（立绘 ANM 文件）
    PlayerWorld* world = nullptr;            // dialogue 标志的宿主镜像（DAT_004a6dcc->0x1c8 != 0）
    GameGlobals* globals = nullptr;
    const InputState* input = nullptr;       // DAT_004a52c8 / DAT_004a52d4 / DAT_004a5244[]
    const StageEntry* entry = nullptr;       // DAT_004a6f18
    bool replay_playback = false;            // GameManager+0xac != 0
    const u32* spellcard_flags = nullptr;    // DAT_004a6db0->0x78（0x429ff0(-2) 的续关判定 bit7）
    const Spellcard* spellcard = nullptr;    // DAT_004a6db0（0x428e70 的符卡用时文字）
    class Bomb* bomb = nullptr;              // DAT_004a6da8（0x42e150 过关时仍在发动 → 收尾 vtable+0x14）
    class Bomb* season_release = nullptr;    // DAT_004a6da4（0x427cf0：解放可发动 ↔ 面板炸弹可用标）
    std::vector<i32> sfx_queue;              // 0x45e150：op 11 按键推进 0
    SoundRequests* sfx_sink = nullptr;
    void play_sfx(i32 id, float x = 0.0f) { if (sfx_sink != nullptr) sfx_sink->push(id, x); else sfx_queue.push_back(id); }
    std::vector<TextIntoTextureRequest>* text_paints = nullptr;  // 0x46d990 入队
    MusicManager* music = nullptr;           // 0x43c3f0 切曲（ECL 518 -1 / -3、op 19）/ 0x43c470 淡出（op 22 / 27）
    std::string error;

    // 0x427ca0 构造 + 0x426c10：进游戏时一次性绑定 front.anm 并做关卡初始化
    bool init(AnmManager& anm, AnmFile* front_anm, AnmFile* logo_anm, const StageEntry& stage, std::vector<u8> msg_bytes);
    // 0x426c10（msg 部分）：logo ANM 由 GameSession 装入槽 6 后传入；+0x154 计时复位、显示分 = 当前分、Boss 计时 -1
    bool stage_init(AnmFile* logo_anm, const StageEntry& stage, std::vector<u8> msg_bytes);
    // 0x427970：换关——删 Msg、释放 msg 文件（logo 槽 6 由 GameSession 卸载）
    void end_stage();
    // 0x42c1b0：下一关首帧——过关奖励 VM 中断 1、清 bit8、过关计时复位
    void clear_stage_bonus();
    // 0x42d7b0 的过关等待：bit8 置且 +0x1b4 < 0x78 时 GameManager 整帧不推进
    bool clear_bonus_pending() const noexcept { return (flags_1ac & 0x100u) != 0 && clear_timer.current < 0x78; }
    // 0x42bc10
    void roll_score();
    // 0x429ff0：entry ≥ 0 开始对话；-1 / -3 切曲 + logo 标题；-2 过关（符卡 bit7 续关流程未接入时记错误）
    bool start_dialogue(i32 entry_index);
    // ECL 519（0x41dcb0 case 0x40）：对话存在且 ecl_release == 0 时需等待
    bool ecl_waits() const noexcept { return dialogue != nullptr && dialogue->ecl_release == 0; }
    // 0x42c240：ECL 554 Boss 登场标题
    void show_boss_title();
    // 0x42c480
    void ensure_boss_name();
    // 0x42e150：过关（MSG op 21 / 0x429ff0(-2)）——普通模式 1..5 面：奖励、子机收拢、场景请求 0xc 并推进关卡号
    void stage_clear();
    // 0x427cf0（Msg 部分）
    void update(float game_speed);
    // 0x427a20 / 0x4264a0：离开游戏时删掉全部对话 VM
    void shutdown();
    // 0x43f350（对话 -2 且符卡标志 bit7）/ 0x43f500（练习 / 符卡练习过关）：GameMenu 的续关 / 结算入口，GameSession 绑定
    std::function<void()> on_continue_screen;
    std::function<void()> on_practice_result;
    std::function<void()> on_return_to_title;    // 0x43f240：回放中过关 / 第 6 面 / Extra 通关
    std::function<void()> on_final_clear;        // 0x42e150 第 6 面 / Extra：GameManager flags |= 0x4000、+0xb0 = 0、0x42dbc0
    std::function<void()> on_hud_init;           // 0x426d70 首行：Gui 的更新链与两条绘制链 |= 2（GameSession 在此启用绘制链）
    ScoreFile* scores = nullptr;                 // DAT_004a6f0c：过关记录（通关次数 / 练习解锁位）
    // 0x42b610 / 0x42b820：暂停 / 续关菜单打开与关闭时隐藏 / 显示对话的全部 VM（flags_lo bit1 及其子树）
    void hide_dialogue();
    void show_dialogue();
    void update_boss_timer();        // 0x427cf0 中段：Boss 剩余秒数显示 / 警告音 0xb / 0xc
    enum class TimerDigitMove { Aside, Back, Reset };
    void move_timer_digit(AnmVm* vm, TimerDigitMove move);
    // 0x42c580 / 0x42c5c0：暂停 / 续关菜单期间隐藏 / 恢复 HUD 根 VM（+0x12c；th16 无创建路径，恒为空操作）
    void hide_hud();
    void show_hud();

    // ---- HUD（GuiHud.cpp）----
    // 0x426d70：关卡首帧（0x42dc50 / 0x42dee0）启用 Gui 三条链并建 HUD VM；残机 / 炸弹格与计时数字只在第一次建，
    // 面板 / 难度名牌 / 第 1 面操作提示 / 演示横幅按条件建；最后刷新残机、炸弹、季节量表显示
    void init_hud(AnmFile* ascii);
    // 0x42c280：前 lives 格中断 2（亮）、第 lives 格按碎片比例中断 7 + 碎片×5 / 阈值、其余中断 3（暗）；lives ≥ 8 时无碎片格
    void refresh_lives_display(i32 lives, i32 fragments);
    // 0x42c390：同上，碎片格中断 7 + fragments
    void refresh_bombs_display(i32 bombs, i32 fragments);
    // 0x42c600：量表条长 = 本级进度 × 100；等级 VM 0 级隐藏、≥1 级中断 7 + 级；等级有无变化时量表条中断 2 / 3
    void refresh_season_gauge();
    // 0x42bcf0：场地中央弹出通知
    enum class Notice : i32 {
        SpellBonus = 0,      // 符卡取得：front 0x3d + ascii 数字（value = 奖励分）+ 奖励文字底 0x60
        SpellFailed = 1,     // front 0x3e + 文字底 0x60
        FullPower = 2,       // front 0x3f
        Kind3 = 3,           // front 0x40（th16 无调用方）
        Extend = 4,          // front 0x41
        None = 5,            // 原版跳转表指向函数尾，无操作
        Kind6 = 6,           // front 0x42（th16 无调用方）
    };
    void show_notice(Notice kind, i32 value);
    // 0x43e080 每次 extend 的 Gui 侧：残机显示刷新、音效 0x11、extend 弹出
    void celebrate_extend();
    // 0x42dc50 换关首帧：正在显示的弹出通知中断 1
    void dismiss_notice();
    // ECL 527：Boss 槽 slot 血条第 index 个刻度 = ratio（0x41dcb0 case 0x48）
    void set_boss_bar_marker(i32 slot, i32 index, float ratio, i32 flag);
    // 0x4175d0（符卡开始：中断 2）/ 0x417650（符卡结束：中断 3）：两位计时数字立即切样式
    enum class TimerStyle : i32 { Spell = 2, Normal = 3 };
    void set_boss_timer_style(TimerStyle style);
    // 0x428e70（绘制链 0x30）：把 HUD 文字入队到 AsciiManager——过关奖励数、符卡用时、最高分 / 得分（末位续关数）、
    // 下一 extend 分、炸弹碎片 n/5、火力 x.xx/x.00、得点值（舍到十位）、擦弹数、Boss 计时的百分秒、场地底部季节等级
    void draw_hud_text(AsciiManager& text);

private:
    // 0x427cf0 的 HUD 段，按原版顺序由 update 调用
    void update_notice_label();        // +0x14c / +0x128：文字底 VM 消失后停止画字
    void slide_hud_for_player();       // 自机在面板附近（y > 400 且 x < -64）时面板中断 5 让位、离开中断 4
    void update_boss_bars();           // Boss 槽 0 / 1 血条：比例逼近、刻度朝向、贴近淡出；Boss 消失后收回
    void retire_boss_bar(BossBar& bar);
    void update_spell_stars();         // spell_star_count 个星星 VM（front 0x46 + i），多余的中断 1
    void update_boss_indicator();      // 场地下沿 Boss 方位标：x 跟随、靠近自机变淡、按血量阶段中断 7..10
    void update_bomb_ready_mark();     // Bomb 0x40dda0 可发动 ↔ 面板子 VM 0x76 中断 2 / 3
    AnmVm* panel_child(i32 script);    // find_child(panel_vm, script, 0)，面板不在则清 id
    u32 spawn_world(AnmFile& file, i32 script);     // 0x406380(file, script, -1)
    u32 spawn_hidden_world(AnmFile& file, i32 script, AnmVm** out_vm);  // 0x426d70 计时数字：建后整树隐藏
    // 0x429b20
    void create_dialogue(i32 entry_index, const u8* script);
    // 0x4264a0
    void destroy_dialogue();
    // 0x42a1d0：返回 true 表示脚本结束（op 0）
    bool run_dialogue(float game_speed);
    void execute(Msg& m, const MsgInstruction& ins, float game_speed);
    // 0x42ba30 / 0x42bb30
    void create_text_box(Msg& m, i32 index);
    void resize_text_box(Msg& m, i32 index);
    // 0x46f3e0：按 id 设 entity_offset（找不到则清 id）
    void set_offset(u32& id, const Vec3& pos);
    // 0x42b480（绘制钩子 5）：文字 VM 绘制前按文字框子 VM（0xb4 + 下标）的当前屏幕位置重定 entity_offset；
    // 与更新尾段同式，但右侧展开中（scale.x < 1）不加 0.125
    void dialogue_text_follow(AnmVm& vm) override;
    // 0x46f2e0：找 VM，找不到则清 id
    AnmVm* find_or_clear(u32& id);
    // 0x46d990 请求
    void draw_text(Msg& m, u32& id, const char* text, u32 color, u32 shadow, i32 font, i32 x, i32 y);
    void clear_text_vms(Msg& m);
    void kill_vm(u32& id);
    void sync_world() noexcept;
    AnmFile* enemy_anm(i32 index) const;
    static std::string decrypt_text(const u8* src);  // 0x42bbe0
};

}  // namespace th16
