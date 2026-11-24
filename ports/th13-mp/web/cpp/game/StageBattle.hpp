#pragma once

#include "GameInput.hpp"
#include "ItemManager.hpp"
#include "SessionState.hpp"
#include "StageSim.hpp"
#include "YoumuBomb.hpp"

#include <array>

namespace th13 {

class AnmRegistry;

namespace multiplayer {
class WorldState;
struct StageEntry;
}

extern bool damage_probe_enabled;
extern u32 damage_probe_first_frame;
extern u32 damage_probe_last_frame;

struct StageBattleServices {
    virtual ~StageBattleServices() = default;
    virtual bool life_hud(i32 lives, i32 pieces) = 0;
    virtual bool bomb_hud(i32 bombs, i32 pieces) = 0;
    virtual bool life_hud_seat(i32 seat, i32 lives, i32 pieces) {
        (void)seat;
        return life_hud(lives, pieces);
    }
    virtual bool bomb_hud_seat(i32 seat, i32 bombs, i32 pieces) {
        (void)seat;
        return bomb_hud(bombs, pieces);
    }
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
    i32 owner_seat = 0;
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
    i32 owner_seat = 0;
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
    bool ghost = false;
    bool final_death = false;
    bool ghost_just_entered = false;
    u32 ghost_random = 1, ghost_clock = 0;
    i32 ghost_dx = 0, ghost_dy = 0;
    u8 life_transfer_state = 0;
    u8 power_transfer_state = 0;
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
    bool death_option_rebuild = false;
    struct RespawnDamage {
        Vec3 center{};
        float radius = 0;
        Timer remaining;
        bool active = false;
        i32 registration_slot = -1;
    } respawn_damage;
    Vec3 respawn_origin{};
    i32 bomb_timer = 0;
    i32 bomb_protection = 0;
    i32 bomb_age = -1;
    float bomb_age_accumulator = 0;
    u32 bomb_generation = 0;
    struct BombRay {
        Vec3 origin{}, center{};
        float angle = 0;
    };
    std::array<BombRay, 5> bomb_rays{};
    std::array<PlayerBombOrb, 8> bomb_orbs{};
    float bomb_angle = -1.5707964f;
    float bomb_center_x = 0, bomb_center_y = 0;
    float bomb_rotation_speed = 0;
    float bomb_visual_angle = 0;
    i32 bomb_duration = 0;
    YoumuBomb youmu_bomb;
    u32 input_state = 0;
    struct TouchMotion {
        int mode = 0;
        float x = 0, y = 0;
    } touch;
    i32 speed_orth = 576, speed_diag = 407;
    i32 speed_orth_focus = 256, speed_diag_focus = 181;
    std::array<std::array<std::array<i32, 2>, 10>, 2> option_offsets{};
    std::array<std::vector<ShtShotRecord>, 11> sht_shots;
    bool sht_shots_loaded = false;
    i32 character = 0;
    float hit_size = 2.0f;
    bool trance_protection = false;
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
    StagePlayerState::RespawnDamage& respawn_damage = player_state_.respawn_damage;
    Vec3& respawn_origin = player_state_.respawn_origin;
    i32& bomb_timer = player_state_.bomb_timer;
    i32& bomb_age = player_state_.bomb_age;
    float& bomb_age_accumulator_ = player_state_.bomb_age_accumulator;
    u32& bomb_generation = player_state_.bomb_generation;
    using BombRay = StagePlayerState::BombRay;
    std::array<BombRay, 5>& bomb_rays = player_state_.bomb_rays;
    Vec3 bomb_center() const noexcept { return {bomb_center_x_, bomb_center_y_, 0}; }
    float bomb_visual_angle() const noexcept { return bomb_visual_angle_; }
    std::array<PlayerBombOrb, 8>& bomb_orbs = player_state_.bomb_orbs;
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
    std::function<bool(const Vec3&, const Vec3&, float, i32)> beam_hit_effect;
    std::function<bool(StageBattle&)> option_power_changed;
    bool& death_option_rebuild = player_state_.death_option_rebuild;
    std::function<bool(PlayerShot&, u32)> player_shot_birth;
    AnmRegistry* mp_registry_hook = nullptr;
    AnmRegistry* mp_registry() { return mp_registry_hook; }
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
    i32& speed_orth = player_state_.speed_orth;
    i32& speed_diag = player_state_.speed_diag;
    i32& speed_orth_focus = player_state_.speed_orth_focus;
    i32& speed_diag_focus = player_state_.speed_diag_focus;
    StagePlayerState::TouchMotion& touch = player_state_.touch;
    u32& input_state = player_state_.input_state;
    YoumuBomb& youmu_bomb_ = player_state_.youmu_bomb;

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
    static constexpr i32 kTranceExitInvulnerability = 70;
    static constexpr float kTranceDrainPerFrame = 3.0f;

