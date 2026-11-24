#pragma once

#include "GameInput.hpp"
#include "ItemManager.hpp"
#include "SessionState.hpp"
#include "StageSim.hpp"
#include "YoumuBomb.hpp"

#include <array>

namespace th13 {

extern bool damage_probe_enabled;
extern u32 damage_probe_first_frame;
extern u32 damage_probe_last_frame;

struct StageBattleServices {
    virtual ~StageBattleServices() = default;
    virtual bool life_hud(i32 lives, i32 pieces) = 0;
    virtual bool bomb_hud(i32 bombs, i32 pieces) = 0;
    virtual bool request_game_over() = 0;
    virtual bool request_player_game_over() { return request_game_over(); }
    virtual bool hud_sound(i32 id) = 0;
    virtual bool hud_sound(i32 id, float x) {
        (void)x;
        return hud_sound(id);
    }
    virtual bool stop_sound(i32 id) {
        (void)id;
        return true;
    }
    virtual bool set_voice_pan(i32 id, float x) {
        (void)id;
        (void)x;
        return true;
    }
    virtual bool trance_music(bool active) {
        (void)active;
        return true;
    }
    virtual bool trance_gauge_refresh() { return true; }
    virtual bool hud_notice(i32 front_script) {
        (void)front_script;
        return true;
    }
    virtual bool hud_spirit_notice(i32 level, float x, float y, float z) {
        (void)level;
        (void)x;
        (void)y;
        (void)z;
        return true;
    }
    virtual bool score_popup(i32 value, float x, float y, u32 color) {
        (void)value;
        (void)x;
        (void)y;
        (void)color;
        return true;
    }
    virtual void enemy_retire(u32 serial) { (void)serial; }
};

struct PlayerShot {
    float x = 0, y = 0, damage = 0;
    float angle = -1.5707964f;
    i32 script = -1;
    bool alive = false;
    bool hit_fade = false;
    i32 state = 1;
    bool animation_finished = false;
    float speed = 12.0f;
    float width = 8.0f, height = 8.0f;
    float beam_visual_width = 0;
    i32 age = 0, behavior = 0, type = 0;
    i32 anchor = 0, anchor_power = 0;
    float target_angle = 0, damage_x = 0, damage_y = 0;
    Timer age_clock;
    Timer damage_clock{9999998, 9999999, 9999999.f, 0, 1};
    u32 target_serial = 0;
    u32 generation = 0;
    i32 hit_behavior = 0;
    bool sine_moving = false;
    StageMover sine_motion;
    i32 registration_slot = -1;
    bool beam_contact = false, beam_contact_seen = false;
    i32 beam_interrupt = 0;
};

struct DamageRegistration {
    enum class Owner { Shot, Area };
    bool active = false;
    u32 flags = 0;
    Owner owner = Owner::Shot;
    i32 orb = -1;
    float radius = 0, radius_growth = 0;
    float center_x = 0, center_y = 0, center_z = 0;
    float travel_speed = 0, travel_angle = 0;
    float width = 0, height = 0, angle = 0;
    Timer clock{9999998, 9999999, 9999999.f, 0, 1};
    i32 interval = 1;
    float damage = 0;
    i32 accumulated_damage = 0;
    i32 threshold = 999999;
    u32 generation = 0;
};

struct PlayerShotGeometrySnapshot {
    bool valid = false;
    i32 slot = -1;
    i32 age = 0;
    i32 script = -1;
    u32 generation = 0;
    std::array<Vec3, 4> quad{};
};

struct PlayerOption {
    float x = 0, y = 0, angle = -1.5707964f;
    bool active = false;
};
struct PlayerBombOrb {
    float x = 0, y = 0, angle = 0;
    bool active = false, exploding = false;
    i32 explosion_frame = 0;
    Timer explosion_damage_clock{10, 11, 11.f, 0, 1};
    float radius = 0, speed = 0;
    float velocity_x = 0, velocity_y = 0;
    float accumulated_damage = 0;
    i32 registration_slot = -1;
    void step(i32 age, i32 index, float player_x, float player_y, const StageEnemy* target);
};

struct EnemyScanState {
    u32 serial = 0;
    float x = 0, y = 0;
    float hurtbox[2] = {0, 0};
};

struct ShtShotRecord {
    i32 interval = 0;
    i32 phase = 0;
    i32 script = 0;
    i32 anchor = 0;
    float x = 0, y = 0;
    float angle = 0;
    float damage = 0;
    float speed = 0;
    float width = 0, height = 0;
    i32 behavior = 0, type = 0;
    i32 sound = -1;
    i32 initializer = 0, hit_behavior = 0;
    float motion_phase = 0;
};

struct StagePlayerState {
    enum class Phase : i32 { Entering = 0, Active = 1, Dying = 2, Deathbomb = 4 };
    bool initialized = false;
    float x = 0, y = 400;
    i32 fixed_x = 0, fixed_y = 51200;
    bool focus = false, dead = false;
    Phase phase = Phase::Active;
    i32 phase_frame = 0, invulnerable = 0;
    i32 phase_frame_previous = 0;
    Timer phase_clock, fire_clock, startup_clock, charge_clock;
    Vec3 charge_origin{};
    std::vector<PlayerShot> shots;
    std::array<DamageRegistration, 256> damage_registrations{};
    i32 damage_registration_cursor = 0;
    std::vector<PlayerShotGeometrySnapshot> shot_geometry;
    u32 shot_generation = 0;
    std::array<PlayerOption, 8> options{};
    i32 option_count = 0;
    std::array<std::array<i32, 2>, 8> option_fixed{};
    std::array<std::array<i32, 2>, 33> movement_history{};
    i32 option_power = -1;
    bool option_reset_pending = false, option_movement_tick = false;
};

class StageBattle {
    std::unique_ptr<StagePlayerState> owned_player_state_;
    StagePlayerState& player_state_;
    void activate_stage_player();

public:
    StageBattle() : StageBattle(nullptr) {}
    explicit StageBattle(GameRandom* shared_random, StagePlayerState* shared_player = nullptr)
        : owned_player_state_(shared_player ? nullptr : std::make_unique<StagePlayerState>()),
          player_state_(shared_player ? *shared_player : *owned_player_state_), sim(shared_random),
          item_manager(shared_random) {}
    StageSim sim;
    float frame_damage_total = 0.0f;
    ItemManager item_manager;
    YoumuBomb youmu_bomb_;
    std::vector<YoumuBomb::Spawn> youmu_slash_spawns_;
    float& player_x = player_state_.x;
    float& player_y = player_state_.y;
    bool& focus = player_state_.focus;
    bool& player_dead = player_state_.dead;
    using PlayerPhase = StagePlayerState::Phase;
    PlayerPhase& player_phase = player_state_.phase;
    i32& player_phase_frame = player_state_.phase_frame;
    i32& player_phase_frame_previous = player_state_.phase_frame_previous;
    bool player_visible() const noexcept { return !player_dead && player_phase != PlayerPhase::Dying; }
    i32& invulnerable = player_state_.invulnerable;
    struct RespawnDamage {
        Vec3 center{};
        float radius = 0;
        Timer remaining;
        bool active = false;
        i32 registration_slot = -1;
    } respawn_damage;
    Vec3 respawn_origin{};
    i32 bomb_timer = 0;
    i32 bomb_age = -1;
    float bomb_age_accumulator_ = 0;
    u32 bomb_generation = 0;
    struct BombRay {
        Vec3 origin{}, center{};
        float angle = 0;
    };
    std::array<BombRay, 5> bomb_rays{};
    Vec3 bomb_center() const noexcept { return {bomb_center_x_, bomb_center_y_, 0}; }
    float bomb_visual_angle() const noexcept { return bomb_visual_angle_; }
    std::array<PlayerBombOrb, 8> bomb_orbs{};
    std::array<PlayerOption, 8>& options = player_state_.options;
    i32& option_count = player_state_.option_count;
    i32 player_character() const noexcept { return character_; }
    i32 player_power() const noexcept { return player_ ? player_->power : 0; }
    i32 player_power_max() const noexcept { return player_ ? player_->max_power : 0; }
    i32 power_level() const noexcept {
        return player_ && player_->power_step > 0 ? player_->power / player_->power_step : 0;
    }
    bool full_power() const noexcept { return player_ && player_->power >= player_->max_power; }
    std::vector<PlayerShot>& shots = player_state_.shots;
    std::function<bool(const Vec3&, const Vec3&, float)> beam_hit_effect;
    std::function<bool(StageBattle&)> option_power_changed;
    bool death_option_rebuild = false;
    std::function<bool(PlayerShot&, u32)> player_shot_birth;
    std::function<bool(StageBattle&)> youmu_bomb_update;
    std::function<bool(i32, const Vec3&)> youmu_charge_effect;
    std::function<bool(i32, i32, i32, i32)> bomb_screen_shake;
    std::function<bool()> reimu_explosion_shake;
    std::function<bool(StageBattle&, i32)> reimu_bomb_orb_birth;
    std::array<DamageRegistration, 256>& damage_registrations = player_state_.damage_registrations;
    i32& damage_registration_cursor = player_state_.damage_registration_cursor;

