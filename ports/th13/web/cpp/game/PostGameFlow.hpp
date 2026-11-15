// th13 对局内覆盖层（DAT_004c22c0 / 0x43dbc0 分发）骨架移植——S3 第一阶段。
//
// 一个覆盖层、三个分支状态机（docs/plans/results-overlay-anatomy.md §0/§1）：
//   state 0      对局中监视（ESC/失焦 → 结算A 入口）
//   1..0xc       结算A（0x43e780：暂停菜单 + 练习关终结算共体）
//   0xd..0x15    GameOver/续关（0x43fa90）
//   0x16..0x1f   结算B（0x440960：Extra/符卡/演示终局）
// 公共语义：入场置 GameInf+0x60 的 0x10 位（冻结对局，0x42c550:263 三旗门）；
// 移植侧 = SessionState.scene_flags 0x10（RunPause 同位同源）。年龄计数
// param+0x14 = 每帧 +1。
//
// 本阶段落地主干（anatomy §4 过关触发链）：监视态 → Boss 后对话 entry1 尾
// opcode 21 → 0x42cd40 关终了 → 每关百万奖（0x42abf0）+ dcc24=0xc 无条件
// 换关请求（stage<7 则关号+1）。六面通关演出（0x4000 路径 180f/379f）与
// Extra/练习/全灭入口建骨架；续关数值、名前录入、回放保存、结算菜单交互、
// fade/HUD 通关演出（stagectx+0x110 / +0x180|=0x100）留 TODO（下一阶段
// 随结算窗 oracle 一起校准）。
#pragma once

#include <string>

#include "SessionState.hpp"
#include "Types.hpp"

namespace th13 {

// 31 态全表（anatomy §1 状态机全表；案值 = 原版覆盖层 state(+4)）。
enum class OverlayState : u8 {
    Monitor = 0,             // 对局中监视
    PauseEntry = 1,          // 结算A：暂停入场（ESC 来）
    PracticeEntry = 2,       // 结算A：练习关终入场
    PauseMenu = 3,           // 结算A：主菜单（5 项）
    QuitConfirm = 4,         // 结算A：退出口确认
    ReplaySaveConfirm = 5,   // 结算A：回放保存确认（7 同族）
    ReplaySaveMenu = 6,      // 结算A：回放保存菜单
    ReplaySlots = 8,         // 结算A：回放选槽（th13_%.2d.rpy 25 槽）
    ReplayConfirm = 9,       // 结算A：选槽确认+排名门
    NameEntry = 10,          // 结算A：名前录入（8 字）
    Blank = 11,              // 空（break）
    MessageWait = 12,        // 结算A：消息等待
    GameOverEntry = 13,      // GameOver：入场
    GameOverShow = 14,       // GameOver：GAME OVER 展示（0x43d650 落分）
    ContinueMenu = 15,       // GameOver：续关菜单
    GoReplaySave = 16,       // （预留：GameOver 族回放保存入口 0x11 前置）
    GoNameEntry = 17,        // GameOver：名前录入（进榜）
    GoPostName = 18,         // GameOver：录入后处理
    GoNoRankWait = 19,       // GameOver：无榜等待
    GoMessageWait = 20,      // GameOver：消息等待
    ContinueExec = 21,       // GameOver：续关决策执行（0x15）
    BEntry = 22,             // 结算B：入场（0x16）
    BShow = 23,              // 结算B：展示（0x17）
    BGameOver = 24,          // 结算B：GameOver 变体（0x18）
    BMenu = 25,              // 结算B：主菜单（0x19）
    BReplayConfirm = 26,     // 结算B：回放保存确认（0x1a）
    BNameEntry = 27,         // 结算B：名前录入（0x1b）
    BNoRank = 28,            // 结算B：无榜路径（0x1c）
    BExitExec = 29,          // 结算B：退出执行（0x1d）
    BSpellVariant = 30,      // 结算B：符卡练习变体（0x1e）
    BClear = 31,             // 结算B：Extra/符卡通关（0x1f）
};

// 覆盖层平台请求（Application 层接线；raw 案值 = DAT_004dcc24 真值，
// 见 results-flow.md §1：2 挂起/3 退出/4 标题/7 常规开局/0xa 重试/0xb 练习
// 重试/0xc 下一关/0xd 练习开局/0xe 重开/0xf 结局/0x10 战后标题）。
struct PostGamePlatform {
    virtual ~PostGamePlatform() = default;
    virtual void request_destination(i32 raw) = 0;
    virtual bool practice_end() { return true; }
    virtual bool request_practice_end() { return practice_end(); }
    // 42c550 frame 180: fullscreen black fade, duration 200.
    virtual bool request_stage6_fade() { return false; }
    virtual bool request_extra_results() { return false; }
    // GameOver/结算B 的 BGM「th128_08」（0x43f570:58）与音效族；id 先以
    // 0x15030(菜单音)/7(暂停 BGM 位) 语义透传，音频接线属 TODO。
    virtual void bgm(i32 id) = 0;
};

class PostGameFlow {
public:
    PostGameFlow(SessionState& progress, HudPlayerState& player, HudScoreState& score,
                 PostGamePlatform& host)
        : progress_(progress), player_(player), score_(score), host_(host) {}

    OverlayState state() const { return state_; }
    u32 age() const { return age_; }
    bool frozen() const { return (progress_.scene_flags & 0x10u) != 0; }

    // 0x42cd40 关终了（双入口：对话 opcode 21 的 case 0x15、ECL 事件 -2 的
    // 0x428820:106-113 无死亡分支）。
    void on_stage_clear();
    // 0x43f570 全灭入口（mode0 → 态 0xe 族；mode≠0 → 结算B）。
    void on_game_over();
    // 0x43e5a0 练习关终入口（结算A）——触发平台练习结算覆盖层。
    bool on_practice_end();
    // 0x43f720 Extra/符卡终局入口（结算B）——同上骨架。
    void on_extra_end();

    std::string error;

    // 每帧推进（覆盖层激活与六面通关演出；0x43dbc0 分发的行为子集）。
    void update();

    // 六面通关演出进行中（GameInf 0x4000；对局侧 fade/计时由本类驱动）。
    bool clear_sequence() const { return clear_sequence_; }

private:
    void enter(OverlayState next);
    void apply_stage_clear_bonus();  // 0x42abf0

    SessionState& progress_;
    HudPlayerState& player_;
    HudScoreState& score_;
    PostGamePlatform& host_;
    OverlayState state_ = OverlayState::Monitor;
    u32 age_ = 0;
    bool clear_sequence_ = false;  // GameInf+0x60 0x4000（六面通关演出）
    u32 clear_frame_ = 0;          // GameInf+0x78 计数（0xb4 淡出/0x17b 出口）
};

}  // namespace th13
