#include "StageSim.hpp"

#include "AnmVm.hpp"
#include "BulletAppearance.hpp"
#include "SessionState.hpp"
#include "StageMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace th13 {

EclSpriteObserver ecl_sprite_observer = nullptr;

namespace {
constexpr float kKeep = -999999.0f;

bool resolve_float(EclContext& c, u32 argument, float& value) {
    return c.floating(argument, c.instruction->argument<float>(argument), value);
}

bool resolve_int(EclContext& c, u32 argument, i32& value) {
    return c.integer(argument, c.instruction->argument<i32>(argument), value);
}

void shortest_arc(float& from, float& to) {
    const float diff = to >= from ? to - from : from - to;
    if (diff < 3.1415927f) return;
    if (to <= from)
        to = to + 6.2831855f;
    else
        from = from + 6.2831855f;
}

u32 postname_dword(const EclInstruction* ins, u32 index) {
    const u32 name_length = u32(ins->argument<i32>(0));
    const u32 base = ((name_length + 4) >> 2) + index;
    u32 raw = 0;
    std::memcpy(&raw, reinterpret_cast<const u8*>(ins) + 16 + base * 4, 4);
    return raw;
}

bool resolve_postname_float(EclContext& c, u32 index, u32 argument, float& value) {
    const u32 raw = postname_dword(c.instruction, index);
    float literal;
    std::memcpy(&literal, &raw, 4);
    return c.floating(argument, literal, value);
}

bool resolve_postname_int(EclContext& c, u32 index, u32 argument, i32& value) {
    const u32 raw = postname_dword(c.instruction, index);
    return c.integer(argument, i32(raw), value);
}

bool mirrored_variant(u16 op) { return op == 404 || op == 405 || op == 406 || op == 407; }
}

