#pragma once

#include "CurveLaserPath.hpp"
#include "EclResource.hpp"
#include "EclThreads.hpp"
#include "EnemyFog.hpp"
#include "Rng.hpp"
#include "SpellState.hpp"
#include "StageEvent.hpp"
#include "Types.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace th13 {

struct SessionState;
class AnmVm;

struct StagePolarItp {
    float from_a = 0, from_b = 0, to_a = 0, to_b = 0;
    float duration = 0;
    float elapsed = 0;
    i32 mode = 0;
    bool active() const { return i32(duration) > 0; }
    void setup(float dur, i32 new_mode, float fa, float fb, float ta, float tb) {
        duration = dur;
        elapsed = 0;
        mode = new_mode;
        from_a = fa;
        from_b = fb;
        to_a = ta;
        to_b = tb;
    }
};

struct StagePosItp {
    float start[3] = {0, 0, 0}, target[3] = {0, 0, 0};
    float p1[3] = {0, 0, 0}, p2[3] = {0, 0, 0};
    float duration = 0;
    float elapsed = 0;
    i32 mode = 0;
    bool per_axis = false;
    i32 axis_mode[3] = {};
    bool active() const { return i32(duration) > 0; }
};

struct StageMover {
    float pos[3] = {0, 0, 0};
    float speed = 0, angle = 0;
    float radius = 0, growth = 0;
    float ellipse_angle = 0, ellipse_ratio = 0;
    float origin[3] = {0, 0, 0};
    float sine_phase = 0;
    float aux[3] = {0, 0, 0};
    u32 mode = 0;
    StagePolarItp polar;
    FloatInterpolation<2> radial;
    StagePosItp postime;
    void advance(float rate);
    void accumulate(float rate = 1.0f);
};

struct AnmEffectRequest {
    i32 file = 0, script = 0;
    float x = 0, y = 0, z = 0;
    u8 kind = 0;
    u16 opcode = 0;
    i32 slot = 0;
    u32 serial = 0;
};

