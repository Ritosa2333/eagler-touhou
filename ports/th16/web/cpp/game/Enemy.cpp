// th16 Enemy / EnemyManager 还原——见 Enemy.hpp 锚点。
#include "Enemy.hpp"

#include <algorithm>

#include "AnmGeometry.hpp"
#include "AnmManager.hpp"
#include "Effect.hpp"
#include "Gui.hpp"
#include "ScorePopup.hpp"
#include "Stage.hpp"
#include "Item.hpp"
#include "Laser.hpp"
#include "Spellcard.hpp"
#include "GameGlobals.hpp"
#include "Player.hpp"
#include <cmath>
#include <cstring>

namespace th16 {
namespace {
constexpr float kSentinel = -999999.0f;
constexpr u32 kFlagMirror = 0x80000;
constexpr u32 kFlagBossLife = 0x40000000;
constexpr u32 kFlagReentry = 0x40000;
constexpr u32 kFlagAlive = 0x10000;
constexpr float kPi = 3.1415927410125732f;
constexpr float kHalfPi = 1.5707963705062866f;  // 0x494534
constexpr float kTwoPi = 6.2831854820251465f;    // 0x4945b8

// 0x4258b0：敌机位置插值推进。结束帧返回 goal（模式 7 / 0x11 返回 initial）；flags bit0 时按 axis_modes 逐轴计算。
// 模式 0x11 的累加量放在 bezier2（+0x30），与 AnmVm 的 Interp 不同
Vec3 step_pos_interp(EnemyPosInterp& it, float game_speed) {
    const i32 end = it.end_time;
    bool running = false;
    if (end > 0) {
        it.timer.tick(game_speed);
        if (it.timer.current < end) {
            running = true;
        } else {
            it.timer.set(end);
            it.end_time = 0;
        }
    } else if (end != 0) {
        running = true;
    }
    if (!running) return (it.mode != 7 && it.mode != 0x11) ? it.goal : it.initial;
    float* cur = &it.current.x;
    float* ini = &it.initial.x;
    float* goal = &it.goal.x;
    float* b1 = &it.bezier1.x;
    float* b2 = &it.bezier2.x;
    if ((it.flags & 1u) == 0) {
        if (it.mode == 7) {
            for (int i = 0; i < 3; ++i) { ini[i] = goal[i] + ini[i]; cur[i] = ini[i]; }
        } else if (it.mode == 0x11) {
            for (int i = 0; i < 3; ++i) { ini[i] = b2[i] + ini[i]; cur[i] = ini[i]; }
            for (int i = 0; i < 3; ++i) b2[i] = b2[i] + goal[i];
        } else if (it.mode == 8) {
            const float r = it.timer.current_f / float(end);
            for (int i = 0; i < 3; ++i) cur[i] = hermite(ini[i], goal[i], b1[i], b2[i], r);
        } else {
            const float f = interp_ease(it.mode, it.timer.current_f, float(end));
            for (int i = 0; i < 3; ++i) cur[i] = (goal[i] - ini[i]) * f + ini[i];
        }
        return it.current;
    }
    for (int i = 0; i < 3; ++i) {
        const i32 mode = it.axis_modes[i];
        if (mode == 7) {
            ini[i] = goal[i] + ini[i];
            cur[i] = ini[i];
        } else if (mode == 0x11) {
            ini[i] = ini[i] + b2[i];
            cur[i] = ini[i];
            b2[i] = goal[i] + b2[i];
        } else if (mode == 8) {
            const float r = it.timer.current_f / float(end);
            cur[i] = hermite(ini[i], goal[i], b1[i], b2[i], r);
        } else {
            const float f = interp_ease(mode, it.timer.current_f, float(end));
            cur[i] = f * (goal[i] - ini[i]) + ini[i];
        }
    }
    return it.current;
}

// 0x41dcb0 case 0x18 / 0x19 / 0x29 / 0x2a：镜像（flags & 0x80000）时的角度：π/2 - wrap(a - π/2) 再 wrap
inline float mirror_angle(float a) noexcept { return wrap_angle(float(kHalfPi - wrap_angle(float(a - kHalfPi)))); }

// 0x4033d0：轨道模式（4）把 pos2 对齐到 pos，再按当前模式积分一次
inline void sync_motion_center(MotionState& m, float game_speed) noexcept {
    if ((m.mode & 0xf) == 4) m.pos2 = m.pos;
    m.integrate(game_speed);
}

// EtShooter+0x340 激光参数区（ECL 700 / 701 写入；702 / 703 / 711 / 713 读取）
struct ShooterLaserParams {
    float width = 0;       // +0x340（ECL 700 第 4 浮点）
    float unused[3]{};     // +0x344..0x34c
    i32 telegraph = 0;     // +0x350（ECL 701：预警帧 / 曲线激光采样点数）
    i32 expand = 0;        // +0x354
    i32 active = 0;        // +0x358
    i32 shrink = 0;        // +0x35c
    u32 flags = 0;         // +0x360（并入激光 flags）
};
static_assert(sizeof(ShooterLaserParams) == sizeof(EtShooter::extra), "激光参数区为 0x24 字节");

ShooterLaserParams laser_params(const EtShooter& s) noexcept {
    ShooterLaserParams p;
    std::memcpy(&p, s.extra, sizeof p);
    return p;
}
void store_laser_params(EtShooter& s, const ShooterLaserParams& p) noexcept { std::memcpy(s.extra, &p, sizeof p); }
}  // namespace

// ECL 601 / 702 / 703 / 711 / 713 共用：发射器的实际发射坐标（ECL 628 绝对坐标优先，否则敌机坐标 + 偏移）
Vec3 Enemy::shooter_origin(int slot) const noexcept {
    const Vec3& off = shooter_offset[slot];
    if (shooter_pos2[slot].z > 0.9f) return {shooter_pos2[slot].x + off.x, off.y + shooter_pos2[slot].y, 0.0f};
    return {off.x + pos.x, off.y + pos.y, off.z + pos.z};
}

const char* Enemy::routine_name() const noexcept {
    if (!program || primary.routine < 0 || u32(primary.routine) >= program->definitions.size()) return nullptr;
    return program->subroutine(u32(primary.routine)).name.c_str();
}

// 0x443840：自机相对 from 的方向角；重合时返回 π/2（DAT_00494534）
float Enemy::angle_to_player(const Vec3& from) const noexcept {
    if (player == nullptr) return kHalfPi;
    const float dy = player->pos.y - from.y;
    const float dx = player->pos.x - from.x;
    if (dy == 0.0f && dx == 0.0f) return kHalfPi;
    return atan2_f(dy, dx);
}

// 0x417580：id 为 0 或链表为空 → 空；否则命中该 id，未命中返回链表最后一只（与 find_id_or_last 同一遍历序）
Enemy* Enemy::boss(int slot) const {
    return manager != nullptr ? manager->find_id_or_last(manager->boss_ids[slot]) : nullptr;
}

bool Enemy::integer(i32 var, i32& value) {
    switch (var) {
        case -10000: value = rng ? signed_bits(rng->next32() & 0x7fffffffu) : 0; return true;
        case -9999: value = rng ? truncate_int(rng->unit()) : 0; return true;
        case -9997: case -9977: value = truncate_int(pos.x); return true;
        case -9996: case -9976: value = truncate_int(pos.y); return true;
        case -9995: case -9975: value = truncate_int(motion1.pos.x); return true;
        case -9994: case -9974: value = truncate_int(motion1.pos.y); return true;
        case -9993: case -9973: value = truncate_int(motion2.pos.x); return true;
        case -9992: case -9972: value = truncate_int(motion2.pos.y); return true;
        case -9991: case -9965: value = truncate_int(player ? player->pos.x : 0); return true;
        case -9990: case -9964: value = truncate_int(player ? player->pos.y : 0); return true;
        case -9988: value = life_timer.current; return true;
        case -9987: value = rng ? truncate_int(rng->signed_unit()) : 0; return true;
        case -9986: value = (flags >> 24) & 1; return true;
        case -9985: value = int_regs[0]; return true;
        case -9984: value = int_regs[1]; return true;
        case -9983: value = int_regs[2]; return true;
        case -9982: value = int_regs[3]; return true;
        case -9981: value = truncate_int(float_regs[0]); return true;
        case -9980: value = truncate_int(float_regs[1]); return true;
        case -9979: value = truncate_int(float_regs[2]); return true;
        case -9978: value = truncate_int(float_regs[3]); return true;
        case -9971: value = truncate_int(motion1.angle); return true;   // +0x12b0
        case -9970: value = truncate_int(motion2.angle); return true;   // +0x12f4
        case -9969: value = truncate_int(motion1.speed); return true;   // +0x12ac
        case -9968: value = truncate_int(motion2.speed); return true;   // +0x12f0
        case -9967: value = truncate_int(motion1.radius); return true;  // +0x12b4
        case -9966: value = truncate_int(motion2.radius); return true;  // +0x12f8
        case -9963: { const Enemy* b = boss(0); value = b ? truncate_int(b->pos.x) : 0; return true; }
        case -9962: { const Enemy* b = boss(0); value = b ? truncate_int(b->pos.y) : 0x80; return true; }
        case -9958: value = truncate_int(atan2_f(delta.y, delta.x)); return true;  // 0x4052a0(+0x1288, +0x1284)
        case -9931: value = manager ? i32(manager->last_id) : 0; return true;     // DAT_004a6dc0+0x94
        case -9960: value = globals ? globals->rank : 0; return true;  // DAT_004a57bc
        case -9959: value = globals ? globals->difficulty : 0; return true;
        case -9954: value = life; return true;
        case -9953: value = globals && globals->difficulty == 0; return true;
        case -9952: value = globals && globals->difficulty == 1; return true;
        case -9951: value = globals && globals->difficulty == 2; return true;
        case -9950: value = globals && globals->difficulty == 3; return true;
        case -9949: case -9948: case -9947:
            value = manager ? manager->counters[var + 9949] : 0;
            return true;
        case -9946: value = manager ? manager->enemy_count : 0; return true;
        case -9945: value = globals ? globals->shot + globals->shot_aux : 0; return true;
        case -9943: case -9942: case -9941: case -9940:
            value = int_regs[var + 9943];
            return true;
        case -9935: case -9934: case -9933: case -9932:
            value = truncate_int(temp_regs[var + 9935]);
            return true;
        case -9926: case -9925: case -9924: case -9923:
            value = manager ? manager->global_ints[var + 9926] : 0;
            return true;
        case -9922: case -9921: case -9920: case -9919:
        case -9918: case -9917: case -9916: case -9915:
            value = manager ? truncate_int(manager->global_floats[var + 9922]) : 0;
            return true;
        case -9914: value = i32(id); return true;
        case -9907: value = globals ? globals->replay_slot : 0; return true;
        case -9906: value = (flags >> 19) & 1; return true;
        case -9905: value = globals ? globals->stage_frame : 0; return true;
        case -9903: value = globals ? globals->season : 0; return true;
        case -9902: value = globals ? globals->power : 0; return true;
        default: error = "Enemy integer var " + std::to_string(var); return false;
    }
}

bool Enemy::floating(i32 var, float& value) {
    switch (var) {
        case -10000: value = rng ? float(rng->next32() & 0x7fffffffu) : 0; return true;
        case -9999: value = rng ? rng->unit() : 0; return true;
        case -9998: value = rng ? float(rng->signed_unit() * kPi) : 0; return true;
        case -9997: case -9977: value = pos.x; return true;
        case -9996: case -9976: value = pos.y; return true;
        case -9995: case -9975: value = motion1.pos.x; return true;
        case -9994: case -9974: value = motion1.pos.y; return true;
        case -9993: case -9973: value = motion2.pos.x; return true;
        case -9992: case -9972: value = motion2.pos.y; return true;
        case -9991: case -9965: value = player ? player->pos.x : 0; return true;
        case -9990: case -9964: value = player ? player->pos.y : 0; return true;
        case -9988: value = life_timer.current_f; return true;
        case -9987: value = rng ? rng->signed_unit() : 0; return true;
        case -9981: value = float_regs[0]; return true;
        case -9980: value = float_regs[1]; return true;
        case -9979: value = float_regs[2]; return true;
        case -9978: value = float_regs[3]; return true;
        case -9939: case -9938: case -9937: case -9936:
            value = float_regs[var + 9939];
            return true;
        case -9935: case -9934: case -9933: case -9932:
            value = temp_regs[var + 9935];
            return true;
        case -9922: case -9921: case -9920: case -9919:
        case -9918: case -9917: case -9916: case -9915:
            value = manager ? manager->global_floats[var + 9922] : 0;
            return true;
        case -9989: value = angle_to_player(pos); return true;          // 0x443840(+0x1250)
        case -9971: value = motion1.angle; return true;
        case -9970: value = motion2.angle; return true;
        case -9969: value = motion1.speed; return true;
        case -9968: value = motion2.speed; return true;
        case -9967: value = motion1.radius; return true;
        case -9966: value = motion2.radius; return true;
        case -9963: { const Enemy* b = boss(0); value = b ? b->pos.x : 0.0f; return true; }
        case -9962: { const Enemy* b = boss(0); value = b ? b->pos.y : 128.0f; return true; }
        case -9958: value = atan2_f(delta.y, delta.x); return true;
        case -9956: value = angle_to_player(motion1.pos); return true;  // 0x443840(+0x1294)
        case -9955: value = angle_to_player(motion2.pos); return true;  // 0x443840(+0x12d8)
        case -9931: value = manager ? float(double(manager->last_id)) : 0.0f; return true;  // 无符号 → double → float
        default: {
            i32 i = 0;
            if (!integer(var, i)) return false;
            value = float(i);
            return true;
        }
    }
}

i32* Enemy::integer_destination(i32 var) {
    switch (var) {
        case -9985: case -9984: case -9983: case -9982:
            return &int_regs[var + 9985];
        case -9949: case -9948: case -9947:
            return manager ? &manager->counters[var + 9949] : nullptr;
        case -9943: case -9942: case -9941: case -9940:
            return &int_regs[var + 9943];
        case -9926: case -9925: case -9924: case -9923:
            return manager ? &manager->global_ints[var + 9926] : nullptr;
        default: error = "Enemy integer dest " + std::to_string(var); return nullptr;
    }
}

float* Enemy::float_destination(i32 var) {
    switch (var) {
        case -9997: case -9977: return &pos.x;
        case -9996: case -9976: return &pos.y;
        case -9995: case -9975: return &motion1.pos.x;
        case -9994: case -9974: return &motion1.pos.y;
        case -9993: case -9973: return &motion2.pos.x;
        case -9992: case -9972: return &motion2.pos.y;
        case -9981: case -9980: case -9979: case -9978:
            return &float_regs[var + 9981];
        case -9939: case -9938: case -9937: case -9936:
            return &float_regs[var + 9939];
        case -9935: case -9934: case -9933: case -9932:
            return &temp_regs[var + 9935];
        case -9922: case -9921: case -9920: case -9919:
        case -9918: case -9917: case -9916: case -9915:
            return manager ? &manager->global_floats[var + 9922] : nullptr;
        case -9969: return &motion1.angle;
        case -9968: return &motion2.angle;
        case -9967: return &motion1.speed;
        case -9966: return &motion2.speed;
        default: error = "Enemy float dest " + std::to_string(var); return nullptr;
    }
}

// 0x474430：新建上下文（0x41a400）、头插异步线程链（主上下文节点 +0x11fc 之后），
// 再以 0x471db0 在新上下文上建立调用帧并传入 spawn 指令的参数
bool Enemy::spawn(EclContext& source, i32 id, u32 skipped_arguments) {
    auto ctx = std::make_unique<EclContext>();
    ctx->variables = this;
    ctx->threads = this;
    ctx->commands = this;
    ctx->game_rng = rng;
    ctx->difficulty = source.difficulty;
    ctx->interpolation_rate = source.interpolation_rate;
    ctx->live_rate = source.live_rate;
    ctx->thread_id = id;
    ctx->program = program;
    if (!source.instruction || !program) return false;
    if (!source.call_into(*ctx, skipped_arguments)) { error = source.error; return false; }
    async_threads.push_front(std::move(ctx));
    return true;
}

EclContext* Enemy::find(i32 id) {
    for (auto& t : async_threads) if (t && t->thread_id == id) return t.get();
    return nullptr;
}

void Enemy::terminate_children() {
    for (auto& t : async_threads) if (t) t->instruction_offset = -1;
}

void Enemy::reset_context(EclContext& ctx, i32 routine) {
    const u8 diff = primary.difficulty ? primary.difficulty : 0xff;
    ctx = EclContext{};
    ctx.variables = this;
    ctx.threads = this;
    ctx.commands = this;
    ctx.game_rng = rng;
    ctx.program = program;
    ctx.difficulty = diff;
    if (routine >= 0) ctx.bind(*program, routine);
}

bool Enemy::init(EnemyManager& mgr, EclProgram& prog, const char* sub, const EnemySpawnArgs& args,
                 GameGlobals& g, Player* pl, Rng& r, u8 difficulty) {
    manager = &mgr;
    program = &prog;
    globals = &g;
    player = pl;
    rng = &r;
    bullets = mgr.bullets;
    items = mgr.items;
    effects = mgr.effects;
    std::memset(vm_ids, 0, sizeof vm_ids);
    for (int i = 0; i < kVmSyncSlots; ++i) { vm_offsets[i] = Vec3{}; vm_parent_slot[i] = -1; }
    anm_slot = anm_slot_main = main_script = main_script_base = sprite_direction = 0;
    anm_layer = 0;
    anm_layer_offset = 1;  // 0x41b580：+0x1488（EnemyInf+0x27c）= 1，主 VM 默认落在层 8（1 + 7），ECL 552 可改
    delta = Vec3{};
    vm_rotation = 0;
    death_anm_slot = 0;
    death_script = 0;  // 0x41b580：+0x51f4 = 0（首帧 ECL 后仍为 0 则按主脚本取默认爆散脚本）
    std::memset(static_cast<void*>(shooters), 0, sizeof shooters);
    for (int i = 0; i < kShooterSlots; ++i) { shooter_offset[i] = Vec3{}; shooter_pos2[i] = Vec3{}; shooter_ex_counter[i] = 0; }
    safe_radius_sq = 0;
    id = mgr.next_id;
    mgr.last_id = id;  // DAT_004a6dc0+0x94（ECL -9931）
    mgr.next_id = mgr.next_id + 1;
    if (mgr.next_id == 0) mgr.next_id = 1;
    flags = 0;
    if (args.mirror) flags |= kFlagMirror;
    if (args.life >= 1000) flags |= kFlagBossLife;
    pos = args.pos;
    motion1 = MotionState{};
    motion2 = MotionState{};
    motion1.pos = args.pos;
    hitbox = {24.0f, 24.0f, 0.0f};
    hurtbox = {24.0f, 24.0f, 0.0f};
    life = truncate_int(args.life);
    life_max = life;
    life_display = 0;
    phase_life = 0;
    life_budget = 0;  // 0x41aa70 不预置血量池（op 511 / 522 族才写 life × 7）
    life_flags = 0;
    damage_accum = 0;
    damage_rate = 1.0f;
    season_item_step = season_item_next = 0;
    hit_timer = Timer{};
    no_hit_timer = Timer{};
    score_bonus = 0;
    score_bonus_spell = globals != nullptr ? globals->spell_id : 0;  // puVar6[0x15cd] = DAT_004a5798
    bomb_script = bomb_script_restore = 0;
    for (auto& it : interrupts) it = EnemyInterrupt{};
    std::memcpy(int_regs, args.int_regs, sizeof int_regs);
    std::memcpy(float_regs, args.float_regs, sizeof float_regs);
    std::memcpy(temp_regs, args.temp_regs, sizeof temp_regs);
    invuln.set(2);
    life_timer.set(0x3c);
    season_drop_timer.set(0x3c);
    season_drop_duration = 0x3c;
    season_drop_max = 10;        // 0x41aa70：puVar6[0x1477] = 10
    season_drop_min = 1;
    drop_item_type = args.item;  // 0x41aa70：+0x519c = 生成参数 +0x10
    std::memset(drop_counts, 0, sizeof drop_counts);
    drop_radius = {32.0f, 32.0f};
    death_sub.clear();
    primary.difficulty = difficulty;
    const i32 routine = prog.find_index(sub);
    if (routine < 0) { error = std::string("missing sub ") + sub; return false; }
    reset_context(primary, routine);
    // 0x41aa70 → 0x41d1e0：立即跑一帧；0x40000 留给管理器清除（从而再隔一帧才第二次真正更新）
    update(1.0f);
    death_sfx = i32((id & 1u) + 3u);
    if (death_script == 0) apply_default_death_effect();
    return alive;
}

// 0x41aa70 尾段：ECL 未指定（op 323）死亡特效时，按 enemy.anm 主脚本号选 effect.anm 的爆散脚本
// （44 白 / 40 红 / 48 蓝 / 52 绿 / 58 / 57 / 56），其它 anm 槽一律 44
void Enemy::apply_default_death_effect() {
    death_script = 44;
    if (anm_slot_main == 2) {
        switch (main_script) {
        case 0: case 0x14: case 0x3b: case 0x3e: case 0x57: death_script = 44; break;
        case 5: case 0x19: case 0x35: case 0x4f: death_script = 40; break;
        case 10: case 0x38: case 0x53: death_script = 48; break;
        case 0xf: case 0x5b: death_script = 52; break;
        case 0x1e: death_script = 58; break;
        case 0x23: death_script = 57; break;
        case 0x28: death_script = 56; break;
        default: break;
        }
    }
    death_anm_slot = 1;
}

float Enemy::live_speed(float fallback) const noexcept {
    return manager != nullptr && manager->global_game_speed != nullptr ? *manager->global_game_speed : fallback;
}

const float* Enemy::live_speed_ptr(float& fallback) const noexcept {
    return manager != nullptr && manager->global_game_speed != nullptr ? manager->global_game_speed : &fallback;
}

bool Enemy::update(float game_speed) {
    if (!alive) return false;
    // 0x406190（计时推进）与 0x402ff0 / 0x403110（运动积分）每次都直接读全局速度 DAT_004a5788：本敌机的 ECL 547 之后
    // 立刻是新值；ECL 步进 0x473bc0 的 elapsed 取调用当时的值。没有管理器（独立驱动）时用入口参数
    const float* const live = live_speed_ptr(game_speed);
    // 0x41d2e0：若 0x40000 已置位则整帧跳过（生成后首个管理器回合只清标志）
    if ((flags & kFlagReentry) != 0) return true;
    flags |= kFlagReentry;

    // 0x41bb50：先推进两组运动的插值器（角度 +0x3c8/+0x428、速度 +0x3f8/+0x458、半径 +0x488/+0x4cc），
    // 再按位置插值（+0x2f8/+0x360）或模式算速度，最后积分
    for (int slot = 0; slot < 2; ++slot) {
        MotionState& m = slot == 0 ? motion1 : motion2;
        if (angle_interp[slot].end_time != 0 && (m.mode & 0xf) != 2 && (m.mode & 0xf) != 3)
            m.angle = wrap_angle(wrap_angle(step_interp_float(angle_interp[slot], *live)));
        if (speed_interp[slot].end_time != 0) m.speed = step_interp_float(speed_interp[slot], *live);
    }
    for (int slot = 0; slot < 2; ++slot) {
        MotionState& m = slot == 0 ? motion1 : motion2;
        if (radius_interp[slot].end_time != 0) {
            const Vec2 r = step_interp_vec2(radius_interp[slot], *live);
            m.radius = r.x;
            m.radius_speed = r.y;
        }
    }
    for (int slot = 0; slot < 2; ++slot) {
        MotionState& m = slot == 0 ? motion1 : motion2;
        if (pos_interp[slot].end_time == 0) {
            m.update_velocity(*live);
        } else {
            const Vec3 target = step_pos_interp(pos_interp[slot], *live);
            m.velocity.x = target.x - m.pos.x;
            m.velocity.y = target.y - m.pos.y;
            m.velocity.z = target.z - m.pos.z;
        }
    }
    motion1.integrate(*live);
    // flags & 0x4000000：motion2.pos += DAT_004c140c..14（从未写入，恒 0）
    motion2.integrate(*live);
    apply_motion_position();

    // 0x41bb50 尾：朝向切换 + 从主 VM 回读精灵宽高（+0x3f58/+0x3f5c，绑定前为 0），再判进入场地与出界销毁
    sync_sprite_direction();
    const float half_w = sprite_size.x * 0.5f, half_h = sprite_size.y * 0.5f;
    if (pos.x + half_w < -192.0f || 192.0f < pos.x - half_w) {
        if ((flags & 0x10004u) == 0x10000u) { alive = false; return false; }
    } else if (pos.y + half_h < 0.0f || 448.0f < pos.y - half_h) {
        if ((flags & kFlagAlive) != 0 && (flags & 8u) == 0) { alive = false; return false; }
    } else {
        flags |= kFlagAlive;
    }

    // 0x473bc0：先取下一节点再执行当前上下文——执行中头插的新线程（0x474430）本帧不跑
    primary.live_rate = live;
    primary.interpolation_rate = *live;
    auto it = async_threads.begin();
    int r = primary.step(*live);
    if (r == -2) { error = primary.error; alive = false; return false; }
    if (r == -1) { alive = false; return false; }
    while (it != async_threads.end()) {
        auto next = std::next(it);
        auto& t = *it;
        if (!t || t->instruction_offset < 0) { async_threads.erase(it); it = next; continue; }
        t->live_rate = live;
        t->interpolation_rate = *live;
        const int ar = t->step(*live);
        if (ar == -2) { error = t->error; alive = false; return false; }
        if (ar == -1) async_threads.erase(it);
        it = next;
    }

    if (frame_hook != FrameHook::None && run_frame_hook(frame_hook) != 0) { alive = false; return false; }
    if (resolve_hits(*live) != 0) { alive = false; return false; }
    update_distortion(*live);
    sync_vm_offsets();

    if (invuln.current > 0) invuln.untick(*live);
    if (no_hit_timer.current > 0) no_hit_timer.untick(*live);
    life_timer.tick(*live);
    if (season_drop_timer.current > 0) season_drop_timer.untick(*live);
    // 注意：不在这里清除 kFlagReentry——由 EnemyManager::update 清除（对齐 0x41b3d0）
    return true;
}

bool EnemyManager::dialogue_active() const noexcept { return world != nullptr && world->dialogue; }

AnmFile* EnemyManager::anm_file(i32 index) const noexcept {
    if (animations == nullptr) return nullptr;
    if (index == 0) return animations->file(7);
    if (index == 1) return animations->file(8);
    return index >= 2 ? animations->file(8 + index) : nullptr;
}

AnmFile* Enemy::anm_file(i32 index) const noexcept {
    return manager ? manager->anm_file(index) : nullptr;
}

// ECL 502 / 503：对全部 16 个 VM 槽（EnemyInf+0x124..0x160）按 id 找到即整棵子树应用
void Enemy::for_each_vm_tree(void (*apply)(AnmVm&)) {
    for (int slot = 0; slot < kVmSlots; ++slot) {
        if (AnmVm* vm = vm_at(slot)) apply(*vm);
    }
}

AnmVm* Enemy::vm_at(int slot) noexcept {
    if (slot < 0 || slot >= kVmSlots || manager == nullptr || manager->animations == nullptr) return nullptr;
    AnmVm* vm = manager->animations->find(vm_ids[slot]);
    if (vm == nullptr) vm_ids[slot] = 0;
    return vm;
}

AnmVm* Enemy::rebind_vm(int slot, i32 source, i32 script, bool full) {
    if (slot < 0 || slot >= kVmSlots || manager == nullptr || manager->animations == nullptr) return nullptr;
    AnmManager& anm = *manager->animations;
    anm.kill(vm_ids[slot]);
    vm_ids[slot] = 0;
    AnmFile* file = anm_file(source);
    if (file == nullptr) return nullptr;
    vm_ids[slot] = anm.spawn_front(*file, script, anm_layer_offset + 7);
    AnmVm* vm = vm_at(slot);
    if (vm == nullptr) return nullptr;
    if (!full) return vm;
    if (slot == 0) {
        sprite_size.x = vm->sprite_size.y * vm->scale.y;
        sprite_size.y = vm->sprite_size.x * vm->scale.x;
    }
    if ((flags & 0x20u) != 0) AnmManager::hide_tree(*vm);
    return vm;
}

// 0x41bb50 尾段
// 0x41c1f0：本帧位移存 EnemyInf+0x78（朝向切换与 0x80 位 VM 的朝向角用），pos 以 mode 0 的 0x403110 积分
void Enemy::apply_motion_position() {
    delta.x = (motion1.pos.x + motion2.pos.x) - pos.x;
    delta.y = (motion1.pos.y + motion2.pos.y) - pos.y;
    delta.z = (motion1.pos.z + motion2.pos.z) - pos.z;
    pos.x = floor_f((pos.x + delta.x) * 100.0f) / 100.0f;
    pos.y = floor_f((pos.y + delta.y) * 100.0f) / 100.0f;
    pos.z = pos.z + delta.z;
    if ((flags & 0x20000u) != 0) {
        const float hx = bound_size.x * 0.5f, hy = bound_size.y * 0.5f;
        if (pos.x < bound_center.x - hx) pos.x = bound_center.x - hx;
        else if (pos.x > bound_center.x + hx) pos.x = bound_center.x + hx;
        if (pos.y < bound_center.y - hy) pos.y = bound_center.y - hy;
        else if (pos.y > bound_center.y + hy) pos.y = bound_center.y + hy;
        motion1.pos.x = pos.x - motion2.pos.x;
        motion1.pos.y = pos.y - motion2.pos.y;
        motion1.pos.z = pos.z - motion2.pos.z;
    }
}

void Enemy::sync_sprite_direction() {
    if (manager == nullptr || manager->animations == nullptr) return;
    AnmManager& anm = *manager->animations;
    if ((flags & 0x100000u) != 0) {
        const i32 dir = delta.x < -0.03f ? -1 : (0.03f < delta.x ? 1 : 0);
        if (sprite_direction != dir) {
            i32 step = 0;
            if (sprite_direction == -1) step = dir != 0 ? 2 : 3;
            else if (sprite_direction == 0) step = dir != -1 ? 2 : 1;
            else if (sprite_direction == 1) step = dir == 0 ? 4 : 1;
            AnmVm* old = vm_at(0);
            Vec3 offset{};
            if (old != nullptr) {
                offset = old->pos;
                anm.kill(vm_ids[0]);
                vm_ids[0] = 0;
            }
            if (AnmFile* file = anm_file(anm_slot_main)) {
                vm_ids[0] = anm.spawn_vm(*file, main_script_base + step, anm_layer_offset + 7, offset, 0.0f, 8, false);
            }
            sprite_direction = dir;
        }
    }
    if (AnmVm* vm = vm_at(0)) {
        sprite_size.x = std::fabs(vm->sprite_size.y * vm->scale.y);
        sprite_size.y = std::fabs(vm->sprite_size.x * vm->scale.x);
    }
}

// 0x41cbd0：Boss 畸变光环每帧更新
//   1. 半径未到目标则 += 2 × 速度；
//   2. 以 Boss 为中心、半径 + 20 为半边长重铺 17 × 17 网格（0x418df0）；
//   3. 逐节点（列主序，相位 angle_a / angle_b 跨整张网格连续推进）：圈内（r² − d² >= 0）强度 s = (r² − d²) / r²，
//      顶点色 = 圈色各分量向 255 过渡 s、alpha 置 255，顶点沿径向外推 32·s 再叠加 sin 相位 × 8·s 的抖动；
//      圈外只把 alpha 清 0。随后节点坐标截到场地 (0, 16) 起 384 × 448 的开区间内（两侧各留 1 像素），
//      UV 以截断后的节点坐标 / 渲染目标尺寸重算（负值截 0）；
//   4. 相位整体推进 π/16 × 速度 / π/32 × 速度。
void Enemy::update_distortion(float game_speed) {
    // 0x41cbd0 的三处（+0x44c8 / +0x44d0 / +0x44d4）都直接读 DAT_004a5788：本敌机的 ECL 547 之后已是新值
    const float live = live_speed(game_speed);
    if (!distortion || manager == nullptr || manager->animations == nullptr) return;
    DistortionGrid& grid = *distortion;
    const float radius = distortion_radius;
    float angle_a = distortion_angle_a;
    float angle_b = distortion_angle_b;
    if (radius < distortion_radius_target) distortion_radius = live * 2.0f + radius;

    static const AnmGeometry kDefaultGeometry;
    const AnmGeometry& geometry = manager->animations->geometry != nullptr ? *manager->animations->geometry : kDefaultGeometry;
    const u32 target_w = manager->animations->render_target_width;
    const u32 target_h = manager->animations->render_target_height;
    const float size = radius + radius + 40.0f;
    grid.fill(pos.x - radius - 20.0f, pos.y - radius - 20.0f, size, size, geometry, target_w, target_h);
    if (grid.columns() <= 0) return;

    const float center_x = float(target_w) * 0.5f + pos.x;
    const float center_y = geometry.field_offset_y + pos.y;
    const float center_z = pos.z;
    const float radius_sq = radius * radius;
    ScreenColorUvVertex* vertex = grid.vertices().data();
    Vec3* node = grid.nodes().data();
    const i32 total = grid.columns() * grid.rows();
    for (i32 i = 0; i < total; ++i, ++vertex, ++node) {
        float dx = node->x - center_x;
        float dz = node->z - center_z;
        float dy = node->y - center_y;
        const float inside = radius_sq - (dy * dy + dx * dx);
        if (inside < 0.0f) {
            vertex->color &= 0x00ffffffu;
        } else {
            const float strength = inside / radius_sq;
            vertex->color = distortion_color | 0xff000000u;
            u32 color = vertex->color;
            for (int shift = 16; shift >= 0; shift -= 8) {
                const u32 channel = (color >> shift) & 0xffu;
                const u32 lifted = u32(i32(255.0f - float(i32(0xff - channel)) * strength)) & 0xffu;
                color = (color & ~(0xffu << shift)) | (lifted << shift);
            }
            vertex->color = color;
            const float push = strength * 32.0f;
            // D3DXVec3Normalize：float32 长度，零向量保持为零
            const float length = std::sqrt(dx * dx + dy * dy + dz * dz);
            if (length != 0.0f) { dx /= length; dy /= length; dz /= length; }
            float offset_x = push * dx;
            const float offset_y0 = push * dy;
            offset_x = float(std::sin(double(angle_a))) * strength * 8.0f + offset_x;
            const float offset_y = float(std::sin(double(angle_b))) * strength * 8.0f + offset_y0;
            vertex->x = vertex->x + offset_x;
            vertex->z = 0.0f;
            vertex->y = vertex->y + offset_y;
            node->z = 0.0f;
        }
        angle_a = wrap_angle(angle_a + 0.09817477f);
        angle_b = wrap_angle(angle_b - 0.049087387f);
        // 0x41cfe7：节点处于渲染目标坐标，按 DAT_004d9d38 起的场地开区间截断。
        const float field_x = geometry.field_offset_x - 192.0f;
        if (field_x < node->x) {
            if (field_x + 384.0f <= node->x) { vertex->x = field_x + 384.0f - 1.0f; node->x = field_x + 384.0f - 1.0f; }
        } else {
            vertex->x = field_x + 1.0f;
            node->x = field_x + 1.0f;
        }
        const float field_y = geometry.field_offset_y;
        if (field_y < node->y) {
            if (field_y + 448.0f <= node->y) { vertex->y = field_y + 448.0f - 1.0f; node->y = field_y + 448.0f - 1.0f; }
        } else {
            vertex->y = field_y + 1.0f;
            node->y = field_y + 1.0f;
        }
        vertex->u = node->x / float(target_w);
        vertex->v = node->y / float(target_h);
        if (vertex->u < 0.0f) vertex->u = 0.0f;
        if (vertex->v < 0.0f) vertex->v = 0.0f;
    }
    distortion_angle_a = wrap_angle(live * 0.19634955f + distortion_angle_a);
    distortion_angle_b = wrap_angle(live * 0.09817477f + distortion_angle_b);
}

// 0x41d2e0 后段
void Enemy::sync_vm_offsets() {
    if (manager == nullptr || manager->animations == nullptr) return;
    if ((flags & 0x4000000u) != 0) {
        for (int i = 0; i < kVmSyncSlots; ++i) {
            if (AnmVm* vm = manager->animations->find(vm_ids[i])) vm->entity_offset = pos;
        }
        return;
    }
    for (int i = 0; i < kVmSyncSlots; ++i) {
        AnmVm* vm = vm_at(i);
        if (vm == nullptr) continue;
        Vec3 offset{vm_offsets[i].x + pos.x, vm_offsets[i].y + pos.y, pos.z + vm_offsets[i].z};
        if (vm_parent_slot[i] >= 0) {
            if (AnmVm* parent = vm_at(vm_parent_slot[i])) {
                offset.x = parent->pos.x + offset.x;
                offset.y = parent->pos.y + offset.y;
                offset.z = parent->pos.z + offset.z;
            }
        }
        vm->entity_offset = offset;
        if ((vm->flags_hi & 0x80u) != 0) {
            const float angle = atan2_f(delta.y, delta.x);
            vm->flags_lo |= AnmVm::kRotationDirty;
            vm->rotation.z = angle;
            vm_rotation = angle;
        }
    }
}

// DAT_004917b8 函数表（op 632 / 637 / 639）
int Enemy::run_frame_hook(FrameHook hook) {
    switch (hook) {
        case FrameHook::TriggerBulletsNearPlayer:  // 0x4252d0：半径取 F0（EnemyInf+0x29c）
            if (manager != nullptr && manager->bullets != nullptr && player != nullptr)
                manager->bullets->trigger_ex_param_near(player->pos, float_regs[0]);
            return 0;
        case FrameHook::DrainPowerAndSeason:  // 0x4253b0：min(上限, 0)——上限非负时即归零；随后 0x4440e0 重建子机、0x42c600 刷 HUD
            if (globals != nullptr) {
                globals->season_gauge = globals->season_gauge_max < 0 ? globals->season_gauge_max : 0;
                globals->power = globals->power_max < 0 ? globals->power_max : 0;
                if (player != nullptr && manager != nullptr && manager->animations != nullptr)
                    player->setup_options(*manager->animations, *globals);
                // 量表归零当帧就刷量表条与等级图标（0x42c600）
                if (manager != nullptr && manager->gui != nullptr) manager->gui->refresh_season_gauge();
            }
            return 0;
        case FrameHook::None: break;
    }
    return 0;
}

// 0x41c330 尾段（主 VM 闪烁 / 受击音效）
void Enemy::update_hit_flash() {
    AnmVm* vm = vm_at(0);
    if (vm == nullptr) return;
    auto flash = [&](u32 bgra) {
        std::memcpy(vm->color2, &bgra, 4);
        vm->flags_lo = (vm->flags_lo & ~0x40000u) | 0x20000u;
    };
    if (hit_flash_timer != 0) {
        vm->flags_lo &= ~0x60000u;
        --hit_flash_timer;
        return;
    }
    const u32 spell = manager != nullptr && manager->spellcard != nullptr ? manager->spellcard->flags : 0u;
    // 0x40800000 位（Boss 家族）在非符卡取得阶段按血量池阈值闪蓝
    auto low_life_flash = [&](i32 spell_limit, i32 normal_limit) {
        if ((flags & 0x40800000u) == 0 || (spell & 9u) == 9u) return false;
        return (spell & 1u) != 0 ? life_display < spell_limit : life_display < normal_limit;
    };
    if ((flags & 0x80000000u) != 0) {
        if (life_timer.current % 4 == 0) flash(0xffff00ffu);
        else vm->flags_lo &= ~0x60000u;
    }
    if ((flags & 0x200000u) == 0 || (flags & 0x2000u) != 0) {
        if (life_timer.current % 4 == 0) {
            if (low_life_flash(100, 500)) flash(0xff0000ffu);
        } else {
            vm->flags_lo &= ~0x60000u;
        }
        return;
    }
    flash(0xff0000ffu);
    hit_flash_timer = 4;
    if (manager == nullptr) return;
    if (hit_sfx >= 0) manager->play_sfx(hit_sfx, pos.x);
    else manager->play_sfx(low_life_flash(200, 900) ? 0x23 : 0x22, pos.x);
}

// 0x424e40
void Enemy::set_interrupt(int slot, i32 life_threshold, i32 time_threshold, const char* sub) {
    if (slot < 0 || slot >= kInterruptSlots) return;
    EnemyInterrupt& it = interrupts[slot];
    it.life_threshold = life_threshold;
    it.time_threshold = time_threshold;
    if (sub != nullptr) { it.sub = sub; it.timeout_sub = sub; }
    else { it.sub.clear(); it.timeout_sub.clear(); }
}

// 0x424f00
const std::string* Enemy::life_interrupt() {
    const i32 current = life;
    life_display = current;
    phase_life = 0;
    for (int i = 0; i < kInterruptSlots; ++i) {
        EnemyInterrupt& it = interrupts[i];
        if (it.life_threshold < 0) continue;
        life_display = current - it.life_threshold;
        phase_life = it.life_threshold;
        if (it.life_threshold < current) return nullptr;
        if (score_bonus != 0 && globals != nullptr && score_bonus_spell == globals->spell_id) {
            globals->enemy_bonus += score_bonus;
            score_bonus = 0;
        }
        life = it.life_threshold;
        it.life_threshold = -1;
        life_timer.reset();
        flags &= ~0x1000000u;
        return &it.sub;
    }
    return nullptr;
}

// 0x425010
const std::string* Enemy::time_interrupt() {
    int slot = 0;
    for (;; ++slot) {
        if (slot >= kInterruptSlots) return nullptr;
        if (interrupts[slot].life_threshold >= 0 && interrupts[slot].time_threshold >= 1) break;
    }
    EnemyInterrupt& it = interrupts[slot];
    if ((flags & 0x800000u) != 0 && manager != nullptr && manager->gui != nullptr) {
        // Boss：Gui 显示的剩余秒数 / 百分秒（上限 99），到 2 秒 / 5 秒时 Gui 播报警音
        i32 remaining = it.time_threshold - life_timer.current;
        i32 seconds = remaining / 0x3c;
        i32 fraction = ((remaining % 0x3c) * 100) / 0x3c;
        if (seconds > 99) seconds = fraction = 99;
        manager->gui->boss_timer_display = seconds;
        manager->gui->boss_timer_fraction = fraction;
    }
    if (life_timer.current < it.time_threshold) return nullptr;
    life = it.life_threshold;
    it.life_threshold = -1;
    life_timer.reset();
    flags |= 0x1000000u;
    Spellcard* spell = manager != nullptr ? manager->spellcard : nullptr;
    const u32 sflags = spell != nullptr ? spell->flags : 0u;
    if ((sflags & Spellcard::kSurvival) == 0) {
        flags &= ~0x1000000u;
        if (spell != nullptr) {
            spell->flags |= Spellcard::kTimedOut;
            if ((spell->flags & Spellcard::kActive) != 0) {
                if (spell->timer.current > 0x3b) {
                    spell->bonus = 0;
                    spell->flags &= ~(Spellcard::kBonusAvailable | Spellcard::kBombUsed);
                    manager->counters[2] = 0;
                    score_bonus = 0;
                    return &it.timeout_sub;
                }
                if (manager->bomb_active) spell->flags |= Spellcard::kBombUsed;
            }
        }
        if (manager != nullptr) manager->counters[2] = 0;
    } else if ((sflags & (Spellcard::kSurvival | Spellcard::kActive)) == (Spellcard::kSurvival | Spellcard::kActive)) {
        if (globals != nullptr) globals->enemy_bonus += score_bonus;
    }
    score_bonus = 0;
    return &it.timeout_sub;
}

// 0x41a4f0 + 0x41a5a0 + 0x474740/0x474890 + 0x473bc0
bool Enemy::switch_routine(const std::string& sub, float game_speed) {
    if (program == nullptr) return true;
    const i32 routine = program->find_index(sub.c_str());
    async_threads.clear();
    if (routine < 0) { error = "missing interrupt sub " + sub; return false; }
    reset_context(primary, routine);
    primary.live_rate = live_speed_ptr(game_speed);
    primary.interpolation_rate = live_speed(game_speed);
    const int r = primary.step(live_speed(game_speed));
    if (r == -2) { error = primary.error; return false; }
    return r == 0;
}

// 0x41c330
int Enemy::resolve_hits(float game_speed) {
    if (manager == nullptr) return 0;
    Spellcard* spell = manager->spellcard;
    const u32 sflags = spell != nullptr ? spell->flags : 0u;
    AnmManager* anm = manager->animations;

    // Bomb / 季节解放期间把主 VM 换成 bomb_script（op 546），结束后换回
    const bool burst = manager->bomb_active || manager->release_active;
    auto replace_main = [&](i32 script) {
        // 0x46f440：旧 VM 停止（flags_lo bit0 清、ins_offset = -1），同文件 script 在层 -1 新建
        main_script_base = script;
        AnmVm* vm = vm_at(0);
        if (vm == nullptr || anm == nullptr) return;
        AnmFile* file = anm->file(vm->file_slot);
        vm->flags_lo &= ~1u;
        vm->ins_offset = -1;
        if (file != nullptr) vm_ids[0] = anm->spawn(*file, script, -1);
    };
    if ((flags & 0x10000000u) != 0 && burst && (flags & 0x20000000u) == 0) {
        replace_main(bomb_script);
        flags |= 0x20000001u;
    } else if (!burst && (flags & 0x20000000u) != 0) {
        replace_main(bomb_script_restore);
        flags &= ~0x20000001u;
    }

    // flags & 0x800：被炸弹类伤害源碰到即死（只判定不结算）
    if ((flags & 0x800u) != 0 && player != nullptr && anm != nullptr && globals != nullptr) {
        bool bomb_hit = false;
        const i32 probe = player->apply_damage_sources(*anm, *globals, pos, hitbox.x * 0.5f, id, &hit_pos, &bomb_hit, true);
        if (probe != 0 && bomb_hit) { die(game_speed); return 1; }
    }

    // 0x425010：时间阶段中断
    if (const std::string* sub = time_interrupt()) {
        if (!switch_routine(*sub, game_speed)) return -1;
    }

    const u32 before = flags;
    flags &= ~0x200000u;
    bool bomb_hit = false;
    if ((before & 0x21u) == 0) {
        i32 dmg = 0;
        if (hitbox.x > 0.0f && player != nullptr && anm != nullptr && globals != nullptr) {
            dmg = player->apply_damage_sources(*anm, *globals, pos, hitbox.x * 0.5f, id, &hit_pos, &bomb_hit);
            dmg = i32(float(dmg) * player->scale2);
        }
        // +0x4520 伤害回调 / +0x3fe0 待加伤害：无 ECL 写入路径，恒 0
        if (player != nullptr && (player->state == 2 || player->state == 0)) dmg /= 5;
        i32 applied = manager->dialogue_active() ? 0 : dmg;
        if (applied > 0) {
            if (!bomb_hit) {
                manager->damage_dealt += applied;
            } else if (applied < life) {
                manager->damage_dealt_bomb += applied;
            } else {
                manager->damage_dealt_bomb += ((applied - life) >> 2) + life;
            }
        }
        if (manager->bomb_active && damage_rate < 1.0f) {
            if (applied != 0 && damage_rate <= 0.0f) manager->play_sfx(0x24, pos.x);
            applied = i32(float(applied) * damage_rate);
        }
        if (applied != 0) {
            // 符卡中用过炸弹（flags & 0x21 == 0x21）→ 伤害 /30
            if ((sflags & (Spellcard::kActive | Spellcard::kBombUsed)) == (Spellcard::kActive | Spellcard::kBombUsed))
                applied /= 0x1e;
            // 0x41a6d0 无论是否符卡都先把伤害累加到 +0x3f88（op 1001 按此放季节道具），再扣血。
            // 无敌（flags & 0x10 或无敌计时）不调用它，0x41c330 只累加、不扣血。
            if ((flags & 0x10u) == 0 && invuln.current < 1) {
                damage_accum += applied;
                if ((life_flags & 1u) != 0) {
                    life_budget -= applied;
                    life = (life_budget - phase_life * 7) / 7 + phase_life;
                } else {
                    life -= applied;
                }
            } else {
                damage_accum += applied;
            }
            // op 1001：每累计 season_item_step 伤害放一个季节道具（visual_rng：unit + angle）
            if (season_item_step > 0 && season_item_next < damage_accum && rng != nullptr) {
                do {
                    season_item_next += season_item_step;
                    const float speed = float(double(rng->unit()) + double(1.2f));   // x87：unit + 1.2f
                    const float angle = rng->angle();
                    if (items != nullptr) items->spawn(0x10, pos, angle, speed, 0);
                } while (season_item_next < damage_accum);
            }
            hit_timer.set(0x1e);
            if (const std::string* sub = life_interrupt()) {
                if (!switch_routine(*sub, game_speed)) return -1;
            }
            if (life < 1 && ((flags >> 7) & 1u) == 0) { die(game_speed); return 1; }
            flags |= 0x200000u;
        }
    }
    if ((life_flags & 2u) != 0) { die(game_speed); return 1; }

    // 体碰撞（0x4439e0；flags & 0x1000 的矩形体碰撞 0x443af0 无 ECL 设置路径）与擦弹（flags & 0x200）
    if ((flags & 0x22u) == 0 && no_hit_timer.current < 1 && (flags & 0x4000000u) == 0 && player != nullptr) {
        int hit = 0;
        if (anm != nullptr)
            hit = player->collide_body(*anm, anm_file(1), pos, hurtbox.x * 0.5f, manager->dialogue_active(), false);
        if ((flags & 0x200u) != 0 && hit == 2 && life_timer.current % 6 == 0 && bullets != nullptr)
            bullets->graze(player->pos);
    }

    update_hit_flash();
    if (hit_timer.current > 0) hit_timer.untick(game_speed);
    return 0;
}

// 0x41d700
void Enemy::scatter_items() {
    if (rng == nullptr) return;
    auto request = [&](i32 type, const Vec3& at, float angle, float speed) {
        if (items != nullptr) items->spawn(type, at, angle, speed, 0);
    };
    float angle = float(double(rng->signed_unit()) * double(kPi));
    for (int index = 0; index < 16; ++index) {
        if (index == 15) {
            for (i32 i = 0; i < season_drop_max; ++i) {
                const float speed = float(double(rng->unit()) * double(1.9f) + double(0.2f));   // x87：unit × 1.9f + 0.2f
                const float a = rng->angle();
                request(0x10, pos, a, speed);
            }
            continue;
        }
        for (i32 i = 0; i < drop_counts[index]; ++i) {
            // 0x426240：椭圆半径上的点 × [0.5, 1) 随机缩放
            const float lx = float(std::cos(double(angle)) * double(drop_radius.x));
            const float ly = float(std::sin(double(angle)) * double(drop_radius.y));
            const float t = float(0.5 + double(rng->unit()) * 0.5);
            const Vec3 at{pos.x + t * lx, pos.y + ly * t, pos.z + 0.0f};
            request(index + 1, at, -1.5707964f, 2.2f);
            const float jitter = rng->signed_unit();
            angle = (angle + 1.5707964f) + (jitter * kPi) * 0.25f;
            for (int k = 0; angle > kPi && k <= 0x21; ++k) angle -= 6.2831855f;
            for (int k = 0; angle < -kPi && k <= 0x21; ++k) angle += 6.2831855f;
        }
    }
    std::memset(drop_counts, 0, sizeof drop_counts);
    season_drop_max = 0;
}

// 0x41a720
void Enemy::release_items() {
    if (drop_item_type != 0 && items != nullptr) items->spawn(drop_item_type, pos, -1.5707964f, 2.2f, 0);
    scatter_items();
    drop_item_type = 0;
}

void Enemy::settle_season_drop() {
    i32 count = season_drop_min;
    if (season_drop_timer.current >= 1 && season_drop_duration != 0) {
        count = ((season_drop_max - season_drop_min) * season_drop_timer.current) / season_drop_duration +
                season_drop_min;
    }
    season_drop_max = count;
}

// 0x41d520
void Enemy::die(float game_speed) {
    if (death_sfx >= 0 && manager != nullptr) manager->play_sfx(death_sfx, pos.x);
    if (death_script >= 0) {
        const float dx = hit_pos.x - pos.x, dy = hit_pos.y - pos.y;
        float angle = -1.5707964f;
        if (dx * dx + dy * dy >= 0.040000003f) angle = atan2_f(pos.y - hit_pos.y, pos.x - hit_pos.x);
        AnmFile* file = anm_file(death_anm_slot);
        if (file != nullptr && manager->animations != nullptr) {
            const u32 vid = manager->animations->spawn_effect(*file, death_script, 3, pos, angle);
            if (effects != nullptr) effects->register_id(vid);
        }
    }
    settle_season_drop();
    release_items();
    if (score_bonus > 0 && globals != nullptr && score_bonus_spell == globals->spell_id) {
        globals->enemy_bonus += score_bonus;
        score_bonus = 0;
    }
    if (!death_sub.empty() && program != nullptr) {
        const i32 routine = program->find_index(death_sub.c_str());
        async_threads.clear();
        if (routine >= 0) {
            reset_context(primary, routine);
            primary.live_rate = live_speed_ptr(game_speed);
            primary.interpolation_rate = live_speed(game_speed);
            primary.step(live_speed(game_speed));
        }
        death_sub.clear();
    }
}

// 0x41ba10：移出列表时（flags2 & 4 未置）先注销 Boss 槽，再杀掉全部绑定 VM 及其子树；畸变网格无条件析构
void Enemy::destroy() {
    distortion.reset();
    if (manager == nullptr) return;
    // 0x41ba54：flags & 0x800000 的敌机析构时清掉所在槽（+0x527c）。不清的话槽里留着已消失的 id，
    // 之后 0x417580 查不到就落到链表最后一只——Gui 给同一只 Boss 画出第二套血环，ECL 520 的等待也一直不满足
    if ((flags & 0x800000u) != 0 && boss_slot >= 0 && boss_slot < 3) manager->boss_ids[boss_slot] = 0;
    if (manager->animations == nullptr) return;
    for (u32& vid : vm_ids) {
        if (AnmVm* vm = manager->animations->find(vid)) AnmManager::kill_tree(*vm);
        vid = 0;
    }
}

int Enemy::execute(EclContext& ctx, u16 opcode) {
    auto iarg = [&](u32 n) {
        i32 v = 0;
        if (!ctx.integer(n, ctx.instruction->argument<i32>(n), v, false)) error = ctx.error;
        return v;
    };
    auto farg = [&](u32 n) {
        float v = 0;
        if (!ctx.floating(n, ctx.instruction->argument<float>(n), v, false)) error = ctx.error;
        return v;
    };
    auto fail = [&](const char* m) { error = m; ctx.error = m; return -2; };

    switch (opcode) {
        case 309: case 310: case 311: case 312:
            // 0x41dd5a：0x417580(0) 非空就不生成（槽号过期仍指向已消失的 id 时，原版落在链表最后一只上，一样拦住生成）
            if (manager != nullptr && manager->boss_slot(0) != nullptr)
                return 0;
            [[fallthrough]];
        case 300: case 301: case 304: case 305: case 321: {
            // 0x423050
            if (!ctx.instruction || !manager) return fail("spawn: no manager");
            const u32 name_size = ctx.instruction->argument<u32>(0);
            const auto* bytes = reinterpret_cast<const u8*>(ctx.instruction);
            if (name_size == 0 || name_size % 4 || name_size > ctx.instruction->length - 20)
                return fail("spawn: bad name");
            const char* sub = reinterpret_cast<const char*>(bytes + 20);
            const u32 base = 20 + name_size;
            // 参数用 pop=false：引用栈槽由指令 stack_adjustment 统一清理（避免双重弹栈）
            auto read_f = [&](u32 arg_index) {
                float lit = 0;
                if (base + arg_index * 4 + 4 <= ctx.instruction->length)
                    std::memcpy(&lit, bytes + base + arg_index * 4, 4);
                float v = lit;
                ctx.floating(arg_index + 1, lit, v, false);
                return v;
            };
            auto read_i = [&](u32 arg_index) {
                i32 lit = 0;
                if (base + arg_index * 4 + 4 <= ctx.instruction->length)
                    std::memcpy(&lit, bytes + base + arg_index * 4, 4);
                i32 v = lit;
                ctx.integer(arg_index + 1, lit, v, false);
                return v;
            };
            EnemySpawnArgs args;
            float x = read_f(0), y = read_f(1);
            const bool relative = opcode == 300 || opcode == 309 || opcode == 321 || opcode == 311 || opcode == 304;
            if (relative) {
                args.pos.x = pos.x + x;
                args.pos.y = pos.y + y;
            } else {
                args.pos.x = x;
                args.pos.y = y;
            }
            if (opcode == 311 || opcode == 304 || opcode == 312 || opcode == 305) args.mirror = 1;
            if ((flags & kFlagMirror) != 0) {
                args.pos.x = args.pos.x * -1.0f;
                args.mirror ^= 1u;
            }
            args.life = float(read_i(2));
            args.score = read_i(3);
            args.item = read_i(4);
            std::memcpy(args.int_regs, int_regs, sizeof args.int_regs);
            std::memcpy(args.float_regs, float_regs, sizeof args.float_regs);
            std::memcpy(args.temp_regs, temp_regs, sizeof args.temp_regs);
            if (!manager->spawn_enemy(sub, args)) { error = manager->error; return -2; }
            return 0;
        }
        case 302: anm_slot = iarg(0); return 0;
        case 500: {  // case 0x2d：首次设定判定时登记为“可计奖励”敌机
            hitbox.x = farg(0); hitbox.y = farg(1);
            if (score_bonus == 0) {
                score_bonus = 1;
                if (globals != nullptr) globals->enemy_bonus_pending += 1;
            }
            return 0;
        }
        case 501: hurtbox.x = farg(0); hurtbox.y = farg(1); return 0;
        case 502:  // case 0x2f：置位后若处于隐藏（0x20）则整棵 VM 树隐藏（0x46f340）
            flags |= u32(iarg(0));
            if ((flags & 0x20u) != 0) for_each_vm_tree(AnmManager::hide_tree);
            return 0;
        case 503:  // case 0x30：清位后若不再隐藏则整棵 VM 树显示（0x46f300）——即使本次未动 0x20 位也会执行
            flags &= ~u32(iarg(0));
            if ((flags & 0x20u) == 0) for_each_vm_tree(AnmManager::show_tree);
            return 0;
        case 504: {
            const float x = farg(0), y = farg(1), w = farg(2), h = farg(3);
            flags |= 0x20000u;
            bound_center = {x, y};
            bound_size = {w, h};
            return 0;
        }
        case 505: flags &= ~0x20000u; return 0;
        case 506: {  // case 0x33：清掉落表（保留季节道具数）
            std::memset(drop_counts, 0, sizeof drop_counts);
            return 0;
        }
        case 507: {  // case 0x34：掉落表 +0x3f90[arg0] = arg1（0 = 单掉道具种类、1..15 = 各类数量、16 = 季节道具数）
            const i32 slot = iarg(0), count = iarg(1);
            if (slot == 0) drop_item_type = count;
            else if (slot >= 1 && slot <= 15) drop_counts[slot - 1] = count;
            else if (slot == 16) season_drop_max = count;
            return 0;
        }
        case 509: {  // case 0x36：符卡练习模式外立即掉落
            if (globals != nullptr && (globals->flags & GameGlobals::kModeMask) == 0x20u) return 0;
            settle_season_drop();
            release_items();
            return 0;
        }
        case 508: {  // case 0x35
            const float rx = farg(0), ry = farg(1);
            drop_radius = {rx, ry};
            return 0;
        }
        case 510: drop_item_type = iarg(0); return 0;  // case 0x37
        case 556: {  // case 0x61：死亡子程序名
            if (!ctx.instruction) return 0;
            const auto* bytes = reinterpret_cast<const u8*>(ctx.instruction);
            const u32 limit = ctx.instruction->length;
            std::string name;
            for (u32 k = 20; k < limit && bytes[k] != 0; ++k) name.push_back(char(bytes[k]));
            death_sub = std::move(name);
            return 0;
        }
        case 562: release_items(); return 0;  // case 0x67
        case 511: {  // case 0x38
            life = life_max = life_display = iarg(0);
            if ((flags & 0x800000u) != 0) flags |= kFlagBossLife;
            life_budget = life * 7;
            return 0;
        }
        case 514: {  // case 0x3b：阶段中断登记（符卡练习里 Boss 槽 0 的中断一律改为 BossDead / BossEscape）
            if (!ctx.instruction) return 0;
            const auto* bytes = reinterpret_cast<const u8*>(ctx.instruction);
            const i32 slot = iarg(0), life_threshold = iarg(1), time_threshold = iarg(2);
            if (slot < 0 || slot >= kInterruptSlots) return 0;
            if (globals != nullptr && (globals->flags & GameGlobals::kModeMask) == 0x20u && (flags & 0x800000u) != 0) {
                const bool stage7_early = globals->stage == 7 && globals->spell_id < 0x29;
                const char* dead = boss_slot == 0 ? (stage7_early ? "MBossDead" : "BossDead")
                                                  : (stage7_early ? "MBossBDead" : "BossDead");
                set_interrupt(slot, 0, time_threshold, dead);
                interrupts[slot].timeout_sub = "BossEscape";
                return 0;
            }
            interrupts[slot].life_threshold = life_threshold;
            if (life_threshold >= 0) {
                std::string name;
                for (u32 k = 32; k < ctx.instruction->length && bytes[k] != 0; ++k) name.push_back(char(bytes[k]));
                set_interrupt(slot, life_threshold, time_threshold, name.c_str());
            }
            return 0;
        }
        case 521: {  // case 0x42：单独改超时子程序
            if (!ctx.instruction) return 0;
            const auto* bytes = reinterpret_cast<const u8*>(ctx.instruction);
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kInterruptSlots) return 0;
            std::string name;
            for (u32 k = 24; k < ctx.instruction->length && bytes[k] != 0; ++k) name.push_back(char(bytes[k]));
            interrupts[slot].timeout_sub = std::move(name);
            return 0;
        }
        case 516: if (manager) manager->play_sfx(iarg(0), pos.x); return 0;  // case 0x3d
        case 517: {  // case 0x3e：0x45d150(1, duration, from, to) 画面震动（每帧消耗两次 game_rng）
            const i32 to = iarg(2), from = iarg(1), duration = iarg(0);
            if (manager) manager->shake_requests.push_back({duration, from, to});
            return 0;
        }
        case 540: {  // case 0x51：Gui+0x188 Boss 剩余符卡星数
            const i32 count = iarg(0);
            if (manager && manager->gui) manager->gui->spell_star_count = count;
            return 0;
        }
        case 547:  // case 0x58：全局游戏速度 DAT_004a5788
            if (manager && manager->global_game_speed) *manager->global_game_speed = farg(0);
            else (void)farg(0);
            return 0;
        case 554:  // case 0x5f：0x42c240 Boss 登场标题（logo ANM 脚本 0）
            if (manager && manager->gui) manager->gui->show_boss_title();
            return 0;
        case 567:  // case 0x6c：符卡布局上下翻转位（Spellcard kFlipped）
            if (manager && manager->spellcard)
                manager->spellcard->flags ^= ((u32(iarg(0)) << 8) ^ manager->spellcard->flags) & Spellcard::kFlipped;
            else (void)iarg(0);
            return 0;
        case 524: {  // case 0x45：阶段编号（DAT_004a5798 / Gui+0xa8），奖励归属重置
            const i32 v = iarg(0);
            if (globals) globals->spell_id = v;
            score_bonus_spell = v;
            score_bonus = 0;
            return 0;
        }
        case 525: if (manager) manager->kill_all(0, nullptr); return 0;     // case 0x46：0x41d900
        case 551: {  // case 0x5c：0x41da30（仅 anm_layer == arg0 的敌机）
            const i32 layer = iarg(0);
            if (manager) manager->kill_all(1, &layer);
            return 0;
        }
        case 571: if (manager) manager->kill_all(2, nullptr); return 0;     // case 0x70：0x41db70（并清死亡子程序）
        case 527: {  // case 0x48：本 Boss 槽血条第 arg0 个刻度 = arg1 / 最大血量（Gui+0x1ec），arg2 原样存入
            const i32 flag = iarg(2);
            const float ratio = farg(1) / float(life_max);
            const i32 index = iarg(0);
            if (manager && manager->gui) manager->gui->set_boss_bar_marker(boss_slot, index, ratio, flag);
            return 0;
        }
        case 541: no_hit_timer.set(iarg(0)); return 0;  // case 0x52
        case 542: if (manager && manager->spellcard) manager->spellcard->flags |= Spellcard::kSurvival; return 0;
        case 543: {  // case 0x54
            if (manager && manager->spellcard) {
                manager->spellcard->flags |= Spellcard::kNoTimeout;
                if (manager->animations) manager->animations->kill(manager->spellcard->vm_ids[4]);
                manager->spellcard->vm_ids[4] = 0;
            }
            return 0;
        }
        case 546: {  // case 0x57：Bomb / 季节解放期间换用的主 VM 脚本
            flags = (flags & ~0x10000000u) | ((u32(iarg(0)) << 28) & 0x10000000u);
            flags &= ~0x20000001u;
            bomb_script = iarg(1);
            bomb_script_restore = main_script;
            return 0;
        }
        case 549: flags = (flags & 0x7fffffffu) | (u32(iarg(0)) << 31); return 0;  // case 0x5a
        case 565: damage_rate = farg(0); return 0;  // case 0x6a
        case 569: {  // case 0x6e：击破奖励登记（score_bonus 为 0 时登记，为 1（op 500 占位）时覆盖）
            if (score_bonus == 0) {
                const i32 v = iarg(0);
                score_bonus = v;
                if (v > 0 && globals) globals->enemy_bonus_pending += v;
            } else if (score_bonus == 1) {
                const i32 v = iarg(0);
                score_bonus = v;
                if (globals) globals->enemy_bonus_pending += v - 1;
            }
            return 0;
        }
        case 570: {  // case 0x6f：奖励结算到 enemy_bonus
            if (score_bonus != 0 && globals && score_bonus_spell == globals->spell_id) {
                globals->enemy_bonus += score_bonus;
                score_bonus = 0;
            }
            return 0;
        }
        case 572: {  // case 0x71
            life = iarg(0);
            life_budget = life * 7;
            return 0;
        }
        case 512: {
            const i32 slot = iarg(0);
            if (slot < 0) {
                if ((flags & 0x800000u) != 0 && manager && boss_slot >= 0 && boss_slot < 3)
                    manager->boss_ids[boss_slot] = 0;
                flags &= ~0x800000u;
                boss_slot = -1;
            } else if (slot < 3) {
                flags |= 0x800000u;
                boss_slot = slot;
                if (manager) manager->boss_ids[slot] = id;
            }
            return 0;
        }
        case 513: life_timer.set(0); return 0;
        case 515: invuln.set(iarg(0)); return 0;
        case 520: {  // 0x421708：三个 Boss 槽任一个 0x417580 非空就等待
            if (manager != nullptr && (manager->boss_slot(0) != nullptr || manager->boss_slot(1) != nullptr ||
                                       manager->boss_slot(2) != nullptr))
                return -1;
            return 0;
        }
        case 518: {  // case 0x3f：对话开始 0x429ff0 + 0x416f40 全屏消弹 + 0x431af0 激光全灭 + 落入 case 0x46 敌机全灭
            const i32 entry = iarg(0);
            if (manager && manager->gui) {
                if (!manager->gui->start_dialogue(entry)) { error = manager->gui->error; return -2; }
                if (manager->bullets) manager->bullets->cancel_all(0);
                if (manager->lasers) manager->lasers->cancel_all(0);
                manager->kill_all(0, nullptr);
            }
            return 0;
        }
        case 519:  // case 0x40：对话存在且 Msg+0x18c == 0 时等待（op 12 放行）
            return (manager && manager->gui && manager->gui->ecl_waits()) ? -1 : 0;
        // —— 运动指令 400–447（0x41dcb0 case 0x16–0x2c）：偶数码作用于 motion1、奇数码作用于 motion2 ——
        case 400: case 402: {  // case 0x16：直接设坐标
            MotionState& m = opcode == 402 ? motion2 : motion1;
            const float x = farg(0), y = farg(1);
            if (x > kSentinel) m.pos.x = x;
            if (y > kSentinel) m.pos.y = y;
            m.mode &= ~0xfu;
            pos.x = motion2.pos.x + motion1.pos.x;
            pos.y = motion2.pos.y + motion1.pos.y;
            pos.z = motion2.pos.z + motion1.pos.z;
            apply_motion_position();
            return 0;
        }
        case 416: case 417: {  // case 0x1f：坐标偏移
            MotionState& m = opcode == 417 ? motion2 : motion1;
            m.pos.x = m.pos.x + farg(0);
            m.pos.y = m.pos.y + farg(1);
            m.pos.z = m.pos.z + farg(2);
            apply_motion_position();
            return 0;
        }
        case 401: case 403: case 436: case 437: {  // case 0x17：位置插值（436/437 为相对量，镜像时 x 取反）
            const int slot = (opcode == 403 || opcode == 437) ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            EnemyPosInterp& it = pos_interp[slot];
            const i32 frames = iarg(0), mode = iarg(1);
            float x = farg(2), y = farg(3);
            if (frames <= 0) { it.end_time = 0; return 0; }
            if (opcode == 436 || opcode == 437) {
                x = (flags & kFlagMirror) != 0 ? m.pos.x - x : m.pos.x + x;
                y = m.pos.y + y;
            }
            it.end_time = frames;
            it.bezier1 = {};
            it.bezier2 = {};
            it.mode = mode;
            it.flags &= ~1u;
            it.initial = m.pos;
            it.goal = {x > kSentinel ? x : m.pos.x, y > kSentinel ? y : m.pos.y, 0.0f};
            it.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 434: case 435: case 438: case 439: {  // case 0x28：逐轴模式的位置插值（438/439 为相对量）
            const int slot = (opcode == 435 || opcode == 439) ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            EnemyPosInterp& it = pos_interp[slot];
            const i32 frames = iarg(0), mode_x = iarg(1), mode_y = iarg(2);
            float x = farg(3), y = farg(4);
            if (frames <= 0) { it.end_time = 0; return 0; }
            if (opcode == 438 || opcode == 439) {
                x = (flags & kFlagMirror) != 0 ? m.pos.x - x : m.pos.x + x;
                y = m.pos.y + y;
            }
            it.end_time = frames;
            it.bezier1 = {};
            it.bezier2 = {};
            it.flags |= 1u;
            it.axis_modes[0] = mode_x;
            it.axis_modes[1] = mode_y;
            it.initial = m.pos;
            it.goal = {x > kSentinel ? x : m.pos.x, y > kSentinel ? y : m.pos.y, 0.0f};
            it.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 425: case 426: {  // case 0x24：Hermite 位置插值（mode 8；f1/f2、f5/f6 为两端切线）
            const int slot = opcode == 426 ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            EnemyPosInterp& it = pos_interp[slot];
            it.flags &= ~1u;
            it.bezier1 = {farg(1), farg(2), 0.0f};
            it.end_time = iarg(0);
            it.bezier2 = {farg(5), farg(6), 0.0f};
            it.mode = 8;
            it.initial = m.pos;
            const float x = farg(3), y = farg(4);
            it.goal = {x > kSentinel ? x : m.pos.x, y > kSentinel ? y : m.pos.y, 0.0f};
            it.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 404: case 406: case 428: case 430: {  // case 0x18：设角度/速度（404/406 受镜像）
            MotionState& m = (opcode == 406 || opcode == 430) ? motion2 : motion1;
            float angle = farg(0);
            const float speed = farg(1);
            if (angle > kSentinel) {
                if ((flags & kFlagMirror) != 0 && (opcode == 404 || opcode == 406)) angle = mirror_angle(angle);
                m.angle = wrap_angle(angle);
            }
            if (speed > kSentinel) m.speed = speed;
            m.mode &= ~0xfu;
            return 0;
        }
        case 405: case 407: case 429: case 431: {  // case 0x19：角度/速度同时插值（mode 7 相对；两组插值器都重置）
            const int slot = (opcode == 407 || opcode == 431) ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            float angle = farg(2), speed = farg(3);
            const i32 frames = iarg(0);
            if (frames <= 0) { angle_interp[slot].end_time = 0; speed_interp[slot].end_time = 0; return 0; }
            const i32 mode = iarg(1);
            const bool mirrored = (flags & kFlagMirror) != 0 && (opcode == 405 || opcode == 407);
            if (mode == 7) {
                if (angle > kSentinel) { if (mirrored) angle = -angle; }
                else angle = 0.0f;
                if (!(speed > kSentinel)) speed = 0.0f;
            } else {
                if (angle > kSentinel) { if (mirrored) angle = mirror_angle(angle); }
                else angle = m.angle;
                if (!(speed > kSentinel)) speed = m.speed;
            }
            float start_a = m.angle;
            if (!(std::fabs(float(start_a - angle)) < kPi)) {
                if (angle > start_a) start_a = float(start_a + kTwoPi);
                else angle = float(angle + kTwoPi);
            }
            Interp<float>& ai = angle_interp[slot];
            ai.initial = start_a;
            ai.goal = angle;
            ai.end_time = frames;
            ai.mode = mode;
            ai.bezier1 = 0.0f;
            ai.bezier2 = 0.0f;
            ai.timer.reset();
            Interp<float>& si = speed_interp[slot];
            si.initial = m.speed;
            si.goal = speed;
            si.end_time = iarg(0);
            si.mode = mode;
            si.bezier1 = 0.0f;
            si.bezier2 = 0.0f;
            si.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 440: case 442: {  // case 0x29：设角度（镜像翻转，无哨兵判断）
            MotionState& m = opcode == 442 ? motion2 : motion1;
            float angle = farg(0);
            if ((flags & kFlagMirror) != 0) angle = mirror_angle(angle);
            m.angle = wrap_angle(angle);
            m.mode &= ~0xfu;
            return 0;
        }
        case 441: case 443: {  // case 0x2a：角度插值（目标相对当前角取最短弧）
            const int slot = opcode == 443 ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            Interp<float>& ai = angle_interp[slot];
            const i32 frames = iarg(0);
            if (frames <= 0) { ai.end_time = 0; return 0; }
            const i32 mode = iarg(1);
            float target = farg(2);
            const bool mirrored = (flags & kFlagMirror) != 0;
            if (mode == 7) {
                if (target > kSentinel) { if (mirrored) target = -target; }
                else target = 0.0f;
            } else {
                if (target > kSentinel) { if (mirrored) target = mirror_angle(target); }
                else target = m.angle;
            }
            const float cur = m.angle;
            float d;
            if (target - cur > kPi) d = target - float(cur + kTwoPi);
            else if (cur - target > kPi) d = target - float(cur - kTwoPi);
            else d = target - cur;
            ai.end_time = frames;
            ai.mode = mode;
            ai.bezier1 = 0.0f;
            ai.bezier2 = 0.0f;
            ai.initial = cur;
            ai.goal = d + cur;
            ai.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 444: case 446: {  // case 0x2b：设速度
            MotionState& m = opcode == 446 ? motion2 : motion1;
            const float speed = farg(0);
            if (speed > kSentinel) m.speed = speed;
            m.mode &= ~0xfu;
            return 0;
        }
        case 445: case 447: {  // case 0x2c：速度插值（mode 7 相对）
            const int slot = opcode == 447 ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            Interp<float>& si = speed_interp[slot];
            const i32 frames = iarg(0);
            if (frames <= 0) { si.end_time = 0; return 0; }
            const i32 mode = iarg(1);
            float speed = farg(2);
            if (!(speed > kSentinel)) speed = mode == 7 ? 0.0f : m.speed;
            si.initial = m.speed;
            si.goal = speed;
            si.end_time = frames;
            si.mode = mode;
            si.bezier1 = 0.0f;
            si.bezier2 = 0.0f;
            si.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 408: case 410: {  // case 0x1a：轨道模式（mode 2）参数
            MotionState& m = opcode == 410 ? motion2 : motion1;
            if ((m.mode & 0xf) != 2) m.velocity = m.pos;
            const float angle = farg(0), speed = farg(1), radius = farg(2), radius_speed = farg(3);
            if (angle > kSentinel) m.angle = wrap_angle(angle);
            if (speed > kSentinel) m.speed = speed;
            if (radius > kSentinel) m.radius = radius;
            if (radius_speed > kSentinel) m.radius_speed = radius_speed;
            m.mode = (m.mode & ~0xdu) | 2u;
            sync_motion_center(m, ctx.rate());
            apply_motion_position();
            return 0;
        }
        case 409: case 411: {  // case 0x1b：轨道模式速度/半径插值
            const int slot = opcode == 411 ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            const i32 frames = iarg(0);
            if (frames <= 0) { speed_interp[slot].end_time = 0; radius_interp[slot].end_time = 0; return 0; }
            const i32 mode = iarg(1);
            float speed = farg(2), radius = farg(3), radius_speed = farg(4);
            if (!(speed > kSentinel)) speed = m.speed;
            if (!(radius > kSentinel)) radius = m.radius;
            if (!(radius_speed > kSentinel)) radius_speed = m.radius_speed;
            Interp<float>& si = speed_interp[slot];
            si.initial = m.speed;
            si.goal = speed;
            si.end_time = frames;
            si.mode = mode;
            si.bezier1 = 0.0f;
            si.bezier2 = 0.0f;
            si.timer.reset();
            Interp<Vec2>& ri = radius_interp[slot];
            ri.end_time = frames;
            ri.bezier1 = {};
            ri.bezier2 = {};
            ri.mode = mode;
            ri.initial = {m.radius, m.radius_speed};
            ri.goal = {radius, radius_speed};
            ri.timer.reset();
            m.mode = (m.mode & ~0xdu) | 2u;
            sync_motion_center(m, ctx.rate());
            apply_motion_position();
            return 0;
        }
        case 418: case 419: {  // case 0x20：直接设速度向量
            MotionState& m = opcode == 419 ? motion2 : motion1;
            const float vx = farg(0), vy = farg(1);
            if (vx > kSentinel) m.velocity.x = vx;
            if (vy > kSentinel) m.velocity.y = vy;
            sync_motion_center(m, ctx.rate());
            apply_motion_position();
            return 0;
        }
        case 420: case 422: {  // case 0x21：椭圆模式（mode 3）参数
            MotionState& m = opcode == 422 ? motion2 : motion1;
            const float angle = farg(0), speed = farg(1), radius = farg(2), radius_speed = farg(3);
            const float angle2 = farg(4), ratio = farg(5);
            if ((m.mode & 0xf) != 2) m.velocity = m.pos;
            if (angle > kSentinel) m.angle = wrap_angle(angle);
            if (speed > kSentinel) m.speed = speed;
            if (radius > kSentinel) m.radius = radius;
            if (radius_speed > kSentinel) m.radius_speed = radius_speed;
            if (angle2 > kSentinel) m.angle2 = wrap_angle(wrap_angle(angle2));
            if (ratio > kSentinel) m.f2c = ratio;
            m.mode = (m.mode & ~0xcu) | 3u;
            sync_motion_center(m, ctx.rate());
            apply_motion_position();
            return 0;
        }
        case 421: case 423: {  // case 0x22：椭圆模式插值（速度 / 半径 / (angle2, 轴比)）
            const int slot = opcode == 423 ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            float speed = farg(2), radius = farg(3), radius_speed = farg(4), angle2 = farg(5), ratio = farg(6);
            if (!(speed > kSentinel)) speed = m.speed;
            if (!(radius_speed > kSentinel)) radius_speed = m.radius_speed;
            if (!(radius > kSentinel)) radius = m.radius;
            if (!(ratio > kSentinel)) ratio = m.f2c;
            if (!(angle2 > kSentinel)) angle2 = m.angle2;
            const i32 frames = iarg(0), mode = iarg(1);
            if (frames <= 0) {
                speed_interp[slot].end_time = 0;
                radius_interp[slot].end_time = 0;
                ellipse_interp[slot].end_time = 0;
                return 0;
            }
            Interp<float>& si = speed_interp[slot];
            si.initial = m.speed;
            si.goal = speed;
            si.mode = mode;
            si.end_time = frames;
            si.bezier1 = 0.0f;
            si.bezier2 = 0.0f;
            si.timer.reset();
            Interp<Vec2>& ri = radius_interp[slot];
            ri.end_time = frames;
            ri.bezier1 = {};
            ri.bezier2 = {};
            ri.mode = mode;
            ri.initial = {m.radius, m.radius_speed};
            ri.goal = {radius, radius_speed};
            ri.timer.reset();
            Interp<Vec2>& ei = ellipse_interp[slot];
            ei.end_time = frames;
            ei.bezier1 = {};
            ei.bezier2 = {};
            ei.mode = mode;
            ei.initial = {m.angle2, m.f2c};
            ei.goal = {angle2, ratio};
            ei.timer.reset();
            m.velocity = m.pos;
            m.mode = (m.mode & ~0xcu) | 3u;
            sync_motion_center(m, ctx.rate());
            apply_motion_position();
            return 0;
        }
        case 424:  // case 0x23：镜像位（flags bit 19）
            flags = flags ^ (((u32(iarg(0)) << 19) ^ flags) & kFlagMirror);
            return 0;
        case 427: {  // case 0x25：motion2 并入 motion1 并清空全部插值
            motion1.pos.x = motion2.pos.x + motion1.pos.x;
            motion1.pos.y = motion2.pos.y + motion1.pos.y;
            motion1.pos.z = motion2.pos.z + motion1.pos.z;
            motion2.pos = {};
            motion2.speed = 0.0f;
            motion1.speed = 0.0f;
            motion1.velocity = {};
            motion2.velocity = {};
            motion1.mode &= ~0xfu;
            motion2.mode &= ~0xfu;
            for (int slot = 0; slot < 2; ++slot) {
                pos_interp[slot].end_time = 0;
                angle_interp[slot].end_time = 0;
                speed_interp[slot].end_time = 0;
                radius_interp[slot].end_time = 0;
                ellipse_interp[slot].end_time = 0;
            }
            return 0;
        }
        case 414: case 415: {  // case 0x1d / 0x1e：取 Boss 0 的坐标
            if (Enemy* b = boss(0)) (opcode == 415 ? motion2 : motion1).pos = b->pos;
            return 0;
        }
        case 432: case 433: {  // case 0x26 / 0x27：取指定 id 敌机的坐标
            if (manager) {
                if (Enemy* e = manager->find_id(u32(iarg(0)))) (opcode == 433 ? motion2 : motion1).pos = e->pos;
            }
            return 0;
        }
        case 412: case 413: {  // case 0x1c：随机漫游（413 作用于 motion2；方向依场地四分区与自机位置）
            if (!rng) return fail("412: no rng");
            const int slot = opcode == 413 ? 1 : 0;
            MotionState& m = slot ? motion2 : motion1;
            float angle = 0;
            const float x = pos.x, qw = bound_size.x * 0.25f;
            if (bound_center.x - qw > x) angle = float(float(rng->signed_unit() * kPi) / 3.0f);
            else if (x > bound_center.x + qw) angle = wrap_angle(float(float(float(rng->signed_unit() * kPi) / 3.0f) + kPi));
            else {
                angle = float(float(rng->signed_unit() * kPi) * 0.25f);
                const u16 w = rng->next16();
                const float px = player ? player->pos.x : 0.0f;
                if (px > x ? (w % 3 == 0) : (w % 3 != 0)) angle = float(angle + kPi);
            }
            const float qh = bound_size.y * 0.25f, y = pos.y;
            if (bound_center.y - qh > y) angle = std::fabs(angle);
            else if (y > bound_center.y + qh) angle = -std::fabs(angle);
            if (std::fabs(float(angle + kHalfPi)) < 0.05000000074505806f)
                angle = angle < -kHalfPi ? -1.6207963228225708f : -1.5207964181900024f;
            else if (double(std::fabs(float(angle - kHalfPi))) < 0.05)
                angle = angle < kHalfPi ? 1.5207964181900024f : 1.6207963228225708f;
            const i32 duration = iarg(0), mode = iarg(1);
            const float speed = farg(2);
            Interp<float>& ai = angle_interp[slot];
            ai.initial = angle;
            ai.goal = angle;
            ai.mode = mode;
            ai.end_time = duration;
            ai.bezier1 = 0.0f;
            ai.bezier2 = 0.0f;
            ai.timer.reset();
            // 原版把 mode 再次写进角度插值器（+0x2c），速度插值器的 mode 保持上次值
            Interp<float>& si = speed_interp[slot];
            si.initial = speed;
            si.goal = 0.0f;
            si.end_time = duration;
            si.bezier1 = 0.0f;
            si.bezier2 = 0.0f;
            si.timer.reset();
            m.mode &= ~0xfu;
            return 0;
        }
        case 1000: {
            const i32 d = iarg(0);
            season_drop_duration = d;
            season_drop_timer.set(d);
            season_drop_max = iarg(1);
            season_drop_min = iarg(2);
            return 0;
        }
        case 1001:  // case 0xac：按累计伤害放季节道具
            season_item_step = iarg(0);
            season_item_next = damage_accum;
            return 0;
        // —— 800..802（0x41dcb0 case 0xa8..0xaa）：跨敌机控制 ——
        case 800: {  // 0x422966：0x41a9c0 按 id 找敌机 → 0x41a4f0 释放子线程、0x41a5a0 复位主上下文、0x474890 绑定子程序（下帧起跑）
            Enemy* target = manager ? manager->find_id(u32(iarg(0))) : nullptr;
            if (target == nullptr || !ctx.instruction) return 0;
            const auto* bytes = reinterpret_cast<const u8*>(ctx.instruction);
            std::string name;
            for (u32 k = 0x18; k < ctx.instruction->length && bytes[k] != 0; ++k) name.push_back(char(bytes[k]));
            target->async_threads.clear();
            target->reset_context(target->primary, target->program ? target->program->find_index(name.c_str()) : -1);
            return 0;
        }
        case 801: {  // 0x4228a2：指定敌机坐标原样拷入两个整型目标槽（与 324 不同：找不到不回退自身）
            const Enemy* target = manager ? manager->find_id(u32(iarg(2))) : nullptr;
            if (target == nullptr) return 0;
            i32* dx = ctx.integer_destination(0);
            if (!dx) return -2;
            std::memcpy(dx, &target->pos.x, 4);
            i32* dy = ctx.integer_destination(1);
            if (!dy) return -2;
            std::memcpy(dy, &target->pos.y, 4);
            return 0;
        }
        case 802: {  // 0x4228da：其他 Boss 槽（0x417580，非自身槽）的中断表项 n 若有效，则切到其 sub（不立即执行）
            const i32 n = iarg(0);
            if (n < 0 || n >= kInterruptSlots) return 0;
            for (int slot = 0; slot < 3; ++slot) {
                if (slot == boss_slot) continue;
                Enemy* other = boss(slot);
                if (other == nullptr || other->interrupts[n].life_threshold < 0) continue;
                other->async_threads.clear();
                other->reset_context(other->primary,
                                     other->program ? other->program->find_index(other->interrupts[n].sub.c_str()) : -1);
            }
            return 0;
        }
        // —— ANM 绑定 302..340（0x41dcb0 case 1..0x15 / 0x423260 / 0x4233a0）——
        case 303: {  // 0x423260
            const i32 slot = iarg(0);
            const i32 script = iarg(1);
            if (slot < 0 || slot >= kVmSlots) return 0;
            if (manager && manager->animations) manager->animations->kill(vm_ids[slot]);
            vm_ids[slot] = 0;
            if (script < 0) return 0;
            if (slot == 0) { main_script = script; anm_slot_main = anm_slot; }
            rebind_vm(slot, anm_slot, script, true);
            return 0;
        }
        case 306: {  // case 3：主体精灵 + 朝向脚本族
            const i32 slot = iarg(0);
            const i32 script = iarg(1);
            if (slot < 0 || slot >= kVmSlots) return 0;
            rebind_vm(slot, anm_slot, script, true);
            if (slot == 0) {
                flags |= 0x100000u;
                main_script_base = main_script = script;
                sprite_direction = 0;
                anm_slot_main = anm_slot;
            }
            return 0;
        }
        case 307: {  // case 4：以敌机坐标为 entity_offset 的独立 VM（链表头，creation_flags 2）
            const i32 anm = iarg(0), script = iarg(1);
            AnmFile* file = anm_file(anm);
            if (file == nullptr || !manager->animations) return 0;
            const u32 vid = manager->animations->spawn_vm(*file, script, -1, pos, 0.0f, 2, true);
            if (effects) effects->register_id(vid);
            return 0;
        }
        case 308: {  // case 5
            const i32 anm = iarg(0), script = iarg(1);
            AnmFile* file = anm_file(anm);
            if (file == nullptr || !manager->animations) return 0;
            const u32 vid = manager->animations->spawn_front(*file, script, -1);
            if (effects) effects->register_id(vid);
            return 0;
        }
        case 313: {  // case 7：朝向族 +5 脚本
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kVmSlots) return 0;
            rebind_vm(slot, anm_slot, main_script_base + 5, true);
            return 0;
        }
        case 314: {  // case 8：0x40e5c0 特效 VM（entity_offset = pos）
            const i32 script = iarg(1), anm = iarg(0);
            AnmFile* file = anm_file(anm);
            if (file == nullptr || !manager->animations) return 0;
            const u32 vid = manager->animations->spawn_effect(*file, script, -1, pos, 0.0f);
            if (effects) effects->register_id(vid);
            return 0;
        }
        case 315: {  // case 9：同 307 但带 rotation.z
            const i32 anm = iarg(0), script = iarg(1);
            const float rot = farg(2);
            AnmFile* file = anm_file(anm);
            if (file == nullptr || !manager->animations) return 0;
            const u32 vid = manager->animations->spawn_vm(*file, script, -1, pos, 0.0f, 2, true);
            if (AnmVm* vm = manager->animations->find(vid)) {
                vm->entity_offset = pos;
                vm->flags_lo |= AnmVm::kRotationDirty;
                vm->rotation.z = rot;
            }
            if (effects) effects->register_id(vid);
            return 0;
        }
        case 316: {  // case 0xa：朝向族 +5+n（n < 0 → 基号）
            const i32 slot = iarg(0), n = iarg(1);
            if (slot < 0 || slot >= kVmSlots) return 0;
            rebind_vm(slot, anm_slot, n < 0 ? main_script_base : main_script_base + 5 + n, true);
            return 0;
        }
        case 317: {  // case 0xb：0x46f0b0 中断
            const i32 slot = iarg(0), value = iarg(1);
            if (slot < 0 || slot >= kVmSlots || !manager->animations) return 0;
            manager->animations->interrupt(vm_ids[slot], value);
            return 0;
        }
        case 318: {  // case 0xc：回到基号脚本并关闭朝向切换
            rebind_vm(0, anm_slot, main_script_base, false);
            flags &= ~0x100000u;
            main_script = main_script_base;
            sprite_direction = 0;
            anm_slot_main = anm_slot;
            return 0;
        }
        case 319: case 325: case 326: case 327: case 328: case 329: case 330: case 331: case 332: case 333:
        case 335: case 336: case 337: {  // 0x4233a0
            const i32 slot = iarg(0);
            if (u32(slot) >= 0x10u) return 0;
            AnmVm* vm = vm_at(slot);
            if (vm == nullptr) return 0;
            switch (opcode) {
                case 319: vm->flags_lo |= AnmVm::kRotationDirty; vm->rotation.z = farg(1); break;
                case 325: {
                    const i32 b = iarg(3), g = iarg(2), r = iarg(1);
                    vm->color[2] = u8(r); vm->color[1] = u8(g); vm->color[0] = u8(b);
                    break;
                }
                case 326: {
                    const u8 r = u8(iarg(3)), g = u8(iarg(4)), b = u8(iarg(5));
                    const i32 mode = iarg(2), time = iarg(1);
                    anm_begin_color_interp(vm->color_interp, time, mode, vm->color, b, g, r);
                    break;
                }
                case 327: vm->color[3] = u8(iarg(1)); break;
                case 328: {
                    const i32 goal = iarg(3), mode = iarg(2), time = iarg(1);
                    anm_begin_scalar_interp(vm->alpha_interp, time, mode, vm->color[3], goal & 0xff);
                    break;
                }
                case 329: {
                    const float y = farg(2), x = farg(1);
                    vm->flags_lo |= AnmVm::kScaleDirty;
                    vm->scale = {x, y};
                    break;
                }
                case 330: {
                    const float y = farg(4), x = farg(3);
                    const i32 mode = iarg(2), time = iarg(1);
                    anm_begin_vec2_interp(vm->scale_interp, time, mode, vm->scale, {x, y});
                    break;
                }
                case 331: vm->color2[3] = u8(iarg(1)); break;
                case 332: {
                    const i32 goal = iarg(3), mode = iarg(2), time = iarg(1);
                    anm_begin_scalar_interp(vm->alpha2_interp, time, mode, vm->color2[3], goal & 0xff);
                    vm->flags_lo = (vm->flags_lo & ~0x40000u) | 0x20000u;
                    break;
                }
                case 333: {
                    const float y = farg(4), x = farg(3);
                    (void)iarg(0);
                    const i32 mode = iarg(2), time = iarg(1);
                    anm_begin_vec3_interp(vm->pos_interp, time, mode, vm->entity_offset, {x, y, 0.0f});
                    break;
                }
                case 335: {
                    const float y = farg(2), x = farg(1);
                    vm->flags_lo |= AnmVm::kScaleDirty;
                    vm->scale2 = {x, y};
                    break;
                }
                case 336: anm_set_layer(*vm, iarg(1)); break;
                case 337: {
                    const u32 blend = u32(iarg(1)) & 0xffu;
                    vm->flags_lo ^= ((blend << 5) ^ vm->flags_lo) & 0x1e0u;
                    break;
                }
                default: break;
            }
            return 0;
        }
        case 320: {  // case 0xe
            const i32 slot = iarg(0);
            const float x = farg(1), y = farg(2);
            if (slot < 0 || slot >= kVmSyncSlots) return 0;
            vm_offsets[slot] = {x, y, 0.0f};
            return 0;
        }
        case 322: {  // case 0xf
            const i32 parent = iarg(1), slot = iarg(0);
            if (slot < 0 || slot >= kVmSyncSlots) return 0;
            vm_parent_slot[slot] = parent;
            return 0;
        }
        case 323: death_anm_slot = iarg(0); death_script = iarg(1); return 0;  // case 0x10
        case 324: {  // case 0x11：取指定敌机坐标（找不到 → 自身）
            const Enemy* target = manager ? manager->find_id(u32(iarg(2))) : nullptr;
            float* dx = ctx.float_destination(0);
            if (!dx) return -2;
            *dx = target ? target->pos.x : pos.x;
            float* dy = ctx.float_destination(1);
            if (!dy) return -2;
            *dy = target ? target->pos.y : pos.y;
            return 0;
        }
        case 334: if (effects) effects->spawn(iarg(0), pos); return 0;  // case 0x12：0x40e730
        case 338: {  // case 0x13：偏移 + rotation.z 的独立 VM
            const float dy = farg(3), dx = farg(2);
            const Vec3 offset{dx + pos.x, pos.y + dy, pos.z + 0.0f};
            const float rot = farg(4);
            const i32 script = iarg(1), anm = iarg(0);
            AnmFile* file = anm_file(anm);
            if (file == nullptr || !manager->animations) return 0;
            const u32 vid = manager->animations->spawn_vm(*file, script, -1, offset, rot, 2, true);
            if (effects) effects->register_id(vid);
            return 0;
        }
        case 339: {  // case 0x14：生成后再预跑 arg2 帧
            const i32 anm = iarg(0), script = iarg(1);
            AnmFile* file = anm_file(anm);
            if (file == nullptr || !manager->animations) return 0;
            const u32 vid = manager->animations->spawn_front(*file, script, -1);
            if (effects) effects->register_id(vid);
            AnmVm* vm = manager->animations->find(vid);
            for (i32 i = 0; i < iarg(2); ++i) {
                if (vm) manager->animations->run(*vm);
            }
            return 0;
        }
        case 340: {  // case 0x15
            if (Enemy* target = manager ? manager->find_id(u32(iarg(0))) : nullptr) target->flags |= 0x2000000u;
            return 0;
        }
        // —— 弹幕发射器 600..641（0x42085b..0x4226d0）——
        case 600: {  // 0x42085b
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].reset();
            shooter_offset[slot].x = 0;
            shooter_offset[slot].y = 0;
            shooter_pos2[slot] = Vec3{};
            shooter_ex_counter[slot] = 0;
            return 0;
        }
        case 601: {  // 0x4209cf
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots || !bullets) return 0;
            EtShooter& s = shooters[slot];
            s.pos = shooter_origin(slot);
            bullets->safe_radius_sq = safe_radius_sq;
            bullets->shoot(s);
            bullets->safe_radius_sq = 0;
            return 0;
        }
        case 602: {  // 0x420ab6
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].type = i16(iarg(1));
            shooters[slot].color = i16(iarg(2));
            return 0;
        }
        case 603: {  // 0x420af0
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooter_offset[slot].x = farg(1);
            shooter_offset[slot].y = farg(2);
            return 0;
        }
        case 604: {  // 0x420c34
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].angle1 = farg(1);
            shooters[slot].angle2 = farg(2);
            return 0;
        }
        case 605: {  // 0x420c72
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].speed1 = farg(1);
            shooters[slot].speed2 = farg(2);
            return 0;
        }
        case 606: {  // 0x420cb0
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].way = i16(iarg(1));
            shooters[slot].stack = i16(iarg(2));
            return 0;
        }
        case 607: {  // 0x421152
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].aim_mode = i16(iarg(1));
            return 0;
        }
        case 608: {  // 0x42117f
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].shoot_sfx = iarg(1);
            shooters[slot].step_sfx = iarg(2);
            return 0;
        }
        case 609: case 610: case 611: case 612: {  // 0x4211b9：写 Ex 步骤（609/610 指定下标，611/612 用递增计数）
            i32 slot = iarg(0);
            if (slot >= 18 || slot < 0) slot = 0;
            if (slot >= kShooterSlots) return 0;
            i32 idx;
            u32 next;
            if (opcode == 609 || opcode == 610) { idx = iarg(1); next = 2; }
            else { idx = shooter_ex_counter[slot]; next = 1; }
            if (idx < 0 || idx >= 18) return 0;
            BulletExStep& step = shooters[slot].ex[idx];
            const bool wide = (opcode == 610 || opcode == 612);
            step.async = iarg(next);
            step.type = u32(iarg(next + 1));
            step.a = iarg(next + 2);
            step.b = iarg(next + 3);
            next += 4;
            if (wide) { step.c = iarg(next); step.d = iarg(next + 1); next += 2; }
            step.r = farg(next);
            step.s = farg(next + 1);
            next += 2;
            if (wide) { step.m = farg(next); step.n = farg(next + 1); }
            if (opcode == 609 || opcode == 610) shooter_ex_counter[slot] = idx + 1;
            else shooter_ex_counter[slot] = shooter_ex_counter[slot] + 1;
            return 0;
        }
        case 613:  // 0x421655：全屏消弹（0x416f40 的 fastcall 参数为残留的操作码 613，仅计数）+ 0x431af0(1) 道具吸引
            if (bullets) bullets->cancel_all(613);
            if (manager && manager->lasers) manager->lasers->cancel_all(1);  // 0x431af0(1)
            return 0;
        case 614: {  // 0x420925：复制发射器（含偏移与绝对坐标，不含 Ex 计数）
            const i32 dst = iarg(0), src = iarg(1);
            if (dst < 0 || dst >= kShooterSlots || src < 0 || src >= kShooterSlots) return 0;
            shooters[dst] = shooters[src];
            shooter_offset[dst] = shooter_offset[src];
            shooter_pos2[dst] = shooter_pos2[src];
            return 0;
        }
        case 615: case 616: case 635: case 636: {  // 0x42219e / 0x42221d / 0x422253 / 0x422289：圆形消弹 + 道具吸引
            const float radius = farg(0);
            const i32 item_mode = (opcode == 615 || opcode == 635) ? 1 : 0;
            if (bullets) bullets->cancel_radius(pos, radius, item_mode, opcode >= 635);
            if (manager && manager->lasers) manager->lasers->cancel_radius(pos, radius, item_mode, true);  // 0x431a70
            return 0;
        }
        case 617: {  // 0x420ded：按秩三档选速度
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 rank = globals ? globals->rank : 0;
            u32 base = 3;
            if (rank >= 0x200) base = 5;
            else if (rank < -0x200) base = 1;
            shooters[slot].speed1 = farg(base);
            shooters[slot].speed2 = farg(base + 1);
            return 0;
        }
        case 618: {  // 0x420e51：按秩五档选速度
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 rank = globals ? globals->rank : 0;
            u32 base = 1;
            if (rank >= 600) base = 9;
            else if (rank >= 200) base = 7;
            else if (rank >= -200) base = 5;
            else if (rank >= -600) base = 3;
            shooters[slot].speed1 = farg(base);
            shooters[slot].speed2 = farg(base + 1);
            return 0;
        }
        case 619: {  // 0x420ed4：按秩线性插值速度（rank+1024 / 2048）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const float f1 = farg(1), f2 = farg(2), f3 = farg(3), f4 = farg(4);
            const float t = float(globals ? globals->rank : 0) + 1024.0f;
            shooters[slot].speed1 = ((f3 - f1) * t) * 0.00048828125f + f1;
            shooters[slot].speed2 = ((f4 - f2) * t) * 0.00048828125f + f2;
            return 0;
        }
        case 620: {  // 0x420fa7：按秩三档选 way/stack
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 rank = globals ? globals->rank : 0;
            u32 base = 3;
            if (rank >= 0x200) base = 5;
            else if (rank < -0x200) base = 1;
            shooters[slot].way = i16(iarg(base));
            shooters[slot].stack = i16(iarg(base + 1));
            return 0;
        }
        case 621: {  // 0x421002：按秩五档选 way/stack
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 rank = globals ? globals->rank : 0;
            u32 base = 1;
            if (rank >= 600) base = 9;
            else if (rank >= 200) base = 7;
            else if (rank >= -200) base = 5;
            else if (rank >= -600) base = 3;
            shooters[slot].way = i16(iarg(base));
            shooters[slot].stack = i16(iarg(base + 1));
            return 0;
        }
        case 622: {  // 0x42107c：按秩线性插值 way/stack（有符号 /2048 向零截断）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 a1 = iarg(1), a2 = iarg(2), a3 = iarg(3), a4 = iarg(4);
            const i32 t = (globals ? globals->rank : 0) + 0x400;
            auto lerp = [t](i32 lo, i32 hi) -> i16 {
                const i32 p = t * (hi - lo);
                return i16(((p + ((p >> 31) & 0x7ff)) >> 11) + lo);
            };
            shooters[slot].way = lerp(a1, a3);
            shooters[slot].stack = lerp(a2, a4);
            return 0;
        }
        case 623: {  // 0x4225d1：(x,y) → 自机方向角
            const float px = player ? player->pos.x : 0.0f;
            const float py = player ? player->pos.y : 0.0f;
            const float dx = px - farg(1);
            const float dy = py - farg(2);
            const float a = float(std::atan2(double(dy), double(dx)));
            float* dst = ctx.float_destination(0);
            if (!dst) return -2;
            *dst = a;
            return 0;
        }
        case 624: {  // 0x420cec：按难度选速度
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 d = globals ? globals->difficulty : 0;
            const u32 base = (d == 0) ? 1 : (d == 1) ? 2 : (d == 2) ? 3 : 4;
            shooters[slot].speed1 = farg(base);
            shooters[slot].speed2 = farg(base + 4);
            return 0;
        }
        case 625: {  // 0x420d64：按难度选 way/stack
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const i32 d = globals ? globals->difficulty : 0;
            const u32 base = (d == 0) ? 1 : (d == 1) ? 2 : (d == 2) ? 3 : 4;
            shooters[slot].way = i16(iarg(base));
            shooters[slot].stack = i16(iarg(base + 4));
            return 0;
        }
        case 626: {  // 0x420b25：极坐标偏移
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const float a = farg(1), len = farg(2);
            shooter_offset[slot].x = float(std::cos(double(a)) * double(len));
            shooter_offset[slot].y = float(std::sin(double(a)) * double(len));
            return 0;
        }
        case 627: {  // 0x420b95
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            shooters[slot].distance = farg(1);
            return 0;
        }
        case 628: {  // 0x420bc3：绝对发射坐标（x < -990 关闭）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            const float x = farg(1), y = farg(2);
            shooter_pos2[slot] = {x, y, (x < -990.0f) ? 0.0f : 1.0f};
            return 0;
        }
        case 640: {  // 0x4213e9：Ex 步骤的子程序名（Ex 0x1000000 用；字符串位于指令 +0x1c）
            const i32 slot = iarg(0), idx = iarg(1);
            if (slot < 0 || slot >= kShooterSlots || idx < 0 || idx >= 18 || !ctx.instruction) return 0;
            shooters[slot].ex[idx].str = reinterpret_cast<const char*>(ctx.instruction) + 0x1c;
            return 0;
        }
        case 641: {  // 0x4213c2：Ex 计数 -1（下限 0）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            if (--shooter_ex_counter[slot] < 0) shooter_ex_counter[slot] = 0;
            return 0;
        }
        case 526: {  // 0x41ff5d
            const float r = farg(0);
            safe_radius_sq = r * r;
            return 0;
        }
        case 632: case 639: {  // case 0x8f / 0x96：登记每帧钩子（639 另置 +0x451c = 1）
            const i32 index = iarg(0);
            frame_hook = (index >= 1 && index <= 2) ? FrameHook(index) : FrameHook::None;
            frame_hook_flag = opcode == 639 ? 1 : 0;
            return 0;
        }
        case 637: {  // case 0x94：立即调用一次钩子（不登记）
            const i32 index = iarg(0);
            if (index >= 1 && index <= 2) run_frame_hook(FrameHook(index));
            return 0;
        }
        case 630:  // case 0x8d：背景 STD 脚本跳到标签（0x40c040）
            if (manager && manager->stage) manager->stage->jump_to_label(iarg(0));
            return 0;
        case 642:  // case 0x8e：EnemyManager+0x88 bit0 = 参数（隐藏 Boss 血条 / 计时）
            if (manager) manager->hud_hidden = (u32(iarg(0)) & 1u) != 0;
            return 0;
        case 901: case 902:
            // DebugSkipFunc / DebugSkipStopFunc 标记：0x422d70 表把 901 / 902 映到默认分支，原版即空操作
            return 0;
        case 638: {  // case 0x95：0x43e080 加分（含 extend）后在敌机位置弹出白色得分数字
            const i32 value = iarg(0);
            if (manager) {
                manager->globals->add_score(value, [this] {
                    if (manager->gui != nullptr) manager->gui->celebrate_extend();
                });
                if (manager->score_popups != nullptr) manager->score_popups->push(pos, value, 0xffffffffu);
            }
            return 0;
        }
        case 629: {  // case 0x8c：Boss 畸变光环（半径 > 0 才建 17 × 17 网格；颜色为圈内顶点色）
            distortion.reset();
            distortion_radius_target = farg(0);
            distortion_radius = 16.0f;
            distortion_color = u32(iarg(1));
            distortion_angle_a = wrap_angle(0.0f);
            distortion_angle_b = wrap_angle(0.0f);
            if (0.0f < distortion_radius_target && manager != nullptr && manager->animations != nullptr) {
                AnmFile* text = manager->animations->file(0);   // DAT_004c17f8 text.anm
                if (text != nullptr) {
                    const bool ready = manager->render_targets_ready != nullptr && *manager->render_targets_ready;
                    distortion = std::make_unique<DistortionGrid>(*manager->animations, *text, ready, 0x11);
                }
            }
            return 0;
        }
        case 631: case 633: case 634:
            // 631 EnemyManager 标志、633/634 伤害 / 判定回调表（全部 ECL 未使用）
            return 0;
        // —— 激光 700..714（0x41dcb0 case 0x99..0xa7）：复用发射器槽的弹种 / 颜色 / 角度 / 速度 / Ex，
        //    长度与时序参数放在发射器 +0x340 参数区与 pos 字段 ——
        case 700: {  // case 0x99：激光长度参数（初始长 / 目标长 / 最大行程 / 宽度）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            EtShooter& s = shooters[slot];
            s.pos = {farg(1), farg(2), farg(3)};
            ShooterLaserParams p = laser_params(s);
            p.width = farg(4);
            store_laser_params(s, p);
            return 0;
        }
        case 701: {  // case 0x9a：照射激光时序（预警 / 展开 / 维持 / 收束）与标志；曲线激光借 telegraph 作采样点数
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots) return 0;
            ShooterLaserParams p = laser_params(shooters[slot]);
            p.telegraph = iarg(1);
            p.expand = iarg(2);
            p.active = iarg(3);
            p.shrink = iarg(4);
            p.flags = u32(iarg(5));
            store_laser_params(shooters[slot], p);
            return 0;
        }
        case 702: {  // case 0x9b：发射直线激光（0x431130）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots || !manager || !manager->lasers) return 0;
            const EtShooter& s = shooters[slot];
            const ShooterLaserParams p = laser_params(s);
            LineLaserArgs args;
            std::memcpy(args.ex, s.ex, sizeof args.ex);
            args.origin = shooter_origin(slot);
            args.type = s.type;
            args.color = s.color;
            args.angle = wrap_angle(s.angle1);
            args.flags = p.flags | 0x1u;
            args.speed = s.speed1;
            args.initial_length = s.pos.x;
            args.target_length = s.pos.y;
            args.max_extent = s.pos.z;
            args.width = p.width;
            args.spawn_sfx = s.shoot_sfx;
            args.step_sfx = s.step_sfx;
            args.start_offset = s.distance;
            args.start_ex = 0;
            manager->lasers->spawn_line(args);
            return 0;
        }
        case 703: {  // case 0x9c：发射照射激光（0x431860），第 2 参数为 ECL 侧句柄
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots || !manager || !manager->lasers) return 0;
            const EtShooter& s = shooters[slot];
            const ShooterLaserParams p = laser_params(s);
            InfiniteLaserArgs args;
            std::memcpy(args.ex, s.ex, sizeof args.ex);
            args.origin = shooter_origin(slot);
            args.type = s.type;
            args.color = s.color;
            args.angle = wrap_angle(s.angle1);
            args.telegraph_time = p.telegraph;
            args.expand_time = p.expand;
            args.active_time = p.active;
            args.shrink_time = p.shrink;
            args.speed = s.speed1;
            args.initial_length = s.pos.x;
            args.target_length = s.pos.y;
            args.width = p.width;
            args.flags = p.flags | 0x2u;
            args.spawn_sfx = s.shoot_sfx;
            args.step_sfx = s.step_sfx;
            args.start_offset = s.distance;
            args.handle = iarg(1);
            manager->lasers->spawn_infinite(args);
            return 0;
        }
        case 704: {  // case 0x9d：按句柄改激光起点
            if (!manager || !manager->lasers) return 0;
            if (Laser* l = manager->lasers->find_by_id(iarg(0))) l->pos = {farg(1), farg(2), 0.0f};
            return 0;
        }
        case 705: {  // case 0x9e：写参数包 +0x0c 起 3 个 float——照射激光为起点平移速度
            if (!manager || !manager->lasers) return 0;
            Laser* l = manager->lasers->find_by_id(iarg(0));
            if (l == nullptr) return 0;
            switch (l->kind) {
                case LaserKind::Infinite: l->infinite.velocity = {farg(1), farg(2), 0.0f}; break;
                // 其它子类同偏移落在无关字段上（直线：角度 / 目标长 / 初始长；曲线：角度 / 宽 / 速）；本作 ECL 只对照射激光使用
                case LaserKind::Line:
                    l->line.angle = farg(1); l->line.target_length = farg(2); l->line.initial_length = 0.0f; break;
                case LaserKind::Curve:
                    l->curve.angle = farg(1); l->curve.width = farg(2); l->curve.speed = 0.0f; break;
                case LaserKind::Beam: break;
            }
            return 0;
        }
        case 706: {  // case 0x9f：按句柄改速度
            if (!manager || !manager->lasers) return 0;
            if (Laser* l = manager->lasers->find_by_id(iarg(0))) l->speed = farg(1);
            return 0;
        }
        case 707: {  // case 0xa0：按句柄改宽度
            if (!manager || !manager->lasers) return 0;
            if (Laser* l = manager->lasers->find_by_id(iarg(0))) l->width = farg(1);
            return 0;
        }
        case 708: {  // case 0xa1：按句柄改朝向角
            if (!manager || !manager->lasers) return 0;
            if (Laser* l = manager->lasers->find_by_id(iarg(0))) l->angle = farg(1);
            return 0;
        }
        case 709: {  // case 0xa2：写参数包 +0x1c——照射激光为角速度（直线激光同偏移为宽度）
            if (!manager || !manager->lasers) return 0;
            Laser* l = manager->lasers->find_by_id(iarg(0));
            if (l == nullptr) return 0;
            if (l->kind == LaserKind::Infinite) l->infinite.angular_vel = farg(1);
            else if (l->kind == LaserKind::Line) l->line.width = farg(1);
            return 0;
        }
        case 710:  // case 0xa3：按句柄终止全部同句柄激光（不转道具）并清句柄
            if (manager && manager->lasers) manager->lasers->cancel_handle(iarg(0));
            return 0;
        case 711: {  // case 0xa4：发射曲线激光（0x431900）；采样点数借用 701 的预警参数
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots || !manager || !manager->lasers) return 0;
            const EtShooter& s = shooters[slot];
            const ShooterLaserParams p = laser_params(s);
            CurveLaserArgs args;
            std::memcpy(args.ex, s.ex, sizeof args.ex);
            args.origin = shooter_origin(slot);
            args.type = s.type;
            args.color = s.color;
            args.angle = wrap_angle(s.angle1);
            args.node_count = p.telegraph;
            args.flags |= 0x1u;
            args.spawn_sfx = s.shoot_sfx;
            args.step_sfx = s.step_sfx;
            args.speed = s.speed1;
            args.width = p.width;
            args.start_offset = s.distance;
            manager->lasers->spawn_curve(args);
            return 0;
        }
        case 713: {  // case 0xa6：发射占位激光（0x4318c0；本作 ECL 未使用）
            const i32 slot = iarg(0);
            if (slot < 0 || slot >= kShooterSlots || !manager || !manager->lasers) return 0;
            const EtShooter& s = shooters[slot];
            const ShooterLaserParams p = laser_params(s);
            BeamLaserArgs args;
            std::memcpy(args.ex, s.ex, sizeof args.ex);
            args.origin = shooter_origin(slot);
            args.color = s.color;
            args.handle = iarg(1);
            args.angle = wrap_angle(s.angle1);
            args.node_count = p.telegraph;
            args.width = p.width;
            args.length = s.pos.z;
            args.start_offset = s.distance;
            manager->lasers->spawn_beam(args);
            return 0;
        }
        case 714: {  // case 0xa7：0x431310 写 +0x60c bit0（占位激光参数包 flags；对非占位激光落在 Ex 区，本作未使用）
            if (!manager || !manager->lasers) return 0;
            Laser* l = manager->lasers->find_by_id(iarg(0));
            if (l != nullptr && l->kind == LaserKind::Beam) l->beam.flags = (l->beam.flags & ~0x1u) | (u32(iarg(1)) & 0x1u);
            return 0;
        }
        case 535: {
            i32 vals[4]{iarg(1), iarg(2), iarg(3), iarg(4)};
            i32* dst = ctx.integer_destination(0);
            if (!dst) return -2;
            const i32 d = (globals && globals->difficulty >= 0 && globals->difficulty < 3) ? globals->difficulty : 3;
            *dst = vals[d];
            return 0;
        }
        case 536: {
            float vals[4]{farg(1), farg(2), farg(3), farg(4)};
            float* dst = ctx.float_destination(0);
            if (!dst) return -2;
            const i32 d = (globals && globals->difficulty >= 0 && globals->difficulty < 3) ? globals->difficulty : 3;
            *dst = vals[d];
            return 0;
        }
        case 548: {
            i32 delays[4]{iarg(0), iarg(1), iarg(2), iarg(3)};
            const i32 d = (globals && globals->difficulty >= 0 && globals->difficulty < 3) ? globals->difficulty : 3;
            ctx.time = float(ctx.time - float(delays[d]));
            return 0;
        }
        case 522: case 528: case 537: case 538: case 539: {
            // case 0x43：解密指令偏移 32 处的符卡名（长度在 28；密钥 0x77 起、步长 7、步长每字节 +0x10），
            // 537/538/539 的编号加难度偏移 d / d-1 / d-2；0x417f00 开始符卡后置 7 倍减伤位并重算血量池，
            // life_timer 归零（switchD_0041dd51_caseD_3a）
            char spell_name[0x41]{};
            if (ctx.instruction) {
                const auto* bytes = reinterpret_cast<const u8*>(ctx.instruction);
                i32 length = 0;
                if (ctx.instruction->length >= 32) std::memcpy(&length, bytes + 28, 4);
                u8 key = 0x77, step = 7;
                for (i32 i = 0; i < length && 32 + i < i32(ctx.instruction->length); ++i) {
                    const u8 c = u8(bytes[32 + i] ^ key);
                    key = u8(key + step);
                    step = u8(step + 0x10);
                    if (i < i32(sizeof(spell_name)) - 1) spell_name[i] = char(c);
                }
            }
            i32 spell_id = iarg(0);
            const i32 difficulty = globals ? globals->difficulty : 0;
            if (opcode == 537) spell_id += difficulty;
            else if (opcode == 538) spell_id += difficulty - 1;
            else if (opcode == 539) spell_id += difficulty - 2;
            const i32 anm_index = iarg(2);
            const i32 limit = iarg(1);
            if (manager && manager->spellcard) manager->spellcard->start(spell_id, spell_name, limit, anm_index);
            life_flags |= 1u;
            life_budget = life * 7;
            life_timer.set(0);
            return 0;
        }
        case 523:  // case 0x44：0x4182f0 结束符卡，清 7 倍减伤位
            if (manager && manager->spellcard) manager->spellcard->end();
            life_flags &= ~1u;
            return 0;
        case 568:
            life_flags = (life_flags & ~1u) | (u32(iarg(0)) & 1u);
            return 0;
        case 550: anm_layer = iarg(0); return 0;         // case 0x5b
        case 552: anm_layer_offset = iarg(0); return 0;  // case 0x5d
        case 561: {  // case 0x66：死亡音效 + 死亡特效（朝向 = 最后受击点 → 敌机）
            if (death_sfx >= 0 && manager) manager->play_sfx(death_sfx, pos.x);
            if (death_script < 0) return 0;
            const float ddx = hit_pos.x - pos.x, ddy = hit_pos.y - pos.y;
            float angle = -1.5707964f;
            if (!(0.04f > ddx * ddx + ddy * ddy)) angle = atan2_f(pos.y - hit_pos.y, pos.x - hit_pos.x);
            AnmFile* file = anm_file(death_anm_slot);
            if (file && manager->animations) manager->animations->spawn_effect(*file, death_script, 3, pos, angle);
            return 0;
        }
        case 545:  // case 0x56：0x4319e0 全部激光清出界豁免与免疫后带道具消弹
            if (manager && manager->lasers) manager->lasers->cancel_all_ignoring_immunity();
            return 0;
        default:
            // 未识别操作码：先空操作以免整条 ECL 链因后续关卡指令中断；oracle 对拍时再逐个补全
            if (manager) {
                auto& seen = manager->unknown_ops;
                if (std::find(seen.begin(), seen.end(), i32(opcode)) == seen.end()) seen.push_back(i32(opcode));
            }
            return 0;
    }
}