int StageCommands::execute(EclContext& c, u16 op) {
    StageSim& sim = *this->sim;
    StageEnemy& e = *sim.current;
    const EclInstruction* ins = c.instruction;

    const auto emit_move = [&](const u32* integral, std::initializer_list<float> values) {
        sim.sink.enemy_move(e.serial, op, integral, values.begin(), u32(values.size()));
    };
    const auto flush_anm_slots = [&]() -> bool {
        if (!sim.flush_anm_slots) return true;
        if (sim.flush_anm_slots(&e)) return true;
        if (sim.error_.empty()) sim.error_ = "Enemy slot animation flush failed";
        return false;
    };

    switch (op) {
    case 300:
    case 301:
    case 304:
    case 305:
    case 309:
    case 310:
    case 311:
    case 312:
    case 321:
        return create_enemy(c, op);
    case 302: {
        i32 file = 0;
        if (resolve_int(c, 0, file)) e.anm_file = file;
        return 0;
    }
    case 320: {
        i32 slot = 0;
        float x = 0, y = 0;
        if (!resolve_int(c, 0, slot) || !resolve_float(c, 1, x) || !resolve_float(c, 2, y)) {
            sim.error_ = c.error;
            return -2;
        }
        if (slot < 0 || slot >= StageEnemy::kAnimationSlots) {
            sim.error_ = "animation offset slot";
            return -2;
        }
        e.anm_slot_offset[slot] = {x, y, 0};
        return 0;
    }
    case 303:
    case 306: {
        i32 slot = 0, script = 0;
        const bool resolved = resolve_int(c, 0, slot) && resolve_int(c, 1, script);
        if (resolved && slot >= 0 && slot < StageEnemy::kAnimationSlots) e.anm_script[slot] = script;
        if (resolved && op == 306 && slot == 0) {
            e.anm_base_script = script;
            e.mirror_dir = 0;
            e.mirror_ready = false;
            e.mirror_prev_x = e.position[0];
        }
        if (resolved && !sim.anm_effects_noop && sim.ecl_slot_anm && slot >= 0 && slot < StageEnemy::kAnimationSlots) {
            if (op == 306 && slot == 0) e.flags |= 0x200000u;
            sim.anm_effects.push_back({e.anm_file, script, 0, 0, 0, 1, u16(op), slot, e.serial});
            if (!flush_anm_slots()) return -2;
        }
        if (!resolved) {
            sim.error_ = c.error.empty() ? "anmScript args" : c.error;
            return -2;
        }
        return 0;
    }
    case 317: {
        i32 slot = 0, label = 0;
        if (!resolve_int(c, 0, slot) || !resolve_int(c, 1, label)) {
            sim.error_ = c.error;
            return -2;
        }
        if (slot < 0 || slot >= StageEnemy::kAnimationSlots) {
            sim.error_ = "animation interrupt slot";
            return -2;
        }
        label = i32(i16(u16(label)));
        if (sim.enemy_animation_interrupt && !sim.enemy_animation_interrupt(e, slot, label)) {
            if (sim.error_.empty()) sim.error_ = "Enemy animation interrupt failed";
            return -2;
        }
        return 0;
    }
    case 318: {
        e.flags &= ~0x200000u;
        e.mirror_dir = 0;
        e.anm_script[0] = e.anm_base_script;
        if (!sim.anm_effects_noop && sim.ecl_slot_anm) {
            sim.anm_effects.push_back({e.anm_file, e.anm_base_script, 0, 0, 0, 1, u16(op), 0, e.serial});
            if (!flush_anm_slots()) return -2;
        }
        return 0;
    }
    case 307:
    case 308: {
        if (sim.anm_effects_noop) return 0;
        i32 file = 0, script = 0;
        if (!resolve_int(c, 0, file) || !resolve_int(c, 1, script)) return -2;
        if (sim.ecl_effect_create) {
            const float x = op == 308 ? 0 : e.position[0];
            const float y = op == 308 ? 0 : e.position[1];
            const float z = op == 308 ? 0 : e.position[2];
            if (!sim.ecl_effect_create(file, script, {x, y, z})) return -2;
        } else if (op == 308)
            sim.anm_effects.push_back({file, script, 0, 0, 0});
        else
            sim.anm_effects.push_back({file, script, e.position[0], e.position[1], e.position[2]});
        return 0;
    }
    case 313:
    case 316: {
        if (sim.anm_effects_noop) return 0;
        i32 slot = 0;
        if (!resolve_int(c, 0, slot)) {
            sim.error_ = c.error.empty() ? "anmFlip slot" : c.error;
            return -2;
        }
        i32 delta = 0;
        if (op == 316 && !resolve_int(c, 1, delta)) {
            sim.error_ = c.error.empty() ? "anmFlip delta" : c.error;
            return -2;
        }
        if (sim.ecl_slot_anm && slot >= 0 && slot < StageEnemy::kAnimationSlots) {
            const i32 base = e.anm_base_script;
            sim.anm_effects.push_back({e.anm_file, base + delta + 5, 0, 0, 0, 1, u16(op), slot, e.serial});
            if (!flush_anm_slots()) return -2;
        }
        return 0;
    }
    case 334: {
        i32 kind = 0;
        if (!resolve_int(c, 0, kind)) {
            sim.error_ = c.error;
            return -2;
        }
        const Vec3 position{e.position[0], e.position[1], e.position[2]};
        if (!sim.scene_effect || !sim.scene_effect(kind, position)) {
            sim.error_ = "Scene effect " + std::to_string(kind) + " unavailable";
            return -2;
        }
        return 0;
    }
    case 319:
    case 325:
    case 326:
    case 327:
    case 328:
    case 329:
    case 330:
    case 331:
    case 332:
    case 333:
    case 335: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot)) {
            sim.error_ = c.error;
            return -2;
        }
        if (slot < 0 || slot >= StageEnemy::kAnimationSlots) {
            sim.error_ = "animation mutation slot";
            return -2;
        }
        if (!sim.enemy_animation) return 0;
        AnmVm* vm = sim.enemy_animation(e, slot);
        if (!sim.error_.empty()) return -2;
        if (!vm) return 0;
        const auto integer = [&](u32 n) {
            i32 v = 0;
            resolve_int(c, n, v);
            return v;
        };
        const auto real = [&](u32 n) {
            float v = 0;
            resolve_float(c, n, v);
            return v;
        };
        if (op == 319) {
            vm->variables.rotation.z = real(1);
            vm->visual.flags |= 4;
        } else if (op == 327 || op == 331) {
            const u32 alpha = u8(integer(1));
            u32& color = op == 327 ? vm->visual.color : vm->visual.secondary_color;
            color = (color & 0xffffffu) | alpha << 24;
        } else if (op == 328 || op == 332) {
            const i32 alpha = u8(integer(3)), mode = integer(2), duration = integer(1);
            auto& channel = op == 328 ? vm->interpolators.alpha : vm->interpolators.secondary_alpha;
            const u32 color = op == 328 ? vm->visual.color : vm->visual.secondary_color;
            channel.control1 = {};
            channel.control2 = {};
            channel.begin(duration, mode, {i32(color >> 24)}, {alpha});
        } else if (op == 329 || op == 335) {
            const float y = real(2), x = real(1);
            auto& scale = op == 329 ? vm->visual.scale : vm->visual.secondary_scale;
            scale = {x, y};
            vm->visual.flags |= 8;
        } else if (op == 330) {
            const float y = real(4), x = real(3);
            const i32 mode = integer(2), duration = integer(1);
            vm->interpolators.scale.begin(duration, mode, {vm->visual.scale.x, vm->visual.scale.y}, {x, y});
        } else if (op == 333) {
            const float y = real(4), x = real(3);
            const i32 source_slot = integer(0);
            if (!c.error.empty()) {
                sim.error_ = c.error;
                return -2;
            }
            if (source_slot < 0 || source_slot >= StageEnemy::kAnimationSlots) {
                sim.error_ = "animation position source slot";
                return -2;
            }
            AnmVm* source = sim.enemy_animation(e, source_slot);
            if (!source) {
                sim.error_ = "animation position source missing";
                return -2;
            }
            const i32 mode = integer(2), duration = integer(1);
            auto& channel = vm->interpolators.position;
            channel.control1 = {};
            channel.control2 = {};
            channel.begin(duration, mode,
                          {source->visual.translation.x, source->visual.translation.y, source->visual.translation.z},
                          {x, y, 0});
        } else if (op == 325) {
            const u32 b = u8(integer(3)), g = u8(integer(2)), r = u8(integer(1));
            vm->visual.color = (vm->visual.color & 0xff000000u) | b | g << 8 | r << 16;
        } else {
            const i32 r = u8(integer(3)), g = u8(integer(4)), b = u8(integer(5));
            const i32 mode = integer(2), duration = integer(1);
            const u32 color = vm->visual.color;
            auto& channel = vm->interpolators.color;
            channel.control1 = {};
            channel.control2 = {};
            channel.begin(duration, mode, {i32(color & 255), i32((color >> 8) & 255), i32((color >> 16) & 255)},
                          {b, g, r});
        }
        if (!c.error.empty()) {
            sim.error_ = c.error;
            return -2;
        }
        return 0;
    }
    case 553: {
        if (!resolve_int(c, 0, e.hit_sound)) {
            sim.error_ = c.error;
            return -2;
        }
        return 0;
    }
    case 554:
        if (sim.stage_logo && !sim.stage_logo()) {
            sim.error_ = "Stage logo creation failed";
            return -2;
        }
        return 0;

    case 400:
    case 402: {
        const u32 integral[2] = {0, 0};
        StageMover& m = op == 400 ? e.vel : e.pos;
        float x = 0, y = 0;
        if (!resolve_float(c, 0, x) || !resolve_float(c, 1, y)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {x, y});
        if (kKeep < x) m.pos[0] = x;
        if (kKeep < y) m.pos[1] = y;
        m.mode &= 0xFFFFFFF0u;
        for (u32 i = 0; i < 3; ++i) {
            const float next = float(e.vel.pos[i] + e.pos.pos[i]);
            e.motion_delta[i] = next - e.position[i];
            e.position[i] = next;
        }
        e.position[0] = round001(e.position[0]);
        e.position[1] = round001(e.position[1]);
        if (e.flags & 0x40000u) {
            const float hw = e.limit_half[0] * 0.5f, hh = e.limit_half[1] * 0.5f;
            e.position[0] = std::max(e.limit_center[0] - hw, std::min(e.position[0], e.limit_center[0] + hw));
            e.position[1] = std::max(e.limit_center[1] - hh, std::min(e.position[1], e.limit_center[1] + hh));
            for (u32 i = 0; i < 3; ++i)
                e.vel.pos[i] = float(e.position[i] - e.pos.pos[i]);
        }
        return 0;
    }
    case 401:
    case 403: {
        const u32 integral[5] = {1, 1, 0, 0, 0};
        StageMover& m = op == 401 ? e.vel : e.pos;
        i32 dur = 0, mode = 0;
        float x = 0, y = 0;
        if (!resolve_float(c, 2, x) || !resolve_float(c, 3, y) || !resolve_int(c, 0, dur)) {
            sim.error_ = c.error;
            return -2;
        }
        m.mode &= 0xFFFFFFF0u;
        if (dur < 1) {
            m.postime.duration = 0;
            return 0;
        }
        if (!resolve_int(c, 0, dur) || !resolve_int(c, 1, mode)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {float(dur), float(mode), x, y});
        if (mode == 7 || mode == 17) {
            sim.error_ = "movePosTime interpolation mode not implemented";
            return -2;
        }
        m.postime.duration = float(dur);
        m.postime.elapsed = 0;
        m.postime.mode = mode;
        m.postime.per_axis = false;
        m.postime.start[0] = m.pos[0];
        m.postime.start[1] = m.pos[1];
        m.postime.start[2] = m.pos[2];
        m.postime.target[0] = kKeep < x ? x : m.pos[0];
        m.postime.target[1] = kKeep < y ? y : m.pos[1];
        m.postime.target[2] = m.pos[2];
        return 0;
    }
    case 404:
    case 406:
    case 428:
    case 430: {
        const u32 integral[2] = {0, 0};
        StageMover& m = (op == 404 || op == 428) ? e.vel : e.pos;
        float angle = 0, speed = 0;
        if (!resolve_float(c, 0, angle) || !resolve_float(c, 1, speed)) {
            sim.error_ = c.error;
            return -2;
        }
        const float values[2] = {angle, speed};
        sim.sink.enemy_move(e.serial, op, integral, values, 2);
        if (kKeep < angle) {
            if ((e.flags & 0x100000u) && mirrored_variant(op)) angle = mirror_angle(angle);
            m.angle = wrap_angle(angle);
        }
        if (kKeep < speed) m.speed = speed;
        m.mode &= 0xFFFFFFF0u;
        return 0;
    }
    case 405:
    case 407:
    case 429:
    case 431: {
        const u32 integral[4] = {1, 1, 0, 0};
        StageMover& m = (op == 405 || op == 429) ? e.vel : e.pos;
        i32 dur = 0, mode = 0;
        float angle = 0, speed = 0;
        if (!resolve_float(c, 2, angle) || !resolve_float(c, 3, speed) || !resolve_int(c, 0, dur)) {
            sim.error_ = c.error;
            return -2;
        }
        if (dur < 1) {
            m.polar.duration = 0;
            return 0;
        }
        if (!resolve_int(c, 1, mode)) {
            sim.error_ = c.error;
            return -2;
        }
        if (mode == 7 || mode == 8 || mode == 17) {
            sim.error_ = "moveVelTime interpolation mode not implemented";
            return -2;
        }
        const float trace_angle = angle;
        if (kKeep < angle && (e.flags & 0x100000u) && mirrored_variant(op)) angle = mirror_angle(angle);
        float from_a = m.angle, from_b = m.speed;
        float to_a = kKeep < angle ? angle : m.angle;
        const float to_b = kKeep < speed ? speed : m.speed;
        shortest_arc(from_a, to_a);
        if (!resolve_int(c, 0, dur)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {float(dur), float(mode), trace_angle, speed});
        m.polar.setup(float(dur), mode, from_a, from_b, to_a, to_b);
        m.mode &= 0xFFFFFFF2u;
        return 0;
    }
    case 408:
    case 410: {
        const u32 integral[4] = {0, 0, 0, 0};
        StageMover& m = (op == 408) ? e.vel : e.pos;
        float a0 = 0, a1 = 0, a2 = 0, a3 = 0;
        if (!resolve_float(c, 0, a0) || !resolve_float(c, 1, a1) || !resolve_float(c, 2, a2) ||
            !resolve_float(c, 3, a3)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {a0, a1, a2, a3});
        if ((m.mode & 0xFu) != 2) {
            m.aux[0] = m.pos[0];
            m.aux[1] = m.pos[1];
            m.aux[2] = m.pos[2];
        }
        if (kKeep < a0) m.angle = wrap_angle(a0);
        if (kKeep < a1) m.speed = a1;
        if (kKeep < a2) m.radius = a2;
        if (kKeep < a3) m.growth = a3;
        m.mode = (m.mode & 0xFFFFFFF2u) | 2u;
        {
            float ox = 0, oy = 0;
            polar_xy(m.angle, m.radius, ox, oy);
            m.pos[0] = m.aux[0] + ox;
            m.pos[1] = m.aux[1] + oy;
            m.pos[2] = m.aux[2] + 0.0f;
            m.pos[0] = round001(m.pos[0]);
            m.pos[1] = round001(m.pos[1]);
            for (int k = 0; k < 3; ++k) {
                const float delta = (e.vel.pos[k] + e.pos.pos[k]) - e.position[k];
                e.motion_delta[k] = delta;
                e.position[k] = e.position[k] + delta;
            }
            e.position[0] = round001(e.position[0]);
            e.position[1] = round001(e.position[1]);
        }
        return 0;
    }
    case 420:
    case 422: {
        const u32 integral[6] = {};
        StageMover& m = op == 420 ? e.vel : e.pos;
        float args[6] = {};
        for (u32 n = 0; n < 6; ++n) {
            if (!resolve_float(c, n, args[n])) {
                sim.error_ = c.error;
                return -2;
            }
        }
        emit_move(integral, {args[0], args[1], args[2], args[3], args[4], args[5]});
        if ((m.mode & 15) != 2)
            for (u32 n = 0; n < 3; ++n)
                m.aux[n] = m.pos[n];
        if (kKeep < args[0]) m.angle = wrap_angle(args[0]);
        if (kKeep < args[1]) m.speed = args[1];
        if (kKeep < args[2]) m.radius = args[2];
        if (kKeep < args[3]) m.growth = args[3];
        if (kKeep < args[4]) m.ellipse_angle = wrap_angle(wrap_angle(args[4]));
        if (kKeep < args[5]) m.ellipse_ratio = args[5];
        m.mode = (m.mode & 0xfffffff3u) | 3u;
        m.accumulate();
        for (u32 n = 0; n < 3; ++n) {
            const float delta = (e.vel.pos[n] + e.pos.pos[n]) - e.position[n];
            e.motion_delta[n] = delta;
            e.position[n] = e.position[n] + delta;
        }
        e.position[0] = round001(e.position[0]);
        e.position[1] = round001(e.position[1]);
        return 0;
    }
    case 409:
    case 411: {
        const u32 integral[5] = {1, 1, 0, 0, 0};
        StageMover& m = (op == 409) ? e.vel : e.pos;
        i32 dur = 0, mode = 0;
        float speed_to = 0, radius = 0, growth = 0;
        if (!resolve_float(c, 2, speed_to) || !resolve_float(c, 3, radius) || !resolve_float(c, 4, growth) ||
            !resolve_int(c, 0, dur) || !resolve_int(c, 1, mode)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {float(dur), float(mode), speed_to, radius, growth});
        if (dur < 1) {
            m.polar.duration = 0;
            m.radial.duration = 0;
            return 0;
        } else {
            if (mode == 7 || mode == 8 || mode == 17) {
                sim.error_ = "moveCircleTime interpolation mode not implemented";
                return -2;
            }
            const float from_b = m.speed;
            const float to_b = kKeep < speed_to ? speed_to : m.speed;
            m.polar.setup(float(dur), mode, 0.0f, from_b, 0.0f, to_b);
        }
        m.radial.control1 = {};
        m.radial.control2 = {};
        m.radial.begin(dur, mode, {m.radius, m.growth},
                       {kKeep < radius ? radius : m.radius, kKeep < growth ? growth : m.growth});
        m.mode = (m.mode & 0xFFFFFFF2u) | 2u;
        m.accumulate();
        for (u32 n = 0; n < 3; ++n) {
            const float delta = (e.vel.pos[n] + e.pos.pos[n]) - e.position[n];
            e.motion_delta[n] = delta;
            e.position[n] += delta;
        }
        e.position[0] = round001(e.position[0]);
        e.position[1] = round001(e.position[1]);
        return 0;
    }
    case 432:
    case 433: {
        const u32 integral[1] = {1};
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {float(id)});
        StageEnemy* target = sim.find_by_script_id(id);
        StageMover& m = (op == 432) ? e.vel : e.pos;
        if (!target) {
            m.pos[0] = 0.0f;
            m.pos[1] = 0.0f;
            m.pos[2] = 0.0f;
            return 0;
        }
        m.pos[0] = target->position[0];
        m.pos[1] = target->position[1];
        m.pos[2] = target->position[2];
        return 0;
    }
    case 424: {
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        e.flags = (e.flags & ~0x100000u) | ((u32(value) << 20) & 0x100000u);
        return 0;
    }
    case 425: {
        const u32 integral[7] = {1, 0, 0, 0, 0, 0, 0};
        StageMover& m = e.vel;
        i32 dur = 0;
        float x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
        if (!resolve_float(c, 3, x2) || !resolve_float(c, 4, y2) || !resolve_float(c, 1, x1) ||
            !resolve_float(c, 2, y1) || !resolve_float(c, 5, x3) || !resolve_float(c, 6, y3) ||
            !resolve_int(c, 0, dur)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {float(dur), x1, y1, x2, y2, x3, y3});
        m.mode &= 0xFFFFFFF0u;
        if (dur < 1) {
            m.postime.duration = 0;
            return 0;
        }
        m.postime.duration = float(dur);
        m.postime.elapsed = 0;
        m.postime.mode = 8;
        m.postime.per_axis = false;
        m.postime.start[0] = m.pos[0];
        m.postime.start[1] = m.pos[1];
        m.postime.start[2] = m.pos[2];
        m.postime.target[0] = kKeep < x2 ? x2 : m.pos[0];
        m.postime.target[1] = kKeep < y2 ? y2 : m.pos[1];
        m.postime.target[2] = 0.0f;
        m.postime.p1[0] = x1;
        m.postime.p1[1] = y1;
        m.postime.p1[2] = 0.0f;
        m.postime.p2[0] = x3;
        m.postime.p2[1] = y3;
        m.postime.p2[2] = 0.0f;
        return 0;
    }
    case 412: {
        const u32 integral[3] = {1, 1, 0};
        i32 dur = 0, mode = 0;
        float speed = 0;
        const float pi = 3.1415927410125732f;
        const float qx = e.limit_half[0] * 0.25f;
        const float qy = e.limit_half[1] * 0.25f;
        const float cx = e.limit_center[0], cy = e.limit_center[1];
        float angle;
        if (e.position[0] < cx - qx) {
            GameRandom::tag(7);
            angle = float(float(sim.random.signed_unit() * pi) / 3.0);
        } else if (e.position[0] > cx + qx) {
            GameRandom::tag(7);
            angle = wrap_angle(float(pi + float(sim.random.signed_unit() * pi) / 3.0));
        } else {
            GameRandom::tag(7);
            const float quarter = float(float(sim.random.signed_unit() * pi) * 0.25);
            GameRandom::tag(8);
            const bool third = (u32(sim.random.step16()) % 3u) == 0;
            float side_x, side_y;
            sim.nearest_player(e.position[0], e.position[1], side_x, side_y);
            const bool left_of_player = side_x > e.position[0];
            angle = (left_of_player ? third : !third) ? float(quarter + pi) : quarter;
        }
        if (e.position[1] < cy - qy) {
            angle = angle < 0 ? -angle : angle;
        } else if (e.position[1] > cy + qy) {
            angle = angle < 0 ? angle : -angle;
        }
        if (std::fabs(angle + 1.5707964f) < 0.05f) {
            angle = angle >= -1.5707964f ? -1.5207964f : -1.6207963f;
        } else if (std::fabs(angle - 1.5707964f) < 0.05f) {
            angle = angle > 1.5707964f ? 1.6207963f : 1.5207964f;
        }
        if (!resolve_int(c, 1, mode) || !resolve_float(c, 2, speed) || !resolve_int(c, 0, dur)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral, {float(dur), float(mode), speed});
        StageMover& m = e.vel;
        if (dur < 1) {
            m.polar.duration = 0;
            return 0;
        }
        if (mode == 7 || mode == 8 || mode == 17) {
            sim.error_ = "moveRand interpolation mode not implemented";
            return -2;
        }
        m.polar.setup(float(dur), mode, angle, speed, angle, 0.0f);
        m.mode &= 0xFFFFFFF0u;
        return 0;
    }
    case 434:
    case 435:
    case 436:
    case 437:
    case 438:
    case 439: {
        const bool separate = op != 436 && op != 437;
        const u32 integral[5] = {1, 1, u32(separate), 0, 0};
        StageMover& m = (op & 1) ? e.pos : e.vel;
        float x = 0, y = 0;
        i32 duration = 0, mx = 0, my = 0;
        if (!resolve_float(c, separate ? 3 : 2, x) || !resolve_float(c, separate ? 4 : 3, y) ||
            !resolve_int(c, 0, duration)) {
            sim.error_ = c.error;
            return -2;
        }
        if (duration < 1) {
            m.postime.duration = 0;
            return 0;
        }
        const float trace_x = x, trace_y = y;
        if (op >= 436) {
            x = (e.flags & 0x100000u) ? m.pos[0] - x : m.pos[0] + x;
            y = m.pos[1] + y;
        }
        if (!resolve_int(c, 0, duration) || !resolve_int(c, 1, mx) || (separate && !resolve_int(c, 2, my))) {
            sim.error_ = c.error;
            return -2;
        }
        if (separate)
            emit_move(integral, {float(duration), float(mx), float(my), trace_x, trace_y});
        else
            emit_move(integral, {float(duration), float(mx), trace_x, trace_y});
        auto& itp = m.postime;
        itp.duration = float(duration);
        itp.elapsed = 0;
        itp.per_axis = separate;
        if (!separate) itp.mode = mx;
        itp.axis_mode[0] = mx;
        itp.axis_mode[1] = my;
        for (int i = 0; i < 3; ++i) {
            itp.start[i] = m.pos[i];
            itp.p1[i] = 0;
            itp.p2[i] = 0;
        }
        itp.target[0] = kKeep < x ? x : m.pos[0];
        itp.target[1] = kKeep < y ? y : m.pos[1];
        itp.target[2] = 0;
        m.mode &= 0xfffffff0u;
        return 0;
    }
    case 427: {
        for (int i = 0; i < 3; ++i) {
            e.vel.pos[i] = e.vel.pos[i] + e.pos.pos[i];
            e.pos.pos[i] = 0;
        }
        for (StageMover* m : {&e.vel, &e.pos}) {
            m->speed = 0;
            m->aux[0] = m->aux[1] = m->aux[2] = 0;
            m->mode &= 0xFFFFFFF0u;
            m->polar.duration = 0;
            m->postime.duration = 0;
            m->radial.duration = 0;
        }
        return 0;
    }

    case 500: {
        float w = 0, h = 0;
        if (resolve_float(c, 0, w) && resolve_float(c, 1, h)) {
            e.hurtbox[0] = w;
            e.hurtbox[1] = h;
            return 0;
        }
        sim.error_ = c.error;
        return -2;
    }
    case 501: {
        float w = 0, h = 0;
        if (resolve_float(c, 0, w) && resolve_float(c, 1, h)) {
            e.hitbox[0] = w;
            e.hitbox[1] = h;
            return 0;
        }
        sim.error_ = c.error;
        return -2;
    }
    case 502: {
        i32 mask = 0;
        if (!resolve_int(c, 0, mask)) {
            sim.error_ = c.error;
            return -2;
        }
        e.flags |= u32(mask);
        return 0;
    }
    case 503: {
        i32 mask = 0;
        if (!resolve_int(c, 0, mask)) {
            sim.error_ = c.error;
            return -2;
        }
        e.flags &= ~u32(mask);
        return 0;
    }
    case 504: {
        float cx = 0, cy = 0, w = 0, h = 0;
        if (resolve_float(c, 0, cx) && resolve_float(c, 1, cy) && resolve_float(c, 2, w) && resolve_float(c, 3, h)) {
            e.flags |= 0x40000u;
            e.limit_center[0] = cx;
            e.limit_center[1] = cy;
            e.limit_half[0] = w;
            e.limit_half[1] = h;
            return 0;
        }
        sim.error_ = c.error;
        return -2;
    }
    case 505:
        e.flags &= ~0x40000u;
        return 0;
    case 506: {
        for (i32& slot2 : e.drop_extra)
            slot2 = 0;
        return 0;
    }
    case 507: {
        i32 id = 0, count = 0;
        if (!resolve_int(c, 0, id) || !resolve_int(c, 1, count)) {
            sim.error_ = c.error;
            return -2;
        }
        if (id == 0) {
            e.drop_main = count;
        } else if (id >= 1 && id <= 14) {
            e.drop_extra[id - 1] = count;
        } else {
            sim.error_ = "dropExtra id outside table";
            return -2;
        }
        return 0;
    }
    case 508: {
        float x = 0, y = 0;
        if (!resolve_float(c, 0, x) || !resolve_float(c, 1, y)) {
            sim.error_ = c.error;
            return -2;
        }
        e.drop_area[0] = x;
        e.drop_area[1] = y;
        return 0;
    }
    case 509:
        if ((sim.mode_flags & 0x30u) != 0x20u) sim.drop_table(e);
        return 0;
    case 510: {
        i32 kind = 0;
        if (!resolve_int(c, 0, kind)) {
            sim.error_ = c.error;
            return -2;
        }
        e.drop_main = kind;
        return 0;
    }
    case 511: {
        i32 life = 0;
        if (resolve_int(c, 0, life)) {
            e.life = float(life);
            e.spell_base_life = life;
            e.spell_life = life * 7;
            if (sim.replay_events) {
                const i32 args[1] = {life};
                sim.sink.boss_op(e.serial, 511, args, 1, "");
            }
            return 0;
        }
        sim.error_ = c.error;
        return -2;
    }
    case 512: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.boss_leaving = false;
        if (slot >= 0 && slot < 3) {
            sim.boss_slot[slot] = e.script_id;
            e.flags |= 0x1000000u;
        } else {
            for (i32& entry : sim.boss_slot)
                if (entry == e.script_id) entry = -1;
            e.flags &= ~0x1000000u;
        }
        if (sim.replay_events) {
            const i32 args[1] = {slot};
            sim.sink.boss_op(e.serial, 512, args, 1, "");
        }
        return 0;
    }
    case 552: {
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        e.mirror_script_base = value;
        return 0;
    }
    case 558: {
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        e.flags ^= ((u32(value) << 20) ^ e.flags) & 0x100000u;
        return 0;
    }
    case 513:
        if (e.serial == sim.boss_main_serial && sim.boss_nonspell_frame < 0) sim.boss_nonspell_frame = i32(sim.frame);
        if (sim.replay_events) sim.sink.boss_op(e.serial, 513, nullptr, 0, "");
        e.age = 0;
        return 0;
    case 515:
    case 541: {
        i32 frames = 0;
        if (!resolve_int(c, 0, frames)) {
            sim.error_ = c.error;
            return -2;
        }
        if (op == 515) {
            e.invulnerable = frames;
            e.invulnerable_clock.set(frames);
        } else {
            e.body_invulnerable = frames;
            e.body_invulnerable_clock.set(frames);
        }
        return 0;
    }
    case 522:
    case 528:
    case 537:
    case 538:
    case 539: {
        const u32 length = ins->argument<u32>(3);
        if (ins->length < 32 || length > u32(ins->length - 32)) {
            sim.error_ = "spell name outside ECL instruction";
            return -2;
        }
        e.spell_name.clear();
        u8 key = 0x77, increment = 7;
        const u8* bytes = reinterpret_cast<const u8*>(ins) + 32;
        for (u32 n = 0; n < length; ++n) {
            const u8 decoded = bytes[n] ^ key;
            if (!decoded) break;
            e.spell_name.push_back(char(decoded));
            key = u8(key + increment);
            increment = u8(increment + 0x10);
        }
        i32 id = 0, duration = 0, bonus = 0;
        if (!resolve_int(c, 0, id) || !resolve_int(c, 2, bonus) || !resolve_int(c, 1, duration)) {
            sim.error_ = c.error;
            return -2;
        }
        e.spell_id = id + (op >= 537 ? sim.diff - (op - 537) : 0);
        e.spell_active = true;
        e.spell_life = i32(e.life) * 7;
        e.age = 0;
        if (!sim.emit_spell(sim.spell.begin(e.spell_id, e.spell_name, duration, sim.session ? sim.session->stage : 1,
                                            sim.diff, sim.bomb_active),
                            e)) {
            if (sim.error_.empty()) sim.error_ = sim.spell.error;
            return -2;
        }
        return 0;
    }
    case 523:
        if (!sim.emit_spell(sim.spell.end(), e)) return -2;
        e.spell_active = false;
        e.spell_id = -1;
        e.spell_name.clear();
        return 0;
    case 323: {
        if (!resolve_int(c, 0, e.death_effect_file) || !resolve_int(c, 1, e.death_effect)) {
            sim.error_ = c.error;
            return -2;
        }
        return 0;
    }
    case 516: {
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        if (id >= 0 && sim.enemy_position_sound && !sim.enemy_position_sound(id, e.position[0])) {
            sim.error_ = "ECL sound failed";
            return -2;
        }
        return 0;
    }
    case 547: {
        float rate = 0;
        if (!resolve_float(c, 0, rate)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.set_time_rate(rate);
        return 0;
    }
    case 517: {
        i32 duration = 0, start = 0, end = 0;
        if (!resolve_int(c, 2, end) || !resolve_int(c, 1, start) || !resolve_int(c, 0, duration)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.screen_shake && !sim.screen_shake(duration, start, end)) {
            sim.error_ = "Screen shake registration failed";
            return -2;
        }
        return 0;
    }
    case 557: {
        i32 duration = 0, mode = 0, color = 0;
        float near_distance = 0, far_distance = 0;
        if (!resolve_int(c, 0, duration) || !resolve_int(c, 1, mode) || !resolve_int(c, 2, color) ||
            !resolve_float(c, 4, far_distance) || !resolve_float(c, 3, near_distance)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.stage_fog) sim.stage_fog(duration, mode, u32(color), near_distance, far_distance);
        return 0;
    }
    case 559:
    case 638:
        return 0;
    case 524: {
        i32 chapter = 0;
        if (!resolve_int(c, 0, chapter)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.session) {
            if (sim.session->chapter != chapter) sim.session->chapter_frame = 0;
            sim.session->chapter = chapter;
        }
        return 0;
    }
    case 527: {
        i32 color = 0, index = 0;
        float life = 0;
        if (!resolve_int(c, 2, color) || !resolve_float(c, 1, life) || !resolve_int(c, 0, index)) {
            sim.error_ = c.error;
            return -2;
        }
        i32 boss_index = 0;
        for (i32 i = 0; i < 3; ++i)
            if (sim.boss_slot[i] == e.script_id) {
                boss_index = i;
                break;
            }
        if (index < 0 || index >= 4) {
            sim.error_ = "Boss marker index";
            return -2;
        }
        auto& marker = sim.boss_markers[size_t(boss_index)][size_t(index)];
        marker.color = color;
        marker.ratio = life / float(e.spell_base_life);
        return 0;
    }
    case 540: {
        if (!resolve_int(c, 0, sim.boss_remaining_stars)) {
            sim.error_ = c.error;
            return -2;
        }
        return 0;
    }
    case 526: {
        float radius = 0;
        if (!resolve_float(c, 0, radius)) {
            sim.error_ = c.error;
            return -2;
        }
        e.bullet_protect_radius2 = float(double(radius) * double(radius));
        return 0;
    }
    case 560: {
        float width = 0, height = 0;
        if (!resolve_float(c, 0, width) || !resolve_float(c, 1, height)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.bullet_bounce_size[0] = width;
        sim.bullet_bounce_size[1] = height;
        return 0;
    }
    case 545:
        sim.cancel_lasers();
        return sim.error_.empty() ? 0 : -2;
    case 556: {
        const u32 length = u32(ins->argument<i32>(0));
        if (ins->length < 20 || length > u32(ins->length - 20)) {
            sim.error_ = "setDeath string";
            return -2;
        }
        const char* name = reinterpret_cast<const char*>(ins) + 20;
        e.death_routine.assign(name, strnlen(name, length));
        return 0;
    }
    case 542:
        sim.spell.set_survival();
        return 0;
    case 543:
        sim.spell.hide_background();
        return 0;
    case 514: {
        const bool spell_route = (sim.mode_flags & 0x30u) == 0x20u && (e.flags & 0x1000000u);
        i32 id = 0, t1 = 0, t2 = 0;
        if (!resolve_int(c, 2, t2) || (!spell_route && !resolve_int(c, 1, t1)) || !resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        std::string name;
        const EclInstruction* i = c.instruction;
        if (spell_route) {
            name = "BossDead";
        } else if (i->parameter_count > 3) {
            const u32 length = u32(i->argument<i32>(3));
            const char* bytes = reinterpret_cast<const char*>(i) + 16 + 4 * 4;
            name.assign(bytes, strnlen(bytes, length ? length - 1 : 0));
        }
        if (id >= 0 && id < 8) {
            auto& slot = e.interrupts[u32(id)];
            if (spell_route) {
                slot.life = 0;
                slot.timeout = t2;
                slot.health_label = "BossDead";
                slot.timeout_label = "BossDead";
            } else {
                slot.life = t1;
                if (t1 >= 0) {
                    slot.timeout = t2;
                    slot.health_label = name;
                    slot.timeout_label = name;
                }
            }
        }
        if (id == 0) e.interrupt_label = name;
        if (spell_route) {
            i32 timeout_slot = 0;
            if (!resolve_int(c, 0, timeout_slot)) {
                sim.error_ = c.error;
                return -2;
            }
            if (timeout_slot >= 0 && timeout_slot < 8) e.interrupts[u32(timeout_slot)].timeout_label = "BossEscape";
        }
        if (sim.replay_events) {
            const i32 args[3] = {id, t1, t2};
            sim.sink.boss_op(e.serial, 514, args, 3, name.c_str());
        }
        return 0;
    }
    case 521: {
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        std::string name;
        const EclInstruction* i = c.instruction;
        if (i->parameter_count > 1) {
            const u32 length = u32(i->argument<i32>(1));
            const char* bytes = reinterpret_cast<const char*>(i) + 16 + 4 * 2;
            name.assign(bytes, strnlen(bytes, length ? length - 1 : 0));
        }
        if (id >= 0 && id < 8) e.interrupts[u32(id)].timeout_label = name;
        if (sim.replay_events) {
            const i32 args[1] = {id};
            sim.sink.boss_op(e.serial, 521, args, 1, name.c_str());
        }
        return 0;
    }
    case 518: {
        i32 entry = 0;
        if (!resolve_int(c, 0, entry)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.dialog_host) sim.dialog_host->open_dialog(entry);
        sim.clear_bullets();
        sim.cancel_lasers(false);
        if (!sim.error_.empty()) return -2;
    }
        [[fallthrough]];
    case 525: {
        return sim.clear_enemies() ? 0 : -2;
    }
    case 519:
        return sim.dialog_host ? (sim.dialog_host->dialog_wait_released() ? 0 : 1) : sim.boss_dialog_wait();
    case 520:
        return sim.boss() ? 1 : 0;
    case 535: {
        i32 value = 0;
        if (!resolve_int(c, 1 + sim.diff, value)) {
            sim.error_ = c.error;
            return -2;
        }
        i32* dest = c.integer_destination(0);
        if (!dest) {
            sim.error_ = c.error.empty() ? "diffI destination" : c.error;
            return -2;
        }
        *dest = value;
        return 0;
    }
    case 536: {
        float value = 0;
        if (!resolve_float(c, 1 + sim.diff, value)) {
            sim.error_ = c.error;
            return -2;
        }
        float* dest = c.float_destination(0);
        if (!dest) {
            sim.error_ = c.error.empty() ? "diffF destination" : c.error;
            return -2;
        }
        *dest = value;
        return 0;
    }
    case 548: {
        i32 frames = 0;
        if (!resolve_int(c, sim.diff, frames)) {
            sim.error_ = c.error;
            return -2;
        }
        c.time -= float(frames);
        return 0;
    }
    case 555: {
        i32 id = 0;
        if (!resolve_int(c, 1, id)) {
            sim.error_ = c.error;
            return -2;
        }
        i32* dest = c.integer_destination(0);
        if (!dest) {
            sim.error_ = c.error.empty() ? "enmAlive destination" : c.error;
            return -2;
        }
        *dest = 0;
        for (const auto& enemy : sim.enemies)
            if (id && enemy->script_id == id) {
                *dest = 1;
                break;
            }
        return 0;
    }
    case 562: {
        sim.drop_table(e);
        return 0;
    }
    case 561: {
        if (e.death_sound >= 0 && sim.enemy_position_sound && !sim.enemy_position_sound(e.death_sound, e.position[0])) {
            sim.error_ = "Enemy death sound failed";
            return -2;
        }
        if (e.death_effect >= 0 && sim.enemy_death_effect) {
            const double dx = double(e.position[0]) - double(e.last_hit_position[0]);
            const double dy = double(e.position[1]) - double(e.last_hit_position[1]);
            const float distance = float(dx * dx + dy * dy);
            const float angle = distance >= 0.040000003f ? float(std::atan2(dy, dx)) : -1.5707964f;
            if (!sim.enemy_death_effect(e.death_effect_file, e.death_effect,
                                        {e.position[0], e.position[1], e.position[2]}, angle)) {
                sim.error_ = "Enemy death animation failed";
                return -2;
            }
        }
        return 0;
    }

    case 600: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = "etNew slot";
            return -2;
        }
        e.et[slot].reset();
        return 0;
    }
    case 601:
        return fire_et(c, e);
    case 602: {
        i32 slot = 0, s1 = 0, s2 = 0;
        if (resolve_int(c, 0, slot) && resolve_int(c, 1, s1) && resolve_int(c, 2, s2) && slot >= 0 && slot < 8) {
            e.et[slot].sprite = u32(s1);
            e.et[slot].subtype = u32(s2);
            if (ecl_sprite_observer) ecl_sprite_observer(sim.frame, e.serial, slot, s1, s2);
            return 0;
        }
        sim.error_ = "etSprite args";
        return -2;
    }
    case 603: {
        i32 slot = 0;
        float x = 0, y = 0;
        if (resolve_int(c, 0, slot) && resolve_float(c, 1, x) && resolve_float(c, 2, y) && slot >= 0 && slot < 8) {
            e.et[slot].offset[0] = x;
            e.et[slot].offset[1] = y;
            return 0;
        }
        sim.error_ = "etOffset args";
        return -2;
    }
    case 604: {
        i32 slot = 0;
        float a = 0, b = 0;
        if (resolve_int(c, 0, slot) && resolve_float(c, 1, a) && resolve_float(c, 2, b) && slot >= 0 && slot < 8) {
            e.et[slot].angle1 = a;
            e.et[slot].angle2 = b;
            return 0;
        }
        sim.error_ = "etAngle args";
        return -2;
    }
    case 605: {
        i32 slot = 0;
        float a = 0, b = 0;
        if (resolve_int(c, 0, slot) && resolve_float(c, 1, a) && resolve_float(c, 2, b) && slot >= 0 && slot < 8) {
            e.et[slot].speed1 = a;
            e.et[slot].speed2 = b;
            return 0;
        }
        sim.error_ = "etSpeed args";
        return -2;
    }
    case 606: {
        i32 slot = 0, count = 0, ways = 0;
        if (resolve_int(c, 0, slot) && resolve_int(c, 1, count) && resolve_int(c, 2, ways) && slot >= 0 && slot < 8) {
            e.et[slot].count = i16(count);
            e.et[slot].ways = i16(ways);
            return 0;
        }
        sim.error_ = "etCount args";
        return -2;
    }
    case 607: {
        i32 slot = 0, mode = 0;
        if (resolve_int(c, 0, slot) && resolve_int(c, 1, mode) && slot >= 0 && slot < 8) {
            e.et[slot].aim = i16(mode);
            return 0;
        }
        sim.error_ = "etAim args";
        return -2;
    }
    case 608: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8 || !resolve_int(c, 1, e.et[slot].sound) ||
            !resolve_int(c, 2, e.et[slot].transform_sound)) {
            sim.error_ = c.error.empty() ? "etSound args" : c.error;
            return -2;
        }
        return 0;
    }
    case 609:
    case 610:
    case 611:
    case 612: {
        i32 slot = 0, index = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = "etEx args";
            return -2;
        }
        const bool append = op == 611 || op == 612;
        const bool extended = op == 610 || op == 612;
        u32 argument = 1;
        if (append)
            index = i32(e.et[slot].ex_next);
        else if (!resolve_int(c, argument++, index)) {
            sim.error_ = c.error;
            return -2;
        }
        if (index < 0 || index >= 18) {
            sim.error_ = "etEx queue index";
            return -2;
        }
        StageEt::Ex& item = e.et[slot].ex[index];
        i32 flags = 0, ex_op = 0;
        if (!resolve_int(c, argument++, flags) || !resolve_int(c, argument++, ex_op)) {
            sim.error_ = c.error;
            return -2;
        }
        item.flags = u32(flags);
        item.op = u32(ex_op);
        const u32 count = extended ? 4 : 2;
        for (u32 n = 0; n < count; ++n) {
            i32 value = 0;
            if (!resolve_int(c, argument++, value)) {
                sim.error_ = c.error;
                return -2;
            }
            item.args[4 + n] = float(value);
            item.integer_arguments[n] = value;
        }
        for (u32 n = 0; n < count; ++n)
            if (!resolve_float(c, argument++, item.args[n])) {
                sim.error_ = c.error;
                return -2;
            }
        e.et[slot].ex_next = u32(index + 1);
        return 0;
    }
    case 613:
        sim.clear_bullets();
        sim.cancel_lasers(false);
        return sim.error_.empty() ? 0 : -2;
    case 614: {
        i32 to = 0, from = 0;
        if (resolve_int(c, 0, to) && resolve_int(c, 1, from) && to >= 0 && to < 8 && from >= 0 && from < 8) {
            const u32 append_cursor = e.et[to].ex_next;
            e.et[to] = e.et[from];
            e.et[to].ex_next = append_cursor;
            return 0;
        }
        sim.error_ = "etCopy args";
        return -2;
    }
    case 615:
    case 616: {
        float radius = 0;
        if (!resolve_float(c, 0, radius)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.cancel_bullets_radius(e, radius, op == 615);
        sim.cancel_lasers_radius({e.position[0], e.position[1], e.position[2]}, radius);
        return sim.error_.empty() ? 0 : -2;
    }
    case 626: {
        i32 slot = 0;
        float angle = 0, radius = 0;
        if (!resolve_int(c, 0, slot) || !resolve_float(c, 1, angle) || !resolve_float(c, 2, radius) || slot < 0 ||
            slot >= 8) {
            sim.error_ = c.error.empty() ? "etOffsetRad args" : c.error;
            return -2;
        }
        polar_xy(angle, radius, e.et[slot].offset[0], e.et[slot].offset[1]);
        return 0;
    }
    case 627: {
        i32 slot = 0;
        float distance = 0;
        if (!resolve_int(c, 0, slot) || !resolve_float(c, 1, distance) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "etDist args" : c.error;
            return -2;
        }
        e.et[slot].spin = distance;
        return 0;
    }
    case 628: {
        i32 slot = 0;
        float x = 0, y = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8 || !resolve_float(c, 1, x) || !resolve_float(c, 2, y)) {
            sim.error_ = c.error.empty() ? "absolute emitter offset arguments" : c.error;
            return -2;
        }
        e.et[slot].absolute[0] = x;
        e.et[slot].absolute[1] = y;
        e.et[slot].absolute[2] = x >= -990.f ? 1.f : 0.f;
        return 0;
    }
    case 624: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = "etSpeedD slot";
            return -2;
        }
        float s1 = 0, s2 = 0;
        if (!resolve_float(c, 1 + sim.diff, s1) || !resolve_float(c, 5 + sim.diff, s2)) {
            sim.error_ = c.error;
            return -2;
        }
        e.et[slot].speed1 = s1;
        e.et[slot].speed2 = s2;
        return 0;
    }
    case 625: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = "etCountD slot";
            return -2;
        }
        i32 count = 0, ways = 0;
        if (!resolve_int(c, 1 + sim.diff, count) || !resolve_int(c, 5 + sim.diff, ways)) {
            sim.error_ = c.error;
            return -2;
        }
        e.et[slot].count = i16(count);
        e.et[slot].ways = i16(ways);
        return 0;
    }
    case 640: {
        i32 slot = 0, index = 0;
        if (!resolve_int(c, 0, slot) || !resolve_int(c, 1, index) || slot < 0 || slot >= 8 || index < 0 ||
            index >= 18) {
            sim.error_ = c.error.empty() ? "etExSub slot" : c.error;
            return -2;
        }
        if (ins->length < 32) {
            sim.error_ = "Truncated etExSub name";
            return -2;
        }
        const u32 length = ins->argument<u32>(2);
        const char* name = reinterpret_cast<const char*>(ins) + 28;
        if (!length || length > u32(ins->length - 28) || !std::memchr(name, 0, length)) {
            sim.error_ = "Invalid etExSub name";
            return -2;
        }
        e.et[slot].ex[index].subroutine = name;
        return 0;
    }
    case 630: {
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        if (!sim.stage_script_interrupt) {
            sim.error_ = "STD camera script unavailable for ECL630 interrupt " + std::to_string(id);
            return -2;
        }
        if (!sim.stage_script_interrupt(id)) {
            sim.error_ = "STD camera script interrupt failed for id " + std::to_string(id);
            return -2;
        }
        return 0;
    }
    case 631: {
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.boss_leaving = (value & 1) != 0;
        return 0;
    }
    case 629: {
        float radius = 0;
        i32 color = 0;
        e.fog.reset();
        if (!resolve_float(c, 0, radius) || !resolve_int(c, 1, color)) {
            sim.error_ = c.error;
            return -2;
        }
        if (radius > 0) {
            e.fog = std::make_unique<EnemyFog>();
            e.fog->target = radius;
            e.fog->color = u32(color);
        }
        if (sim.enemy_fog && !sim.enemy_fog(e, true)) {
            sim.error_ = "Enemy fog creation failed";
            return -2;
        }
        return 0;
    }
    case 632:
    case 639: {
        i32 index = 0;
        if (!resolve_int(c, 0, index)) {
            sim.error_ = c.error;
            return -2;
        }
        if (index < 0 || index > 6) {
            sim.error_ = "Enemy function table index";
            return -2;
        }
        e.native_function = index;
        e.native_function_mode = op == 639 ? 1 : 0;
        return 0;
    }
    case 637: {
        i32 index = 0;
        if (!resolve_int(c, 0, index)) {
            sim.error_ = c.error;
            return -2;
        }
        return sim.invoke_enemy_function(e, index) ? 0 : -2;
    }
    case 633: {
        i32 index = 0;
        if (!resolve_int(c, 0, index)) {
            sim.error_ = c.error;
            return -2;
        }
        if (index < 0 || index > 1) {
            sim.error_ = "Enemy damage function index";
            return -2;
        }
        e.damage_function = index;
        return 0;
    }

    case 700: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "laserNew slot" : c.error;
            return -2;
        }
        for (u32 n = 0; n < 3; ++n) {
            if (!resolve_float(c, n + 1, e.et[slot].spawn[n])) {
                sim.error_ = c.error;
                return -2;
            }
        }
        if (!resolve_float(c, 4, e.et[slot].laser_width)) {
            sim.error_ = c.error;
            return -2;
        }
        return 0;
    }
    case 701: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "laserTiming slot" : c.error;
            return -2;
        }
        for (u32 n = 0; n < 5; ++n) {
            if (!resolve_int(c, n + 1, e.et[slot].laser_timing[n])) {
                sim.error_ = c.error;
                return -2;
            }
        }
        return 0;
    }
    case 703: {
        i32 slot = 0, id = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8 || !resolve_int(c, 1, id)) {
            sim.error_ = c.error.empty() ? "laserStOn args" : c.error;
            return -2;
        }
        sim.emit_infinite_laser(e.et[slot], e, id);
        return sim.error_.empty() ? 0 : -2;
    }
    case 708: {
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        if (StageLaser* laser = sim.find_laser(id)) {
            if (!resolve_float(c, 1, laser->angle)) {
                sim.error_ = c.error;
                return -2;
            }
        }
        return 0;
    }
    case 704:
    case 705: {
        i32 id = 0;
        float x = 0, y = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        StageLaser* laser = sim.find_laser(id);
        if (!resolve_float(c, 1, x) || !resolve_float(c, 2, y)) {
            sim.error_ = c.error;
            return -2;
        }
        if (laser) {
            if (op == 704) {
                laser->origin[0] = x;
                laser->origin[1] = y;
                laser->origin[2] = 0;
            } else if (laser->kind == StageLaser::Kind::Infinite) {
                laser->velocity[0] = x;
                laser->velocity[1] = y;
                laser->velocity[2] = 0;
            } else {
                laser->line_descriptor_angle = x;
                laser->target_length = y;
                laser->line_initial_length = 0;
            }
        }
        return 0;
    }
    case 706:
    case 707:
    case 709:
    case 714: {
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        if (StageLaser* laser = sim.find_laser(id)) {
            if (op == 714) {
                i32 value = 0;
                if (!resolve_int(c, 1, value)) {
                    sim.error_ = c.error;
                    return -2;
                }
                if (laser->kind == StageLaser::Kind::Beam)
                    laser->beam_descriptor_flags = (laser->beam_descriptor_flags & ~1u) | (u32(value) & 1u);
            } else {
                float value = 0;
                if (!resolve_float(c, 1, value)) {
                    sim.error_ = c.error;
                    return -2;
                }
                if (op == 706)
                    laser->length_speed = value;
                else if (op == 707)
                    laser->width = value;
                else if (laser->kind == StageLaser::Kind::Infinite)
                    laser->rotation = value;
                else
                    laser->target_width = value;
            }
        }
        return 0;
    }
    case 710: {
        for (;;) {
            i32 id = 0;
            if (!resolve_int(c, 0, id)) {
                sim.error_ = c.error;
                return -2;
            }
            StageLaser* laser = sim.find_laser(id);
            if (!laser) return 0;
            if (!sim.cancel_laser(*laser)) return -2;
            laser->id = 0;
        }
    }
    case 702: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "line laser emitter slot" : c.error;
            return -2;
        }
        sim.emit_line_laser(e.et[slot], e);
        return sim.error_.empty() ? 0 : -2;
    }
    case 713: {
        i32 slot = 0, id = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8 || !resolve_int(c, 1, id)) {
            sim.error_ = c.error.empty() ? "beam laser arguments" : c.error;
            return -2;
        }
        sim.emit_beam_laser(e.et[slot], e, id);
        return sim.error_.empty() ? 0 : -2;
    }
    case 711: {
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "curve laser slot" : c.error;
            return -2;
        }
        sim.emit_curve_laser(e.et[slot], e);
        return sim.error_.empty() ? 0 : -2;
    }
    case 801: {
        i32 id = 0;
        if (!resolve_int(c, 2, id)) {
            sim.error_ = c.error;
            return -2;
        }
        StageEnemy* source = sim.find_by_script_id(id);
        if (!source) {
            sim.error_ = "enmPosRead missing enemy " + std::to_string(id);
            return -2;
        }
        float* x = c.float_destination(0);
        if (!x) {
            sim.error_ = c.error;
            return -2;
        }
        *x = source->position[0];
        float* y = c.float_destination(1);
        if (!y) {
            sim.error_ = c.error;
            return -2;
        }
        *y = source->position[1];
        return 0;
    }
    case 800: {
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        StageEnemy* target = sim.find_by_script_id(id);
        if (!target) return 0;
        if (!target->threads) {
            sim.error_ = "ECL800 target threads unavailable";
            return -2;
        }
        if (ins->length < 28) {
            sim.error_ = "Truncated ECL800 routine name";
            return -2;
        }
        const u32 name_length = ins->argument<u32>(1);
        const char* name = reinterpret_cast<const char*>(ins) + 0x18;
        if (!name_length || name_length > u32(ins->length - 0x18) || !std::memchr(name, 0, name_length)) {
            sim.error_ = "Invalid ECL800 routine name";
            return -2;
        }
        const std::string routine(name, strnlen(name, name_length - 1));
        const u8 difficulty = target->threads->main.difficulty;
        if (!target->threads->restart(routine.c_str(), difficulty)) {
            sim.error_ = "ECL800 remount " + routine + ": " + target->threads->error;
            return -2;
        }
        if (&target->threads->main == &c) {
            if (!c.code || c.routine < 0) {
                return -1;
            }
            const auto* first = reinterpret_cast<const EclInstruction*>(c.code);
            if (first->length < 16 || first->length % 4) {
                sim.error_ = "ECL800 remount landed on invalid instruction";
                return -2;
            }
            c.instruction = first;
            c.instruction_offset = i32(first->length) - i32(ins->length);
        }
        return 0;
    }
    case 900:
        return 0;
    case 1000: {
        i32 kind = 0;
        if (!resolve_int(c, 0, kind)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.items) sim.items->spawn_value_item(kind, e.position[0], e.position[1], e.position[2], 2.0f);
        return 0;
    }
    case 1001:
    case 1002:
    case 1003: {
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        if (op == 1001)
            e.death_spirit_limit = value;
        else if (op == 1002)
            e.death_spirit_count = value;
        else
            e.damage_spirits_disabled = (value & 1) != 0;
        return 0;
    }

    default:
        sim.error_ = "unimplemented game opcode " + std::to_string(op);
        return -2;
    }
}