struct StageEt {
    u32 sprite = 0, subtype = 0;
    float spawn[3] = {0, 0, 0};
    float laser_width = 0;
    i32 sound = 21, transform_sound = 38;
    i32 laser_timing[5] = {};
    float angle1 = 0, angle2 = 0;
    float speed1 = 2.0f, speed2 = 0;
    float spin = 0;
    i16 count = 1, ways = 1;
    i16 aim = 0;
    float offset[3] = {0, 0, 0};
    float absolute[3] = {0, 0, 0};
    struct Ex {
        u32 op = 0;
        u32 flags = 0;
        float args[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        std::array<i32, 4> integer_arguments{};
        const char* subroutine = nullptr;
    } ex[18];
    u32 ex_next = 0;
    void reset();
};

struct StageLaser {
    enum class Kind { Infinite, Line, Curve, Beam };
    Kind kind = Kind::Infinite;
    i32 id = 0;
    u32 generation = 0;
    float origin[3] = {}, velocity[3] = {};
    float angle = 0, rotation = 0;
    float length = 0, target_length = 0, length_speed = 0;
    float width = 2, target_width = 0, start_offset = 0;
    i32 timing[4] = {};
    i32 time = 0, previous_time = -1;
    float time_fraction = 0;
    i32 phase = 3;
    u32 flags = 0, sprite = 0, subtype = 0;
    u32 beam_descriptor_flags = 0;
    i32 sound = -1, transform_sound = -1;
    StageEt::Ex ex[18];
    i32 ex_cursor = 0, cancel_protection = 0;
    u32 behavior_flags = 0;
    u32 anm_flags = 0x03000020;
    bool active = true;
    bool pending_remove = false;
    bool has_updated = false;
    float line_limit = 0;
    float line_descriptor_angle = 0, line_initial_length = 0;
    CurveLaserPath curve_path;
    std::vector<CurveLaserSample> curve_samples;
    i32 acceleration_time = 0, acceleration_duration = 0;
    float acceleration_fraction = 0;
    float acceleration = 0, acceleration_velocity[3] = {};
    i32 wait_remaining = 0;
    float wait_fraction = 0;
    i32 cull_timer = 0, graze_timer = 0;
    bool collision_enabled() const {
        if (kind == Kind::Beam) return false;
        return length > 16 && (kind == Kind::Line ? width > 3 : phase == 4 || phase == 2);
    }
    float collision_width() const;
    i32 player_contact(float x, float y, float half_x, float half_y) const;
    bool update(const float* first_boss_position, float rate = 1);
};

struct StageSpawnArguments {
    float z = 0;
    i32 score = 0, drop_main = 0;
    std::array<i32, 4> integers{};
    std::array<float, 8> floats{};
};

struct StageEnemy {
    i32 native_function = 0;
    i32 native_function_mode = 0;
    std::unique_ptr<EnemyFog> fog;
    u32 serial = 0;
    i32 script_id = 0;
    i32 trace_id = 0;
    std::string routine;
    float life = 0;
    i32 spawn_score = 0;
    float bullet_protect_radius2 = 0;
    StageMover vel, pos;
    float position[3] = {0, 0, 0};
    float last_hit_position[2] = {0, 0};
    float motion_delta[3] = {};
    i32 death_sound = 0, death_effect = 0, death_effect_file = 0;
    float age = 0;
    u32 flags = 0;
    float hurtbox[2] = {24.0f, 24.0f};
    float hitbox[2] = {24.0f, 24.0f};
    float body_angle = 0.0f;
    float limit_center[2] = {0, 0};
    float limit_half[2] = {0, 0};
    StageEt et[8];
    i32 i_slots[4] = {0, 0, 0, 0};
    float f_slots[8] = {};
    std::unique_ptr<EclThreads> threads;
    bool dead = false;
    bool tick_gate = false;
    std::string interrupt_label;
    std::array<EclContext::Interrupt, 8> interrupts{};
    i32 invulnerable = 0;
    Timer invulnerable_clock;
    i32 body_invulnerable = 0;
    Timer body_invulnerable_clock;
    i32 damage_function = 0;
    i32 pending_damage = 0;
    bool spell_active = false;
    i32 spell_base_life = 0, spell_life = 0;
    i32 spell_id = -1;
    std::string spell_name;
    i32 phase_life_remaining = 0, phase_life_threshold = 0;
    bool phase_timed_out = false;
    i32 drop_main = 0;
    i32 drop_extra[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float drop_area[2] = {32, 32};
    float spirit_accum = 0;
    i32 spirit_threshold = 0;
    i32 hit_anim_timer = 0;
    i32 hit_feedback_cooldown = 0;
    i32 hit_sound = -1;
    bool periodic_flash = false;
    i32 death_spirit_limit = 60;
    i32 death_spirit_count = 1;
    bool damage_spirits_disabled = false;
    std::string death_routine;
    i32 anm_file = 0;
    i32 anm_base_script = 0;
    i32 anm_slot0_bank = -1;
    i32 anm_slot0_script_offset = 0;
    static constexpr i32 kAnimationSlots = 14;
    std::array<i32, kAnimationSlots> anm_script = [] {
        std::array<i32, kAnimationSlots> value{};
        value.fill(-1);
        return value;
    }();
    u32 anm_slot_handle[kAnimationSlots] = {};
    Vec3 anm_slot_offset[kAnimationSlots] = {};
    float anm_size_a[2] = {0, 0};
    float anm_size_b[2] = {0, 0};
    i32 mirror_dir = 0;
    float mirror_velocity_x = 0;
    bool mirror_ready = false;
    float mirror_prev_x = 0;
    i32 mirror_script_base = 1;
};

struct StageBullet {
    i32 pool_slot = -1;
    float x = 0, y = 0, z = 0, angle = 0, speed = 0;
    float vel[3] = {0, 0, 0};
    u32 sprite = 0, subtype = 0;
    float hit_size[2] = {4.0f, 4.0f};
    u32 flags = 0x12;
    float hit_scale = 1.0f;
    FloatInterpolation<1> scale_tween;
    bool alive = true;
    bool grazed = false;
    StageEt::Ex ex[18];
    i32 ex_cursor = 0;
    i32 native_trigger = 0;
    i32 transform_sound = 38;
    u32 behavior_flags = 0;
    float b98_mirror = 0;
    u16 state = 1;
    u32 source_enemy = 0;
    u32 generation = 0;
    i32 delay = 0;
    bool animation_ready = true;
    i32 animation_interrupt = 0;
    i32 animation_blend = -1;
    i32 state_age = 0;
    i32 cancel_protection = 0;
    float state_time = 0;
    bool collision_ready = true;
    bool animation_geometry = false;
    float sprite_width = 0, sprite_height = 0;
    i32 cull_grace = 5;
    float steer[3] = {0, 0, 0};
    float ex4_acceleration = 0;
    i32 ex4_duration = 0;
    i32 ex4_timer = 0;
    float ex4_time = 0;
    i32 ex1_timer = 0;
    float ex1_time = 0;
    float ex8_speed_delta = 0;
    float ex8_angle_delta = 0;
    i32 ex8_duration = 0;
    i32 ex8_timer = 0;
    float ex8_time = 0;
    float ex10_speed = 0;
    float ex10_angle = 0;
    i32 ex10_duration = 0;
    i32 ex10_repeats = 0;
    i32 ex10_count = 0;
    i32 ex10_mode = 0;
    i32 ex10_timer = 0;
    float ex10_time = 0;
    float ex40_speed = 0;
    float ex40_w = 384.0f;
    float ex40_h = 448.0f;
    i32 ex40_bounces = 0;
    i32 ex40_limit = 0;
    u32 ex40_mode = 0;
    i32 offscreen_timer = 0;
    float offscreen_time = 0;
    i32 offscreen_directional = 0;
    i32 ex1000_count = 0;
    i32 ex1000_limit = 0;
    u32 ex1000_mask = 0;
    i32 wait_timer = 0;
    float wait_time = 0;
    float saved_speed = 0, saved_angle = 0, saved_pos[3] = {0, 0, 0};
    i32 target_speed_time = 0, target_speed_duration = 0;
    float target_speed_fraction = 0;
    float target_speed_step = 0, target_speed_velocity[3] = {};
};

class StageSim;
using EclSpriteObserver = void (*)(i32, u32, i32, i32, i32);
extern EclSpriteObserver ecl_sprite_observer;

struct StageItemHost {
    virtual ~StageItemHost() = default;
    virtual void spawn_item(float x, float y, float z, i32 kind, float angle, float speed) = 0;
    virtual void spawn_value_item(i32 kind, float x, float y, float z, float base_speed) = 0;
    virtual void spawn_value_item(i32 kind, float x, float y, float z, float base_speed, bool sound) {
        (void)sound;
        spawn_value_item(kind, x, y, z, base_speed);
    }
    virtual void on_et_clear(float x, float y, float z) = 0;
};

struct StageDialogHost {
    virtual ~StageDialogHost() = default;
    virtual void open_dialog(i32 entry) = 0;
    virtual bool dialog_active() const = 0;
    virtual bool dialog_wait_released() const { return !dialog_active(); }
};

class StageGvars final : public EclVariables {
public:
    StageSim* sim = nullptr;
    bool integer(i32 id, i32& value) override;
    bool floating(i32 id, float& value) override;
    i32* integer_destination(i32 id) override;
    float* float_destination(i32 id) override;

private:
    std::vector<std::pair<i32, i32>> writable_ints_;
};

class StageCommands final : public EclCommands {
public:
    StageSim* sim = nullptr;
    int execute(EclContext& context, u16 opcode) override;
    int unimplemented(u16 opcode);
    int execute_300(EclContext& context, u16 opcode);
    int execute_400(EclContext& context, u16 opcode);
    int execute_500(EclContext& context, u16 opcode);
    int execute_600(EclContext& context, u16 opcode);
    int execute_700(EclContext& context, u16 opcode);
    int execute_800(EclContext& context, u16 opcode);
    int execute_900(EclContext& context, u16 opcode);
    int execute_1000(EclContext& context, u16 opcode);

private:
    int create_enemy(EclContext& context, u16 opcode);
    int fire_et(EclContext& context, StageEnemy& enemy);
};

class StageSim {
    GameRandom owned_random_{0};
    float standalone_rate_ = 1;

public:
    explicit StageSim(GameRandom* shared_random = nullptr) : random(shared_random ? *shared_random : owned_random_) {
        for (i32 slot = 0; slot < 2000; ++slot)
            bullet_free_slots_.push_back(slot);
    }
    float time_rate() const;
    void set_time_rate(float rate);
    std::function<void(float)> time_rate_changed;
    bool load(const char* resource_dir, const char* event_path, const char* stage = nullptr);
    bool begin(EclResourceProvider& provider, const char* stage_file, i32 difficulty, const char* event_path = nullptr);
    bool run(u32 frames);
    bool start();
    bool step_frame();
    std::string debug_bullet_queue() const;
    const std::string& error() const { return error_; }
    u32 spawn_count() const { return spawned_; }
    u64 event_count() const { return sink.emitted(); }

