// th16 GameManager——原版 DAT_004a6dd4（0xb4 字节，0x42d700 创建；加载线程 0x42cb60；更新 0x42d7b0）。
//
// 锚点：
//   0x42d700  new 0xb4、flags |= 4（装载中）、+0xac = 模式参数、起线程 0x42d1f0 → 0x42cb60
//   0x42cb60  装载：等启动画面；新游戏时先建 Player 0x441c60；按 DAT_004a6f18（关卡表）建 Replay 0x447e20 /
//             Stage 0x409db0 / Gui 0x427ca0 / Bullet 0x411dd0 / Item 0x42f440 / Laser 0x4314a0 / 0x43e5a0 / 0x449e50 /
//             Enemy(ECL) 0x41b340 / Bomb 0x40d890 / Spellcard 0x4178a0；BGM 0x43c370；注册更新链 0xf（0x42d7b0）与绘制链 2；清 flags&4。
//             换关（DAT_004a5bec bit1，DAT_004c17d0 == 0）：Player / Bullet / Item / Gui 沿用；0x449030 把 game_rng 种子设为
//             visual_rng 种子、0x426c10 Gui 关卡初始化、0x409db0 新 Stage（上一关转入 DAT_004a6d9c 继续运行）、0x41b340 新 ECL、
//             0x4178a0 新 Spellcard
//   0x42d7b0  更新：bit14（过关结算）计数；Gui bit8 且过关计时 < 0x78 → 整帧跳过；+0x10（帧计时）== 0 → 0x42dc50 首帧；
//             == 0x1e → 0x42dee0；上一关 bit3 → 0x4099a0 析构；0x42bc10 显示分；+0x88 位：4 装载中 / 8 装载失败 /
//             0x10|0x20|0x40 结束 / 0x800 换关待初始化 / 0x4000 过关 / 0x10000 关卡切换；末尾 DAT_004a579c++、DAT_004a57a0++、
//             DAT_004c0f40 递减、+0xc 计时推进
//   0x42dc50  首帧：0x42c1b0 清过关奖励；装载失败 → 场景 2；0x40add0 建面 VM；有上一关 → 0x40c0d0（ScreenEffect kind 2、
//             上一关 bit1 0x1e 帧）+ 0x40c210（新关 bit2 0x3c 帧）+ flags |= 0x800 + Gui+0xc8 中断 1 后返回；
//             否则直接做 0x42dee0 的初始化段
//   0x42dee0  flags bit11 → 清位、Bullet 0x411b70 / Player 0x441740 / Item 0x4184a0 / Enemy 0x4185b0 / Laser 0x42cb00 复位、
//             帧计数清零、Replay 0x448eb0、生成 "main"、HUD 0x426d70、启用各管理器链、0x4440e0 子机、BGM、
//             "Loading" VM 中断、+0xc 计时复位
//
// Gui 接入对话（Msg）、过关结算、显示分与 HUD（残机 / 炸弹格、季节量表、Boss 血条 / 计时 / 方位标、弹出通知）。
#pragma once
#include "AnmVm.hpp"
#include "Bomb.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
#include "Gui.hpp"
#include "Input.hpp"
#include "Music.hpp"
#include "Item.hpp"
#include "Laser.hpp"
#include "Player.hpp"
#include "ScorePopup.hpp"
#include "Stage.hpp"
#include "StageTable.hpp"
#include "Types.hpp"
#include <functional>
#include <memory>

namespace th16 {

class AnmManager;
struct GameGlobals;

class GameManager {
public:
    enum : u32 {
        kLoading = 0x4,
        kLoadFailed = 0x8,
        kMenuOpen = 0x10,            // 暂停 / 续关菜单打开（0x43f0f0 / 0x43f350 置；0x42d7b0 据此返回 3 截断其后更新链）
        kEndMask = 0x70,
        kFirstUpdate = 0x80,
        kRetry = 0x800,
        kStageClear = 0x4000,
        kStageTransition = 0x10000,
    };