bool EnemyManager::create(const char* ecl_name, GameGlobals& g, Player* pl, Rng& r,
                          AnmManager& anm, u8 difficulty) {
    globals = &g;
    player = pl;
    rng = &r;
    animations = &anm;
    enemies.clear();
    next_id = 1;
    last_id = 0;
    enemy_count = 0;
    timer.set(0);
    program = EclProgram{};
    if (!program.load(ecl_name, *this)) { error = program.error; return false; }
    (void)difficulty;
    return true;
}

void EnemyManager::reset() {
    // 0x4185b0
    if (animations != nullptr) {
        for (int slot = 10; slot < 16; ++slot) animations->kill_file(slot);
    }
    // 每只敌机经 0x41ba10 → 0x41ade0 析构：摘链并让存活计数（+0x18c）-1
    for (auto& e : enemies) {
        if (!e) continue;
        e->destroy();
        if (enemy_count > 0) --enemy_count;
    }
    enemies.clear();
    for (u32& id : boss_ids) id = 0;
}

void EnemyManager::destroy() {
    // 0x41b1a0
    reset();
    program = EclProgram{};
    if (animations != nullptr) {
        for (int slot = 10; slot < 16; ++slot) animations->unload(slot);
    }
}

bool EnemyManager::spawn_main() {
    EnemySpawnArgs args;
    args.pos = {};
    args.life = 0;
    return spawn_enemy("main", args) != nullptr;
}