    EclProgram program;
    GameRandom& random;
    StageGvars gvars;
    StageCommands commands;
    StageEventSink sink;
    std::vector<std::unique_ptr<StageEnemy>> enemies;
    std::vector<StageBullet> bullets;
    std::function<bool(StageBullet&, bool)> bullet_animation;
    std::function<bool(StageBullet&, i32)> bullet_animation_initialize;
    std::function<bool(i32, const Vec3&)> scene_effect;
    std::function<bool(i32, i32, i32)> screen_shake;
    std::function<void(i32, i32, u32, float, float)> stage_fog;
    std::function<bool(i32)> stage_script_interrupt;
    std::function<bool()> stage_logo;
    float bullet_bounce_size[2] = {0, 0};
    std::vector<StageLaser> lasers;
    float active_bullet_protect_radius2 = 0;
    std::function<bool(StageLaser&)> laser_birth, laser_animation;
    std::function<void(u32)> laser_retire;
    std::function<bool(i32)> laser_sound;
    std::function<bool(i32, const Vec3&)> laser_cancel_effect;
    std::function<i32(const StageLaser&)> laser_collision;
    std::array<Vec3, 3> laser_collision_centers{};
    i32 laser_collision_count = 0;
    std::function<void(StageBullet&)> bullet_player_collision;
    bool defer_laser_update = false;
    bool defer_bullet_update = false;
    StageEnemy* current = nullptr;
    u32 frame = 0;
    float player_x = 192.0f, player_y = 420.0f;
    i32 boss_slot[3] = {-1, -1, -1};
    i32 boss_timer_seconds = -1, boss_timer_centiseconds = 0;
    bool boss_leaving = false;
    struct BossMarker {
        float ratio = 0;
        i32 color = 0;
    };
    std::array<std::array<BossMarker, 4>, 3> boss_markers{};
    i32 boss_remaining_stars = 0;
    std::vector<std::string> anim_list;
    std::vector<AnmEffectRequest> anm_effects;
    std::function<bool(StageEnemy*)> flush_anm_slots;
    std::function<bool(i32, i32, const Vec3&)> ecl_effect_create;
    std::function<AnmVm*(StageEnemy&, i32)> enemy_animation;
    std::function<bool(StageEnemy&, i32, i32)> enemy_animation_interrupt;
    std::function<bool(StageEnemy&, bool)> enemy_fog;
    std::function<bool(StageEnemy&)> enemy_absorb_spirits;
    std::function<bool(i32)> enemy_sound;
    std::function<bool(i32, float)> enemy_position_sound;
    std::function<bool(i32, float)> enemy_hit_sound;
    std::function<bool(i32, i32, const Vec3&, float)> enemy_death_effect;
    std::function<bool(i32, const Vec3&, i32)> player_effect;
    bool anm_effects_noop = false;
    bool ecl_slot_anm = false;
    std::string error_;
    bool interactive = false;
    i32 difficulty_rank = 0x08;
    SessionState* session = nullptr;
    SpellState spell;
    bool bomb_active = false;
    std::function<void(i32)> spell_award;
    std::function<bool(const SpellEvent&, const StageEnemy&)> spell_event;
    std::function<void(StageEnemy&)> enemy_hit;
    std::function<void(StageEnemy&)> enemy_body_collision;
    bool emit_spell(const SpellEvent& event, const StageEnemy& enemy);
    i32 diff = 3;
    i32 rank = 0;
    i32 player_lives = 0;
    std::array<i32, 4> shared_integers{};
    std::array<float, 8> shared_floats{};

