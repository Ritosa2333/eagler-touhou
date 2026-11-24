#pragma once
// 全回放回归 digest 的状态遍历器（仅 tests/harness 使用，不进生产构建）。
//
// 目的：给「行为保持」重构提供逐帧、位精确的对照面。每帧把可达的模拟状态按
// 固定顺序喂进 FNV-1a 64；浮点一律按 binary32 位型（memcpy），绝不散列指针、
// 地址或时间。数组/向量只按存储序遍历，绝不遍历 unordered_map/指针键容器。
//
// 覆盖（公开成员可达面）：
//   StageBattle：自机位置/相位/无敌/定点坐标、炸弹计时与公开炸弹字段、自机弹、
//     伤害登记表、妖梦斩击池（visit_state）、options、Bomb 球/射线；
//   StageSim：frame/玩家位置/难度/rank/shared 变量/boss 槽与计时/标记/符卡/
//     敌机（全部 StageEt、ECL 线程 PC/栈/插值通道、fog 顶点）、弹幕、激光
//     （含曲线路径段/采样）、ANM 效果请求队列、ANM 动画名单；
//   ItemManager：道具池/神霊池（存活槽全字段 + 槽号）与事件队列；
//   SessionState/HudPlayerState/HudScoreState 全字段；
//   两条 RNG（game/visual）与 AnmEnvironment 的 ANM 侧 RNG；
//   RunScene 公开字段（age/输入/转场旗标）与 capture_rollback() 的标量面。
//
// 不覆盖（无公开访问器，或属于表现层/不可散列）：
//   StageBattle 私有：phase/fire/startup/charge Timer、炸弹内角/中心/时长、
//     trance 保护与任务计时、option_offsets_、character_（经 player_character()
//     覆盖）、player_state_ 的 shot_geometry/movement_history/定点 option 槽；
//   StageSim 私有：spawned_/next_serial_/next_bullet_generation_、bullet 空闲槽
//     与遍历序 bullet_order_（向量存储序不等于模拟遍历序）、gvars 私有可写表
//     （-9949/-9948/-9947 经公开 integer() 读取覆盖）、owned_random_；
//   ItemManager 私有：空闲槽栈、spirit 序列/溢出/窗口计数、cancel 计数、
//     last_player_ 快照；
//   AnmManager/AnmRegistry/StagePresentation/StageScenery：指针键容器与表现层
//     内部状态（banks/nodes/fog_handles_ 等不得散列），视觉结果由 draws digest
//     覆盖；AnmEnvironment 的指针成员（shared_game_rng/chapter/fallback）不散列；
//   EclContext::instruction、StageEt::Ex::subroutine、各 std::function 回调；
//   StageEnemy 槽位 ANM 句柄只散列占用位（0/1），不散列句柄值——句柄分配序属
//     表现层，视觉差异由 draws digest 负责。
#include "game/AnmEnvironment.hpp"
#include "game/CurveLaserPath.hpp"
#include "game/EclContext.hpp"
#include "game/EclThreads.hpp"
#include "game/EnemyFog.hpp"
#include "game/Interpolation.hpp"
#include "game/ItemManager.hpp"
#include "game/Rng.hpp"
#include "game/RunScene.hpp"
#include "game/SessionState.hpp"
#include "game/SpellState.hpp"
#include "game/StageBattle.hpp"
#include "game/StageSim.hpp"
#include "game/Timer.hpp"
#include "game/Types.hpp"

#include <cstdint>
#include <cstring>

namespace th13 {
namespace harness {

// FNV-1a 64 累加器：域分隔（tag）、显式宽度整数、float 位型与原始字节。
// 注意：成员短名 u8/u32/... 会遮蔽同名类型，类内一律用 th13:: 限定类型。
class Digest64 final {
public:
    static constexpr th13::u64 kOffset = 0xcbf29ce484222325ull;
    static constexpr th13::u64 kPrime = 0x100000001b3ull;

    void byte(th13::u8 value) noexcept {
        state_ ^= value;
        state_ *= kPrime;
    }
    void bytes(const void* data, size_t size) noexcept {
        const auto* p = static_cast<const th13::u8*>(data);
        for (size_t i = 0; i < size; ++i)
            byte(p[i]);
    }
    // 域分隔：长度 + 字节。长度参与散列，避免 "ab"+"c" 与 "a"+"bc" 同流。
    void tag(const char* text) noexcept {
        const size_t size = std::strlen(text);
        u32(th13::u32(size));
        bytes(text, size);
    }
    void u8_value(th13::u8 value) noexcept { byte(value); }
    void u16_value(th13::u16 value) noexcept { u32_value(th13::u32(value)); }
    void u32_value(th13::u32 value) noexcept {
        byte(static_cast<th13::u8>(value));
        byte(static_cast<th13::u8>(value >> 8));
        byte(static_cast<th13::u8>(value >> 16));
        byte(static_cast<th13::u8>(value >> 24));
    }
    void u64_value(th13::u64 value) noexcept {
        u32_value(static_cast<th13::u32>(value));
        u32_value(static_cast<th13::u32>(value >> 32));
    }
    void i32_value(th13::i32 value) noexcept { u32_value(static_cast<th13::u32>(value)); }
    void i64_value(th13::i64 value) noexcept { u64_value(static_cast<th13::u64>(value)); }
    void bool_value(bool value) noexcept { byte(value ? static_cast<th13::u8>(1) : static_cast<th13::u8>(0)); }
    void f32_value(float value) noexcept {
        th13::u32 bits = 0;
        std::memcpy(&bits, &value, sizeof bits);
        u32_value(bits);
    }
    // 兼容短名（遍历代码可读性）。
    void u8(th13::u8 value) noexcept { u8_value(value); }
    void u16(th13::u16 value) noexcept { u16_value(value); }
    void u32(th13::u32 value) noexcept { u32_value(value); }
    void u64(th13::u64 value) noexcept { u64_value(value); }
    void i32(th13::i32 value) noexcept { i32_value(value); }
    void i64(th13::i64 value) noexcept { i64_value(value); }
    void f32(float value) noexcept { f32_value(value); }
    void boolean(bool value) noexcept { bool_value(value); }