Enemy* EnemyManager::spawn_enemy(const char* sub, const EnemySpawnArgs& args) {
    auto e = std::make_unique<Enemy>();
    const u8 diff = globals ? u8(1u << (globals->difficulty & 31)) : 0xff;
    if (!e->init(*this, program, sub, args, *globals, player, *rng, diff)) {
        error = e->error;
        return nullptr;
    }
    Enemy* raw = e.get();
    enemies.push_back(std::move(e));
    ++enemy_count;
    return raw;
}

Enemy* EnemyManager::find_id(u32 id) noexcept {
    for (auto& e : enemies) if (e && e->id == id) return e.get();
    return nullptr;
}

Enemy* EnemyManager::find_id_or_last(u32 id) noexcept {
    if (id == 0) return nullptr;
    Enemy* last = nullptr;
    for (auto& e : enemies) {
        if (!e) continue;
        last = e.get();
        if (last->id == id) return last;
    }
    return last;
}

u32 EnemyManager::nearest_targetable_id(const Vec3& pos, float range) const noexcept {
    float best_d2 = range * range;
    u32 best_id = 0;
    for (const auto& e : enemies) {
        if (!e || (e->flags & 0xc000021u) != 0) continue;
        const float dy = pos.y - e->pos.y;
        const float dx = pos.x - e->pos.x;
        const float d2 = dy * dy + dx * dx;
        if (d2 < best_d2) { best_d2 = d2; best_id = e->id; }
    }
    return best_id;
}