    i32& pos_fx = player_state_.fixed_x;
    i32& pos_fy = player_state_.fixed_y;
    void set_player_pos(float x, float y) {
        player_x = x;
        player_y = y;
        pos_fx = i32(x * 128.0f);
        pos_fy = i32(y * 128.0f);
        sim.player_x = x;
        sim.player_y = y;
    }
    void restore_replay_player(i32 x, i32 y, bool initial_focus);
    i32 speed_orth = 576, speed_diag = 407;
    i32 speed_orth_focus = 256, speed_diag_focus = 181;
    struct TouchMotion {
        int mode = 0;
        float x = 0, y = 0;
    } touch;
    u32 input_state = 0;

    static constexpr i32 kFireInterval = 4;
    static constexpr float kShotDamage = 12.0f;
    static constexpr float kShotSpeed = 12.0f;
    static constexpr float kShotHalf = 4.0f;
    static constexpr float kShotOffsetX = 8.0f;
    static constexpr float kShotOffsetY = 12.0f;
    static constexpr float kPlayerHitSize[4] = {2.0f, 3.0f, 3.0f, 3.0f};
    static constexpr float kPlayerHitRadius = 3.0f;
    static constexpr float kBulletHitRadius = 4.0f;
    static constexpr float kEnemyBodyRadius = 20.0f;
    static constexpr i32 kInvulnFrames = 240;
    static constexpr i32 kBombFrames = 180;
    static constexpr float kBombDamagePerFrame = 3.0f;
    static constexpr i32 kKillScore = 5000;
    static constexpr i32 kKillPower = 8;
    static constexpr i32 kTranceActivateFrame = 30;
    static constexpr float kTranceDrainPerFrame = 3.0f;