    th13::u64 value() const noexcept { return state_; }
    void set(th13::u64 state) noexcept { state_ = state; }

private:
    th13::u64 state_ = kOffset;
};

// ---- 基础值 ----
inline void digest_value(Digest64& d, const Timer& v) {
    d.i32(v.previous);
    d.i32(v.current);
    d.f32(v.fractional);
    d.u32(v.rate_index);
    d.u32(v.flags);
}
inline void digest_value(Digest64& d, const Vec2& v) {
    d.f32(v.x);
    d.f32(v.y);
}
inline void digest_value(Digest64& d, const Vec3& v) {
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.z);
}
inline void digest_value(Digest64& d, const AnmGeometryVertex& v) {
    digest_value(d, v.position);
    d.f32(v.reciprocal_w);
    d.u32(v.color);
    digest_value(d, v.uv);
}
inline void digest_value(Digest64& d, const std::string& v) {
    d.u64(v.size());
    d.bytes(v.data(), v.size());
}

// ---- 插值与 mover ----
template <u32 N, bool Angle> void digest_value(Digest64& d, const FloatInterpolation<N, Angle>& v) {
    for (float value : v.start)
        d.f32(value);
    for (float value : v.end)
        d.f32(value);
    for (float value : v.control1)
        d.f32(value);
    for (float value : v.control2)
        d.f32(value);
    for (float value : v.current)
        d.f32(value);
    digest_value(d, v.timer);
    d.i32(v.duration);
    d.i32(v.mode);
}
template <u32 N> void digest_value(Digest64& d, const IntegerInterpolation<N>& v) {
    for (i32 value : v.start)
        d.i32(value);
    for (i32 value : v.end)
        d.i32(value);
    for (i32 value : v.control1)
        d.i32(value);
    for (i32 value : v.control2)
        d.i32(value);
    for (i32 value : v.current)
        d.i32(value);
    digest_value(d, v.timer);
    d.i32(v.duration);
    d.i32(v.mode);
}
inline void digest_value(Digest64& d, const StagePolarItp& v) {
    d.f32(v.from_a);
    d.f32(v.from_b);
    d.f32(v.to_a);
    d.f32(v.to_b);
    d.f32(v.duration);
    d.f32(v.elapsed);
    d.i32(v.mode);
}
inline void digest_value(Digest64& d, const StagePosItp& v) {
    for (float value : v.start)
        d.f32(value);
    for (float value : v.target)
        d.f32(value);
    for (float value : v.p1)
        d.f32(value);
    for (float value : v.p2)
        d.f32(value);
    d.f32(v.duration);
    d.f32(v.elapsed);
    d.i32(v.mode);
    d.boolean(v.per_axis);
    for (i32 value : v.axis_mode)
        d.i32(value);
}
inline void digest_value(Digest64& d, const StageMover& v) {
    for (float value : v.pos)
        d.f32(value);
    d.f32(v.speed);
    d.f32(v.angle);
    d.f32(v.radius);
    d.f32(v.growth);
    d.f32(v.ellipse_angle);
    d.f32(v.ellipse_ratio);
    for (float value : v.origin)
        d.f32(value);
    d.f32(v.sine_phase);
    for (float value : v.aux)
        d.f32(value);
    d.u32(v.mode);
    digest_value(d, v.polar);
    digest_value(d, v.radial);
    digest_value(d, v.postime);
}
inline void digest_value(Digest64& d, const EclInterpolation& v) {
    d.i32(v.duration);
    d.i32(v.mode);
    d.f32(v.start);
    d.f32(v.end);
    d.f32(v.control1);
    d.f32(v.control2);
    d.f32(v.current);
    digest_value(d, v.timer);
}

// ---- ECL 线程 ----
// 插值目标只散列 (routine, 指令字节偏移)，不散列地址：跨构建地址必变，
// 而程序内身份是稳定的。无法映射的目标以 0/1/2 区分「空/已映射/域外」。
inline void digest_interpolation_targets(Digest64& d, const EclContext& context) {
    d.tag("ecl.targets");
    for (const EclInstruction* target : context.interpolation_targets) {
        if (!target) {
            d.u8(0);
            continue;
        }
        bool mapped = false;
        const EclProgram* program = context.program;
        if (program) {
            const auto address = reinterpret_cast<std::uintptr_t>(target);
            for (u32 routine = 0; routine < program->definitions.size() && !mapped; ++routine) {
                const EclResource& file = *program->definitions[routine].file;
                const EclSubroutine& sub = program->subroutine(routine);
                if (sub.size < 32 || sub.offset + 16 > file.bytes.size()) continue;
                const auto* code = file.bytes.data() + sub.offset + 16;
                const auto base = reinterpret_cast<std::uintptr_t>(code);
                if (address < base || address - base > sub.size - 32) continue;
                const u32 offset = u32(address - base);
                if (offset % 4) continue;
                // 逐条指令走一遍，确认落在指令边界（与解释器/MP 快照同判）。
                u32 cursor = 0;
                while (cursor < offset) {
                    const auto* instruction = reinterpret_cast<const EclInstruction*>(code + cursor);
                    if (instruction->length < 16 || instruction->length % 4 ||
                        instruction->length > sub.size - 16 - cursor)
                        break;
                    cursor += instruction->length;
                }
                if (cursor == offset) {
                    d.u8(1);
                    d.u32(routine);
                    d.u32(offset);
                    mapped = true;
                }
            }
        }
        if (!mapped) d.u8(2);
    }
}
inline void digest_value(Digest64& d, const EclContext& context) {
    d.f32(context.time);
    d.i32(context.routine);
    d.i32(context.instruction_offset);
    // 栈区全量（4096 字节，零初始化；已弹出字节保留原值，属确定性存储态）。
    d.u64(context.stack.size());
    d.bytes(context.stack.data(), context.stack.size());
    d.i32(context.stack_pointer);
    d.i32(context.local_base);
    d.u8(context.difficulty);
    d.i32(context.thread_id);
    d.i32(context.thread_state);
    d.u32(context.thread_flags);
    d.boolean(context.instruction != nullptr);
    d.boolean(context.code != nullptr);
    d.u32(context.code_size);
    for (const EclInterpolation& channel : context.interpolators)
        digest_value(d, channel);
    digest_interpolation_targets(d, context);
}
inline void digest_value(Digest64& d, const EclThreadsSnapshot& threads) {
    d.tag("ecl.main");
    digest_value(d, threads.main);
    d.u64(threads.children.size());
    for (const EclContext& child : threads.children) {
        d.tag("ecl.child");
        digest_value(d, child);
    }
}

// ---- 敌机 ----
inline void digest_value(Digest64& d, const StageEt::Ex& v) {
    d.u32(v.op);
    d.u32(v.flags);
    for (float value : v.args)
        d.f32(value);
    for (i32 value : v.integer_arguments)
        d.i32(value);
    // v.subroutine：指向 ECL 程序的指针，不散列（见文件头）。
}
inline void digest_value(Digest64& d, const StageEt& v) {
    d.u32(v.sprite);
    d.u32(v.subtype);
    for (float value : v.spawn)
        d.f32(value);
    d.f32(v.laser_width);
    d.i32(v.sound);
    d.i32(v.transform_sound);
    for (i32 value : v.laser_timing)
        d.i32(value);
    d.f32(v.angle1);
    d.f32(v.angle2);
    d.f32(v.speed1);
    d.f32(v.speed2);
    d.f32(v.spin);
    d.i32(v.count);
    d.i32(v.ways);
    d.i32(v.aim);
    for (float value : v.offset)
        d.f32(value);
    for (float value : v.absolute)
        d.f32(value);
    for (const StageEt::Ex& entry : v.ex)
        digest_value(d, entry);
    d.u32(v.ex_next);
}
inline void digest_value(Digest64& d, const EnemyFog& v) {
    d.f32(v.target);
    d.f32(v.radius);
    d.f32(v.angle_x);
    d.f32(v.angle_y);
    d.u32(v.color);
    for (const AnmGeometryVertex& vertex : v.vertices)
        digest_value(d, vertex);
}
inline void digest_value(Digest64& d, const EclContext::Interrupt& v) {
    d.i32(v.life);
    d.i32(v.timeout);
    digest_value(d, v.health_label);
    digest_value(d, v.timeout_label);
}
inline void digest_value(Digest64& d, const StageEnemy& v) {
    d.tag("enemy");
    d.i32(v.native_function);
    d.i32(v.native_function_mode);
    d.u32(v.serial);
    d.i32(v.script_id);
    d.i32(v.trace_id);
    digest_value(d, v.routine);
    d.f32(v.life);
    d.i32(v.spawn_score);
    d.f32(v.bullet_protect_radius2);
    digest_value(d, v.vel);
    digest_value(d, v.pos);
    for (float value : v.position)
        d.f32(value);
    for (float value : v.last_hit_position)
        d.f32(value);
    for (float value : v.motion_delta)
        d.f32(value);
    d.i32(v.death_sound);
    d.i32(v.death_effect);
    d.i32(v.death_effect_file);
    d.f32(v.age);
    d.u32(v.flags);
    for (float value : v.hurtbox)
        d.f32(value);
    for (float value : v.hitbox)
        d.f32(value);
    d.f32(v.body_angle);
    for (float value : v.limit_center)
        d.f32(value);
    for (float value : v.limit_half)
        d.f32(value);
    for (const StageEt& emitter : v.et)
        digest_value(d, emitter);
    for (i32 value : v.i_slots)
        d.i32(value);
    for (float value : v.f_slots)
        d.f32(value);
    d.boolean(v.dead);
    d.boolean(v.tick_gate);
    digest_value(d, v.interrupt_label);
    for (const EclContext::Interrupt& interrupt : v.interrupts)
        digest_value(d, interrupt);
    d.i32(v.invulnerable);
    digest_value(d, v.invulnerable_clock);
    d.i32(v.body_invulnerable);
    digest_value(d, v.body_invulnerable_clock);
    d.i32(v.damage_function);
    d.i32(v.pending_damage);
    d.boolean(v.spell_active);
    d.i32(v.spell_base_life);
    d.i32(v.spell_life);
    d.i32(v.spell_id);
    digest_value(d, v.spell_name);
    d.i32(v.phase_life_remaining);
    d.i32(v.phase_life_threshold);
    d.boolean(v.phase_timed_out);
    d.i32(v.drop_main);
    for (i32 value : v.drop_extra)
        d.i32(value);
    for (float value : v.drop_area)
        d.f32(value);
    d.f32(v.spirit_accum);
    d.i32(v.spirit_threshold);
    d.i32(v.hit_anim_timer);
    d.i32(v.hit_feedback_cooldown);
    d.i32(v.hit_sound);
    d.boolean(v.periodic_flash);
    d.i32(v.death_spirit_limit);
    d.i32(v.death_spirit_count);
    d.boolean(v.damage_spirits_disabled);
    digest_value(d, v.death_routine);
    d.i32(v.anm_file);
    d.i32(v.anm_base_script);
    d.i32(v.anm_slot0_bank);
    d.i32(v.anm_slot0_script_offset);
    for (i32 value : v.anm_script)
        d.i32(value);
    // 句柄只取占用位：分配序属表现层（见文件头）。
    for (u32 handle : v.anm_slot_handle)
        d.boolean(handle != 0);
    for (const Vec3& offset : v.anm_slot_offset)
        digest_value(d, offset);
    for (float value : v.anm_size_a)
        d.f32(value);
    for (float value : v.anm_size_b)
        d.f32(value);
    d.i32(v.mirror_dir);
    d.f32(v.mirror_velocity_x);
    d.boolean(v.mirror_ready);
    d.f32(v.mirror_prev_x);
    d.i32(v.mirror_script_base);
    d.boolean(v.fog != nullptr);
    if (v.fog) digest_value(d, *v.fog);
    d.boolean(v.threads != nullptr);
    if (v.threads) digest_value(d, v.threads->snapshot());
}

// ---- 弹幕 ----
inline void digest_value(Digest64& d, const StageBullet& v) {
    d.tag("bullet");
    d.i32(v.pool_slot);
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.z);
    d.f32(v.angle);
    d.f32(v.speed);
    for (float value : v.vel)
        d.f32(value);
    d.u32(v.sprite);
    d.u32(v.subtype);
    for (float value : v.hit_size)
        d.f32(value);
    d.u32(v.flags);
    d.f32(v.hit_scale);
    d.boolean(v.alive);
    d.boolean(v.grazed);
    for (const StageEt::Ex& entry : v.ex)
        digest_value(d, entry);
    d.i32(v.ex_cursor);
    d.i32(v.native_trigger);
    d.i32(v.transform_sound);
    d.u32(v.behavior_flags);
    d.f32(v.b98_mirror);
    d.u16(v.state);
    d.u32(v.source_enemy);
    d.u32(v.generation);
    d.i32(v.delay);
    d.boolean(v.animation_ready);
    d.i32(v.animation_interrupt);
    d.i32(v.animation_blend);
    d.i32(v.state_age);
    d.i32(v.cancel_protection);
    d.f32(v.state_time);
    d.boolean(v.collision_ready);
    d.boolean(v.animation_geometry);
    d.f32(v.sprite_width);
    d.f32(v.sprite_height);
    d.i32(v.cull_grace);
    for (float value : v.steer)
        d.f32(value);
    d.f32(v.ex4_acceleration);
    d.i32(v.ex4_duration);
    d.i32(v.ex4_timer);
    d.f32(v.ex4_time);
    d.i32(v.ex1_timer);
    d.f32(v.ex1_time);
    d.f32(v.ex8_speed_delta);
    d.f32(v.ex8_angle_delta);
    d.i32(v.ex8_duration);
    d.i32(v.ex8_timer);
    d.f32(v.ex8_time);
    d.f32(v.ex10_speed);
    d.f32(v.ex10_angle);
    d.i32(v.ex10_duration);
    d.i32(v.ex10_repeats);
    d.i32(v.ex10_count);
    d.i32(v.ex10_mode);
    d.i32(v.ex10_timer);
    d.f32(v.ex10_time);
    d.f32(v.ex40_speed);
    d.f32(v.ex40_w);
    d.f32(v.ex40_h);
    d.i32(v.ex40_bounces);
    d.i32(v.ex40_limit);
    d.u32(v.ex40_mode);
    d.i32(v.offscreen_timer);
    d.f32(v.offscreen_time);
    d.i32(v.offscreen_directional);
    d.i32(v.ex1000_count);
    d.i32(v.ex1000_limit);
    d.u32(v.ex1000_mask);
    d.i32(v.wait_timer);
    d.f32(v.wait_time);
    d.f32(v.saved_speed);
    d.f32(v.saved_angle);
    for (float value : v.saved_pos)
        d.f32(value);
    d.i32(v.target_speed_time);
    d.i32(v.target_speed_duration);
    d.f32(v.target_speed_fraction);
    d.f32(v.target_speed_step);
    for (float value : v.target_speed_velocity)
        d.f32(value);
}