void EnemyManager::update(float game_speed) {
    // 0x41b3d0：速度在每只敌机开始更新时才从全局量 DAT_004a5788 取一次——同一帧里排在前面的敌机经 ECL 547 改了
    // 速度，后面的敌机本帧就按新速度推进；一只敌机自己的这一轮更新（含 ECL 时钟与插值）全程用入口时的值
    const auto speed_at_entry = [&]() -> float { return global_game_speed != nullptr ? *global_game_speed : game_speed; };
    // 0x41b3d0：更新前先保存 next。列表末尾的敌机在本回合里（含其死亡子程序）生成的新敌机排在它之后，
    // 本帧不再被访问，于是带着 0x40000 进入下一帧并在那里被整帧跳过；即使末尾敌机随即死亡也必须沿用保存的 next，
    // 不能取 erase 返回的后继。kill_all 标过 0x2000000 的直接移出
    damage_dealt_bomb = 0;
    damage_dealt = 0;
    for (auto it = enemies.begin(); it != enemies.end();) {
        auto next = std::next(it);
        auto& e = *it;
        if (!e) { it = enemies.erase(it); continue; }
        if ((e->flags & 0x2000000u) != 0 || !e->update(speed_at_entry())) {
            e->destroy();
            enemies.erase(it);
            it = next;
            if (enemy_count > 0) --enemy_count;
            continue;
        }
        e->flags &= ~kFlagReentry;
        it = next;
    }
    // 自机伤害倍率（0x2c7cc）每帧复位为 1；> 1.01 时置 Player flags & 0x20
    if (player != nullptr) {
        if (player->scale2 <= 1.01f) player->flags &= ~0x20u;
        else player->flags |= 0x20u;
        player->scale2 = 1.0f;
    }
    timer.tick(global_game_speed != nullptr ? *global_game_speed : game_speed);
}

// 0x41d900（mode 0）/ 0x41da30（mode 1：仅 anm_slot == *filter）/ 0x41db70（mode 2：并清死亡子程序）
void EnemyManager::kill_all(int mode, const i32* filter) {
    for (auto& e : enemies) {
        if (!e) continue;
        if ((e->flags & 0xc004a0u) != 0 && (e->flags & 0x100u) == 0) continue;
        if (mode == 1 && (filter == nullptr || e->anm_layer != *filter)) continue;
        e->drop_item_type = 0;
        std::memset(e->drop_counts, 0, sizeof e->drop_counts);
        e->season_drop_max = 0;
        e->drop_radius = {32.0f, 32.0f};
        e->hit_pos = {0.0f, 192.0f, e->hit_pos.z};
        e->score_bonus = 0;
        if (mode == 2) e->death_sub.clear();
        e->die(1.0f);
        e->flags |= 0x2000000u;
    }
    timer.tick(1.0f);
}

}  // namespace th16