    bool load(EclResourceProvider& provider, const char* stage_file, i32 difficulty_rank, StageBattleServices& host,
              SessionState& progress, HudPlayerState& player, HudScoreState& score, const char* event_path = nullptr,
              const char* sht_name = nullptr);
    bool prepare_player(EclResourceProvider& provider, SessionState& progress, HudPlayerState& player,
                        const char* sht_name, bool cold_entry);
    bool step(const GameInput& input);
    bool step_transition(const GameInput& input);
    bool defer_item_side = false;
    void update_items_phase();
    void update_spirits_phase();
    bool continue_player();
    void on_bomb_key();
    bool end_trance();
    bool open_event_sink(const char* path);
    const std::string& error() const { return error_; }

    void begin_player_shot_geometry_snapshot();
    void set_player_shot_geometry(i32 slot, i32 age, i32 script, u32 generation, const std::array<Vec3, 4>& quad);

    static constexpr u32 kShtBlocks = 11;
    std::array<std::vector<ShtShotRecord>, kShtBlocks> sht_shots;
    bool sht_shots_loaded = false;

private:
    bool player_prepared_ = false;
    void rebuild_options_for_power();
    void move_player(u32 held, bool bomb_slow);
    void update_options(bool reset = false);
    void update_firing(bool held);
    void step_bomb();
    float query_enemy_damage(StageEnemy& enemy, float width, float height);
    bool query_enemy_bomb_hit(const StageEnemy& enemy) const;
    void explode_orb(PlayerBombOrb& orb);
    void fire_shots(i32 frame);
    void step_shots();
    i32 reserve_damage_registration();
    i32 register_circle(Vec3 center, float radius, float growth, i32 lifetime, i32 damage, i32 interval = 1,
                        i32 threshold = 999999, i32 orb = -1);
    i32 register_box(Vec3 center, float width, float height, float angle, i32 damage);
    i32 allocate_damage_registration(PlayerShot& shot);
    void sync_damage_registrations();
    void retire_damage_registration(PlayerShot& shot);
    void collide_shots(StageEnemy& enemy);
    void collide_enemy_body(StageEnemy& enemy);
    void consume_player_shot_geometry();
    void collide_player_bullet(StageBullet& bullet);
    i32 collide_laser(const StageLaser&);
    void reward_kill(StageEnemy& enemy);
    void update_item_side();
    ItemCollector make_item_collector();
    void emit_item_events();
    bool update_trance(const GameInput& input);
    bool kill_player();
    bool commit_death();
    bool commit_death_normal();
    bool update_player(const GameInput&);
    StageBattleServices* host_ = nullptr;
    SessionState* progress_ = nullptr;
    HudPlayerState* player_ = nullptr;
    HudScoreState* score_ = nullptr;
    Timer& player_phase_clock_ = player_state_.phase_clock;
    Timer& fire_clock_ = player_state_.fire_clock;
    std::array<std::array<std::array<i32, 2>, 10>, 2> option_offsets_{};
    std::array<std::array<i32, 2>, 8>& option_fixed_ = player_state_.option_fixed;
    std::array<std::array<i32, 2>, 33>& movement_history_ = player_state_.movement_history;
    i32& option_power_ = player_state_.option_power;
    bool& option_reset_pending_ = player_state_.option_reset_pending;
    bool& option_movement_tick_ = player_state_.option_movement_tick;
    float bomb_angle_ = -1.5707964f;
    float bomb_center_x_ = 0, bomb_center_y_ = 0;
    float bomb_rotation_speed_ = 0;
    float bomb_visual_angle_ = 0;
    i32 bomb_duration_ = 0;
    i32 character_ = 0;
    bool life_probe_ = false;
    bool events_ = false;
    bool sink_open_ = false;
    float player_hit_size_ = 2.0f;
    bool trance_protection_ = false;
    i32 trance_task_frame_ = 0;
    std::string error_;
    std::vector<PlayerShotGeometrySnapshot>& shot_geometry_ = player_state_.shot_geometry;
    u32& shot_generation_ = player_state_.shot_generation;
};

}
