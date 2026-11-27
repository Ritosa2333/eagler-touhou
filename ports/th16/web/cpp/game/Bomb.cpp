// th16 炸弹 / 季节解放实现。结构与原版 vtable 一一对应：Bomb 基类 = BombInf 的公共段（0x40d600 / 0x40dda0 /
// 0x40db20 / 0x40dd00 / 0x40de30 / 0x40e040），四个主炸弹与五个解放各自覆写 on_start / on_update / on_cancel。
//
// 共同约定（原版各类反复出现的片段）：
//   - 每帧先 find(primary_vm)，找不到就把 id 清零并走结束路径（主炸弹：中断外层 VM、返回 -1；解放：0x40e040）。
//   - 伤害源经 Player 的池分配（0x444b20 矩形 / 0x4449b0 圆），flags |= 4 标记炸弹伤害（敌机受击减伤走 0x41c330）。
//   - 消弹的 item_mode：1 = 得点（阴阳玉引爆）、4 = 季节（解放）、5 = 炸弹（每 3 颗一个道具）。
//   - 浮点严格按原版：角度用 polar()（FSINCOS 单次舍入），乘 π 用 x87 双精度再舍回单精度。
#include "Bomb.hpp"

#include "AnmManager.hpp"
#include "Ascii.hpp"
#include "Bullet.hpp"
#include "Effect.hpp"
#include "Enemy.hpp"
#include "GameGlobals.hpp"
#include "Gui.hpp"
#include "Laser.hpp"
#include "Player.hpp"
#include "Spellcard.hpp"

#include <cmath>

