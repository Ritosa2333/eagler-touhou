#include <optional>
#pragma once

#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "BulletAppearance.hpp"
#include "GameRandom.hpp"
#include "StageBattle.hpp"

#include <deque>
#include <map>
#include <memory>
#include <utility>
#include <vector>

namespace th13 {

class StagePresentation {
public:
    struct Banks {
        i32 stage = 0, bullet = 0, player = 0;
        i32 effect = 8;
        i32 spirit = 25;
        i32 front = 3;
        std::array<i32, 2> teammate_player{};
    };
    StagePresentation(AnmManager& animations, AnmRenderer& renderer, Banks banks, i32 player_shot_sprite);
    ~StagePresentation();
    bool sync(StageBattle& battle, u32 held);
    bool initialize_player_effects(StageBattle& battle);
    bool activate_player_effects(StageBattle& battle);
    bool graze_effect(const Vec3& position);
    bool hide_player_options();
    bool options_hidden() const noexcept { return player_effects_[0].options_hidden; }
    bool interrupt_player_options();
    bool reimu_bomb_orb_birth(const StageBattle& battle, i32 index);
    bool initialize_player_shot(PlayerShot& shot, u32 index, i32 seat = 0);
    bool tick_bullet(StageBullet& bullet, bool birth);
    bool initialize_bullet(StageBullet& bullet, i32 interrupt);
    bool bullet_cancel(i32 script, const Vec3& position, const Vec3& displacement, bool interpolate = true);
    bool create_ecl_effect(i32 file, i32 script, const Vec3& position);
    bool flush_ecl_animations(StageBattle& battle, StageEnemy* constructing = nullptr, bool slots_only = false);
    bool enemy_death(i32 file, i32 script, const Vec3& position, float angle);
    bool enemy_fog(StageEnemy&, bool reset);
    void retire_enemy_fog(u32 serial);
    u32 player_effect(i32 script, const Vec3& position, i32 layer = -1, u8 owner = 0) {
        return create_player_effect(banks.effect, script, layer, 0, position, owner);
    }
    bool beam_hit_effect(const Vec3& position, const Vec3& displacement, float angle, i32 seat = 0);
    bool sync_player_options(StageBattle& battle, bool power_rebuild = false);
    bool sync_items_phase(StageBattle& battle) { return sync_items(battle); }
    bool sync_item_spirits_phase(StageBattle& battle) { return sync_item_spirits(battle); }
    bool sync_youmu_slash(StageBattle& battle);
    bool youmu_charge_effect(i32 action, const Vec3& position, i32 seat = 0);
    bool spirit_animation_alive(u32 slot, u32 serial) const noexcept;
    bool spirit_pickup(i32 script, const Vec3& position);
    bool spirit_spawn_animation(u32 slot, u32 serial, i32 kind, u32 flags, float x, float y, float z);
    bool enemies_from_ecl = false;
    bool draw(u32 pass);
    void prepare_player_opacity();
    void release_stage();
    void release();
    void release_player_shots();
    struct PlayerEffectsState {
        std::array<u32, YoumuBomb::kSlots> youmu_slash_handles{};
        std::vector<u32> youmu_slash_extra;
        std::array<u32, 8> option_handles{}, full_option_handles{}, bomb_orb_handles{};
        std::array<bool, 8> bomb_orb_exploding{};
        std::array<u32, 5> bomb_ray_handles{};
        u32 focus_handle = 0;
        i32 option_character = -1, option_count = -1;
        bool option_anchor_deferred = false, option_rebuild_pending = false;
        bool options_hidden = false, death_options_queued = false, full_options = false;
        u32 bomb_generation = 0, bomb_background = 0, bomb_beam = 0;
    };
    struct SnapshotData {
        static constexpr u32 kContractVersion = 2;
        u32 version = kContractVersion;
        struct EnemyVisualData {
            u32 serial = 0;
            i32 script = -1;
            bool has_vm = false;
            AnmVm vm;
        };
        struct BulletSlotData {
            bool has_vm = false;
            i32 script = -1, color = -1, source_type = 0, source_color = 0;
            u32 generation = 0;
            bool active = false;
        };
        struct ShotFadeData {
            u32 handle = 0, generation = 0;
            float x = 0, y = 0, angle = 0, drift = 0;
            bool sine_moving = false;
            i32 age = 0;
            StageMover sine_motion;
        };
        std::vector<const AnmVm*> embedded_identities;
        std::vector<EnemyVisualData> enemies;
        std::vector<BulletSlotData> bullets;
        std::vector<AnmVm> bullet_vms;
        std::vector<u32> shot_handles;
        std::vector<AnmVm> shots;
        std::vector<u32> shot_generations;
        std::vector<ShotFadeData> shot_fades;
        std::vector<u64> shot_faded_generations;
        bool has_player = false;
        AnmVm player;
        i32 player_script = -1, player_tilt = 0;
        std::array<std::optional<AnmVm>, 2> teammate_players{};
        std::array<i32, 2> teammate_tilts{};
        std::array<u32, ItemManager::kSpiritCapacity> spirit_handles{}, spirit_serials{};
        std::array<i32, ItemManager::kSpiritCapacity> spirit_scripts{};
        std::array<bool, ItemManager::kItemCapacity> item_vm_present{}, item_side_vm_present{};
        std::vector<AnmVm> item_vms, item_side_vms;
        std::array<u32, YoumuBomb::kSlots> youmu_slash_handles{};
        std::vector<u32> youmu_slash_extra;
        std::array<u32, 8> option_handles{}, full_option_handles{}, bomb_orb_handles{};
        std::array<bool, 8> bomb_orb_exploding{};
        std::array<u32, 5> bomb_ray_handles{};
        u32 trance_handle = 0;
        bool trance_was_active = false;
        std::vector<u32> bullet_cancel_handles;
        u32 focus_handle = 0;
        std::array<u32, 3> youmu_charge_handles{};
        std::unordered_map<u32, std::array<u32, 16>> fog_handles;
        i32 option_character = -1, option_count = -1;
        bool option_anchor_deferred = false, option_rebuild_pending = false;
        bool options_hidden = false, death_options_queued = false;
        bool full_options = false, frame_synced = false;
        u32 bomb_generation = 0, bomb_background = 0, bomb_beam = 0;
        std::vector<u32> player_effect_handles;
        std::array<PlayerEffectsState, 2> teammate_effects{};
    };
    bool capture_presentation(SnapshotData& out) const;
    bool restore_presentation(const SnapshotData& source, std::unordered_map<const AnmVm*, AnmVm*>* remapped = nullptr,
                              std::vector<AnmVm*>* embedded_out = nullptr);
    void poison_dead_slots_for_test();
    void reset_snapshot_caches();
    void install_main_update_hook();
    const std::string& error() const { return error_; }
    std::string debug_report() const;
    static u32 debug_pass_mask;
    const AnmVm* item_body_vm(u32 slot) const { return item_vms_[slot].get(); }
    const AnmVm* item_side_vm(u32 slot) const { return item_side_vms_[slot].get(); }

private:
    std::unordered_map<u32, std::array<u32, 16>> fog_handles_;
    struct EnemyVisual {
        u32 serial = 0;
        i32 script = -1;
        std::unique_ptr<AnmVm> vm;
    };
    bool sync_item_spirits(StageBattle& battle);
    bool sync_trance_effect(const StageBattle&);
    bool sync_ecl_enemies(StageBattle& battle);
    bool sync_enemy_visuals(StageBattle& battle, AnmResource* stage_resource);
    bool sync_bullets(StageBattle& battle, AnmResource* bullet_resource);
    bool sync_player_shots(StageBattle& battle);
    bool sync_shot_fades(StageBattle& battle);
    bool sync_player_ship(StageBattle& battle, u32 held, AnmResource* player_resource);
    u32 trance_handle_ = 0;
    std::vector<u32> bullet_cancel_handles_;
    bool trance_was_active_ = false;
    std::array<u32, ItemManager::kSpiritCapacity> spirit_handles_{};
    std::array<u32, ItemManager::kSpiritCapacity> spirit_serials_{};
    std::array<i32, ItemManager::kSpiritCapacity> spirit_scripts_{};
    bool sync_items(StageBattle& battle);
    void append_item_draws();
    std::array<std::unique_ptr<AnmVm>, ItemManager::kItemCapacity> item_vms_{};
    std::array<std::unique_ptr<AnmVm>, ItemManager::kItemCapacity> item_side_vms_{};
    const std::vector<i32>& stage_file_offsets();
    struct ScriptTarget {
        i32 bank = 0;
        i32 script = 0;
    };
    ScriptTarget resolve_script_target(i32 file, i32 script);
    i32 stage_script_global(i32 file, i32 script);
    std::vector<i32> stage_file_offsets_;
    const AnmResource* stage_offsets_source_ = nullptr;
    struct BulletSlot {
        std::unique_ptr<AnmVm> vm;
        i32 script = -1;
        i32 color = -1;
        u32 generation = 0;
        bool active = false;
        struct Source final : AnmSpriteSource {
            i32 type = 0, color = 0;
            i32 sprite_index(i32 parameter) const noexcept override {
                return bullet_animation_parameter(type, color, parameter);
            }
        } source;
    };
    BulletSlot* bullet_slot(StageBullet& bullet);
    static void bullet_animation_geometry(StageBullet& bullet, const AnmVm& vm);
    AnmManager& animations;
    AnmRenderer& renderer;
    Banks banks;
    void create_color_step(AnmVm* vm);
    GameRandom& random;
    std::vector<EnemyVisual> enemies_;
    std::deque<BulletSlot> bullets_;
    std::vector<std::pair<u32, u32>> bullet_lookup_;
    std::vector<u32> live_bullet_generations_;
    std::vector<BulletSlot*> draw_bullet_order_;
    void rebuild_bullet_lookup();
    std::vector<u32> shot_handles_;
    std::vector<std::unique_ptr<AnmVm>> shots_;
    std::vector<u32> shot_generations_;
    struct ShotFade {
        u32 handle = 0;
        u32 generation = 0;
        float x = 0, y = 0;
        float angle = 0;
        float drift = 0;
        bool sine_moving = false;
        StageMover sine_motion;
        i32 age = 0;
    };
    std::vector<ShotFade> shot_fades_;
    std::map<u32, std::vector<u32>> shot_faded_generations_;
    std::unique_ptr<AnmVm> player_;
    std::array<std::unique_ptr<AnmVm>, 2> teammate_players_{};
    std::array<i32, 2> teammate_tilts_{};
    std::array<PlayerEffectsState, 3> player_effects_{};
    std::array<u32, 3> youmu_charge_handles_{};
    std::vector<u32> player_effect_handles_;
    bool sync_all_player_effects(StageBattle&);
    bool sync_player_options(StageBattle&, bool power_rebuild, i32 seat);
    i32 player_bank(i32 seat) const { return seat ? banks.teammate_player[size_t(seat - 1)] : banks.player; }
    bool sync_player_effects(StageBattle&, i32 seat = -1);
    bool sync_bomb_effects(const StageBattle&, i32 seat);
    bool post_chain_update();
    StageBattle* battle_ = nullptr;
    bool frame_synced_ = false;
    u32 create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering, const Vec3& position);
    enum class EffectOpacity { Attack, Support };
    u32 create_player_effect(i32 bank, i32 script, i32 layer, u32 ordering, const Vec3& position, u8 owner,
                             EffectOpacity opacity = EffectOpacity::Attack);
    bool end_player_effect(u32& handle, bool retire);
    i32 player_script_ = -1, player_tilt_ = 0;
    i32 player_shot_sprite_;
    std::string error_;
    bool fail(const std::string& why) {
        if (error_.empty()) error_ = why;
        return false;
    }
};

}