    u32 flags = kLoading;        // +0x88
    i32 mode = 0;                // +0xac（0x42d700 参数）
    Timer timer;                 // +0x0c
    i32 transition_frame = 0;    // +0x8c
    i32 transition_length = 0;   // +0x90
    i32 end_counter = 0;         // +0xb0
    const StageEntry* entry = nullptr;  // DAT_004a6f18
    std::unique_ptr<StageManager> stage;           // DAT_004a6da0
    std::unique_ptr<StageManager> previous_stage;  // DAT_004a6d9c：换关后仍在运行的上一关（bit3 时 0x4099a0 析构）
    bool stage_live = false;
    Player player;               // DAT_004a6ef8（新游戏时 0x441c60 创建）
    bool player_live = false;
    EnemyManager enemies;        // DAT_004a6dc0
    bool enemies_live = false;
    std::unique_ptr<BulletManager> bullets;  // DAT_004a6dac（0x1403b28 字节，堆分配）
    bool bullets_live = false;
    std::unique_ptr<ItemManager> items;      // DAT_004a6ddc（0x1c972ec 字节，堆分配）
    bool items_live = false;
    std::unique_ptr<LaserManager> lasers;    // DAT_004a6ee0（0x610 字节；0x4314a0）
    bool lasers_live = false;
    ScorePopupManager score_popups;          // DAT_004a6f10（0x449e50，与 Item / Laser 同批创建，换关沿用）
    bool score_popups_live = false;
    BombManager bombs;           // DAT_004a6da8 / DAT_004a6da4（0x40d890 每关装载时建，0x40da90 离开时毁）
    Gui gui;                     // DAT_004a6dcc（0x427ca0；对话 / Boss 标题 / 过关结算 / HUD）
    bool gui_live = false;
    PlayerWorld world;           // 敌人 / 对话 / 过关状态；enemy_count 由 EnemyManager 同步
    // 换关：0x42d200 禁用 Item / Bullet / Replay 链，0x41b340 / 0x4178a0 新建的 Enemy / Spellcard 链也未启用，
    // 直到 0x42dee0 统一 |= 2。GameSession 的对应链 lambda 据此跳过
    bool chains_suspended = false;
    // DAT_004a6d98+0x19248（AsciiManager 的 "Loading" VM，0x41a390 生成）：首帧 / 0x42dee0 中断 1 并清零
    u32 loading_vm = 0;
    // DAT_004a52c8 族（GameSession::game_input）：0x448eb0 → 0x449120 在关卡起点整体清零（下一帧 pressed = held）
    InputState* game_input = nullptr;
    const InputState* system_input = nullptr;   // DAT_004a50b0 组（宿主键盘，演示回放中按键回标题）
    const u32* config_flags = nullptr;          // DAT_004c1800（bit13：回标题改为场景 2）
    ScoreFile* scores = nullptr;                // DAT_004a6f0c：0x43c3f0 关卡曲音乐室已听过位
    // 0x45d150：更新链固定为 0x13，绘制链优先级由调用者提供。
    std::function<void(i32 kind, i32 duration, u32 color, i32 draw_priority)> on_screen_fade;
    // 0x40add0 开头：关卡的更新 / 绘制链在这里才置启用位（0x4097c0 建链时 `& ~2`）。
    // 换关过关奖励展示的 120 帧里 GameManager 整帧跳过，首帧尚未到来，背景 VM 与 std 脚本都不得推进
    std::function<void()> on_stage_started;
    // 0x42dc50 / 0x42dee0 → 0x448eb0：自机复位之后的 ReplayManager 关卡起点（段头读回 / 写入）
    std::function<void()> on_replay_stage_start;
    // 0x4099a0 之后：GameSession 卸载上一关背景 ANM 槽
    std::function<void(StageManager&)> on_previous_stage_destroyed;
    // 0x43f240：回放中通关倒计时结束 → GameMenu 的“回标题”流程
    std::function<void()> on_return_to_title;
    // 0x42d7b0 尾（LAB_0042db05）：0x45f980 ×4 推进 Supervisor 的四个画面合成 VM（ScreenCompositor::tick）
    std::function<void()> on_frame_advanced;
    // 0x42d7b0 换关过场结束（flags 0x10000：th16 内无人置位，保留结构）：0x43c3f0(0, 关卡曲) 重播并 0x45e330 排空
    std::function<void()> on_transition_finished;
    MusicManager* music = nullptr;   // DAT_004d9e10 乐曲命令（0x43c3f0 / 0x43c440）
    void start_stage_bgm();          // 0x43c3f0(0, DAT_004a6f18 +0x28)：播放槽 0 并置音乐室已听过

    // 0x42dc50 首帧初始化；返回 false 表示装载失败（原版返回 1 → 场景请求 2/3）
    bool first_frame(AnmManager& manager, GameGlobals& globals);
    // 0x42dee0：flags bit11 时做换关后的管理器复位与生成 "main"
    void deferred_init(AnmManager& manager, GameGlobals& globals);
    // 0x42d7b0：返回 false 表示链应中断（原版返回 3）
    bool update(AnmManager& manager, GameGlobals& globals, float game_speed);

private:
    // 0x42dc50 / 0x42dee0 共用段：复位、帧计数清零、生成 "main"、启用链
    void reset_for_stage(AnmManager& manager, GameGlobals& globals);
};

}  // namespace th16