namespace th16 {

namespace {

constexpr float kPi = 3.1415927f;
constexpr float kHalfPi = 1.5707964f;
constexpr float kQuarterPi = 0.7853982f;
constexpr float kPiOver64 = 0.049087387f;   // 阴阳玉半径的每帧增量（MotionState radius_speed）
constexpr float kPiOver30 = 0.10471976f;    // 阴阳玉绕自机的每帧角增量

// 原版 `rand * 3.1415927` 在 x87 以双精度相乘后存回 float
float mul_pi(float r) { return float(double(r) * double(kPi)); }

// 0x40db20 / 0x40dda0 内联：季节量表当前等级（阈值表 [1..6] 中不大于量表的个数）
i32 season_level(const GameGlobals& globals) { return globals.season_option_count(); }

// 伤害源 flags bit2：炸弹伤害（EnemyManager 0x41c330 据此减伤；Player::apply_damage_sources 回报 bomb_hit）
void mark_as_bomb_damage(Player& player, i32 source_index) {
    if (DamageSource* source = player.damage_source(source_index)) source->flags |= 4u;
}

}  // namespace

// =============================================================================================
// Bomb 公共段

// 0x40d600 第二段（链已由 BombManager / GameSession 挂好）
void Bomb::init(Role bomb_role) {
    timer.reset();
    role = bomb_role;
    popup_timer.reset();
    shown_bonus = -1.0f;
    release_bonus = 0.0f;
}

// 0x40dda0
bool Bomb::usable() const {
    if (role == Role::Main) {
        if (ctx.globals->bombs < 1) return false;
    } else {
        if (season_level(*ctx.globals) < 1) return false;
        if (timer.current < 0) return false;   // 解放结束后的 45 帧冷却（0x40e040）
    }
    return ctx.main != nullptr && ctx.main->active != 1 &&
           ctx.release != nullptr && ctx.release->active != 1 &&
           ctx.world != nullptr && !ctx.world->dialogue &&
           ctx.enemies != nullptr && ctx.enemies->enemy_count != 0;
}

// 0x40db20
void Bomb::start() {
    if (active != 0) return;
    active = 1;
    timer.reset();
    GameGlobals& globals = *ctx.globals;
    Player& player = *ctx.player;
    const bool is_main = role == Role::Main;

    if (is_main) {
        globals.bombs -= 1;
        if (globals.bombs < 0) globals.bombs = 0;
        else if (globals.bombs > 8) globals.bombs = 8;
        if (ctx.gui != nullptr) ctx.gui->refresh_bombs_display(globals.bombs, globals.bomb_fragments);   // 0x42c390
    }
    // 符卡进行 60 帧以后才发动的炸弹记为「符卡中用过」（取得失败），60 帧内发动不算
    const Spellcard& spell = *ctx.spellcard;
    started_in_spell = (is_main && (spell.flags & Spellcard::kActive) != 0 && spell.timer.current > 0x3b) ? 1 : 0;
    play_sfx(is_main ? 0x2c : 0x4d, player.pos.x);
    on_start();

    if (is_main) {
        ctx.enemies->counters[2] = 0;
        return;
    }

    // 解放：上一轮的奖励飘字若还有 > 60 帧，截到 60 帧并定格显示值与位置
    if (popup_timer.current > 0x3c) {
        popup_timer.set(0x3c);
        shown_bonus = release_bonus;
        popup_pos = player.pos;
        popup_pos.y -= 32.0f;
    }
    release_level = season_level(globals);
    release_bonus = 0.0f;
    // 夏 / 土用只扣掉当前等级那一档的量表，其它季节清空量表
    if (globals.season == 1 || globals.season == 4) {
        i32 remaining = globals.season_gauge - globals.season_step[release_level];
        if (remaining < 0) remaining = 0;
        globals.season_gauge = remaining;
    } else {
        globals.season_gauge = 0;
        if (globals.season_gauge_max < 0) globals.season_gauge = globals.season_gauge_max;
    }
    player.setup_options(*ctx.animations, globals);   // 0x4440e0：子机数随量表变化
    if (ctx.gui != nullptr) ctx.gui->refresh_season_gauge();   // 0x42c600
}

// 0x40dd00（更新链 0x18 / 0x19）
void Bomb::update(float game_speed) {
    game_speed_ = game_speed;
    if (timer.current < 0) {
        timer.speed_slot = 0;
        timer.tick(game_speed);
    }
    if (active == 0) return;
    if (on_update() != 0) {
        active = 0;
        return;
    }
    timer.tick(game_speed);
}

// 0x40da70 → vtable[2]（恒返回 1）→ 0x40de30：解放奖励飘字。
// 解放结束把 popup_timer 设为 0xb4，这里在绘制链上倒数：前 120 帧什么都不画，最后 60 帧（< 0x3d）在自机上方 32 处
// 画 "+奖励"；奖励数值与位置在第一次进入显示段时冻结（shown_bonus < 0 为哨兵），之后自机再动飘字也不跟。
// 颜色按解放等级查表（等级 0 灰 … 6 白）；文字用场地组 / 中号字 / 水平居中 / 底对齐，画完把样式恢复默认。
void Bomb::draw_popup(AsciiManager& text, float game_speed) {
    static constexpr u32 kPopupColorByLevel[7] = {
        0x60606060, 0xa0b0b080, 0xb0b8b880, 0xc0c0c080, 0xd0d0d080, 0xe0e0e080, 0xffffff30,
    };
    if (popup_timer.current < 1) {
        shown_bonus = -1.0f;
        return;
    }
    if (popup_timer.current < 0x3d) {
        if (shown_bonus < 0.0f) {
            shown_bonus = release_bonus;
            popup_pos = ctx.player->pos;
            popup_pos.y -= 32.0f;
        }
        text.style.color = kPopupColorByLevel[popup_level];
        text.style.group = AsciiManager::Group::Field;
        text.style.font = AsciiManager::Font::Medium;
        text.style.h_align = AsciiManager::HAlign::Center;
        text.style.v_align = AsciiManager::VAlign::Bottom;
        // 原版 asm（0x40df08 的 0x66666667 魔数除 10 再 ×10）把奖励截到十位——Ghidra 反编译文本漏掉了这一步
        text.printf(popup_pos, "+%d", (i32(shown_bonus) / 10) * 10);
        text.style.color = 0xffffffff;
        text.style.font = AsciiManager::Font::Default;
        text.style.group = AsciiManager::Group::Ui;
        text.style.h_align = AsciiManager::HAlign::Left;
        text.style.v_align = AsciiManager::VAlign::Top;
    }
    popup_timer.speed_slot = 0;
    popup_timer.untick(game_speed);
}

// 0x40e040
void Bomb::finish_release() {
    shown_bonus = -1.0f;
    popup_level = release_level;
    popup_timer.set(0xb4);
    timer.set(-45);   // 冷却：0x40dd00 在计时为负时每帧推进，回到 0 才可再次发动
}

// 四个主炸弹 on_start 的共有段：符卡开始 60 帧内发动 → 记 kBombUsed；之后发动 → 奖励清零、取得与炸弹标志都清
void Bomb::mark_spellcard_bomb_use() {
    Spellcard& spell = *ctx.spellcard;
    if ((spell.flags & Spellcard::kActive) == 0) return;
    if (spell.timer.current < 0x3c) {
        if (ctx.main == nullptr || ctx.main->active != 1) return;
        spell.flags |= Spellcard::kBombUsed;
    } else {
        spell.bonus = 0;
        spell.flags &= ~(Spellcard::kBonusAvailable | Spellcard::kBombUsed);
    }
}

void Bomb::grant_invincibility(i32 frames) { ctx.player->invincible.set(frames); }

void Bomb::request_linear_shake(i32 duration, i32 amp_from, i32 amp_to) {
    ctx.shake_requests.push_back({BombShakeRequest::kLinear, duration, amp_from, amp_to, 0});
}

void Bomb::request_envelope_shake(i32 amplitude, i32 attack, i32 sustain, i32 decay) {
    ctx.shake_requests.push_back({BombShakeRequest::kEnvelope, amplitude, attack, sustain, decay});
}

// 0x40e730(3, pos)，随后把爆点 VM 的混合模式位（flags_lo bit6..8）改为 0x20（加算）
void Bomb::spawn_burst_particle(const Vec3& at) {
    if (ctx.effects == nullptr) return;
    const u32 handle = ctx.effects->spawn(3, at);
    if (handle == 0) return;
    if (AnmVm* vm = ctx.animations->find(ctx.effects->spawned_vm_id(handle))) {
        vm->flags_lo = (vm->flags_lo & ~0x1c0u) | 0x20u;
    }
}

AnmVm* Bomb::find_vm(u32& id) const {
    AnmVm* vm = ctx.animations->find(id);
    if (vm == nullptr) id = 0;
    return vm;
}

// =============================================================================================
// 灵梦：八枚阴阳玉

// 0x410d10
void ReimuBomb::on_start() {
    Player& player = *ctx.player;
    pos = player.pos;
    play_sfx(0x31);
    mark_spellcard_bomb_use();
    ctx.enemies->counters[1] += 1;
    orbs.assign(8, Orb{});
    secondary_vm = ctx.animations->spawn_effect(*player.anm, 0x17, -1, pos);
}

// 0x410de0
i32 ReimuBomb::on_update() {
    AnmManager& anim = *ctx.animations;
    Player& player = *ctx.player;
    grant_invincibility(0x28);
    if (AnmVm* aura = anim.find(secondary_vm)) aura->entity_offset = player.pos;

    // 第 120 帧起逐枚检查 VM 是否消亡；八枚全部消亡即结束（从第一枚仍存活处停止检查）
    if (timer.current > 0x77) {
        i32 vanished = 0;
        for (Orb& orb : orbs) {
            if (anim.find(orb.vm) != nullptr) break;
            orb.vm = 0;
            if (++vanished > 7) {
                anim.interrupt(secondary_vm, 1);
                orbs.clear();
                return -1;
            }
        }
    }

    if (timer.current == 200) {
        release_orbs();
        anim.interrupt(secondary_vm, 1);
        request_linear_shake(8, 6, 6);
        return 0;
    }

    if (timer.current != timer.previous && timer.current == 0) {
        float angle_step = 0.0f;
        for (i32 i = 0; i < 8; ++i) {
            Orb& orb = orbs[i];
            init_orb(orb, i);
            orb.motion.mode = (orb.motion.mode & ~0xdu) | 2u;   // 绕圆心（velocity 字段）旋转
            orb.motion.radius = 0.0f;
            orb.motion.angle = wrap_angle(angle_step);
            orb.motion.radius_speed = kPiOver64;
            if (DamageSource* source = player.damage_source(orb.damage_source)) source->f7c = 300;   // 累计伤害上限
            angle_step = wrap_angle(angle_step + kQuarterPi);
        }
    }

    for (Orb& orb : orbs) {
        if (orb.active == 0) continue;
        step_orb(orb);
        DamageSource* source = player.damage_source(orb.damage_source);
        if (source != nullptr && source->f78 < 300) {
            source->motion.pos = orb.motion.pos;
        } else {
            // 累计伤害打满（300）即引爆
            detonate_orb(orb);
            play_sfx(0x1b, orb.motion.pos.x);
            request_linear_shake(8, 6, 6);
        }
    }
    on_cancel();
    return 0;
}

// 0x4109d0
void ReimuBomb::init_orb(Orb& orb, i32 index) {
    Player& player = *ctx.player;
    orb.motion.velocity = player.pos;   // mode 2 的圆心
    orb.vm = ctx.animations->spawn_effect(*player.anm, 0xf, -1, orb.motion.pos);
    orb.active = 1;
    orb.timer.reset();
    orb.index = index;
    orb.damage_source = player.alloc_circle_damage_source(orb.motion.pos, 56.0f, 0.0f, 9999, 0xf);
    if (DamageSource* source = player.damage_source(orb.damage_source)) {
        source->flags |= 4u;
        source->f80 = 3;   // 命中间隔
    }
}

// 0x410550：三个阶段——绕自机扩散（< 90 帧）→ 继续绕转到各自的出发时刻 (index + 9) × 10 → 直飞索敌
void ReimuBomb::step_orb(Orb& orb) {
    Player& player = *ctx.player;
    MotionState& m = orb.motion;
    if (orb.timer.current != orb.timer.previous) {
        if (orb.timer.current < 0x5a) {
            m.velocity = player.pos;
            m.radius += 1.5f;
            m.angle = wrap_angle(m.angle + kPiOver30);
        } else {
            const i32 launch_frame = (orb.index + 9) * 10;
            if (orb.timer.current < launch_frame) {
                m.velocity = player.pos;
                m.angle = wrap_angle(m.angle + kPiOver30);
            } else if (orb.timer.current == launch_frame) {
                // 以上一帧的位移为初速度切换成直飞
                m.mode &= ~0xfu;
                m.angle = wrap_angle(atan2_f(orb.delta.y, orb.delta.x));
                m.speed = float(std::sqrt(double(orb.delta.x * orb.delta.x + orb.delta.y * orb.delta.y)));
            } else {
                steer_orb(orb);
            }
        }
    }
    const Vec3 previous = m.pos;
    m.update_velocity(game_speed_);
    m.integrate(game_speed_);
    if (AnmVm* vm = ctx.animations->find(orb.vm)) vm->entity_offset = m.pos;
    orb.delta = {m.pos.x - previous.x, m.pos.y - previous.y, m.pos.z - previous.z};
    orb.timer.speed_slot = 0;
    orb.timer.tick(game_speed_);
}

// 0x410550 直飞段：512 以内有可锁定敌机则转向它（正对时加速到 8，偏离时减速到 1）；无目标且出场则每帧减速 10%
void ReimuBomb::steer_orb(Orb& orb) {
    MotionState& m = orb.motion;
    EnemyManager* enemies = ctx.enemies;
    if (enemies != nullptr) orb.target_id = enemies->nearest_targetable_id(m.pos, 512.0f);
    if (orb.target_id != 0) {
        Enemy* enemy = enemies->find_id_or_last(orb.target_id);
        if (enemy == nullptr || (enemy->flags & 0xc000021u) != 0) return;
        const float to_enemy = atan2_f(enemy->pos.y - m.pos.y, enemy->pos.x - m.pos.x);
        float diff = to_enemy - m.angle;
        if (diff <= kPi) {
            if (kPi < m.angle - to_enemy) diff = to_enemy - (m.angle - 6.2831855f);
        } else {
            diff = to_enemy - (m.angle + 6.2831855f);
        }
        float new_speed = m.speed;
        if (std::fabs(diff) < kQuarterPi) {
            if (std::fabs(diff) < 0.2617994f) new_speed = (m.speed + 0.2f <= 8.0f) ? m.speed + 0.2f : 8.0f;
        } else {
            new_speed = (1.0f <= m.speed - 0.7f) ? m.speed - 0.7f : 1.0f;
        }
        m.angle = wrap_angle(wrap_angle(m.angle + diff * 0.1f));
        m.speed = new_speed;
        return;
    }
    const bool on_field = -160.0f <= m.pos.x && m.pos.x <= 160.0f && 32.0f <= m.pos.y && m.pos.y <= 416.0f;
    if (!on_field) m.speed *= 0.9f;
}

// 0x410ae0：引爆——半径 128 消弹转得点、64 起每帧 +8 的圆形伤害源（11 帧、伤害 100）、VM 中断 1、伤害源失活
void ReimuBomb::detonate_orb(Orb& orb) {
    Player& player = *ctx.player;
    if (orb.active != 0) {
        play_sfx(0x1b, orb.motion.pos.x);
        ctx.bullets->cancel_radius(orb.motion.pos, 128.0f, 1, true);
        ctx.lasers->cancel_radius(orb.motion.pos, 128.0f, 1, true);
        mark_as_bomb_damage(player, player.alloc_circle_damage_source(orb.motion.pos, 64.0f, 8.0f, 0xb, 100));
    }
    ctx.animations->interrupt(orb.vm, 1);
    orb.active = 0;
    if (DamageSource* source = player.damage_source(orb.damage_source)) source->flags &= ~1u;
    orb.damage_source = 0;
}

// 0x410bb0：第 200 帧 / 过关时把尚未引爆的玉全部引爆（原版另有一条「只删 VM 不引爆」的分支，由无人写入的
// +0xd4 字段选择，恒不成立，此处省略）
void ReimuBomb::release_orbs() {
    for (Orb& orb : orbs) detonate_orb(orb);
}

// 0x411280：每帧轮到的一枚（计时 % 8 == 下标）在半径 64 内做炸弹消弹
void ReimuBomb::on_cancel() {
    for (i32 i = 0; i < i32(orbs.size()); ++i) {
        Orb& orb = orbs[i];
        if (orb.active == 0 || timer.current % 8 != i) continue;
        ctx.bullets->cancel_radius(orb.motion.pos, 64.0f, 5, true);
        ctx.lasers->cancel_radius(orb.motion.pos, 64.0f, 5, true);
    }
}

// 0x411320（0x42e150 过关时炸弹仍在发动）
void ReimuBomb::on_stage_end() {
    release_orbs();
    ctx.animations->interrupt(secondary_vm, 1);
    orbs.clear();
    request_linear_shake(8, 6, 6);
    active = 0;
}

// =============================================================================================
// 琪露诺：以自机为心的扩散冰圆

// 0x40f070
void CirnoBomb::on_start() {
    Player& player = *ctx.player;
    pos = player.pos;
    angle = -kHalfPi;
    play_sfx(0x1e);
    primary_vm = ctx.animations->spawn_effect(*player.anm, 10, -1, pos);
    secondary_vm = ctx.animations->spawn_effect(*player.anm, 0xd, -1, pos);
    mark_spellcard_bomb_use();
    grant_invincibility(0x78);
    ctx.enemies->counters[1] += 1;
    request_envelope_shake(0, 8, 300, 0x1e);
}

// 0x40f240
i32 CirnoBomb::on_update() {
    Player& player = *ctx.player;
    AnmVm* ice = find_vm(primary_vm);
    grant_invincibility(0x28);
    if (ice == nullptr) {
        ctx.animations->interrupt(secondary_vm, 1);
        return -1;
    }
    // 两段扩散圆伤害源：前 60 帧 16 → 176，之后 290 帧 176 → 208
    if (timer.current == 0) player.alloc_circle_damage_source(pos, 16.0f, 2.6666667f, 0x3c, 0xf);
    else if (timer.current == 0x3c) player.alloc_circle_damage_source(pos, 176.0f, 0.11034483f, 0x122, 0xf);

    Rng& rng = ctx.animations->visual_rng;
    if (timer.current < 0xfa) {
        if (timer.current > 0x1d) {
            // 冰圆内随机一点的爆点（半径取 VM 当前缩放）
            const float radius = ice->scale.x;
            const float a = mul_pi(rng.signed_unit());
            const float d = rng.unit() * radius;
            const Vec3 offset = polar(a, d);
            spawn_burst_particle({offset.x + pos.x, pos.y + offset.y, pos.z + 0.0f});
        }
    } else {
        Vec3 particle;
        particle.x = rng.signed_unit() * 192.0f;
        particle.y = rng.unit() * 448.0f;
        particle.z = 0.0f;
        spawn_burst_particle(particle);
    }
    on_cancel();
    return 0;
}

// 0x40f4c0：消弹半径随两段伤害源同步增长
void CirnoBomb::on_cancel() {
    const float radius = timer.current < 0x3d ? timer.current_f * 160.0f / 60.0f + 16.0f
                                              : (timer.current_f - 60.0f) * 32.0f / 290.0f + 176.0f;
    ctx.bullets->cancel_radius(pos, radius, 5, true);
    ctx.lasers->cancel_radius(pos, radius, 5, true);
}

// =============================================================================================
// 文：横扫全屏的一阵风

// 0x40e780
void AyaBomb::on_start() {
    Player& player = *ctx.player;
    const Vec3 player_pos = player.pos;
    pos = player_pos;
    pos.y = 224.0f;
    // 按发动瞬间的横向速度决定风墙的倾角与横移方向
    angle = player.velocity.x * 0.0078125f * 0.017453292f * 0.5f - kHalfPi;
    sweep_speed = player.velocity.x * 0.0078125f * 0.05f;
    play_sfx(0x1e);
    primary_vm = ctx.animations->spawn_effect(*player.anm, 0xe, -1, pos);
    if (AnmVm* wall = ctx.animations->find(primary_vm)) {
        wall->flags_lo |= 4u;
        wall->rotation.z = angle;
    }
    secondary_vm = ctx.animations->spawn_effect(*player.anm, 0x13, -1, player_pos);
    mark_spellcard_bomb_use();
    grant_invincibility(0x78);
    ctx.enemies->counters[1] += 1;
    request_envelope_shake(0, 8, 300, 0x1e);
}

// 0x40e9d0
i32 AyaBomb::on_update() {
    Player& player = *ctx.player;
    AnmVm* wall = find_vm(primary_vm);
    grant_invincibility(0x28);
    if (wall == nullptr) {
        ctx.animations->interrupt(secondary_vm, 1);
        return -1;
    }
    if (timer.current == 300) ctx.animations->interrupt(primary_vm, 1);
    wall->entity_offset.x += sweep_speed;
    pos.x += sweep_speed;
    player.alloc_damage_source(pos, angle, 1, 0x12, 640.0f, 118.0f);
    // 全场随机位置的爆点粒子（visual_rng：先 x 后 y）
    Rng& rng = ctx.animations->visual_rng;
    Vec3 particle;
    particle.x = rng.signed_unit() * 192.0f;
    particle.y = rng.unit() * 448.0f;
    particle.z = 0.0f;
    spawn_burst_particle(particle);
    on_cancel();
    return 0;
}

// 0x40ec10
void AyaBomb::on_cancel() {
    ctx.bullets->cancel_rect(pos, Vec2{640.0f, 118.0f}, angle, 5);
    ctx.lasers->cancel_rect(pos, Vec3{640.0f, 118.0f, 0.0f}, angle);
}

// =============================================================================================
// 魔理沙：随输入摆动的巨型光束

// 0x40f930
void MarisaBomb::on_start() {
    Player& player = *ctx.player;
    pos = player.pos;
    angle = -kHalfPi;
    play_sfx(0x31);
    primary_vm = ctx.animations->spawn_effect(*player.anm, 0x11, -1, pos);
    mark_spellcard_bomb_use();
    grant_invincibility(0x78);
    ctx.enemies->counters[1] += 1;
    request_envelope_shake(3, 0x3c, 0xf0, 0x1e);
    secondary_vm = ctx.animations->spawn_effect(*player.anm, 0x19, -1, pos);
    player.flags |= Player::kFlagNoShot;
}

// 0x40fb00
i32 MarisaBomb::on_update() {
    Player& player = *ctx.player;
    AnmVm* beam = find_vm(primary_vm);
    grant_invincibility(0x28);
    if (beam == nullptr) {
        ctx.animations->interrupt(secondary_vm, 1);
        return -1;
    }
    if (timer.current > 300) return 0;
    if (timer.current == 300) {
        ctx.animations->interrupt(primary_vm, 1);
        ctx.animations->interrupt(secondary_vm, 1);
        player.flags &= ~Player::kFlagNoShot;
        player.speed_mult = 1.0f;
    }
    beam->flags_lo |= 4u;
    beam->rotation.z = angle;
    // 左右输入让光束每帧偏转 0.15°；发动期间自机移动倍率 0.2
    if (player.velocity.x > 0.0f) angle += 0.0026179939f;
    else if (player.velocity.x < 0.0f) angle -= 0.0026179939f;
    player.speed_mult = 0.2f;
    pos = player.pos;

    // 每 3 帧沿光束方向放三段矩形伤害源（近 → 远：208 / 240 / 304 处，越远越宽）
    if (timer.current != timer.previous && timer.current % 3 == 0) {
        struct Segment { float distance; i32 lifetime; i32 damage; float width; float height; };
        static constexpr Segment kSegments[] = {
            {208.0f, 0, 0x3c, 512.0f, 32.0f},
            {240.0f, 0, 0x14, 512.0f, 128.0f},
            {304.0f, 0, 0x14, 512.0f, 256.0f},
        };
        for (const Segment& segment : kSegments) {
            const Vec3 offset = polar(angle, segment.distance);
            const Vec3 at{pos.x + offset.x, pos.y + offset.y, pos.z + offset.z};
            mark_as_bomb_damage(player, player.alloc_damage_source(at, angle, segment.lifetime, segment.damage,
                                                                   segment.width, segment.height));
        }
    }
    if (AnmVm* vm = ctx.animations->find(primary_vm)) vm->entity_offset = pos;
    if (AnmVm* vm = ctx.animations->find(secondary_vm)) vm->entity_offset = pos;
    on_cancel();
    return 0;
}

// 0x40fe80：光束 VM 的每个子 VM（脚本 0x18）各作一个 48×160 × 缩放 的旋转矩形消弹
void MarisaBomb::on_cancel() {
    for (i32 i = 0;; ++i) {
        AnmVm* beam = find_vm(primary_vm);
        if (beam == nullptr) return;
        AnmVm* segment = AnmManager::find_child(*beam, 0x18, i);
        if (segment == nullptr) return;
        const Vec2 size{segment->scale.x * 48.0f, segment->scale.y * 160.0f};
        const Vec3 center = anm_world_position(*segment);
        ctx.bullets->cancel_rect(center, size, angle, 5);
        ctx.lasers->cancel_rect(center, Vec3{size.x, size.y, 0.0f}, angle);
    }
}

// =============================================================================================
// 季节解放

namespace {

// 各子季节按等级（0..6）的参数表（.rdata 0x491e70..0x491ff0）
constexpr float kSpringRadius[7] = {0, 60, 100, 140, 190, 240, 300};     // 0x491fd4
constexpr i32 kSpringDuration[7] = {0, 10, 13, 16, 20, 20, 20};           // 0x491fb8
constexpr float kSummerRadius[7] = {0, 40, 42, 44, 46, 48, 50};           // 0x491f54
constexpr i32 kSummerDuration[7] = {0, 30, 30, 30, 30, 30, 30};           // 0x491f38
constexpr float kAutumnRadius[7] = {0, 60, 60, 60, 60, 60, 60};           // 0x491f14
constexpr float kAutumnRadius2[7] = {0, 40, 40, 40, 40, 40, 40};          // 0x491ef0
constexpr i32 kAutumnDuration[7] = {0, 40, 55, 70, 90, 110, 130};         // 0x491ed4
constexpr float kWinterRadius[7] = {0, 30, 35, 40, 45, 50, 60};           // 0x491f94
constexpr i32 kWinterDuration[7] = {0, 30, 70, 110, 150, 170, 190};       // 0x491f78
constexpr float kDoyouRadius[7] = {0, 50, 52, 54, 56, 58, 60};            // 0x491eb0
constexpr float kDoyouRadius2[7] = {0, 30, 30, 30, 30, 30, 30};           // 0x491e8c
constexpr i32 kDoyouDuration[7] = {0, 40, 40, 40, 40, 40, 40};            // 0x491e70

// 列顺序见 SeasonReleaseProfile 声明；类名取自 RTTI（BombReimuSubInf 0x491dcc / BombCirnoSubInf 0x491d94 /
// BombAyaSubInf 0x491d78 / BombMarisaSubInf 0x491db0 / BombAllSubInf 0x491d5c）
constexpr SeasonReleaseProfile kSpring   {"ReimuSub",  3,    4,    kSpringRadius, kSpringRadius,  kSpringDuration, 16.0f, false, 10,   100,  true,  false, false, true};
constexpr SeasonReleaseProfile kSummer   {"CirnoSub",  3,    4,    kSummerRadius, kSummerRadius,  kSummerDuration, 8.0f,  false, 10,   100,  false, false, false, false};
constexpr SeasonReleaseProfile kAutumn   {"AyaSub",    3,    4,    kAutumnRadius, kAutumnRadius2, kAutumnDuration, 8.0f,  false, 0x1e, 0x1e, false, true,  false, false};
constexpr SeasonReleaseProfile kWinter   {"MarisaSub", 0x14, 0x15, kWinterRadius, kWinterRadius,  kWinterDuration, 0.0f,  true,  0x1e, 9,    false, false, true,  false};
constexpr SeasonReleaseProfile kDoyou    {"AllSub",    3,    4,    kDoyouRadius,  kDoyouRadius2,  kDoyouDuration,  8.0f,  false, 10,   100,  false, false, false, false};

}  // namespace

// 0x40d890 switch(DAT_004a57ac)：跳转表 0x40da4c（1 夏 / 2 秋 / 3 冬 / 4 土用，其它为春）
const SeasonReleaseProfile& SeasonRelease::profile_for_season(i32 season) {
    switch (season) {
    case 1: return kSummer;
    case 2: return kAutumn;
    case 3: return kWinter;
    case 4: return kDoyou;
    default: return kSpring;
    }
}

u32 SeasonRelease::spawn_aura(i32 script, float radius, float radius2, i32 duration) {
    u32 id = ctx.animations->spawn_effect(*ctx.player->sub_anm, script, -1, pos);
    AnmVm* vm = ctx.animations->find(id);
    if (vm == nullptr) return 0;
    vm->float_vars[0] = radius;
    vm->float_vars[1] = radius2;
    vm->int_vars[3] = duration;
    return id;
}

// 0x411460 / 0x40e0f0 / 0x40ec70 / 0x40f590 / 0x410150
void SeasonRelease::on_start() {
    Player& player = *ctx.player;
    pos = player.pos;
    angle = -kHalfPi;
    play_sfx(0x4a);
    const i32 level = season_level(*ctx.globals);
    const float inner = profile_.radius_table[level];
    const float inner2 = profile_.radius2_table[level];
    const i32 duration = profile_.duration_table[level];
    primary_vm = spawn_aura(profile_.inner_script, inner, inner2, duration);
    float outer, outer2;
    if (profile_.outer_scaled) {
        outer = inner * 0.2f + inner;
        outer2 = inner2 * 0.2f + inner2;
    } else {
        outer = inner + profile_.outer_growth;
        outer2 = inner2 + profile_.outer_growth;
    }
    secondary_vm = spawn_aura(profile_.outer_script, outer, outer2, duration);
    grant_invincibility(profile_.start_invincible);
    ctx.enemies->counters[1] += 1;
}

// 0x4116a0 / 0x40e330 / 0x40eeb0 / 0x40f7d0 / 0x4103e0
i32 SeasonRelease::on_update() {
    Player& player = *ctx.player;
    AnmVm* aura = find_vm(primary_vm);
    if (profile_.keep_invincible) grant_invincibility(0x32);
    if (profile_.follows_player) player.speed_mult = 1.5f;
    if (profile_.damage_boost) player.scale2 = 1.5f;
    if (aura == nullptr) {
        finish_release();
        return -1;
    }
    if (profile_.follows_player) {
        aura->entity_offset = player.pos;
        if (AnmVm* outer = ctx.animations->find(secondary_vm)) outer->entity_offset = player.pos;
    }
    // 光环当前半径（VM 缩放 x）即每帧 1 帧寿命的圆形伤害源
    player.alloc_circle_damage_source(anm_world_position(*aura), aura->scale.x, 0.0f, 1, profile_.damage);
    if (!profile_.cancel_first_16_only || timer.current < 0x10) on_cancel();
    return 0;
}

// 0x411790 / 0x40e3c0 / 0x40efa0 / 0x40f860 / 0x410480：光环半径内季节消弹
void SeasonRelease::on_cancel() {
    AnmVm* aura = find_vm(primary_vm);
    if (aura == nullptr) return;
    const Vec3 center = anm_world_position(*aura);
    ctx.bullets->cancel_radius(center, aura->scale.x, 4, true);
    ctx.lasers->cancel_radius(center, aura->scale.x, 4, true);
}

// =============================================================================================
// BombManager

// 0x40d890
void BombManager::create(BombContext context) {
    ctx_ = std::move(context);
    const GameGlobals& globals = *ctx_.globals;
    switch (globals.shot + globals.shot_aux) {
    case 1: main_ = std::make_unique<CirnoBomb>(ctx_); break;
    case 2: main_ = std::make_unique<AyaBomb>(ctx_); break;
    case 3: main_ = std::make_unique<MarisaBomb>(ctx_); break;
    default: main_ = std::make_unique<ReimuBomb>(ctx_); break;
    }
    main_->init(Bomb::Role::Main);
    release_ = std::make_unique<SeasonRelease>(ctx_, SeasonRelease::profile_for_season(globals.season));
    release_->init(Bomb::Role::Release);
    ctx_.main = main_.get();
    ctx_.release = release_.get();
}

// 0x40da90 → 0x40d710：VM 删除、链摘除、阴阳玉数组释放
void BombManager::destroy() {
    for (Bomb* bomb : {main_.get(), release_.get()}) {
        if (bomb == nullptr) continue;
        ctx_.animations->kill(bomb->primary_vm);
        ctx_.animations->kill(bomb->secondary_vm);
    }
    main_.reset();
    release_.reset();
    ctx_.main = nullptr;
    ctx_.release = nullptr;
}

}  // namespace th16