// ---- 激光 ----
inline void digest_value(Digest64& d, const CurveLaserSegment& v) {
    d.f32(v.start_time);
    d.f32(v.end_time);
    d.i32(v.mode);
    digest_value(d, v.direction);
    digest_value(d, v.origin);
    d.f32(v.angle);
    d.f32(v.speed);
    d.f32(v.speed_delta);
    d.f32(v.angle_delta);
}
inline void digest_value(Digest64& d, const CurveLaserSample& v) {
    digest_value(d, v.position);
    digest_value(d, v.delta);
    d.f32(v.angle);
    d.f32(v.speed);
}
inline void digest_value(Digest64& d, const StageLaser& v) {
    d.tag("laser");
    d.i32(i32(v.kind));
    d.i32(v.id);
    d.u32(v.generation);
    for (float value : v.origin)
        d.f32(value);
    for (float value : v.velocity)
        d.f32(value);
    d.f32(v.angle);
    d.f32(v.rotation);
    d.f32(v.length);
    d.f32(v.target_length);
    d.f32(v.length_speed);
    d.f32(v.width);
    d.f32(v.target_width);
    d.f32(v.start_offset);
    for (i32 value : v.timing)
        d.i32(value);
    d.i32(v.time);
    d.i32(v.previous_time);
    d.f32(v.time_fraction);
    d.i32(v.phase);
    d.u32(v.flags);
    d.u32(v.sprite);
    d.u32(v.subtype);
    d.u32(v.beam_descriptor_flags);
    d.i32(v.sound);
    d.i32(v.transform_sound);
    for (const StageEt::Ex& entry : v.ex)
        digest_value(d, entry);
    d.i32(v.ex_cursor);
    d.i32(v.cancel_protection);
    d.u32(v.behavior_flags);
    d.u32(v.anm_flags);
    d.boolean(v.active);
    d.boolean(v.pending_remove);
    d.boolean(v.has_updated);
    d.f32(v.line_limit);
    d.f32(v.line_descriptor_angle);
    d.f32(v.line_initial_length);
    d.u64(v.curve_path.segments.size());
    for (const CurveLaserSegment& segment : v.curve_path.segments)
        digest_value(d, segment);
    d.u64(v.curve_samples.size());
    for (const CurveLaserSample& sample : v.curve_samples)
        digest_value(d, sample);
    d.i32(v.acceleration_time);
    d.i32(v.acceleration_duration);
    d.f32(v.acceleration_fraction);
    d.f32(v.acceleration);
    for (float value : v.acceleration_velocity)
        d.f32(value);
    d.i32(v.wait_remaining);
    d.f32(v.wait_fraction);
    d.i32(v.cull_timer);
    d.i32(v.graze_timer);
}