    bool load(EclResourceProvider& provider, const char* stage_file, i32 difficulty_rank, StageBattleServices& host,
              SessionState& progress, HudPlayerState& player, HudScoreState& score, const char* event_path = nullptr,
              const char* sht_name = nullptr);
    bool prepare_player(EclResourceProvider& provider, SessionState& progress, HudPlayerState& player,
                        const char* sht_name, bool cold_entry);
    bool step(const GameInput& input);
    bool step_transition(const GameInput& input);
    bool defer_item_side = false;
    bool protect_all_seats = false;
    void update_items_phase();
    void update_spirits_phase();
    std::array<ItemCollector, 3> make_seat_collectors();
    bool continue_player();
    void on_bomb_key();
    bool end_trance();
    bool open_event_sink(const char* path);
    const std::string& error() const { return error_; }

    void begin_player_shot_geometry_snapshot(i32 seat = -1);
    void set_player_shot_geometry(i32 slot, i32 age, i32 script, u32 generation, const std::array<Vec3, 4>& quad,
                                  i32 seat = -1);

    static constexpr u32 kShtBlocks = 11;
    std::array<std::vector<ShtShotRecord>, kShtBlocks>& sht_shots = player_state_.sht_shots;
    bool& sht_shots_loaded = player_state_.sht_shots_loaded;

