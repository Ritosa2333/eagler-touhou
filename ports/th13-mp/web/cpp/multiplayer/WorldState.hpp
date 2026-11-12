// M6：完整结构化快照（WorldState）——回滚/重演的权威状态 capture/restore。
//
// 快照红线（docs/MP-DEVELOPMENT.md §快照红线）逐条落实：
//   - 禁 memcpy ApplicationState/std::function/容器/unique_ptr/引用/整堆：
//     本文件全部显式逐域拷贝；std::function 回调接线（host_/progress_ 等
//     指针与 sim 的宿主钩子）不快照——它们是 RunScene/ApplicationState 的
//     固定接线，restore 后原值仍然有效（对象身份不变，仅数据回滚）。
//   - FrameScheduler（外部 callback 侵入链）按稳定 callback 身份恢复：
//     TH13 的 callback 由 ApplicationState/RunScene/AnmDrawSchedule 等宿主
//     以成员 FrameCallback 持有、scheduler 只存侵入链指针——回滚不触碰
//     注册结构（callback 集合在确认栅栏内不变），WorldState 不快照调度器；
//     scheduler_callback_rebind case 在宿主重建场景（换关/重试）后按
//     FrameCallback* 身份核对链完整（M6 测试面）。
//   - AnmRegistry 专用 capture/restore（AnmRegistry.hpp SnapshotData）：
//     恢复分配顺序与句柄 generation。
//   - hash：digest() 显式序列化全权威状态（稳定顺序，float 按 bit）；
//     不 hash 指针/padding/unordered_map 顺序/localSeat/墙钟。
//
// 快照边界（M1 模拟/呈现拆分的同一边界）：
//   权威模拟面 = StageBattle（席位/弹/注册池/灵界状态机）+ StageSim
//   （敌机/弹/激光/ECL 线程/gvars/boss）+ ItemManager（道具/神霊池）+
//   SessionState/HudScoreState + SpellState + 双 RNG + AnmRegistry。
//   表现层自备缓存（StagePresentation 的 bullets_/item_vms_/shot_handles_
//   等手账、GameHud 的 VM 句柄）不直接快照：它们由 sync 每帧从权威态重建/
//   对齐，restore 后下一帧 sync 自然恢复（th15 BattleFrameState 同哲学）。
//   例外：AnmRegistry 是权威 VM 池（敌机槽位/自机弹/特效 VM 的真存储），
//   整池 capture/restore；表现层句柄（u32）在 restore 后仍然有效——
//   句柄 = generation<<13|slot，两者都被恢复。
#pragma once

