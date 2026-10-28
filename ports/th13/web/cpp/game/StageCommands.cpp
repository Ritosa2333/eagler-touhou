// stage1 游戏指令执行（0x41b260 处理块的行为子集）。锚点注释对应 ecl-vm.md §4.2。
// 未实现指令一律置 sim->error 并返回 -2（harness 非零退出）——不静默吞掉契约缺口。
#include "StageSim.hpp"

#include "BulletAppearance.hpp"
#include "AnmVm.hpp"
#include "StageMath.hpp"
#include "SessionState.hpp"

#include <cmath>
#include <cstring>

namespace th13 {

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
    const u32 base = ((name_length + 4) >> 2) + index;  // 0x41fcc0:29-30
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

// 镜像翻转表（0x41c137/0x41c21f）：非 NM 变体在敌机镜像位下翻转角度。
bool mirrored_variant(u16 op) { return op == 404 || op == 405 || op == 406 || op == 407; }
}  // namespace

int StageCommands::execute(EclContext& c, u16 op) {
    StageSim& sim = *this->sim;
    StageEnemy& e = *sim.current;
    const EclInstruction* ins = c.instruction;

    // Logging receives already evaluated operands. It must never read a
    // global/random argument or alter the ECL stack on its own.
    const auto emit_move = [&](const u32* integral, std::initializer_list<float> values) {
        sim.sink.enemy_move(e.serial,op,integral,values.begin(),u32(values.size()));
    };

    switch (op) {
    // ---------- 300 族：敌人/ANM ----------
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
    case 302: {  // anmSelect（vm+0x98 文件槽）——事件无涉
        i32 file = 0;
        if (resolve_int(c, 0, file)) e.anm_file = file;
        return 0;
    }
    case 320: { // 41b3ae: per-slot XYZ offset, Z is always zero.
        i32 slot = 0; float x = 0, y = 0;
        if (!resolve_int(c, 0, slot) || !resolve_float(c, 1, x) ||
            !resolve_float(c, 2, y)) { sim.error_ = c.error; return -2; }
        if (slot < 0 || slot >= StageEnemy::kAnimationSlots) {
            sim.error_ = "animation offset slot"; return -2;
        }
        e.anm_slot_offset[slot] = {x,y,0};
        return 0;
    }
    case 303:   // anmScript（0x41fe80：槽位 VM 创建/清槽 + 句柄存储）
    case 306: {  // anmRun（0x41b260 case3：同 303 + slot0 附加字段/主形身份）
        i32 slot = 0, script = 0;
        const bool resolved = resolve_int(c, 0, slot) && resolve_int(c, 1, script);
        if (resolved && slot >= 0 && slot < StageEnemy::kAnimationSlots)
            e.anm_script[slot] = script;  // script < 0 = 清槽（0x41fe80）
        if (resolved && op == 306 && slot == 0) {
            e.anm_base_script = script;
            e.mirror_dir = 0;
        }
        // push 门须与解析成败绑定（0x41fe80:0x41ff00 门 = script ≥ 0；
        // resolve_int 失败时 out 参数保持初值（EclContext 失败路径不写值），
        // 此前残值 slot=0/script=0 会通过范围检查 push 伪创建请求，且 306
        // 残值还会误置镜像门 flags/mirror_prev_x——对比同函数 307/308 分支
        // resolve 失败即 return -2。失败时 c.error 亦不得被 return 0 吞掉。
        if (resolved && !sim.anm_effects_noop && sim.ecl_slot_anm && slot >= 0 && slot < StageEnemy::kAnimationSlots) {
            // case3 slot0 附加：flags 0x200000（镜像翻转重挂门，0x419450:102；
            // +0x270 构造 0、+0x78 首拍速度 0 ⟹ 差分基线=当前生成位，避免
            // 首次 sync 误判朝向）
            if (op == 306 && slot == 0) {
                e.flags |= 0x200000u;
                e.mirror_prev_x = e.position[0];
            }
            sim.anm_effects.push_back({e.anm_file, script, 0, 0, 0,
                                        1, u16(op), slot, e.serial});
        }
        if (!resolved) {
            sim.error_ = c.error.empty() ? "anmScript args" : c.error;
            return -2;
        }
        return 0;
    }
    case 317: { //41b9e7 ->46fc30: deferred interrupt, label narrowed to signed16.
        i32 slot=0,label=0;
        if(!resolve_int(c,0,slot)||!resolve_int(c,1,label)){sim.error_=c.error;return -2;}
        if(slot<0||slot>=StageEnemy::kAnimationSlots){sim.error_="animation interrupt slot";return -2;}
        label=i32(i16(u16(label)));
        if(sim.enemy_animation_interrupt&&!sim.enemy_animation_interrupt(e,slot,label)) {
            if(sim.error_.empty())sim.error_="Enemy animation interrupt failed";
            return -2;
        }
        return 0;
    }
    case 318: { // 41b897: restore slot0 from saved26c, clear automatic turning.
        e.flags &= ~0x200000u;
        e.mirror_dir = 0;
        e.anm_script[0] = e.anm_base_script;
        if (!sim.anm_effects_noop && sim.ecl_slot_anm)
            sim.anm_effects.push_back({e.anm_file, e.anm_base_script, 0, 0, 0,
                                       1, u16(op), 0, e.serial});
        return 0;
    }
    case 307:   // anmAt（case4：file=arg0、script=arg1、位置=敌机 xyz、主池头入链）
    case 308: {  // anmVm（case5 → 0x4243e0：同上但位置清零 0x4243e0:22-24）
        if (sim.anm_effects_noop) return 0;  // 事件契约：stage1 oracle NOOP 同源
        i32 file = 0, script = 0;
        if (!resolve_int(c, 0, file) || !resolve_int(c, 1, script)) return -2;
        if (op == 308)
            sim.anm_effects.push_back({file, script, 0, 0, 0});
        else
            sim.anm_effects.push_back(
                {file, script, e.position[0], e.position[1], e.position[2]});
        return 0;
    }
    case 313:   // anmFlip（case7 @ 0x41b903）：槽 VM 重挂 script_base+5
    case 316: { // anmFlipDelta（case0xa @ 0x41b98f）：script_base+arg1+5
        // 原版不读脚本参数：arg0 = slot（316 另 arg1 = delta），脚本号 =
        // 敌机+0x26c（case3/306 slot0 记录的挂接脚本，port 代理
        // e.anm_base_script）+5（316 再 +delta），文件 = 302 选定的 +0x260。
        // 两 case 汇入 0x41b93c 共享尾（与 306 同构）：0x470110 回收旧槽 VM
        // → 0x4243e0 模板拷贝+立即 tick+0x46f4a0 主池头插 → 句柄写回敌机
        // +0x120 槽数组 → slot0 刷新 +0x3f30/0x3f34 尺寸（(vm+0x84)×(vm+0x64)
        // /(vm+0x80)×(vm+0x60)）；层 = [+0x278]+7。不动 0x200000 镜像门与
        // +0x264..0x270（仅 306 写 / 318 清）。st01 MBoss2 与 st01bs Boss/
        // Boss1/Boss2/BossCard1-4 全部经此翻转敌机槽 0 演出 VM。
        if (sim.anm_effects_noop) return 0;  // 事件契约：headless oracle NOOP 同源
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
            // +0x26c retains306's base even when303 replaces the live slot.
            const i32 base = e.anm_base_script;
            sim.anm_effects.push_back({e.anm_file, base + delta + 5, 0, 0, 0,
                                        1, u16(op), slot, e.serial});
        }
        return 0;
    }
    case 334: { // 41b7ac ->415650(null,kind), global scene-effect table.
        i32 kind=0;if(!resolve_int(c,0,kind)){sim.error_=c.error;return -2;}
        // 41b7ac: push edi+0x44（敌机 VM+0x44 主位置）→415650 [ebp+0x10]=init 401000 位置。
        const Vec3 position{e.position[0],e.position[1],e.position[2]};
        if(!sim.scene_effect||!sim.scene_effect(kind,position)){sim.error_="Scene effect "+std::to_string(kind)+" unavailable";return -2;}
        return 0;
    }
    case 319:
    case 325: case 326: case 327: case 328:
    case 329: case 330: case 331: case 332: case 333: case 335: {
        // 41ffb0: resolve the live animation before any mutation arguments.
        i32 slot = 0;
        if (!resolve_int(c, 0, slot)) { sim.error_ = c.error; return -2; }
        if (slot < 0 || slot >= StageEnemy::kAnimationSlots) { sim.error_ = "animation mutation slot"; return -2; }
        if (!sim.enemy_animation) return 0;
        AnmVm* vm = sim.enemy_animation(e, slot);
        if (!sim.error_.empty()) return -2;
        if (!vm) return 0; // retail skips all remaining argument side effects
        const auto integer = [&](u32 n) { i32 v=0;resolve_int(c,n,v);return v; };
        const auto real = [&](u32 n) { float v=0;resolve_float(c,n,v);return v; };
        if (op == 319) {
            vm->variables.rotation.z = real(1); vm->visual.flags |= 4;
        } else if (op == 327 || op == 331) {
            const u32 alpha = u8(integer(1));
            u32& color = op == 327 ? vm->visual.color : vm->visual.secondary_color;
            color = (color & 0xffffffu) | alpha << 24;
        } else if (op == 328 || op == 332) {
            const i32 alpha = u8(integer(3)), mode = integer(2), duration = integer(1);
            auto& channel = op == 328 ? vm->interpolators.alpha : vm->interpolators.secondary_alpha;
            const u32 color = op == 328 ? vm->visual.color : vm->visual.secondary_color;
            channel.control1 = {}; channel.control2 = {};
            channel.begin(duration, mode, {i32(color >> 24)}, {alpha});
        } else if (op == 329 || op == 335) {
            const float y = real(2), x = real(1);
            auto& scale = op == 329 ? vm->visual.scale : vm->visual.secondary_scale;
            scale = {x,y}; vm->visual.flags |= 8;
        } else if (op == 330) {
            const float y = real(4), x = real(3);
            const i32 mode = integer(2), duration = integer(1);
            vm->interpolators.scale.begin(duration,mode,
                                          {vm->visual.scale.x,vm->visual.scale.y},{x,y});
        } else if (op == 333) {
            // 4202a7: Y,X,slot again,mode,duration. The cached destination
            // VM interpolates from the freshly resolved source's574 XYZ.
            const float y = real(4), x = real(3);
            const i32 source_slot = integer(0);
            if (!c.error.empty()) { sim.error_ = c.error; return -2; }
            if (source_slot < 0 || source_slot >= StageEnemy::kAnimationSlots) {
                sim.error_ = "animation position source slot"; return -2;
            }
            AnmVm* source = sim.enemy_animation(e, source_slot);
            if (!source) { sim.error_ = "animation position source missing"; return -2; }
            const i32 mode = integer(2), duration = integer(1);
            auto& channel = vm->interpolators.position;
            channel.control1 = {}; channel.control2 = {};
            channel.begin(duration,mode,
                {source->visual.translation.x,source->visual.translation.y,source->visual.translation.z},
                {x,y,0});
        } else if (op == 325) {
            const u32 b = u8(integer(3)), g = u8(integer(2)), r = u8(integer(1));
            vm->visual.color = (vm->visual.color & 0xff000000u) | b | g<<8 | r<<16;
        } else { // 326 -> 4240c0
            const i32 r = u8(integer(3)), g = u8(integer(4)), b = u8(integer(5));
            const i32 mode = integer(2), duration = integer(1);
            const u32 color = vm->visual.color;
            auto& channel = vm->interpolators.color;
            channel.control1 = {}; channel.control2 = {};
            channel.begin(duration,mode,{i32(color&255),i32((color>>8)&255),i32((color>>16)&255)},
                                        {b,g,r});
        }
        if (!c.error.empty()) { sim.error_ = c.error; return -2; }
        return 0;
    }
    case 553: { // 41b364: hit voice override at inner+3fc8
        if (!resolve_int(c, 0, e.hit_sound)) { sim.error_ = c.error; return -2; }
        return 0;
    }
    case 554:
        return 0;  // ANM 插值族/关卡 logo：0x41b400/0x42adc0，视觉分期接入

    // ---------- 400 族：运动 ----------
    case 400:
    case 402: {
        const u32 integral[2] = {0, 0};
        StageMover& m = op == 400 ? e.vel : e.pos;  // 0x41bb0d：400 → vel 子位置，402 → pos
        float x = 0, y = 0;
        if (!resolve_float(c, 0, x) || !resolve_float(c, 1, y)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral,{x,y});
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
            e.position[0] = std::max(e.limit_center[0] - hw,
                                     std::min(e.position[0], e.limit_center[0] + hw));
            e.position[1] = std::max(e.limit_center[1] - hh,
                                     std::min(e.position[1], e.limit_center[1] + hh));
            for (u32 i = 0; i < 3; ++i) e.vel.pos[i] = float(e.position[i] - e.pos.pos[i]);
        }
        return 0;
    }
    case 401:
    case 403: {
        const u32 integral[5] = {1, 1, 0, 0, 0};
        StageMover& m = op == 401 ? e.vel : e.pos;
        i32 dur = 0, mode = 0;
        float x = 0, y = 0;
        if (!resolve_float(c,2,x) || !resolve_float(c,3,y) || !resolve_int(c,0,dur)) {
            sim.error_ = c.error;
            return -2;
        }
        m.mode &= 0xFFFFFFF0u;
        if (dur < 1) {
            m.postime.duration = 0;
            return 0;
        }
        // 41bb..: positive duration is read a second time, before mode.
        if (!resolve_int(c,0,dur) || !resolve_int(c,1,mode)) { sim.error_=c.error;return -2; }
        emit_move(integral,{float(dur),float(mode),x,y});
        if (mode == 7 || mode == 17) {  // 模式 8 归 moveBezier（425）路径
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
        StageMover& m = (op == 404 || op == 428) ? e.vel : e.pos;  // 0x41c137
        float angle = 0, speed = 0;
        if (!resolve_float(c, 0, angle) || !resolve_float(c, 1, speed)) {
            sim.error_ = c.error;
            return -2;
        }
        // Record the operands actually consumed above. Reading RANDRAD
        // again for tracing advances game RNG and changes BossDead motion.
        const float values[2] = {angle,speed};
        sim.sink.enemy_move(e.serial,op,integral,values,2);
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
        StageMover& m = (op == 405 || op == 429) ? e.vel : e.pos;  // 0x41c21f
        i32 dur = 0, mode = 0;
        float angle = 0, speed = 0;
        if (!resolve_float(c,2,angle) || !resolve_float(c,3,speed) || !resolve_int(c,0,dur)) {
            sim.error_ = c.error;
            return -2;
        }
        if (dur < 1) {
            m.polar.duration = 0;
            return 0;
        }
        if (!resolve_int(c,1,mode)) { sim.error_=c.error;return -2; }
        if (mode == 7 || mode == 8 || mode == 17) {
            sim.error_ = "moveVelTime interpolation mode not implemented";
            return -2;
        }
        const float trace_angle=angle;
        if (kKeep < angle && (e.flags & 0x100000u) && mirrored_variant(op)) angle = mirror_angle(angle);
        float from_a = m.angle, from_b = m.speed;
        float to_a = kKeep < angle ? angle : m.angle;
        const float to_b = kKeep < speed ? speed : m.speed;
        shortest_arc(from_a, to_a);
        // 41c21f rereads duration after resolving the target/mode.
        if (!resolve_int(c,0,dur)) { sim.error_=c.error;return -2; }
        emit_move(integral,{float(dur),float(mode),trace_angle,speed});
        m.polar.setup(float(dur), mode, from_a, from_b, to_a, to_b);
        m.mode &= 0xFFFFFFF2u;  // 清插值挂接标志（保持其余位）
        return 0;
    }
    case 408:
    case 410: {  // moveCircle（0x41c471）
        const u32 integral[4] = {0, 0, 0, 0};
        StageMover& m = (op == 408) ? e.vel : e.pos;
        float a0 = 0, a1 = 0, a2 = 0, a3 = 0;
        if (!resolve_float(c, 0, a0) || !resolve_float(c, 1, a1) || !resolve_float(c, 2, a2) ||
            !resolve_float(c, 3, a3)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral,{a0,a1,a2,a3});
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
        // 0x41c56b→0x473a10：mode4 时 pos[3..5]=pos[0..2]，随即 0x473780
        // 累加一次（mode2 子位置 = 圆心 + polar(angle, radius) + 量化）；
        // 0x41c572→0x419950：主位置立即同步（指令内生效，不等下帧积分）。
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
    case 422: { // 41c80c, case 0x1e: absolute/relative ellipse movers
        const u32 integral[6] = {};
        StageMover& m = op == 420 ? e.vel : e.pos;
        float args[6] = {};
        for (u32 n = 0; n < 6; ++n) {
            if (!resolve_float(c, n, args[n])) { sim.error_ = c.error; return -2; }
        }
        emit_move(integral,{args[0],args[1],args[2],args[3],args[4],args[5]});
        // Retail tests mode == 2, even when replacing an existing ellipse.
        if ((m.mode & 15) != 2)
            for (u32 n = 0; n < 3; ++n) m.aux[n] = m.pos[n];
        if (kKeep < args[0]) m.angle = wrap_angle(args[0]);
        if (kKeep < args[1]) m.speed = args[1];
        if (kKeep < args[2]) m.radius = args[2];
        if (kKeep < args[3]) m.growth = args[3];
        if (kKeep < args[4]) m.ellipse_angle = wrap_angle(wrap_angle(args[4]));
        if (kKeep < args[5]) m.ellipse_ratio = args[5];
        m.mode = (m.mode & 0xfffffff3u) | 3u;
        m.accumulate(); // 473a10 does not advance angle/radius
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
    case 411: {  // 41c57c: simultaneous angular-speed and radius/growth interpolation
        // Both channels capture their current values; keep sentinels preserve
        // the target. Radius is overwritten by its channel before radial
        // velocity is added each tick, rather than accumulating that velocity.
        const u32 integral[5] = {1, 1, 0, 0, 0};
        StageMover& m = (op == 409) ? e.vel : e.pos;
        i32 dur = 0, mode = 0;
        float speed_to = 0, radius = 0, growth = 0;
        if (!resolve_float(c,2,speed_to) || !resolve_float(c,3,radius) ||
            !resolve_float(c,4,growth) || !resolve_int(c,0,dur) || !resolve_int(c,1,mode)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral,{float(dur),float(mode),speed_to,radius,growth});
        if (dur < 1) {
            m.polar.duration = 0;
            m.radial.duration = 0;
            return 0; // 41c6b9 exits before changing circle mode/position
        } else {
            if (mode == 7 || mode == 8 || mode == 17) {
                sim.error_ = "moveCircleTime interpolation mode not implemented";
                return -2;
            }
            // 0x41c627 fldz：角维 from=to=0.0（恒零）——0x425 尾清 mover mode 后
            // 0x419450 每帧把插值角维（0）写回 vel.angle（实测 MBoss 归零）。
            // speed 维 from=当前值，to=参数 2。
            const float from_b = m.speed;
            const float to_b = kKeep < speed_to ? speed_to : m.speed;
            m.polar.setup(float(dur), mode, 0.0f, from_b, 0.0f, to_b);
        }
        m.radial.control1={};m.radial.control2={};
        m.radial.begin(dur,mode,{m.radius,m.growth},
            {kKeep<radius?radius:m.radius,kKeep<growth?growth:m.growth});
        m.mode = (m.mode & 0xFFFFFFF2u) | 2u;
        m.accumulate(); // 41c780 ->473a10 ->419950, without timer advance
        for(u32 n=0;n<3;++n) {
            const float delta=(e.vel.pos[n]+e.pos.pos[n])-e.position[n];
            e.motion_delta[n]=delta;
            e.position[n]+=delta;
        }
        e.position[0]=round001(e.position[0]);e.position[1]=round001(e.position[1]);
        return 0;
    }
    case 432:
    case 433: {  // moveEnm（0x41cd0c/0x41cd1f）：子位置 = 目标敌主位置
        const u32 integral[1] = {1};
        i32 id = 0;
        if (!resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral,{float(id)});
        StageEnemy* target = sim.find_by_script_id(id);
        StageMover& m = (op == 432) ? e.vel : e.pos;
        if (!target) {
            // 0x417f40 id 0/未命中链尾：id 0 返回 null 后 0x41ccb1/0x41cce3
            // 仍解引用（[0x1230..]——oracle bump 零页读 0），子位置清零。
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
    case 424: {  // moveSetMirror（0x41b30c）：bit20 = arg&1
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        e.flags = (e.flags & ~0x100000u) | ((u32(value) << 20) & 0x100000u);
        return 0;
    }
    case 425: {  // moveBezier（0x41bfb1 → 0x422f80 模式 8）
        const u32 integral[7] = {1, 0, 0, 0, 0, 0, 0};
        StageMover& m = e.vel;
        i32 dur = 0;
        float x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
        if (!resolve_float(c,3,x2) || !resolve_float(c,4,y2) ||
            !resolve_float(c,1,x1) || !resolve_float(c,2,y1) ||
            !resolve_float(c,5,x3) || !resolve_float(c,6,y3) || !resolve_int(c,0,dur)) {
            sim.error_ = c.error;
            return -2;
        }
        emit_move(integral,{float(dur),x1,y1,x2,y2,x3,y3});
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
    case 412: {  // moveRand（0x41cdc2）：按象限选随机角后走 moveVelTime 同款挂接
        const u32 integral[3] = {1, 1, 0};
        i32 dur = 0, mode = 0;
        float speed = 0;
        // 0x41cdc2 逐块反汇编（fcompp 弹栈重序后的语义）：
        //   左缘 x<cx−q：向右 1/3 扇 angle=f32(f32(uπ)/3)；
        //   右缘 x>cx+q：向左 wrap(f32(π+f32(uπ)/3))；
        //   中带含边界：uπ×1/4，x<px 时 r16%3==0 加 π，否则 r16%3!=0 加 π。
        //   Y 翻转：y<cy−q 取 +|a|（向下），y>cy+q 取 −|a|（向上），边界不变。
        //   近垂直：|a+π/2|<0.05 → a≥−π/2?−1.5207964:−1.6207963；
        //           |a−π/2|<0.05 → a>π/2?1.6207963:1.5207964（严格大于）。
        //   尾块（0x41d01c）：from=(angle, speed) → to=(angle, 0)——随机角直冲
        //   减速到停；mover 模式低 4 位清 0（0x41d0b9 and 0xF0，非 405 的 0xF2）。
        const float pi = 3.1415927410125732f;
        const float qx = e.limit_half[0] * 0.25f;
        const float qy = e.limit_half[1] * 0.25f;
        const float cx = e.limit_center[0], cy = e.limit_center[1];
        float angle;
        if (e.position[0] < cx - qx) {
            angle = float(float(sim.random.signed_unit() * pi) / 3.0);
        } else if (e.position[0] > cx + qx) {
            angle = wrap_angle(float(pi + float(sim.random.signed_unit() * pi) / 3.0));
        } else {
            const float quarter = float(float(sim.random.signed_unit() * pi) * 0.25);
            const bool third = (u32(sim.random.step16()) % 3u) == 0;
            const bool left_of_player = sim.player_x > e.position[0];
            angle = (left_of_player ? third : !third) ? float(quarter + pi) : quarter;
        }
        if (e.position[1] < cy - qy) {
            angle = angle < 0 ? -angle : angle;      // 顶带：+|a|（向下）
        } else if (e.position[1] > cy + qy) {
            angle = angle < 0 ? angle : -angle;      // 底带：−|a|（向上）
        }
        if (std::fabs(angle + 1.5707964f) < 0.05f) {
            angle = angle >= -1.5707964f ? -1.5207964f : -1.6207963f;
        } else if (std::fabs(angle - 1.5707964f) < 0.05f) {
            angle = angle > 1.5707964f ? 1.6207963f : 1.5207964f;
        }
        // 41cdc2 draws the direction before reading mode/speed/duration.
        if (!resolve_int(c,1,mode) || !resolve_float(c,2,speed) || !resolve_int(c,0,dur)) {
            sim.error_=c.error;return -2;
        }
        emit_move(integral,{float(dur),float(mode),speed});
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
    case 439: { // 41bdf4: independent X/Y position interpolation
        const bool separate = op != 436 && op != 437;
        const u32 integral[5] = {1,1,u32(separate),0,0};
        StageMover& m = (op & 1) ? e.pos : e.vel;
        float x = 0, y = 0;
        i32 duration = 0, mx = 0, my = 0;
        if (!resolve_float(c,separate?3:2,x) || !resolve_float(c,separate?4:3,y) || !resolve_int(c,0,duration)) {
            sim.error_ = c.error; return -2;
        }
        if (duration < 1) { m.postime.duration = 0; return 0; }
        const float trace_x=x,trace_y=y;
        if (op >= 436) {
            x = (e.flags & 0x100000u) ? m.pos[0] - x : m.pos[0] + x;
            y = m.pos[1] + y;
        }
        // Native resolves duration a second time after applying offsets.
        if (!resolve_int(c,0,duration) || !resolve_int(c,1,mx) || (separate && !resolve_int(c,2,my))) {
            sim.error_ = c.error; return -2;
        }
        if (separate) emit_move(integral,{float(duration),float(mx),float(my),trace_x,trace_y});
        else emit_move(integral,{float(duration),float(mx),trace_x,trace_y});
        auto& itp = m.postime;
        itp.duration = float(duration); itp.elapsed = 0; itp.per_axis = separate;
        if (!separate) itp.mode = mx;
        itp.axis_mode[0] = mx; itp.axis_mode[1] = my;
        for (int i=0;i<3;++i) { itp.start[i]=m.pos[i]; itp.p1[i]=0; itp.p2[i]=0; }
        itp.target[0] = kKeep < x ? x : m.pos[0];
        itp.target[1] = kKeep < y ? y : m.pos[1];
        itp.target[2] = 0;
        m.mode &= 0xfffffff0u;
        return 0;
    }
    case 427: {  // moveReset（0x41ba18）：清 mover 与累积量，主位置保持
        e.pos.pos[0] = e.pos.pos[1] = e.pos.pos[2] = 0;
        e.vel.pos[0] = e.position[0];
        e.vel.pos[1] = e.position[1];
        e.vel.pos[2] = e.position[2];
        for (StageMover* m : {&e.vel, &e.pos}) {
            m->speed = m->angle = m->radius = m->growth = 0;
            m->aux[0] = m->aux[1] = m->aux[2] = 0;
            m->mode = 0;
            m->polar.duration = 0;
            m->postime.duration = 0;
        }
        return 0;
    }

    // ---------- 500 族：属性/关卡事件 ----------
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
    case 501: {  // setHitbox：写入本体碰撞尺寸 +0x118/+0x11c（0x41d203）
        float w = 0, h = 0;
        if (resolve_float(c, 0, w) && resolve_float(c, 1, h)) {
            e.hitbox[0] = w;
            e.hitbox[1] = h;
            return 0;
        }
        sim.error_ = c.error;
        return -2;
    }
    case 502: {  // flagSet：flags |= arg（0x41d231）
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
    case 504: {  // moveLimit（0x41cd32）
        float cx = 0, cy = 0, w = 0, h = 0;
        if (resolve_float(c, 0, cx) && resolve_float(c, 1, cy) && resolve_float(c, 2, w) &&
            resolve_float(c, 3, h)) {
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
    case 506: {  // dropClear（0x41b260 case 0x2c：memset +0x3F6C, 0x3c）
        for (i32& slot2 : e.drop_extra) slot2 = 0;
        // 15 dwords end at +3fa7; main is +3f68, area starts +3fa8.
        return 0;
    }
    case 507: {  // dropExtra（case 0x2d：drop[id] = count，id 0 = 主道具槽）
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
    case 508: {  // dropArea（case 0x2e：散布幅度 x/y → 0xFEA/0xFEB）
        float x = 0, y = 0;
        if (!resolve_float(c, 0, x) || !resolve_float(c, 1, y)) {
            sim.error_ = c.error;
            return -2;
        }
        e.drop_area[0] = x;
        e.drop_area[1] = y;
        return 0;
    }
    case 509:  // dropItems（case 0x2f：(mode & 0x30) != 0x20 → 按表掉落）
        if ((sim.mode_flags & 0x30u) != 0x20u) sim.drop_table(e);
        return 0;
    case 510: {  // dropMain（case 0x30：主道具 kind → 0xFDA）
        i32 kind = 0;
        if (!resolve_int(c, 0, kind)) {
            sim.error_ = c.error;
            return -2;
        }
        e.drop_main = kind;
        return 0;
    }
    case 511: {  // lifeSet（0x41d34c：整型解析，×3 槽 + 0x3f58=7×life）
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
    case 512: {  // setBoss（0x41d395）：slot = arg0，负参撤销自身槽
        i32 slot = 0;
        if (!resolve_int(c, 0, slot)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.boss_leaving=false;
        if (slot >= 0 && slot < 3) {
            sim.boss_slot[slot] = e.script_id;
            e.flags |= 0x1000000u;  // 0x41d3d8：BOSS 位（525 清场豁免）
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
    case 552: {  // 0x41b34f（case 0x56，eclmap 名 zIndex）：敌机内嵌 VM+0x278
        // （=敌机结构+0x1464）= arg0——敌机族 ANM 创建层基。两消费点
        // 0x41fe80:0x41ff24（槽位 VM）与 0x419450:0x419764（镜像重挂 VM）
        // 均以 [+0x278]+7 建层。构造默认 1（0x419195，Unicorn 写 watch
        // 实证），本指令为覆盖（窗口 st01 未用——st05bs 父子敌机有用例）。
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        e.mirror_script_base = value;
        return 0;
    }
    case 558: {  // 0x41e451：bit20 = value&1（shl/xor/and/xor 序列，0 清位 1 置位）
        // 原版序列：eax=value<<20; eax^=flags(+0x4030); eax&=0x100000;
        // flags^=eax。展开后 bit20_new = bit20_old ^ ((v<<20)^flags)&bit
        // —— v 奇数置位、偶数清位（不是异或翻转：v=0 必须清位，MainSub02
        // 2925 帧的 op558(0) 依赖此语义，残留镜像位会使后续 root 相对
        // enmCreate 的 x 全部取反——stage2 f3265 GirlC01 对调、f3515 GirlA02
        // 镜像、f3532 得分首差的实证链）。
        i32 value = 0;
        if (!resolve_int(c, 0, value)) {
            sim.error_ = c.error;
            return -2;
        }
        e.flags ^= ((u32(value) << 20) ^ e.flags) & 0x100000u;
        return 0;
    }
    case 513:
        // Boss 第一非符锚点（§7.3）：MainBoss 创建的 "Boss" 敌机首条 513。
        if (e.serial == sim.boss_main_serial && sim.boss_nonspell_frame < 0)
            sim.boss_nonspell_frame = i32(sim.frame);
        if (sim.replay_events) sim.sink.boss_op(e.serial, 513, nullptr, 0, "");
        e.age = 0;  // 0041e570: reset the enemy phase timer, not invulnerability.
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
    case 539: {  // 0x41e4xx spell family: ids vary with the selected difficulty
        // pfVar11[7] = byte length; encrypted bytes begin at instruction+32.
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
        // 0041e473 resolves id, arg2, then duration; preserve RNG argument order.
        i32 id = 0, duration = 0, bonus = 0;
        if (!resolve_int(c, 0, id) || !resolve_int(c, 2, bonus) ||
            !resolve_int(c, 1, duration)) {
            sim.error_ = c.error;
            return -2;
        }
        // 522/528 retain their id; 537/538/539 add rank, rank-1, rank-2.
        e.spell_id = id + (op >= 537 ? sim.diff - (op - 537) : 0);
        e.spell_active = true;
        e.spell_life = i32(e.life) * 7;
        e.age = 0;  // shared caseD_33 tail: 004016c0(0) on the phase timer
        if (!sim.emit_spell(sim.spell.begin(e.spell_id, e.spell_name, duration,
                            sim.session ? sim.session->stage : 1, sim.diff,
                            sim.bomb_active), e)) {
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
    case 323: { // 41b40c: death effect ANIM file then script
        if(!resolve_int(c,0,e.death_effect_file)||!resolve_int(c,1,e.death_effect)) {
            sim.error_=c.error;return -2;
        }
        return 0;
    }
    case 516: { // 41e382: synchronous sound effect
        i32 id=0;if(!resolve_int(c,0,id)){sim.error_=c.error;return -2;}
        if(id>=0&&sim.enemy_sound&&!sim.enemy_sound(id)){sim.error_="ECL sound failed";return -2;}
        return 0;
    }
    case 547: { // 41f242: resolve float then fstp to global4c0a28, no clamp.
        float rate = 0;
        if (!resolve_float(c,0,rate)) { sim.error_ = c.error; return -2; }
        sim.set_time_rate(rate);
        return 0;
    }
    case 517: { // 41e3a3 ->45fa60(kind1, duration, start, end, 0), Update75
        i32 duration = 0, start = 0, end = 0;
        if (!resolve_int(c,2,end) || !resolve_int(c,1,start) || !resolve_int(c,0,duration)) {
            sim.error_ = c.error; return -2;
        }
        if (sim.screen_shake && !sim.screen_shake(duration,start,end)) {
            sim.error_ = "Screen shake registration failed"; return -2;
        }
        return 0;
    }
    case 557: { // 0041f40b ->004099a0/00405f00; far is evaluated before near.
        i32 duration = 0, mode = 0, color = 0;
        float near_distance = 0, far_distance = 0;
        if (!resolve_int(c,0,duration) || !resolve_int(c,1,mode) ||
            !resolve_int(c,2,color) || !resolve_float(c,4,far_distance) ||
            !resolve_float(c,3,near_distance)) {
            sim.error_ = c.error; return -2;
        }
        if (sim.stage_fog)
            sim.stage_fog(duration,mode,u32(color),near_distance,far_distance);
        return 0;
    }
    case 559:
    case 638:
        return 0;  // 计时/音效/符卡/掉落等：无事件
    case 524: { // 424310: write be824; reset be82c only when chapter changes.
        i32 chapter = 0;
        if (!resolve_int(c, 0, chapter)) { sim.error_ = c.error; return -2; }
        if (sim.session) {
            if (sim.session->chapter != chapter) sim.session->chapter_frame = 0;
            sim.session->chapter = chapter;
        }
        return 0;
    }
    case 527: { // 41d0c2: read color, HP, index; divide by initial life (+3f50).
        i32 color=0,index=0;float life=0;
        if(!resolve_int(c,2,color)||!resolve_float(c,1,life)||!resolve_int(c,0,index)){
            sim.error_=c.error;return -2;
        }
        i32 boss_index=0;
        for(i32 i=0;i<3;++i)if(sim.boss_slot[i]==e.script_id){boss_index=i;break;}
        if(index<0||index>=4){sim.error_="Boss marker index";return -2;}
        auto& marker=sim.boss_markers[size_t(boss_index)][size_t(index)];
        marker.color=color;marker.ratio=life/float(e.spell_base_life);
        return 0;
    }
    case 540: { // 41f1ba: stage context+15c, independent of circular HP markers.
        if(!resolve_int(c,0,sim.boss_remaining_stars)){sim.error_=c.error;return -2;}
        return 0;
    }
    case 526: { // 41cdab: square in x87 and store enemy+4044.
        float radius=0;if(!resolve_float(c,0,radius)){sim.error_=c.error;return -2;}
        e.bullet_protect_radius2=float(double(radius)*double(radius));
        return 0;
    }
    case 560: { // 41f6ea: global bullet reflection width/height, not an effect.
        float width = 0, height = 0;
        if (!resolve_float(c,0,width) || !resolve_float(c,1,height)) {
            sim.error_ = c.error; return -2;
        }
        sim.bullet_bounce_size[0] = width;
        sim.bullet_bounce_size[1] = height;
        return 0;
    }
    case 545: // 0041f1d4 -> 430220: clear protection then cancel all lasers.
        sim.cancel_lasers();
        return sim.error_.empty() ? 0 : -2;
    case 556: { // 41e360 stores the inline string, consumed by41aad0
        const u32 length = u32(ins->argument<i32>(0));
        if (ins->length < 20 || length > u32(ins->length - 20)) {
            sim.error_ = "setDeath string"; return -2;
        }
        const char* name = reinterpret_cast<const char*>(ins) + 20;
        e.death_routine.assign(name, strnlen(name, length));
        return 0;
    }
    case 542:  // 0041e593: survival card, no decay/timeout failure
        sim.spell.set_survival();
        return 0;
    case 543:  // 0041e5a1 -> 00418160: background visibility flag
        sim.spell.hide_background();
        return 0;
    case 514: {  // setInterrupt（0x41e262 → 0x421880）：{id,life,time,label}
        const bool spell_route = (sim.mode_flags & 0x30u) == 0x20u &&
                                 (e.flags & 0x1000000u);
        i32 id = 0, t1 = 0, t2 = 0;
        // Original reads timeout, optional HP, then slot. Spell Practice does
        // not evaluate the ignored HP argument (which can consume RNG).
        if (!resolve_int(c, 2, t2) || (!spell_route && !resolve_int(c, 1, t1)) ||
            !resolve_int(c, 0, id)) {
            sim.error_ = c.error;
            return -2;
        }
        // 名字参数（arg3 起的 {len, name} 尾串——指令名区原样）
        std::string name;
        const EclInstruction* i = c.instruction;
        if (spell_route) {
            name = "BossDead";
        } else if (i->parameter_count > 3) {
            const u32 length = u32(i->argument<i32>(3));
            const char* bytes = reinterpret_cast<const char*>(i) + 16 + 4 * 4;
            name.assign(bytes, strnlen(bytes, length ? length - 1 : 0));
        }
        // 0x41e31c writes the HP threshold; 0x421880 installs the timeout
        // and BOTH labels only for nonnegative thresholds.
        if (id >= 0 && id < 8) {
            auto& slot = e.interrupts[u32(id)];
            // 0041e262: Spell Practice boss interrupts leave the selected card
            // via BossDead/BossEscape instead of progressing to the next phase.
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
        if (id == 0) e.interrupt_label = name;  // 525 中断跳转用（id 0）
        if (spell_route) {
            // 0041e2cd evaluates slot arg0 again for the timeout override.
            // A referenced slot can consume RNG or address another slot.
            i32 timeout_slot = 0;
            if (!resolve_int(c, 0, timeout_slot)) {
                sim.error_ = c.error;
                return -2;
            }
            if (timeout_slot >= 0 && timeout_slot < 8)
                e.interrupts[u32(timeout_slot)].timeout_label = "BossEscape";
        }
        if (sim.replay_events) {
            const i32 args[3] = {id, t1, t2};
            sim.sink.boss_op(e.serial, 514, args, 3, name.c_str());
        }
        return 0;
    }
    case 521: {  // timeout override（0x41e33f：写 vm+0x5240+id*16）
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
    case 518: {  // dialogRead（0x41e3da → 0x428820(param)）：清场尾块与 525
        // 共享（0x41ac70 门控销毁，oracle RNG 对拍实证）；param>=0 另建对话
        // 对象（0x428820 else 分支，挂 stage ctx+0x198）——交互模式经
        // dialog_host 注入 MsgPlayer，无头契约路径不注入（行为不变）。
        i32 entry = 0;
        if (!resolve_int(c, 0, entry)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.dialog_host) sim.dialog_host->open_dialog(entry);
        // 41e3ef/41e3f8: dialogue and music-only entries both cancel
        // bullets and lasers before the shared enemy-retirement tail.
        sim.clear_bullets();
        sim.cancel_lasers(false);
        if (!sim.error_.empty()) return -2;
    }
        // fallthrough：共享 525 的清场尾块
        [[fallthrough]];
    case 525: {  // enmKillAll（0x41e3fd → 0x41ac70）
        // 0x41ac70：flags 门控销毁——(bit5/7 全清 且 bit 0x400/0x800000/0x1000000
        // 全清) 或 bit8 置位。命中者当场走 0x41aad0（0x417b10 开头抽 1 次
        // signed_unit；合成 ANM 态槽弹数为 0，无每弹消耗）并标 0x4000000，
        // 摘除由 0x418e00 遍历完成（无 RNG）。main/MapleEnemy/EtBreak 的
        // ins_502(32)（bit5）豁免清场；BOSS 位（0x1000000，512 setBoss）豁免。
        return sim.clear_enemies() ? 0 : -2;
    }
    case 519:
        // 41e407: no dialogue OR 418090(dialogue+174) != 0 passes. MSG12
        // sets that pulse; the next stage update sees it before MSG decrements.
        return sim.dialog_host ? (sim.dialog_host->dialog_wait_released() ? 0 : 1)
                               : sim.boss_dialog_wait();
    case 520:  // bossWait（0x41e42e）：任一 BOSS 槽存活则让出
        return sim.boss() ? 1 : 0;
    case 535: {  // diffI（0x41d439）：dest = args[1 + DIFF]
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
    case 548: {  // diffWait（0x41f2c4）：time -= args[DIFF]（0..3 四难度，无 dest）
        i32 frames = 0;
        if (!resolve_int(c, sim.diff, frames)) {
            sim.error_ = c.error;
            return -2;
        }
        c.time -= float(frames);
        return 0;
    }
    case 555: {  // enmAlive（0x41f4f3）
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
        // 41f501 calls417f10 (exact existence), not417f40 (lookup whose
        // missing-id result is the last visited node). Phantom01_at0 uses
        // this to leave when its spawning enemy disappears.
        *dest = 0;
        for (const auto& enemy : sim.enemies)
            if (id && enemy->script_id == id) { *dest=1;break; }
        return 0;
    }
    case 562: { // jump-table dense60 ->41d325: forced drops, unlike509's mode gate
        sim.drop_table(e);
        return 0;
    }
    case 561: { // jump-table dense5f ->41b437: death audiovisual effects only
        if(e.death_sound>=0 && sim.enemy_sound && !sim.enemy_sound(e.death_sound)) {
            sim.error_="Enemy death sound failed";return -2;
        }
        if(e.death_effect>=0 && sim.enemy_death_effect) {
            const double dx=double(e.position[0])-e.previous_position[0];
            const double dy=double(e.position[1])-e.previous_position[1];
            const float distance=float(dx*dx+dy*dy);
            const float angle=distance>=0.040000003f?float(std::atan2(dy,dx)):-1.5707964f;
            if(!sim.enemy_death_effect(e.death_effect_file,e.death_effect,
                    {e.position[0],e.position[1],e.position[2]},angle)) {
                sim.error_="Enemy death animation failed";return -2;
            }
        }
        return 0;
    }

    // ---------- 600 族：弹幕发射器 ----------
    case 600: {  // etNew（0x41d573）
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = "etNew slot";
            return -2;
        }
        e.et[slot].reset();
        return 0;
    }
    case 601:  // etOn（0x41d6b1 → 0x40ffe0 → 0x40e840）
        return fire_et(c, e);
    case 602: {
        i32 slot = 0, s1 = 0, s2 = 0;
        if (resolve_int(c, 0, slot) && resolve_int(c, 1, s1) && resolve_int(c, 2, s2) && slot >= 0 &&
            slot < 8) {
            e.et[slot].sprite = u32(s1);
            e.et[slot].subtype = u32(s2);
            return 0;
        }
        sim.error_ = "etSprite args";
        return -2;
    }
    case 603: {
        i32 slot = 0;
        float x = 0, y = 0;
        if (resolve_int(c, 0, slot) && resolve_float(c, 1, x) && resolve_float(c, 2, y) && slot >= 0 &&
            slot < 8) {
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
        if (resolve_int(c, 0, slot) && resolve_float(c, 1, a) && resolve_float(c, 2, b) && slot >= 0 &&
            slot < 8) {
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
        if (resolve_int(c, 0, slot) && resolve_float(c, 1, a) && resolve_float(c, 2, b) && slot >= 0 &&
            slot < 8) {
            e.et[slot].speed1 = a;
            e.et[slot].speed2 = b;
            return 0;
        }
        sim.error_ = "etSpeed args";
        return -2;
    }
    case 606: {
        i32 slot = 0, count = 0, ways = 0;
        if (resolve_int(c, 0, slot) && resolve_int(c, 1, count) && resolve_int(c, 2, ways) &&
            slot >= 0 && slot < 8) {
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
    case 608: { // 0041b260 case0x69: preserve sound IDs for laser descriptors.
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8 ||
            !resolve_int(c, 1, e.et[slot].sound) ||
            !resolve_int(c, 2, e.et[slot].transform_sound)) {
            sim.error_ = c.error.empty() ? "etSound args" : c.error;
            return -2;
        }
        return 0;
    }
    case 609:   // etExSet（0x41df90）：写入 et 槽扩展行为队列
    case 610:   // etExSet2（0x262 变体）：多两个整型参数位
    case 611:   // etEx（0x263）：base 位改读敌机侧待写块计数
    case 612: {  // etEx2（0x264）：同 611 + 两整型位
        // 0x41df90: arg0 is ALWAYS the emitter. 609/610 explicitly select
        // one of 18 queue entries; 611/612 use its append cursor. Treating
        // arg1 as an emitter made BossCard3's explicit entries 8/9 fail.
        // Layout: four floats, four ints, op, flags, routine pointer (44 bytes).
        i32 slot = 0, index = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = "etEx args";
            return -2;
        }
        const bool append = op == 611 || op == 612;
        const bool extended = op == 610 || op == 612;
        u32 argument = 1;
        if (append) index = i32(e.et[slot].ex_next);
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
        // 0x41b260 case0x6b -> 0x411df0 + 0x430300: clear live
        // bullets/lasers, NOT the enemy's emitter configurations.
        sim.clear_bullets();
        sim.cancel_lasers(false); // 430300 passes (1,0); protection is not reset.
        return sim.error_.empty() ? 0 : -2;
    case 614: {  // etCopy（0x41d61a）
        i32 to = 0, from = 0;
        if (resolve_int(c, 0, to) && resolve_int(c, 1, from) && to >= 0 && to < 8 && from >= 0 &&
            from < 8) {
            // 0041d631..0041d6a9 copies the 0x380-byte emitter and its
            // two position vectors. The append cursor at enemy+0x3d70
            // is outside those blocks and remains local to the destination.
            const u32 append_cursor = e.et[to].ex_next;
            e.et[to] = e.et[from];
            e.et[to].ex_next = append_cursor;
            return 0;
        }
        sim.error_ = "etCopy args";
        return -2;
    }
    case 615:  // etCancel（0x4114f0(pos, radius, 1)：场内弹转换取消道具）
    case 616: {  // etClear（0x4114f0(pos, radius, 0) + 0x4302a0 转换点登记）
        float radius = 0;
        if (!resolve_float(c, 0, radius)) {
            sim.error_ = c.error;
            return -2;
        }
        sim.cancel_bullets_radius(e, radius, op == 615);
        sim.cancel_lasers_radius({e.position[0], e.position[1], e.position[2]}, radius);
        return sim.error_.empty() ? 0 : -2;
    }
    case 626: { // 41d837 ->4246f0: polar angle/radius to emitter offset XY
        i32 slot=0;float angle=0,radius=0;
        if(!resolve_int(c,0,slot)||!resolve_float(c,1,angle)||!resolve_float(c,2,radius)||slot<0||slot>=8){
            sim.error_=c.error.empty()?"etOffsetRad args":c.error;return -2;
        }
        polar_xy(angle,radius,e.et[slot].offset[0],e.et[slot].offset[1]);return 0;
    }
    case 627: {  // etDist: emitter+0x24, consumed by fire_et's radial spawn
        i32 slot = 0;
        float distance = 0;
        if (!resolve_int(c, 0, slot) || !resolve_float(c, 1, distance) ||
            slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "etDist args" : c.error;
            return -2;
        }
        e.et[slot].spin = distance;
        return 0;
    }
    case 628: { //41d8e2: absolute XY and enable flag determined by X>=-990.
        i32 slot=0;float x=0,y=0;
        if(!resolve_int(c,0,slot)||slot<0||slot>=8||
           !resolve_float(c,1,x)||!resolve_float(c,2,y)){
            sim.error_=c.error.empty()?"absolute emitter offset arguments":c.error;return -2;
        }
        e.et[slot].absolute[0]=x;e.et[slot].absolute[1]=y;
        e.et[slot].absolute[2]=x>=-990.f?1.f:0.f;
        return 0;
    }
    case 624: {  // etSpeedD（0x41da32）：按 DIFF 选参
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
    case 625: {  // etCountD（0x41dad7）
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
    case 640: { // 41e135: store the inline routine name at ex-entry +0x28
        i32 slot = 0, index = 0;
        if (!resolve_int(c, 0, slot) || !resolve_int(c, 1, index) ||
            slot < 0 || slot >= 8 || index < 0 || index >= 18) {
            sim.error_ = c.error.empty() ? "etExSub slot" : c.error;
            return -2;
        }
        if (ins->length < 32) { sim.error_ = "Truncated etExSub name"; return -2; }
        const u32 length = ins->argument<u32>(2);
        const char* name = reinterpret_cast<const char*>(ins) + 28;
        if (!length || length > u32(ins->length - 28) || !std::memchr(name, 0, length)) {
            sim.error_ = "Invalid etExSub name";
            return -2;
        }
        // No reset of the queue entry or its append cursor. 609..612 may
        // precede or follow this instruction; they leave this pointer intact.
        e.et[slot].ex[index].subroutine = name;
        return 0;
    }
    case 630: { // 41f3f6: 0x421b10(arg0) → call 0x409100——STD 相机脚本
        // ins_16 标号中断：扫描 [stage+0x1c] 脚本找 opcode==16 且 arg0==id 的
        // 标号，命中则游标跳到标号后首指令、计时三槽设为目标 time（生产
        // StageScript::interrupt 同构）。st04 main/MainSub09/MainBossDebug(2)
        // 均 630(1)，对应 st04.std ins_16(1)@t=128（kind1 摇晃 → kind3）。
        // 注意：这不是 ecl-vm.md 旧记的 "callSTD"（default.ecl 函数调用）。
        i32 id = 0;
        if (!resolve_int(c, 0, id)) { sim.error_ = c.error; return -2; }
        if (!sim.stage_script_interrupt) {
            // 原版 stage ctx 恒有 STD 脚本；生产仅交互模式（RunScene 接
            // StageScenery::script）持有——无头路径显式报错，不静默吞中断。
            sim.error_ = "STD camera script unavailable for ECL630 interrupt " +
                         std::to_string(id);
            return -2;
        }
        if (!sim.stage_script_interrupt(id)) {
            sim.error_ = "STD camera script interrupt failed for id " + std::to_string(id);
            return -2;
        }
        return 0;
    }
    case 631: { // 41b32f: replace enemy manager+7c bit0, used by HUD gates.
        i32 value=0;if(!resolve_int(c,0,value)){sim.error_=c.error;return -2;}
        sim.boss_leaving=(value&1)!=0;return 0;
    }
    case 629: { // 41f344: release old mesh, reset radius16/angles, optionally create17x17.
        float radius=0;i32 color=0;
        e.fog.reset();
        if(!resolve_float(c,0,radius)||!resolve_int(c,1,color)){sim.error_=c.error;return -2;}
        if(radius>0) {
            e.fog=std::make_unique<EnemyFog>();e.fog->target=radius;e.fog->color=u32(color);
        }
        if(sim.enemy_fog&&!sim.enemy_fog(e,true)){sim.error_="Enemy fog creation failed";return -2;}
        return 0;
    }
    case 632: case 639: {
        i32 index=0;
        if(!resolve_int(c,0,index)){sim.error_=c.error;return -2;}
        if(index<0||index>6){sim.error_="Enemy function table index";return -2;}
        e.native_function=index;e.native_function_mode=op==639?1:0;
        return 0;
    }
    case 637: {
        i32 index=0;if(!resolve_int(c,0,index)){sim.error_=c.error;return -2;}
        return sim.invoke_enemy_function(e,index)?0:-2;
    }
    case 633: { // 41d183 -> table4bba18, enemy inner+40f0.
        i32 index = 0;
        if (!resolve_int(c, 0, index)) { sim.error_ = c.error; return -2; }
        if (index < 0 || index > 1) { sim.error_ = "Enemy damage function index"; return -2; }
        e.damage_function = index;
        return 0;
    }

    // ---------- 700/800/900/1000 族 ----------
    case 700: { // 0041b260 case0x87 @0041e16c: writes the existing et slot.
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "laserNew slot" : c.error;
            return -2;
        }
        // No allocation/reset. The first three fields alias etOn's xyz,
        // while angle, speed, sprite, timing and the ex queue stay intact.
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
    case 701: { // 0041b260 case0x88 @0041e1ec: five integer stores.
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "laserTiming slot" : c.error;
            return -2;
        }
        // 703 copies these into +0x5f0..5fc (four phase durations) and
        // +0x618 (flags | 2). In particular arg5 is not a duration.
        for (u32 n = 0; n < 5; ++n) {
            if (!resolve_int(c, n + 1, e.et[slot].laser_timing[n])) {
                sim.error_ = c.error;
                return -2;
            }
        }
        return 0;
    }
    case 703: { // 0041e77a: build descriptor; 42fed0 type1 -> 433260.
        i32 slot = 0, id = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8 || !resolve_int(c, 1, id)) {
            sim.error_ = c.error.empty() ? "laserStOn args" : c.error;
            return -2;
        }
        sim.emit_infinite_laser(e.et[slot], e, id);
        return sim.error_.empty() ? 0 : -2;
    }
    case 708: { // 0041edd7 -> 418300: first global ID match, zero never matches.
        i32 id = 0;
        if (!resolve_int(c, 0, id)) { sim.error_ = c.error; return -2; }
        if (StageLaser* laser = sim.find_laser(id)) {
            // Missing ID must not resolve arg1 (RNG/stack references have effects).
            if (!resolve_float(c, 1, laser->angle)) { sim.error_ = c.error; return -2; }
        }
        return 0;
    }
    case 704:
    case 705: { // 41ecd1/41ed2b: both float arguments are read even on ID miss.
        i32 id=0;float x=0,y=0;
        if(!resolve_int(c,0,id)){sim.error_=c.error;return -2;}
        StageLaser* laser=sim.find_laser(id);
        if(!resolve_float(c,1,x)||!resolve_float(c,2,y)){sim.error_=c.error;return -2;}
        if(laser) {
            if(op==704){laser->origin[0]=x;laser->origin[1]=y;laser->origin[2]=0;}
            else if(laser->kind==StageLaser::Kind::Infinite) {
                laser->velocity[0]=x;laser->velocity[1]=y;laser->velocity[2]=0;
            } else {
                // 4182b0 blindly writes +5cc/+5d0/+5d4. Those fields are
                // angle/target length/initial length in a line descriptor.
                laser->line_descriptor_angle=x;laser->target_length=y;
                laser->line_initial_length=0;
            }
        }
        return 0;
    }
    case 706:
    case 707:
    case 709:
    case 714: {
        i32 id=0;
        if(!resolve_int(c,0,id)){sim.error_=c.error;return -2;}
        if(StageLaser* laser=sim.find_laser(id)) {
            if(op==714) {
                // Virtual+8 = 42f5d0 for both line and infinite: no-op,
                // but its integer argument is still evaluated on an ID hit.
                i32 value=0;if(!resolve_int(c,1,value)){sim.error_=c.error;return -2;}
                if(laser->kind==StageLaser::Kind::Beam)
                    laser->beam_descriptor_flags=(laser->beam_descriptor_flags&~1u)|(u32(value)&1u);
            } else {
                float value=0;if(!resolve_float(c,1,value)){sim.error_=c.error;return -2;}
                if(op==706)laser->length_speed=value; // +74; velocity unchanged
                else if(op==707)laser->width=value;   // +70; phase target unchanged
                else if(laser->kind==StageLaser::Kind::Infinite)laser->rotation=value;
                else laser->target_width=value;      // +5dc: descriptor width on lines
            }
        }
        return 0;
    }
    case 710: { // 41ec89: cancel every matching node, zero its ID, then re-read arg0.
        for(;;) {
            i32 id=0;if(!resolve_int(c,0,id)){sim.error_=c.error;return -2;}
            StageLaser* laser=sim.find_laser(id);
            if(!laser)return 0;
            if(!sim.cancel_laser(*laser))return -2;
            laser->id=0;
        }
    }
    case 702: { // 41e5cd -> 42fed0 type0 -> 430370.
        i32 slot=0;
        if(!resolve_int(c,0,slot)||slot<0||slot>=8){
            sim.error_=c.error.empty()?"line laser emitter slot":c.error;return -2;
        }
        sim.emit_line_laser(e.et[slot],e);
        return sim.error_.empty()?0:-2;
    }
    case 713: { // 41eae9: 42fed0 type3, with a separately resolved ID.
        i32 slot=0, id=0;
        if(!resolve_int(c,0,slot)||slot<0||slot>=8||!resolve_int(c,1,id)) {
            sim.error_=c.error.empty()?"beam laser arguments":c.error;return -2;
        }
        sim.emit_beam_laser(e.et[slot],e,id);
        return sim.error_.empty()?0:-2;
    }
    case 711: { // 41eaf3 -> 42fed0 type2: laserCuOn / LaserCurveInf.
        // The retail handler snapshots the emitter exactly like the other
        // laser constructors, then creates a type-2 curve object.  Leaving
        // this as a no-op silently removes every curved laser used by the
        // Stage 3/4/6 boss cards.
        i32 slot = 0;
        if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
            sim.error_ = c.error.empty() ? "curve laser slot" : c.error;
            return -2;
        }
        sim.emit_curve_laser(e.et[slot], e);
        return sim.error_.empty() ? 0 : -2;
    }
    case 801: { // 41b260 case97: 417f40(id), outer+1230/+1234 -> float destinations.
        i32 id=0;
        if(!resolve_int(c,2,id)){sim.error_=c.error;return -2;}
        StageEnemy* source=sim.find_by_script_id(id);
        if(!source){sim.error_="enmPosRead missing enemy "+std::to_string(id);return -2;}
        float* x=c.float_destination(0);
        if(!x){sim.error_=c.error;return -2;}
        *x=source->position[0];
        float* y=c.float_destination(1);
        if(!y){sim.error_=c.error;return -2;}
        *y=source->position[1];
        return 0;
    }
    case 800: { // 41f5e9: 敌机 ECL 重挂（st04 Boss 相位推进）。
        // 原版链：0x421b10(arg0=敌机 id，数据恒 gvar -9926) → 0x417f40 查敌
        // （id 0/空表 → null 走默认尾；未命中返回最后检视者——生产
        // find_by_script_id 同构保留）→ 0x4176c0 释放异步子线程链 →
        // 0x417740 复位 ECL 上下文（flags bit0/routine/SP/FP/线程号/状态/
        // 8 插值通道）→ 0x479a90 用指令 +0x18 内联例程名经 0x4797d0 二分
        // 查到例程首指令指针写 [ctx+4]、time=0。生产同构入口 =
        // EclThreads::restart（仅调用，不改）。
        i32 id = 0;
        if (!resolve_int(c, 0, id)) { sim.error_ = c.error; return -2; }
        StageEnemy* target = sim.find_by_script_id(id);
        if (!target) return 0;  // 默认尾 = 纯 ret（id 0 / 敌链空）
        if (!target->threads) { sim.error_ = "ECL800 target threads unavailable"; return -2; }
        // 内联例程名：arg1 = 含 NUL 的字节长度（st04bs 11 处实测），名区在
        // 指令 +0x18 起、4 对齐衬垫（0x41f5e9: lea ecx,[ebx+0x18]）。
        if (ins->length < 28) { sim.error_ = "Truncated ECL800 routine name"; return -2; }
        const u32 name_length = ins->argument<u32>(1);
        const char* name = reinterpret_cast<const char*>(ins) + 0x18;
        if (!name_length || name_length > u32(ins->length - 0x18) ||
            !std::memchr(name, 0, name_length)) {
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
            // 自重挂（st04 的 800 全部在 Boss 自身 main 线程内触发）：原版
            // 回到 0x4772a0 尾部后重读 [ctx+4]（已是新例程首指令）——扣其
            // stack_adjust（0x478bbf）并把游标推进到新首指令之后
            // （0x477377），即新例程首指令（各例程的 enter(40)）被跳过。
            // 此处把解释器尾部将读的 instruction 换到新首指令，并预置
            // instruction_offset = 首指令长 − 本指令长，使尾部的
            // offset += 本指令长 恰停在新首指令之后。
            if (!c.code || c.routine < 0) {
                // 0x4797d0 查名失败：[ctx+4]=0 → 0x4772b1 直接按线程结束
                // 返回 -1（restart 已置 routine=-1/code=null）。
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
        return 0;  // 原版落默认分支 no-op（ecl-vm.md §4.2 900）
    case 1000: {
        i32 kind = 0;
        if (!resolve_int(c, 0, kind)) {
            sim.error_ = c.error;
            return -2;
        }
        if (sim.items)
            sim.items->spawn_value_item(kind, e.position[0], e.position[1],
                                       e.position[2], 2.0f);
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
        if (op == 1001) e.death_spirit_limit = value;
        else if (op == 1002) e.death_spirit_count = value;
        else e.damage_spirits_disabled = (value & 1) != 0;
        return 0;
    }

    default:
        sim.error_ = "unimplemented game opcode " + std::to_string(op);
        return -2;
    }
}

// enmCreate 族（0x41fcc0 → 0x4183f0）。
int StageCommands::create_enemy(EclContext& c, u16 op) {
    StageSim& sim = *this->sim;
    StageEnemy& e = *sim.current;
    if (op >= 309 && op <= 312 && sim.boss()) return 0;  // BOSS 槽空时才建（0x41b2dd）
    const EclInstruction* ins = c.instruction;
    const u32 length = u32(ins->argument<i32>(0));  // 含 NUL 与 4 对齐
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
    // 0x41fcc0:60-62：311/304/312/305（0x137/0x130/0x138/0x131）产 param2[6]=1
    // ——镜像位源；0x41fcc0:63-66 创建者已镜像时翻转该位并 x 取负；
    // 0x4183f0:28 以 xor-and 把 param2[6]<<20 无条件写入新敌机 flags bit20。
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
    if (!resolve_postname_int(c, 3, 4, initial.score) ||
        !resolve_postname_int(c, 4, 5, initial.drop_main)) {
        sim.error_ = c.error;
        return -2;
    }
    // 41fcc0 copies the parent's constructor parameters before 4183f0's
    // first ECL execution. In-script dropClear preserves initial main drop.
    std::copy_n(e.i_slots, 4, initial.integers.begin());
    std::copy_n(e.f_slots, initial.floats.size(), initial.floats.begin());
    sim.spawn_enemy(routine.c_str(), x, y, float(life), &e, false, mirror, &initial);
    return 0;
}

// etOn（0x41d6b1 + 0x40ffe0 + 0x40e840）：按槽状态合成出生参数后交给
// StageSim::emit_et（ways/count 循环、单弹出生、队列消费与 RNG 次序）。
int StageCommands::fire_et(EclContext& c, StageEnemy& e) {
    StageSim& sim = *this->sim;
    i32 slot = 0;
    if (!resolve_int(c, 0, slot) || slot < 0 || slot >= 8) {
        sim.error_ = "etOn slot";
        return -2;
    }
    StageEt& et = e.et[slot];
    // 出生点：absolute[2] <= 0.9 → 相对（偏移 + 敌机主位置）；否则绝对 + 偏移。
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
    // 0041b260 case0x62 stores into the same three fields read by 702/703.
    // The absolute branch stores z=0, independently of its scratch vector.
    et.spawn[0] = spawn[0];
    et.spawn[1] = spawn[1];
    et.spawn[2] = et.absolute[2] <= 0.9f ? spawn[2] : 0.0f;
    // 瞄准角（0x443f60：出生点 → 玩家，重合取 π/2）；0x40ffe0 在循环外算一次。
    const float dy = sim.player_y - spawn[1];
    const float dx = sim.player_x - spawn[0];
    const float aim = (dy == 0.0f && dx == 0.0f)
                          ? 1.5707964f
                          : float(std::atan2(static_cast<double>(dy), static_cast<double>(dx)));
    sim.active_bullet_protect_radius2=e.bullet_protect_radius2;
    sim.emit_et(et, e.serial, u32(slot), spawn, aim);
    sim.active_bullet_protect_radius2=0; // native601 always clears the manager field
    return sim.error_.empty() ? 0 : -2;
}

}  // namespace th13