inline void digest_value(Digest64& d, const AnmEffectRequest& v) {
    d.i32(v.file);
    d.i32(v.script);
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.z);
    d.u8(v.kind);
    d.u16(v.opcode);
    d.i32(v.slot);
    d.u32(v.serial);
}

// ---- 道具/神霊 ----
inline void digest_value(Digest64& d, const Item& v) {
    d.i32(v.state);
    d.i32(v.type);
    d.i32(v.type2);
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.z);
    d.f32(v.vx);
    d.f32(v.vy);
    d.f32(v.vz);
    d.f32(v.homing_speed);
    d.i32(v.sprite);
    d.i32(v.anm_frame);
    d.i32(v.anm_frame_prev);
    d.f32(v.anm_time);
    d.u32(v.flags);
    d.boolean(v.alive);
}
inline void digest_value(Digest64& d, const Spirit& v) {
    d.i32(v.state);
    d.i32(v.kind);
    d.u32(v.serial);
    d.f32(v.speed);
    d.f32(v.move_speed);
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.z);
    d.f32(v.angle);
    d.i32(v.anm_frame);
    d.i32(v.anm_frame_prev);
    d.f32(v.anm_time);
    d.i32(v.frame);
    digest_value(d, v.lifetime_clock);
    d.u32(v.flags);
    d.boolean(v.fade_requested);
    d.boolean(v.alive);
}
inline void digest_value(Digest64& d, const ItemEvent& v) {
    d.i32(i32(v.kind));
    d.i32(v.value);
    d.f32(v.x);
    d.f32(v.y);
    d.u32(v.color);
}
inline void digest_item_manager(Digest64& d, ItemManager& manager) {
    d.tag("items");
    // 只散列存活槽 + 槽号：全池 2648 槽每帧全字段会把成本放大两个数量级，
    // 而死槽字段不参与后续模拟（alive=false 的唯一语义）。
    auto& items = manager.items();
    u32 live_items = 0;
    for (u32 index = 0; index < items.size(); ++index) {
        const Item& item = items[index];
        if (!item.alive) continue;
        ++live_items;
        d.u32(index);
        digest_value(d, item);
    }
    d.u32(live_items);
    auto& spirits = manager.spirits();
    u32 live_spirits = 0;
    for (u32 index = 0; index < spirits.size(); ++index) {
        const Spirit& spirit = spirits[index];
        if (!spirit.alive) continue;
        ++live_spirits;
        d.u32(index);
        digest_value(d, spirit);
    }
    d.u32(live_spirits);
    std::vector<ItemEvent>& events = manager.events();
    d.u64(events.size());
    for (const ItemEvent& event : events)
        digest_value(d, event);
}