#include "../game/AnmRegistry.hpp"
#include "../game/DialogueText.hpp"
#include "../game/GameInput.hpp"
#include "../game/GameRandom.hpp"  // Rng.hpp 的 GameRandom
#include "../game/ItemManager.hpp"
#include "../game/MsgPlayer.hpp"  // P3.2：对话域快照（MsgPlayer::Snapshot/MsgAdvance）
#include "../game/ScreenShake.hpp"  // P3.2：震动域快照
#include "../game/SpellState.hpp"
#include "../game/StageBattle.hpp"
#include "../game/StagePresentation.hpp"
#include "../game/StageScenery.hpp"  // P3.2：STD 域快照（ExtraSnapshot/StageScriptState）
#include "../game/StageSim.hpp"
#include "../game/RunScene.hpp"  // P3.2：宿主域取值面（capture/restore_shakes 等）

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace th13::multiplayer {

class DigestSink;  // WorldState.cpp 内定义（FNV-1a 64 生产侧 hash）
struct HostState;

// 敌机快照：值域 + ECL 线程组（EclThreads::snapshot 已有）+ fog（值数组）。
struct EnemySnapshot {
    StageEnemy fields;              // 值语义域（routine/spell_name 等 string 拷贝）
    EclThreadsSnapshot threads;     // 主线程 + 子线程值拷贝
    bool has_fog = false;
    EnemyFog fog;                   // 值数组（17×17 顶点）
};

// WorldState：一次 capture 的完整数据。restore 后权威模拟面与 capture 时
// 逐位一致（digest 相等），再喂同输入逐帧一致（M6 验收口径）。
class WorldState {
public:
    // ---- 宿主回放面（回放会话的输入游标——快照边界的一部分：回放输入
    // 流的帧号与权威模拟帧必须同步回滚，否则 restore 后喂错帧输入）。
    // 非回放会话（live run）调用方不设置 replay 参数即可（false 语义）。
    struct ReplayView {
        u32 consumed = 0;
        u32 cursor_index = 0;
        bool cursor_eof = false;
        bool finished = false;
        // 回放输入沿状态机（ApplicationState::replay_input——playback_input
        // 每帧 update(recorded) 维护的 pressed/released/duration/repeat_age）：
        // 与游标同帧回滚，否则 restore 后首帧的输入沿携带发散期历史
        //（M6 实证：seat_inputs digest 首差）。
        GameInput input;
    };
    // ---- capture：从权威面取值。宿主对象（battle/sim/item_manager 的
    // 接线指针）不被修改；visual RNG 由调用方（ApplicationState）供给。
    // presentation 非空时连同表现层手账一起快照（浏览器/生产路径；无头
    // 测试传 nullptr——手账面为空，restore 同样跳过）。
    // P3.2：scene 非空时快照 RunScene 宿主域（震动/对话/STD/转场句柄）；
    // 无头测试不传 = 该组域为空（has_scene_=false），restore 跳过——与
    // presentation 同一门语义。transition_handle 由调用方（Session 接线）
    // 单独供给：RunTransitionEffect 归 ApplicationState 所有，不在 scene。
    bool capture(StageBattle& battle, GameRandom& visual, const ReplayView* replay = nullptr,
                StagePresentation* presentation = nullptr, RunScene* scene = nullptr,
                u32 transition_handle = 0);
    // ---- restore：权威面整场替换。battle 的接线指针（host_/progress_/
    // score_/player_ 窗口）与 sim 的宿主钩子保持原值（对象身份不变）。
    // presentation 非空且 capture 时带手账 → restore 手账（句柄/代映射与
    // 自有 VM 值整场替换，sync 走复用路径——不重置重建，避免 registry
    // 孤儿 VM 与新建 VM 的 RNG 消耗偏移）。
    // P3.2：scene 非空且 capture 时带宿主域 → restore 宿主域（震动重建
    // 经 start_shake 生产路径、对话值域、STD 值域）。transition_handle_out
    // 非空时回写转场句柄（调用方转交 ApplicationState::run_transition）。
    bool restore(StageBattle& battle, GameRandom& visual, ReplayView* replay = nullptr,
                 StagePresentation* presentation = nullptr, RunScene* scene = nullptr,
                 u32* transition_handle_out = nullptr);
    bool ready() const noexcept { return ready_; }
    void discard() noexcept;

    // ---- hash：显式序列化全权威状态（稳定顺序，float 按 bit）。
    // 供回滚一致性断言（S→前进→restore(S)→digest 相等）与锁步确认帧
    // 对拍（M7+）。不 hash 指针/padding/unordered_map 顺序/墙钟。
    uint64_t digest() const;
    // Simulation-domain summary for the periodic wire check. This excludes
    // presentation/registry state; acceptance must also compare digest() and
    // the host domains on every confirmed frame, including rollback replay.
    uint64_t digest_core() const;
    static uint64_t digest_dialogue(const DialogueText::RollbackState&);
    static uint64_t digest_host(const HostState&);
    static std::string diagnostic_host_detail(const HostState&);

    // ---- 字段台账（M6 交付面）：快照覆盖的权威域清单（测试断言用）。
    static const char* field_ledger();
    // ---- 分域 digest（诊断面）：权威模拟面按域输出 FNV-1a 摘要
    //（"battle"|"sim"|"items"|"progress"|"score"|"rng"）。续推分歧定位用。
    void digest_domains(char* buffer, size_t capacity) const;

    std::string diagnostic_detail() const;
    size_t retained_bytes() const noexcept;

private:
    // StageBattle 玩家面（M2 席位架构：seats 权威 + 窗口是观察副本）。
    struct BattleData {
        i32 seat_count = 0, active_seat = 0;
        StagePlayerState single_player{};
        HudPlayerState single_hud{};
        std::array<StagePlayerState, 3> seats{};
        std::array<HudPlayerState, 3> seat_huds{};
        std::array<i32, 3> seat_characters{};
        std::array<GameInput, 3> seat_inputs{};
        StageBattle::TranceMode trance_mode = StageBattle::TranceMode::Idle;
        i32 trance_owner_seat = 0;
        u32 trance_generation = 0;
        u32 death_trance_generation = 0;  // R-T9：死亡演出起始代数（重生清槽守卫）
        i32 trance_task_frame = 0;
        // sim.bomb_active 是派生读面（step_mp 帧尾聚合），不快照。
    } battle_;
    // StageSim 权威面。
    struct SimData {
        u32 frame = 0;
        std::vector<EnemySnapshot> enemies;
        std::vector<StageBullet> bullets;
        std::vector<StageLaser> lasers;
        std::vector<std::pair<i32, i32>> gvars_writable_ints;
        std::array<i32, 4> shared_integers{};
        std::array<float, 8> shared_floats{};
        i32 boss_slot[3]{};
        i32 boss_timer_seconds = -1, boss_timer_centiseconds = 0;
        bool boss_leaving = false;
        StageSim::BossMarker boss_markers[3][4]{};
        i32 boss_remaining_stars = 0;
        i32 boss_nonspell_frame = -1;
        u32 boss_main_serial = 0xFFFFFFFFu;
        i32 last_created_enemy_id = 0;
        u32 laser_serial = 0x10000;
        u32 next_serial = 1;        // 敌机 serial 分配器（418f40 mgr+bc）
        u32 next_bullet_generation = 0;  // 弹体 generation 分配器（弹槽身份）
        float player_x = 192, player_y = 420;
        i32 seat_count = 0;
        std::array<float, 3> seat_x{}, seat_y{};
        std::array<bool, 3> seat_eligible{};
        float bullet_bounce_size[2]{};
        float active_bullet_protect_radius2 = 0;
        u32 mode_flags = 0;
        i32 difficulty_rank = 0x08;
        i32 diff = 3, rank = 0;
        u32 spawned = 0;
        std::vector<i32> bullet_free_slots;
        std::vector<AnmEffectRequest> anm_effects;
        bool bomb_active = false;
        SpellSnapshot spell;
        // StageEnemy* current 是帧内瞬态（步进中指向），不快照。
    } sim_;
    // ItemManager 池（2648 道具 + 256 神霊 + 自由槽表 + 计数域）。
    struct ItemData {
        std::array<Item, ItemManager::kItemCapacity> items{};
        std::array<Spirit, ItemManager::kSpiritCapacity> spirits{};
        std::vector<i32> ordinary_free, cancel_free;
        std::vector<ItemEvent> events;
        ItemCollector last_player{};
        u32 spirit_serial = 0;
        i32 spirit_overflow = 0, spirit_window = 0;
        Timer spirit_window_clock{};
        i32 cancel_total = 0, cancel_wave = 0, point_items = 0;
    } items_;
    // 会话进度 + 分数视图（M4 团队灵界 backing：run_mode_flags bit0x100 +
    // ending_points 在 SessionState——共享槽随快照走）。
    SessionState progress_;
    HudScoreState score_;
    // 双 RNG（game=0x4dc324 战斗流 / visual=0x4dc31c ANM 流）。
    struct RngData {
        u16 seed = 0;
        u32 index = 0;
    } game_rng_, visual_rng_;
    // ANM registry（专用快照：全 VM 池 + 父子链 + 句柄 generation + 链序）。
    AnmRegistry::SnapshotData anm_;
    // 表现层手账（capture 带 presentation 时非空；restore 透传回同一对象）。
    StagePresentation::SnapshotData present_;
    bool has_present_ = false;
    // ---- P3.2 快照扩展域（RunScene 宿主面；scene 非空时有效）----
    // 震动列表（envelope 型消费 game RNG——跨震动帧回滚的 RNG 漂移根因；
    // 非 envelope 型消费 visual RNG，同窗回滚同理）。
    RunScene::RollbackState scene_state_;
    std::vector<ScreenShake> shakes_;
    // MSG 对话域（MsgPlayer 值域 + 宿主锁存输入沿 + dialog_ready）。
    MsgPlayer::Snapshot dialog_;
    MsgAdvance dialog_advance_{};
    bool dialog_ready_ = false;
    // STD 背景域（相机脚本状态 + StageScenery 自持 VM/标志/生命周期）。
    StageScriptState scenery_script_;
    StageScenery::ExtraSnapshot scenery_extra_;
    bool has_scenery_ = false;
    StageScriptState previous_scenery_script_;
    StageScenery::ExtraSnapshot previous_scenery_extra_;
    bool has_previous_scenery_ = false;
    i32 previous_scenery_bank_ = -1;
    // 开局转场句柄（RunTransitionEffect 的唯一值状态；VM 本体在 anm_ 域）。
    u32 transition_handle_ = 0;
    bool has_scene_ = false;  // scene 接线是否在 capture 时可用（整组域的门）
    // 宿主回放面（回放会话：输入游标随权威帧同步回滚）。
    ReplayView replay_;
    bool has_replay_ = false;
    bool ready_ = false;
    // digest 的两段实现（core=权威模拟面；anm=表现层 registry 面仅全量）。
    void digest_scene_sink(DigestSink& d, bool full) const;
    void digest_core_sink(DigestSink&, bool include_animation_ids) const;
    void digest_anm_sink(DigestSink&) const;
};

}  // namespace th13::multiplayer
