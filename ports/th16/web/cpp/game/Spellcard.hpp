// th16 Spellcard——原版 DAT_004a6db0（0xbc 字节；0x4178a0 创建，更新链 0x1e → 0x417930、绘制链 0xc → 0x417d70）。
//
//   0x417f00  开始（ECL 522 / 528 / 537–539）：计时归零、编号 / 名字、标志 (f & ~0x98) | 3、成绩文件挑战计数、
//             Gui 计时 VM 中断 2、ascii.anm 0 / text.anm 2（名字）/ ascii.anm 1 三个名牌 VM、音效 0x21、
//             effect.anm 0xd 倒计时 VM（entity_offset = Boss 坐标，子 VM 0xb / 0xc 的 $I2 = 时限）、
//             奖励 = 难度表 {50 万, 100 万, 150 万, 200 万, 100 万}[难度] × 关卡号（上限 999999999）、
//             ascii.anm 0x14、关卡表 Boss 块背景 VM（flag → kStageBackground）与切入 VM
//   0x417930  每帧：state++；timer > 0x3b 且非 kStageBackground → Stage flags &= ~1；timer > 299 且非耐久 →
//             bonus = ((bonus - (初值 - 初值/3) / (时限 - 300)) / 10) * 10；timer 推进；名牌随自机 y 上下翻（中断 3 / 2）；
//             倒计时 VM 跟随 Boss（0.05 插值）；kBombUsed 在 Bomb 结束后清除
//   0x4182f0  结束（ECL 523）：Stage flags |= 1、名牌 VM 中断 1、kActive 清、背景 VM 删、Gui 计时 VM 中断 3、
//             倒计时 VM 删；kBonusAvailable → 0x43e080 加分 + Gui 0x42bcf0(bonus, 0) + 成绩文件取得计数 + 音效 0x2e，
//             否则 Gui 0x42bcf0(0, 1)；kTimedOut → 音效 0x45
#pragma once
#include <functional>
#include <vector>

#include "AnmVm.hpp"
#include "Sound.hpp"
#include "Types.hpp"

namespace th16 {

class Gui;
class AsciiManager;
struct ScoreFile;
struct TextIntoTextureRequest;

class AnmManager;
class EnemyManager;
struct GameGlobals;
class Player;
struct StageEntry;
class StageManager;

struct Spellcard {
    // +0x78 标志位
    static constexpr u32 kActive = 0x1;             // 符卡进行中（敌机受击按 7 倍血量池折算，0x41a6d0）
    static constexpr u32 kBonusAvailable = 0x2;     // 尚可取得（被弹 / 超时清除）
    static constexpr u32 kNameLowered = 0x4;        // 0x417930：自机靠上时名牌移到下方（VM 中断 3 / 2）
    static constexpr u32 kSurvival = 0x8;           // op 542：耐久符卡（超时 = 取得，奖励不递减）
    static constexpr u32 kNoTimeout = 0x10;         // op 543
    static constexpr u32 kBombUsed = 0x20;          // 0x425010 / 0x443f10：符卡中用过炸弹（与 kActive 同置时伤害 /30，0x41c330）
    static constexpr u32 kTiming = 0x40;            // 0x417bc0：实时计时中
    static constexpr u32 kTimedOut = 0x80;          // 0x425010：计时中断已触发
    static constexpr u32 kFlipped = 0x100;          // op 567：画面上下翻转布局（名牌 / Gui 计时按 352 / 320 阈值翻转）
    static constexpr u32 kStageBackground = 0x200;  // 0x417f00：关卡表 Boss 块 +0x08 位（背景 VM 不遮关卡）