// ---- 自机 ----
inline void digest_value(Digest64& d, const PlayerShot& v) {
    d.tag("shot");
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.damage);
    d.f32(v.angle);
    d.i32(v.script);
    d.boolean(v.alive);
    d.boolean(v.hit_fade);
    d.i32(v.state);
    d.boolean(v.animation_finished);
    d.f32(v.speed);
    d.f32(v.width);
    d.f32(v.height);
    d.f32(v.beam_visual_width);
    d.i32(v.age);
    d.i32(v.behavior);
    d.i32(v.type);
    d.i32(v.anchor);
    d.i32(v.anchor_power);
    d.f32(v.target_angle);
    d.f32(v.damage_x);
    d.f32(v.damage_y);
    digest_value(d, v.age_clock);
    digest_value(d, v.damage_clock);
    d.u32(v.target_serial);
    d.u32(v.generation);
    d.i32(v.hit_behavior);
    d.boolean(v.sine_moving);
    digest_value(d, v.sine_motion);
    d.i32(v.registration_slot);
    d.boolean(v.beam_contact);
    d.boolean(v.beam_contact_seen);
    d.i32(v.beam_interrupt);
}
inline void digest_value(Digest64& d, const DamageRegistration& v) {
    d.boolean(v.active);
    d.u32(v.flags);
    d.i32(i32(v.owner));
    d.i32(v.orb);
    d.f32(v.radius);
    d.f32(v.radius_growth);
    d.f32(v.center_x);
    d.f32(v.center_y);
    d.f32(v.center_z);
    d.f32(v.travel_speed);
    d.f32(v.travel_angle);
    d.f32(v.width);
    d.f32(v.height);
    d.f32(v.angle);
    digest_value(d, v.clock);
    d.i32(v.interval);
    d.f32(v.damage);
    d.i32(v.accumulated_damage);
    d.i32(v.threshold);
    d.u32(v.generation);
}
inline void digest_value(Digest64& d, const PlayerBombOrb& v) {
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.angle);
    d.boolean(v.active);
    d.boolean(v.exploding);
    d.i32(v.explosion_frame);
    digest_value(d, v.explosion_damage_clock);
    d.f32(v.radius);
    d.f32(v.speed);
    d.f32(v.velocity_x);
    d.f32(v.velocity_y);
    d.f32(v.accumulated_damage);
    d.i32(v.registration_slot);
}
inline void digest_value(Digest64& d, const PlayerOption& v) {
    d.f32(v.x);
    d.f32(v.y);
    d.f32(v.angle);
    d.boolean(v.active);
}
inline void digest_value(Digest64& d, const StageBattle::RespawnDamage& v) {
    digest_value(d, v.center);
    d.f32(v.radius);
    digest_value(d, v.remaining);
    d.boolean(v.active);
    d.i32(v.registration_slot);
}
inline void digest_value(Digest64& d, const StageBattle::BombRay& v) {
    digest_value(d, v.origin);
    digest_value(d, v.center);
    d.f32(v.angle);
}
inline void digest_value(Digest64& d, const YoumuBomb::Spawn& v) {
    d.i32(v.slot);
    d.i32(v.script);
    d.f32(v.y);
}
// visit_state 的标量访问器（bool/float/int 各自保持位型，不做统一转换）。
inline void digest_scalar(Digest64& d, bool value) { d.boolean(value); }
inline void digest_scalar(Digest64& d, float value) { d.f32(value); }
inline void digest_scalar(Digest64& d, i32 value) { d.i32(value); }