int StageCommands::create_enemy(EclContext& c, u16 op) {
    StageSim& sim = *this->sim;
    StageEnemy& e = *sim.current;
    if (op >= 309 && op <= 312 && sim.boss()) return 0;
    const EclInstruction* ins = c.instruction;
    const u32 length = u32(ins->argument<i32>(0));
    const char* name = reinterpret_cast<const char*>(ins) + 16 + 4;
    std::string routine(name, strnlen(name, length ? length - 1 : 0));

    float x = 0, y = 0;
    if (!resolve_postname_float(c, 0, 1, x) || !resolve_postname_float(c, 1, 2, y)) {
        sim.error_ = c.error;
        return -2;
    }
    i32 life = 0;
    if (!resolve_postname_int(c, 2, 3, life)) {
        sim.error_ = c.error;
        return -2;
    }
    const bool relative = op == 300 || op == 309 || op == 321 || op == 311 || op == 304;
    bool mirror = op == 311 || op == 304 || op == 312 || op == 305;
    if (relative) {
        x = x + e.position[0];
        y = y + e.position[1];
    }
    if (e.flags & 0x100000u) {
        mirror = !mirror;
        x = x * -1.0f;
    }
    StageSpawnArguments initial;
    if (!resolve_postname_int(c, 3, 4, initial.score) || !resolve_postname_int(c, 4, 5, initial.drop_main)) {
        sim.error_ = c.error;
        return -2;
    }
    std::copy_n(e.i_slots, 4, initial.integers.begin());
    std::copy_n(e.f_slots, initial.floats.size(), initial.floats.begin());
    sim.spawn_enemy(routine.c_str(), x, y, float(life), &e, false, mirror, &initial);
    return 0;
}