    StageEnemy* find_by_script_id(i32 id);
    StageEnemy* boss() const;
    i32 boss_nonspell_frame = -1;
    u32 boss_main_serial = 0xFFFFFFFF;
    int boss_dialog_wait() const { return 0; }
    StageDialogHost* dialog_host = nullptr;
    bool replay_events = false;
    std::function<void(u32)> retire_sink;
    StageItemHost* items = nullptr;
    u32 mode_flags = 0;
    i32 last_created_enemy_id = 0;
    StageEnemy* spawn_enemy(const char* routine, float x, float y, float life, StageEnemy* parent, bool inherit_anm,
                            bool mirror = false, const StageSpawnArguments* arguments = nullptr);
    void damage(StageEnemy& enemy, float amount);
    bool death_effects_and_drops(StageEnemy& enemy);
    bool clear_enemies();
    void update_enemy_feedback(StageEnemy& enemy);
    bool invoke_enemy_function(StageEnemy&, i32 index);
    void clear_bullets();
    bool cancel_bullet(StageBullet&, bool interpolate = true, bool set_delay = true, bool require_active = true);
    bool cancel_respawn_bullets(const Vec3& center, float radius);
    bool cancel_bullet_state4(StageBullet&, bool spawn_cancel, bool item_after_half_step, bool respect_protection);
    std::function<bool(i32, const Vec3&)> stationary_cancel_effect;
    std::function<bool(i32, const Vec3&, const Vec3&)> bullet_cancel_effect;
    std::function<bool(const Vec3&)> graze_effect;
    void process_bullet_queue(StageBullet& bullet, bool spawn_time);
    void advance_bullets();
    void drop_table(StageEnemy& enemy);
    void cancel_bullets_radius(const StageEnemy& enemy, float radius, bool spawn_cancel);
    void cancel_bullets_radius(const Vec3& center, float radius, bool spawn_cancel, bool respect_protection = false);
    void cancel_bullets_box(const Vec3& center, const Vec3& size, float angle, float bullet_radius, bool spawn_cancel,
                            bool respect_protection = false, bool use_bullet_radius = false);
    void emit_et(const StageEt& et, u32 enemy_serial, u32 slot, const float spawn[3], float aim, u32 ex_seed = 0,
                 bool latch_sound = true);
    void advance_lasers();
    void cancel_lasers(bool reset_protection = true);
    bool cancel_laser(StageLaser&);
    void cancel_lasers_radius(const Vec3&, float radius, bool respect_protection = true);
    void cancel_lasers_box(const Vec3&, const Vec3& size, float angle, bool items = false,
                           bool respect_protection = true);
    void split_laser(size_t index, const Vec3&, const Vec3& size, float angle, bool circle, bool items,
                     bool respect_protection);
    bool laser_effect(const StageLaser&, const Vec3&, bool rectangle = false);
    void append_laser(StageLaser);
    StageLaser* find_laser(i32 id);
    void emit_infinite_laser(const StageEt&, const StageEnemy&, i32 id);
    void emit_beam_laser(const StageEt&, const StageEnemy&, i32 id);
    void emit_line_laser(const StageEt&, const StageEnemy&);
    void emit_curve_laser(const StageEt&, const StageEnemy&);
    void split_curve_laser(size_t index, const Vec3&, const Vec3&, float angle, bool circle, bool spawn_items,
                           bool respect_protection);
    bool advance_line_behaviors(StageLaser&);
    u32 laser_serial = 0x10000;

private:
    void step_bullet_behaviors(StageBullet& bullet);
    void tick_enemy(StageEnemy& enemy);
    bool interrupt_enemy(StageEnemy& enemy);
    void integrate(StageEnemy& enemy);
    bool in_bounds(StageEnemy& enemy);
    u32 spawned_ = 0;
    u32 next_serial_ = 1;
    u32 next_bullet_generation_ = 0;
    std::vector<i32> bullet_free_slots_;
    std::vector<size_t> bullet_order_;
    bool advance_one_bullet(StageBullet& bullet, size_t index, float global_rate);
    std::vector<size_t> bullet_pool_order() const;
    void release_bullet_slot(StageBullet& bullet);
};

}