// ---- 模拟主体 ----
inline void digest_value(Digest64& d, const SpellSnapshot& v) {
    d.i32(v.id);
    digest_value(d, v.name);
    d.u32(v.flags);
    d.i32(v.current_bonus);
    d.i32(v.initial_bonus);
    d.i32(v.duration_frames);
    digest_value(d, v.age);
    d.i32(v.active_frames);
}
inline void digest_value(Digest64& d, const StageSim::BossMarker& v) {
    d.f32(v.ratio);
    d.i32(v.color);
}

inline void digest_battle(Digest64& d, StageBattle& battle) {
    d.tag("battle");
    d.f32(battle.player_x);
    d.f32(battle.player_y);
    d.boolean(battle.focus);
    d.boolean(battle.player_dead);
    d.i32(battle.player_phase_frame);
    d.i32(battle.player_phase_frame_previous);
    d.i32(i32(battle.player_phase));
    d.i32(battle.invulnerable);
    d.i32(battle.pos_fx);
    d.i32(battle.pos_fy);
    d.i32(battle.bomb_timer);
    d.i32(battle.bomb_age);
    d.f32(battle.bomb_age_accumulator_);
    d.u32(battle.bomb_generation);
    for (const StageBattle::BombRay& ray : battle.bomb_rays)
        digest_value(d, ray);
    for (const PlayerBombOrb& orb : battle.bomb_orbs)
        digest_value(d, orb);
    for (const PlayerOption& option : battle.options)
        digest_value(d, option);
    d.i32(battle.option_count);
    d.f32(battle.frame_damage_total);
    d.boolean(battle.death_option_rebuild);
    d.u32(battle.input_state);
    d.i32(battle.touch.mode);
    d.f32(battle.touch.x);
    d.f32(battle.touch.y);
    d.i32(battle.speed_orth);
    d.i32(battle.speed_diag);
    d.i32(battle.speed_orth_focus);
    d.i32(battle.speed_diag_focus);
    digest_value(d, battle.respawn_damage);
    digest_value(d, battle.respawn_origin);
    d.u64(battle.shots.size());
    for (const PlayerShot& shot : battle.shots)
        digest_value(d, shot);
    for (const DamageRegistration& registration : battle.damage_registrations)
        digest_value(d, registration);
    d.i32(battle.damage_registration_cursor);
    d.i64(i64(battle.player_character()));
    d.i64(i64(battle.player_power()));
    d.i64(i64(battle.player_power_max()));
    d.boolean(battle.sht_shots_loaded);
    d.u64(battle.youmu_slash_spawns_.size());
    for (const YoumuBomb::Spawn& spawn : battle.youmu_slash_spawns_)
        digest_value(d, spawn);
    d.tag("youmu");
    battle.youmu_bomb_.visit_state([&d](auto... values) { (digest_scalar(d, values), ...); });
}

inline void digest_sim(Digest64& d, StageSim& sim) {
    d.tag("sim");
    d.u32(sim.frame);
    d.f32(sim.player_x);
    d.f32(sim.player_y);
    d.i32(sim.diff);
    d.i32(sim.rank);
    d.i32(sim.difficulty_rank);
    d.u32(sim.mode_flags);
    d.boolean(sim.interactive);
    d.boolean(sim.bomb_active);
    d.boolean(sim.anm_effects_noop);
    d.boolean(sim.ecl_slot_anm);
    d.boolean(sim.replay_events);
    d.boolean(sim.defer_laser_update);
    d.boolean(sim.defer_bullet_update);
    d.u32(sim.spawn_count());
    d.i32(sim.last_created_enemy_id);
    d.u32(sim.laser_serial);
    d.u32(sim.random.seed());
    d.u32(sim.random.index());
    for (i32 value : sim.shared_integers)
        d.i32(value);
    for (float value : sim.shared_floats)
        d.f32(value);
    // gvars 私有可写表：只有 -9949/-9948/-9947 三个计数器可经公开读取面观察。
    for (i32 id : {-9949, -9948, -9947}) {
        i32 value = 0;
        sim.gvars.integer(id, value);
        d.i32(id);
        d.i32(value);
    }
    for (i32 slot : sim.boss_slot)
        d.i32(slot);
    d.i32(sim.boss_timer_seconds);
    d.i32(sim.boss_timer_centiseconds);
    d.boolean(sim.boss_leaving);
    for (const auto& slot : sim.boss_markers)
        for (const StageSim::BossMarker& marker : slot)
            digest_value(d, marker);
    d.i32(sim.boss_remaining_stars);
    d.i32(sim.boss_nonspell_frame);
    d.u32(sim.boss_main_serial);
    for (float value : sim.bullet_bounce_size)
        d.f32(value);
    d.f32(sim.active_bullet_protect_radius2);
    for (const Vec3& center : sim.laser_collision_centers)
        digest_value(d, center);
    d.i32(sim.laser_collision_count);
    // current 只散列在敌机向量中的序号（null=-1），不散列地址。
    i32 current_index = -1;
    for (u32 index = 0; index < sim.enemies.size(); ++index)
        if (sim.enemies[index].get() == sim.current) current_index = i32(index);
    d.i32(current_index);
    digest_value(d, sim.spell.state());
    d.u64(sim.anim_list.size());
    for (const std::string& name : sim.anim_list)
        digest_value(d, name);
    d.u64(sim.anm_effects.size());
    for (const AnmEffectRequest& request : sim.anm_effects)
        digest_value(d, request);
    d.u64(sim.enemies.size());
    for (const auto& enemy : sim.enemies)
        digest_value(d, *enemy);
    d.u64(sim.bullets.size());
    for (const StageBullet& bullet : sim.bullets)
        digest_value(d, bullet);
    d.u64(sim.lasers.size());
    for (const StageLaser& laser : sim.lasers)
        digest_value(d, laser);
}