    i32 seat_count = 0;
    i32 active_seat = 0;
    std::array<StagePlayerState, 3> seats{};
    std::array<HudPlayerState, 3> seat_huds{};
    std::array<i32, 3> seat_characters{};
    std::array<GameInput, 3> seat_inputs{};
    enum class TranceMode : i32 { Idle = 0, Manual = 1, DeathProtection = 2 };
    TranceMode trance_mode = TranceMode::Idle;
    i32 trance_owner_seat = 0;
    u32 trance_generation = 0;
    u32 death_trance_generation = 0;
    bool team_trance_active() const { return progress_ && trance_active(*progress_); }
    bool seat_trance_active() const { return team_trance_active(); }
    bool bomb_allowed() const { return !challenge && !team_trance_active(); }
    bool challenge = false;
    bool keep_bullets_on_miss = false;
    void apply_challenge_locks();
    void swap_player_context(i32 seat);
    void restore_seat_context(i32 restore_seat);
    bool enable_seats(EclResourceProvider& provider, i32 count, const std::array<i32, 3>& characters);
    i32 mp_local_seat = 0;
    bool step_mp();
    bool update_trance_mp();
    i32 ghost_count() const {
        i32 count = 0;
        for (i32 seat = 0; seat < seat_count; ++seat)
            if (seats[size_t(seat)].ghost) ++count;
        return count;
    }
    bool team_wiped() const { return seat_count >= 2 && ghost_count() == seat_count; }
    bool rescue_seat(i32 donor, i32 recipient) {
        if (seat_count < 2 || donor == recipient) return false;
        if (donor < 0 || donor >= seat_count || recipient < 0 || recipient >= seat_count) return false;
        auto& donor_hud = seat_huds[size_t(donor)];
        auto& recipient_hud = seat_huds[size_t(recipient)];
        if (!seats[size_t(recipient)].ghost || seats[size_t(recipient)].ghost_just_entered) return false;
        if (seats[size_t(donor)].ghost || seats[size_t(donor)].dead) return false;
        if (donor_hud.extra_lives <= 0) return false;
        --donor_hud.extra_lives;
        seats[size_t(recipient)].ghost = false;
        seats[size_t(recipient)].dead = false;
        seats[size_t(recipient)].final_death = false;
        recipient_hud.bombs = challenge ? 0 : 2;
        recipient_hud.bomb_pieces = 0;
        recipient_hud.extra_lives = std::max(0, recipient_hud.extra_lives);
        recipient_hud.power = recipient_hud.max_power / 2;
        seats[size_t(recipient)].life_transfer_state = 0;
        seats[size_t(recipient)].power_transfer_state = 0;
        seats[size_t(recipient)].phase = StagePlayerState::Phase::Active;
        seats[size_t(recipient)].phase_frame = 0;
        seats[size_t(recipient)].invulnerable = 240;
        return true;
    }
    void revive_all_on_stage_clear() {
        for (i32 seat = 0; seat < seat_count; ++seat) {
            if (!seats[size_t(seat)].ghost) continue;
            seats[size_t(seat)].ghost = false;
            seats[size_t(seat)].dead = false;
            seats[size_t(seat)].final_death = false;
            seat_huds[size_t(seat)].bombs = challenge ? 0 : 2;
            seat_huds[size_t(seat)].bomb_pieces = 0;
            seat_huds[size_t(seat)].extra_lives = std::max(0, seat_huds[size_t(seat)].extra_lives);
            seat_huds[size_t(seat)].power = seat_huds[size_t(seat)].max_power / 2;
            seats[size_t(seat)].life_transfer_state = 0;
            seats[size_t(seat)].power_transfer_state = 0;
            seats[size_t(seat)].phase = StagePlayerState::Phase::Active;
            seats[size_t(seat)].phase_frame = 0;
            seats[size_t(seat)].invulnerable = 120;
        }
    }
    const StagePlayerState& player_state() const { return player_state_; }
    const HudPlayerState* seat_hud(i32 seat) const {
        return seat >= 0 && seat < i32(seat_huds.size()) ? &seat_huds[size_t(seat)] : nullptr;
    }

private:
    bool team_bomb_protected() const;
    void advance_mp_protection_and_ghosts();
    void share_mp_bomb_protection();
    bool retire_final_death_combat();
    void update_transfers();
    bool player_prepared_ = false;
    void rebuild_options_for_power();
    void collide_player_bullet_seat(StageBullet& bullet, i32 seat);
    int classify_player_bullet(const StagePlayerState& state, const StageBullet& bullet) const;
    bool load_seat_sht(EclResourceProvider& provider, i32 character);
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
    bool enemy_body_collidable(const StageEnemy& enemy) const;
    int enemy_body_contact(const StageEnemy& enemy, float px, float py, float hit_size) const;
    static bool enemy_body_graze_awards(const StageEnemy& enemy);
    void collide_enemy_body(StageEnemy& enemy);
    void consume_player_shot_geometry();
    void collide_player_bullet(StageBullet& bullet);
    i32 laser_contact(const StagePlayerState& state, const StageLaser&) const;
    static bool laser_graze_awards(const StageLaser&);
    i32 collide_laser(const StageLaser&);
    void reward_kill(StageEnemy& enemy);
    void update_item_side();
    ItemCollector make_item_collector();
    void emit_item_events();
    bool update_trance(const GameInput& input);
    bool kill_player();
    bool commit_death();
    bool commit_death_normal();
    bool prepare_expired_deathbombs(std::array<bool, 3>&);
    bool update_player(const GameInput&, bool deathbomb_prepared = false);
    StageBattleServices* host_ = nullptr;
    SessionState* progress_ = nullptr;
    HudPlayerState* player_ = nullptr;
    HudScoreState* score_ = nullptr;
    Timer& player_phase_clock_ = player_state_.phase_clock;
    Timer& fire_clock_ = player_state_.fire_clock;
    std::array<std::array<std::array<i32, 2>, 10>, 2>& option_offsets_ = player_state_.option_offsets;
    std::array<std::array<i32, 2>, 8>& option_fixed_ = player_state_.option_fixed;
    std::array<std::array<i32, 2>, 33>& movement_history_ = player_state_.movement_history;
    i32& option_power_ = player_state_.option_power;
    bool& option_reset_pending_ = player_state_.option_reset_pending;
    bool& option_movement_tick_ = player_state_.option_movement_tick;
    float& bomb_angle_ = player_state_.bomb_angle;
    float& bomb_center_x_ = player_state_.bomb_center_x;
    float& bomb_center_y_ = player_state_.bomb_center_y;
    float& bomb_rotation_speed_ = player_state_.bomb_rotation_speed;
    float& bomb_visual_angle_ = player_state_.bomb_visual_angle;
    i32& bomb_duration_ = player_state_.bomb_duration;
    i32& character_ = player_state_.character;
    bool life_probe_ = false;
    bool events_ = false;
    bool sink_open_ = false;
    float& player_hit_size_ = player_state_.hit_size;
    bool& trance_protection_ = player_state_.trance_protection;
    i32 trance_task_frame_ = 0;
    std::string error_;
    friend class multiplayer::WorldState;
    friend struct multiplayer::StageEntry;
    std::vector<PlayerShotGeometrySnapshot>& shot_geometry_ = player_state_.shot_geometry;
    u32& shot_generation_ = player_state_.shot_generation;
};

}