int StageCommands::fire_et(EclContext& c, StageEnemy& e) {
    StageSim& sim = *this->sim;
    i32 slot = 0;
    if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
        sim.error_ = "etOn slot";
        return -2;
    }
    StageEt& et = e.et[slot];
    float spawn[3];
    if (et.absolute[2] <= 0.9f) {
        spawn[0] = et.offset[0] + e.position[0];
        spawn[1] = et.offset[1] + e.position[1];
        spawn[2] = et.offset[2] + e.position[2];
    } else {
        spawn[0] = et.absolute[0] + et.offset[0];
        spawn[1] = et.absolute[1] + et.offset[1];
        spawn[2] = et.absolute[2] + et.offset[2];
    }
    et.spawn[0] = spawn[0];
    et.spawn[1] = spawn[1];
    et.spawn[2] = et.absolute[2] <= 0.9f ? spawn[2] : 0.0f;
    float aim_px, aim_py;
    sim.nearest_player(spawn[0], spawn[1], aim_px, aim_py);
    const float dy = aim_py - spawn[1];
    const float dx = aim_px - spawn[0];
    const float aim =
        (dy == 0.0f && dx == 0.0f) ? 1.5707964f : float(std::atan2(static_cast<double>(dy), static_cast<double>(dx)));
    sim.active_bullet_protect_radius2 = e.bullet_protect_radius2;
    sim.emit_et(et, e.serial, u32(slot), spawn, aim);
    sim.active_bullet_protect_radius2 = 0;
    return sim.error_.empty() ? 0 : -2;
}

}