// ---- 会话/HUD ----
inline void digest_value(Digest64& d, const SessionState& v) {
    d.tag("session");
    d.i32(v.stage);
    d.i32(v.stage_frame);
    d.i32(v.character);
    d.i32(v.difficulty);
    d.i32(v.continues);
    d.i32(v.continue_stock);
    d.i32(v.high_score);
    d.i32(v.high_score_extra);
    d.i32(v.configured_lives);
    d.i32(v.starting_stage);
    d.i32(v.chapter);
    d.i32(v.chapter_frame);
    d.f32(v.rate);
    d.u32(v.scene_flags);
    d.boolean(v.new_run);
    d.u32(v.run_mode_flags);
    d.i32(v.practice_mode);
    d.i32(v.selected_spell_id);
    d.i32(v.ending_points);
    d.i32(v.good_endings);
    d.i32(v.stage_clear_bonus);
    d.u32(v.stage_cleared_bits);
    d.i32(v.point_items);
    d.i32(v.max_point);
    d.i32(v.max_point_cap);
    d.i32(v.graze);
    d.i32(v.life_piece_threshold_index);
}
inline void digest_value(Digest64& d, const HudPlayerState& v) {
    d.tag("hud.player");
    d.i32(v.extra_lives);
    d.i32(v.life_pieces);
    d.i32(v.bombs);
    d.i32(v.bomb_pieces);
    d.i32(v.power);
    d.i32(v.max_power);
    d.i32(v.power_step);
    d.i32(v.deaths);
    d.u32(v.mode_flags);
}
inline void digest_value(Digest64& d, const HudScoreState& v) {
    d.tag("hud.score");
    d.i32(v.score);
    d.i32(v.life_piece_tier);
}
inline void digest_value(Digest64& d, const GameRandom& v) {
    d.u32(v.seed());
    d.u32(v.index());
}
inline void digest_value(Digest64& d, const AnmEnvironment& v) {
    d.tag("environment");
    digest_value(d, v.game_rng);
    digest_value(d, v.camera_origin);
    digest_value(d, v.screen_translation);
    digest_value(d, v.reference_position);
    digest_value(d, v.background_delta);
    digest_value(d, v.stage_direction);
    digest_value(d, v.screen_shake_offset);
    d.f32(v.resolution_scale);
    d.f32(v.raster_scale);
    for (i32 value : v.screen_offsets)
        d.i32(value);
    digest_value(d, v.playfield_origin);
    d.boolean(v.paused);
    d.u32(v.screen_width);
    d.u32(v.screen_height);
    d.boolean(v.render_target_has_alpha);
    d.i32(v.stage_difficulty);
}