    u32 vm_ids[5]{};        // +0xc..+0x1c：关卡表背景 / ascii.anm 0 / text.anm 2 名字 / ascii.anm 1 / effect.anm 0xd 倒计时
    Timer timer;            // +0x20：符卡计时
    char name[0x41]{};      // +0x33
    i32 id = -1;            // +0x74：符卡编号（含难度偏移）
    u32 flags = 0;          // +0x78
    i32 bonus = 0;          // +0x7c：当前取得奖励（每帧递减）
    i32 bonus_initial = 0;  // +0x80
    i32 time_limit = 0;     // +0x84：op 537–539 arg 1（帧）
    i32 capture_count = 0;  // +0x88：0x417bc0 符卡练习计次
    i32 state = 0;          // +0x8c：开始后逐帧 +1
    i32 timed_frames = 0;   // +0x90：实时计时结束时抄下的 state（Gui 0x428e70 画 "秒.百分秒"）
    double timing_start = 0; // +0x94：开始 QPC 秒（kTiming 置位时）
    double timing_scratch = 0; // +0x9c：结束时差（编码前）
    i32 time_record = 0;    // +0xa4：0x417bc0 按墙钟算出的取得用时，带校验的编码（0x426840 解码；0 = 未计时）
    Vec3 boss_pos;          // +0xa8：倒计时 VM 跟随的 Boss 坐标
    // <0：无墙钟（oracle / harness 默认）→ 结束只抄 timed_frames，time_record 保持 0
    double wall_clock_seconds = -1;

    // 宿主绑定（GameSession）
    AnmManager* animations = nullptr;
    EnemyManager* enemies = nullptr;    // DAT_004a6dc0（Boss 槽 0 坐标）
    Player* player = nullptr;           // DAT_004a6ef8（名牌翻转按自机 y）
    GameGlobals* globals = nullptr;
    StageManager* stage = nullptr;      // DAT_004a6da0（+0x66c4 绘制位）
    const StageEntry* entry = nullptr;  // DAT_004a6f18
    std::vector<i32> sfx_queue;         // 0x45e150：开始 0x21、取得 0x2e、超时 0x45、extend 0x11
    SoundRequests* sfx_sink = nullptr;
    void play_sfx(i32 id, float x = 0.0f) { if (sfx_sink != nullptr) sfx_sink->push(id, x); else sfx_queue.push_back(id); }
    Gui* gui = nullptr;                 // DAT_004a6dcc：计时数字样式（0x4175d0 / 0x417650）、弹出通知（0x42bcf0）、extend 残机显示
    ScoreFile* scores = nullptr;        // DAT_004a6f0c：符卡挑战 / 取得记录
    bool replay_playback = false;       // DAT_004a6f08+0xc == 1（= GameManager+0xac）：回放中不写记录
    std::vector<TextIntoTextureRequest>* text_paints = nullptr;  // 0x46dab0 入队（GameSession::text_into_texture）
    // 0x417bc0 结束：记录局写本关段头 +0x240 + 计次 × 4；回放局从同槽读回 +0xa4
    std::function<void(i32 index, i32 value)> record_time;
    std::function<i32(i32 index)> replay_time;

    // 0x4178a0：memset 0 后 flags |= 2
    void reset();
    // 0x417790：+0x10 / +0x14 / +0x18 的 VM 标删（0x46f1c0）后释放；换关后由 0x4178a0 重建 → 这里接着 reset()
    void destroy();
    // 0x417f00；anm_index 为关卡表 Boss 块下标（切入 VM）
    void start(i32 spell_id, const char* spell_name, i32 limit, i32 anm_index);
    // 0x417930
    void update(float game_speed);
    // 0x417bc0：帧泵侧实时计时；wall_clock_seconds<0 时结束只写 timed_frames（oracle 口径）
    void tick_realtime();
    // 编码秒.百分秒 → +0xa4（与 GuiHud::decode_spell_time 互逆）
    static i32 encode_spell_time(double seconds);
    // 0x417530：+0xa4 校验位不符 → true（回放读回的用时被篡改时改记 0x6ad1584）
    static bool spell_time_invalid(i32 value);
    // 0x4182f0
    void end();
    // 0x417d70（绘制链 0xc）：符卡进行中在名牌 VM（ascii.anm 1）旁入队奖励分（或已失去奖励的 "$"）与取得 / 挑战数
    void draw_text(AsciiManager& text) const;

private:
    Vec3 boss_position() const;  // 0x417f00 / 0x417930 内联：EnemyManager+0x48 槽 0 Boss 坐标，无则取 0
    int record_shot() const;     // 成绩文件机体块：DAT_004a57a4 + DAT_004a57a8
    bool is_spell_practice() const;  // (DAT_004a5bec & 0x30) == 0x20
};

}  // namespace th16
