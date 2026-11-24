#pragma once

#include "../game/AnmRegistry.hpp"
#include "../game/DialogueText.hpp"
#include "../game/GameInput.hpp"
#include "../game/GameRandom.hpp"
#include "../game/ItemManager.hpp"
#include "../game/MsgPlayer.hpp"
#include "../game/ScreenShake.hpp"
#include "../game/SpellState.hpp"
#include "../game/StageBattle.hpp"
#include "../game/StagePresentation.hpp"
#include "../game/StageScenery.hpp"
#include "../game/StageSim.hpp"
#include "../game/RunScene.hpp"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace th13::multiplayer {

class DigestSink;
struct HostState;

struct EnemySnapshot {
    StageEnemy fields;
    EclThreadsSnapshot threads;
    bool has_fog = false;
    EnemyFog fog;
};

class WorldState {
public:
    struct ReplayView {
        u32 consumed = 0;
        u32 cursor_index = 0;
        bool cursor_eof = false;
        bool finished = false;
        GameInput input;
    };
    bool capture(StageBattle& battle, GameRandom& visual, const ReplayView* replay = nullptr,
                StagePresentation* presentation = nullptr, RunScene* scene = nullptr,
                u32 transition_handle = 0);
    bool restore(StageBattle& battle, GameRandom& visual, ReplayView* replay = nullptr,
                 StagePresentation* presentation = nullptr, RunScene* scene = nullptr,
                 u32* transition_handle_out = nullptr);
    bool ready() const noexcept { return ready_; }
    void discard() noexcept;

    uint64_t digest() const;
    uint64_t digest_core() const;
    static uint64_t digest_dialogue(const DialogueText::RollbackState&);
    static uint64_t digest_host(const HostState&);
    static std::string diagnostic_host_detail(const HostState&);

    static const char* field_ledger();
    void digest_domains(char* buffer, size_t capacity) const;

    std::string diagnostic_detail() const;
    size_t retained_bytes() const noexcept;

private:
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
        u32 death_trance_generation = 0;
        i32 trance_task_frame = 0;
    } battle_;
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
        u32 next_serial = 1;
        u32 next_bullet_generation = 0;
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
    } sim_;
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
    SessionState progress_;
    HudScoreState score_;
    struct RngData {
        u16 seed = 0;
        u32 index = 0;
    } game_rng_, visual_rng_;
    AnmRegistry::SnapshotData anm_;
    StagePresentation::SnapshotData present_;
    bool has_present_ = false;
    RunScene::RollbackState scene_state_;
    std::vector<ScreenShake> shakes_;
    MsgPlayer::Snapshot dialog_;
    MsgAdvance dialog_advance_{};
    bool dialog_ready_ = false;
    StageScriptState scenery_script_;
    StageScenery::ExtraSnapshot scenery_extra_;
    bool has_scenery_ = false;
    StageScriptState previous_scenery_script_;
    StageScenery::ExtraSnapshot previous_scenery_extra_;
    bool has_previous_scenery_ = false;
    i32 previous_scenery_bank_ = -1;
    u32 transition_handle_ = 0;
    bool has_scene_ = false;
    ReplayView replay_;
    bool has_replay_ = false;
    bool ready_ = false;
    std::unordered_map<const AnmVm*, AnmVm*> remapped_;
    std::vector<AnmVm*> restored_embedded_;
    void digest_battle_fields(DigestSink& d) const;
    void digest_sim_tail(DigestSink& d, bool include_boss_markers) const;
    void digest_items_fields(DigestSink& d) const;
    void digest_progress_fields(DigestSink& d) const;
    void digest_score_fields(DigestSink& d) const;
    void digest_rng_fields(DigestSink& d) const;
    void digest_scene_sink(DigestSink& d, bool full) const;
    void digest_core_sink(DigestSink&, bool include_animation_ids) const;
    void digest_anm_sink(DigestSink&) const;
};

}