// ---- RunScene 回滚快照 ----
inline void digest_value(Digest64& d, const GameHud::RollbackState& v) {
    d.tag("rollback.hud");
    for (u32 handle : v.life_icons)
        d.u32(handle);
    for (u32 handle : v.bomb_icons)
        d.u32(handle);
    d.u32(v.root);
    d.u32(v.difficulty_notice);
    d.u32(v.difficulty_label);
    d.u32(v.flags);
    digest_value(d, v.intro_age);
    digest_value(d, v.frame_age);
    digest_value(d, v.clear_age);
    d.u32(v.clear_banner);
    d.i32(v.displayed_score);
    d.i32(v.score_increment);
    d.u32(v.gauge_main);
    d.i32(v.trance_skull_label);
    d.u32(v.opening_signature);
    d.u32(v.opening_signature_right);
    d.i32(v.stage_logo_bank);
    d.tag("rollback.hud.boss");
    const BossPresentation::RollbackState& boss = v.boss;
    d.boolean(boss.gate_);
    d.f32(boss.displayed_ratio_);
    d.f32(boss.target_ratio_);
    d.i32(boss.timer_seconds_);
    d.i32(boss.timer_displayed_);
    d.i32(boss.timer_frames_);
    d.i32(boss.timer_centiseconds_);
    for (u32 handle : boss.remaining_star_handles_)
        d.u32(handle);
    d.i32(boss.warn_state_);
    d.u32(boss.move_state_);
    d.i32(boss.escalate_state_);
    d.boolean(boss.proximity_hidden_);
    d.boolean(boss.stars_created_);
    d.boolean(boss.ring_created_);
    d.i32(boss.marker_glyph_);
    d.u32(boss.digit_tens_);
    d.u32(boss.digit_ones_);
    d.u32(boss.ring_arc_);
    for (u32 handle : boss.ring_overlays_)
        d.u32(handle);
    for (u32 handle : boss.star_handles_)
        d.u32(handle);
    d.u32(boss.marker_handle_);
    d.u32(boss.name_handle_);
    d.tag("rollback.hud.spell");
    const SpellPresentation::RollbackState& spell = v.spell;
    d.u32(spell.flags_);
    d.i32(spell.spell_id_);
    digest_value(d, spell.name_);
    d.i32(spell.bonus_base_);
    d.i32(spell.bonus_cap_);
    d.i32(spell.duration_);
    d.u32(spell.background_);
    for (float value : spell.anchor_)
        d.f32(value);
    digest_value(d, spell.snapshot_);
    for (u32 handle : spell.displays_)
        d.u32(handle);
    d.u32(spell.stage_background_);
    d.u32(spell.decoration_);
    d.u32(spell.banner_);
    d.u32(spell.notice_);
    d.u32(spell.spirit_notice_);
    d.u64(spell.spirit_notices_.size());
    for (u32 handle : spell.spirit_notices_)
        d.u32(handle);
    for (u32 handle : spell.bonus_digits_)
        d.u32(handle);
    d.u32(spell.bonus_million_);
    d.u32(spell.bonus_thousand_);
    d.u32(spell.bonus_capture_effect_);
    d.i32(spell.capture_frames_);
    d.boolean(spell.production_);
    d.u64(spell.retiring_displays_.size());
    for (u32 handle : spell.retiring_displays_)
        d.u32(handle);
}
inline void digest_value(Digest64& d, const SpellPresentation::RollbackState& v) {
    d.tag("rollback.spell");
    d.u32(v.flags_);
    d.i32(v.spell_id_);
    digest_value(d, v.name_);
    d.i32(v.bonus_base_);
    d.i32(v.bonus_cap_);
    d.i32(v.duration_);
    d.u32(v.background_);
    for (float value : v.anchor_)
        d.f32(value);
    digest_value(d, v.snapshot_);
    for (u32 handle : v.displays_)
        d.u32(handle);
    d.u32(v.stage_background_);
    d.u32(v.decoration_);
    d.u32(v.banner_);
    d.u32(v.notice_);
    d.u32(v.spirit_notice_);
    d.u64(v.spirit_notices_.size());
    for (u32 handle : v.spirit_notices_)
        d.u32(handle);
    for (u32 handle : v.bonus_digits_)
        d.u32(handle);
    d.u32(v.bonus_million_);
    d.u32(v.bonus_thousand_);
    d.u32(v.bonus_capture_effect_);
    d.i32(v.capture_frames_);
    d.boolean(v.production_);
    d.u64(v.retiring_displays_.size());
    for (u32 handle : v.retiring_displays_)
        d.u32(handle);
}
inline void digest_value(Digest64& d, const MsgPresentation::RollbackState& v) {
    d.tag("rollback.portraits");
    d.u32(v.player_);
    d.u32(v.side_);
    d.u32(v.nameplate_);
}
inline void digest_value(Digest64& d, const ScorePopups::RollbackState& v) {
    d.tag("rollback.popups");
    // glyph（AnmVm）不散列：表现层 VM 状态由 draws digest 覆盖。
    for (const ScorePopups::Entry& entry : v.pool) {
        for (i32 digit : entry.digits)
            d.i32(digit);
        d.i32(entry.count);
        d.f32(entry.x);
        d.f32(entry.y);
        d.f32(entry.z);
        d.u32(entry.color);
        d.f32(entry.scale);
        d.i32(entry.age);
        d.boolean(entry.active);
    }
    d.u32(v.cursor);
    d.boolean(v.available);
}
inline void digest_value(Digest64& d, const LaserPresentation::SnapshotData& v) {
    d.tag("rollback.lasers");
    d.u64(v.entries.size());
    for (const LaserPresentation::SnapshotData::Entry& entry : v.entries) {
        d.u32(entry.generation);
        d.i32(entry.type);
        d.i32(entry.color);
        d.boolean(entry.curve);
        // body/orb/tip（AnmVm）与 identities（指针）不散列。
    }
}
inline void digest_value(Digest64& d, const RunScene::RollbackState& v) {
    d.tag("rollback.run");
    d.u8(u8(v.post.state_));
    d.u32(v.post.age_);
    d.boolean(v.post.clear_sequence_);
    d.u32(v.post.clear_frame_);
    digest_value(d, v.hud);
    digest_value(d, v.spell);
    digest_value(d, v.portraits);
    digest_value(d, v.popups);
    digest_value(d, v.lasers);
    digest_value(d, v.camera_origin);
    digest_value(d, v.screen_translation);
    digest_value(d, v.reference_position);
    digest_value(d, v.background_delta);
    digest_value(d, v.stage_direction);
    digest_value(d, v.shake_offset);
    digest_value(d, v.environment_rng);
    d.f32(v.animation_rate);
    d.boolean(v.paused);
    d.i32(v.dialogue_rows);
    digest_value(d, v.age);
    d.boolean(v.player_activation_pending);
    d.boolean(v.transition_wait);
    d.boolean(v.scenery_started);
}

// 单帧状态摘要。调用点 = RunScene 一帧 update 完成之后（与 TH13_STATE_LOG
// 的 write_state_row 同相位）；只读，不改变任何游戏状态。
inline u64 digest_frame_state(RunScene& scene, const SessionState& progress, const HudPlayerState& player,
                              const HudScoreState& score, const GameRandom& visual, const AnmEnvironment& environment) {
    Digest64 d;
    d.tag("th13-frame-state/v1");
    d.tag("run");
    digest_value(d, scene.age);
    d.u32(scene.input_held);
    d.u32(scene.input_pressed);
    d.u32(scene.input_repeated);
    d.boolean(scene.transition_wait);
    d.boolean(scene.scenery_started);
    d.i32(scene.previous_scenery_bank);
    d.boolean(scene.error.empty());
    digest_value(d, scene.battle_sht);
    digest_value(d, scene.capture_rollback());
    digest_battle(d, scene.battle);
    digest_sim(d, scene.battle.sim);
    digest_item_manager(d, scene.battle.item_manager);
    digest_value(d, progress);
    digest_value(d, player);
    digest_value(d, score);
    d.tag("rng");
    digest_value(d, scene.battle.sim.random);
    digest_value(d, visual);
    digest_value(d, environment);
    return d.value();
}

}  // namespace harness
}  // namespace th13
