// th16 BulletManager / Bullet 实现——按 th16.exe v1.00a 逐函数还原（地址见各函数头）。
// 浮点：全部 SSE 标量单精度（-ffp-contract=off），三角 / 反正切经 double CRT 再单舍入。
#include "Bullet.hpp"

#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "GameGlobals.hpp"
#include "Item.hpp"
#include "Motion.hpp"
#include "Player.hpp"
#include "Rng.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace th16 {
namespace {

constexpr float kPi = 3.1415927410125732f;      // 0x494588
constexpr float kTwoPi = 6.2831854820251465f;   // 0x4945b8
constexpr float kHalfPi = 1.5707963705062866f;  // 0x494534

// 0x490ec0：消弹方式 1 的颜色 → 脚本号表
constexpr i32 kCancelScriptByColor[16] = {4, 8, 12, 16, 20, 24, 28, 34, 0, 1, 2, 3, -1, 4, 5, 6};

// D3DXVec2Normalize：长度为 0 时结果为零向量
void normalize2(float& x, float& y) noexcept {
    const float len = std::sqrt(x * x + y * y);
    if (len == 0.0f) { x = 0.0f; y = 0.0f; return; }
    x = x / len;
    y = y / len;
}

}  // namespace

// ECL 600（0x42085b）：memset 0x380 后置默认值
void EtShooter::reset() noexcept { *this = EtShooter{}; }

// ---------------------------------------------------------------------------------------------
// 0x411dd0 + 0x411a30 + 0x411b70

void BulletManager::create(AnmManager& anm_manager, AnmFile& bullet_anm, AnmFile* effect, GameGlobals& g,
                           Player* pl, const PlayerWorld* w, Rng& game_rng) {
    animations = &anm_manager;
    anm = &bullet_anm;
    effect_anm = effect;
    globals = &g;
    player = pl;
    world = w;
    rng = &game_rng;
    animations->sprite_mappers[1] = &BulletManager::map_sprite;
    bullets.assign(kMaxBullets, Bullet{});
    cancel_effect_ids.assign(kMaxBullets, 0u);
    for (int i = 0; i < kMaxBullets; ++i) {
        bullets[u32(i)].index = i;
        bullets[u32(i)].vm_main.owner = &bullets[u32(i)];
        bullets[u32(i)].vm_extra.owner = &bullets[u32(i)];
    }
    reset();
}

// 0x411b70：bullet.anm VM 全部打删除位、池清零、空闲链按槽号从 0 到 1999 头插（1999 在链头）
void BulletManager::reset() {
    if (animations != nullptr && anm != nullptr) animations->kill_file(anm->slot);
    free_head = nullptr;
    active_head = nullptr;
    for (int i = 0; i < kMaxBullets; ++i) {
        Bullet& b = bullets[u32(i)];
        b = Bullet{};
        b.index = i;
        b.vm_main.owner = &b;
        b.vm_extra.owner = &b;
        b.free_next = free_head;
        free_head = &b;
    }
    for (int i = 0; i < kLayerBuckets; ++i) { layer_heads[i] = nullptr; layer_tails[i] = nullptr; }
    active_count = 0;
    safe_radius_sq = 0;
    bounce_width = 0;
    bounce_height = 0;
    cancel_item_counter = 0;
    total_canceled = 0;
    std::fill(cancel_effect_ids.begin(), cancel_effect_ids.end(), 0u);
    iterate_current_ = nullptr;
    iterate_next_ = nullptr;
    sfx_queue.clear();
}

void BulletManager::push_front_active(Bullet& b) {
    b.active_prev = nullptr;
    b.active_next = active_head;
    if (active_head != nullptr) active_head->active_prev = &b;
    active_head = &b;
}

void BulletManager::unlink_active(Bullet& b) {
    if (b.active_next != nullptr) b.active_next->active_prev = b.active_prev;
    if (b.active_prev != nullptr) b.active_prev->active_next = b.active_next;
    else if (active_head == &b) active_head = b.active_next;
    b.active_next = nullptr;
    b.active_prev = nullptr;
}

// 0x443840
float BulletManager::angle_to_player(const Vec3& from) const noexcept {
    const float dy = player->pos.y - from.y;
    const float dx = player->pos.x - from.x;
    if (dy == 0.0f && dx == 0.0f) return kHalfPi;
    return atan2_f(dy, dx);
}

// 0x417140：弹种表 sprite_map[0] ≥ 0 时按 color*4 + 下标重映射
i32 BulletManager::map_sprite(AnmVm& vm, i32 index) {
    const Bullet* b = static_cast<const Bullet*>(vm.owner);
    if (b == nullptr) return index;
    const BulletTypeInfo& info = bullet_type_info(b->type);
    if (info.sprite_map[0] < 0) return index;
    const i32 slot = i32(b->color) * 4 + index;
    if (slot < 0 || slot >= 64) return index;
    return info.sprite_map[slot];
}

const AnmRuntimeSprite* BulletManager::main_sprite(const Bullet& b) const {
    const AnmFile* f = animations->file(b.vm_main.file_slot);
    if (f == nullptr) return nullptr;
    const i32 idx = b.vm_main.sprite_index;
    if (idx < 0 || u32(idx) >= f->sprites.size()) return nullptr;
    return &f->sprites[u32(idx)];
}

// ---------------------------------------------------------------------------------------------
// 0x412670

void BulletManager::release_bullet(Bullet& b) {
    if (b.state == 0) return;
    b.state = 0;
    b.timer.reset();
    b.tick_timer.reset();
    b.timer1.reset();
    b.timer2.reset();
    b.flags &= 0xfffffcbeu;
    b.unknown_c7c = 0;
    b.ex_param = 0;
    b.free_next = free_head;
    free_head = &b;
    unlink_active(b);
}

// ---------------------------------------------------------------------------------------------
// 0x412cb0 / Ex 0x200 共用的外观绑定：主 VM、附加 VM、消弹脚本、判定与绘制桶

void BulletManager::bind_appearance(Bullet& b, i32 type, i32 color) {
    const BulletTypeInfo& info = bullet_type_info(type);
    animations->reset_vm(b.vm_main);
    b.vm_main.sprite_mapper = 1;
    b.vm_main.owner = &b;
    animations->init_script(*anm, b.vm_main, info.script);
    b.flags |= Bullet::kFlagCircle;
    b.vm_main.flags_hi = (b.vm_main.flags_hi & 0xfff7ffffu) | 0x40000u;
    animations->reset_vm(b.vm_extra);
    b.extra_vm_flags &= ~1u;
    if (info.extra_script != 0) {
        b.extra_vm_flags |= 1u;
        b.vm_extra.owner = &b;
        animations->init_script(*anm, b.vm_extra, info.extra_script);
        b.vm_extra.flags_hi = (b.vm_extra.flags_hi & 0xfff7ffffu) | 0x40000u;
    }
    bool circle = false;
    switch (info.cancel_kind) {
        case 0: b.cancel_script = color * 2 + 4; break;
        case 1: b.cancel_script = kCancelScriptByColor[color & 15]; break;
        case 2: b.cancel_script = -1; circle = true; break;
        case 3: b.cancel_script = 0x10; break;
        case 4: b.cancel_script = 6; break;
        case 5: b.cancel_script = 0xc; break;
        case 6: b.cancel_script = info.sprite_map[(color & 15) * 4 + 3]; circle = true; break;
        case 7: b.cancel_script = 0x104; circle = true; break;
        case 8: b.cancel_script = 0x107; circle = true; break;
        case 9: b.cancel_script = 0x10a; circle = true; break;
        case 10: b.cancel_script = 0x113; circle = true; break;
        default: break;
    }
    if (circle) b.flags |= Bullet::kFlagCircle;
}

// ---------------------------------------------------------------------------------------------
// 0x414da0

void BulletManager::shoot(const EtShooter& shooter) {
    const float aim = angle_to_player(shooter.pos);
    bool pool_empty = false;
    for (i32 stack_idx = 0; stack_idx < i32(shooter.stack) && !pool_empty; ++stack_idx) {
        for (i32 way_idx = 0; way_idx < i32(shooter.way); ++way_idx) {
            if (spawn_single(shooter, way_idx, stack_idx, aim) == 1) { pool_empty = true; break; }
        }
    }
    if ((shooter.flags & 0x20u) != 0) play_sfx(shooter.shoot_sfx, shooter.pos.x);
}

// 0x412cb0：从空闲链头取槽、头插活动链，按瞄准模式算角速度，初始化 VM 与 Ex
int BulletManager::spawn_single(const EtShooter& sh, i32 way_idx, i32 stack_idx, float aim) {
    Bullet* bp = free_head;
    if (bp == nullptr) return 1;
    Bullet& b = *bp;
    free_head = b.free_next;
    b.free_next = nullptr;
    push_front_active(b);

    const float way_f = float(i32(sh.way));
    const i32 stack = i32(sh.stack);
    float speed = (stack < 2) ? sh.speed1
                              : sh.speed1 - ((sh.speed1 - sh.speed2) * float(stack_idx)) / float(stack - 1);
    float angle = 0.0f;
    switch (sh.aim_mode) {
        case 0: case 1: {
            float a;
            if ((sh.way & 1) == 0) a = float(way_idx / 2) * sh.angle2 + sh.angle2 * 0.5f;
            else a = float((way_idx + 1) / 2) * sh.angle2;
            a = a + 0.0f;
            if ((way_idx & 1) != 0) a = a * -1.0f;
            if (sh.aim_mode == 0) a = a + aim;
            angle = sh.angle1 + a;
            break;
        }
        case 2: case 3: {
            float a = (sh.aim_mode == 2) ? aim + 0.0f : 0.0f;
            a = a + (float(way_idx) * kTwoPi) / way_f;
            angle = a + (float(stack_idx) * sh.angle2 + sh.angle1);   // 0x412e4b：先算层项再相加（舍入序）
            break;
        }
        case 4: case 5: {
            float a = (sh.aim_mode == 4) ? aim + 0.0f : 0.0f;
            a = a + kPi / way_f + (float(way_idx) * kTwoPi) / way_f;
            angle = a + (float(stack_idx) * sh.angle2 + sh.angle1);
            break;
        }
        case 6: {
            const float a2 = sh.angle2;
            angle = sh.angle1 + float(rng->signed_unit() * a2);
            break;
        }
        case 7: {
            const float s2 = sh.speed2;
            speed = float(rng->unit() * s2) + sh.speed1;
            angle = (float(way_idx) * kTwoPi) / way_f + 0.0f + (float(stack_idx) * sh.angle2 + sh.angle1);
            break;
        }
        case 8: {
            const float a2 = sh.angle2;
            angle = sh.angle1 + float(rng->signed_unit() * a2);
            const float s2 = sh.speed2;
            speed = float(rng->unit() * s2) + sh.speed1;
            break;
        }
        case 9: case 10: {
            float a = (float(way_idx) * kTwoPi) / way_f;
            if ((sh.stack & 1) == 0) {
                a = float(stack_idx / 2) * sh.angle2 + sh.angle2 * 0.5f + a;
                if (stack > 1)
                    speed = ((sh.speed2 - sh.speed1) * float(u32(stack_idx) & 0xfffeu)) / float(stack - 1) + sh.speed1;
            } else {
                a = float((stack_idx + 1) / 2) * sh.angle2 + a;
                if (stack > 1)
                    speed = ((sh.speed2 - sh.speed1) * float(u32(stack_idx + 1) & 0xfffeu)) / float(stack - 1) + sh.speed1;
            }
            if ((stack_idx & 1) != 0) a = a * -1.0f;
            if (sh.aim_mode == 9) a = a + aim;
            angle = sh.angle1 + a;
            break;
        }
        case 11: {
            const float f = (float(way_idx) * kTwoPi) / way_f;
            angle = sh.angle1 + f + 0.0f;
            speed = speed * (1.0f - std::fabs(float(std::sin(double(f)))) * sh.speed2);
            break;
        }
        case 12: {
            const float f = (float(way_idx) * kTwoPi) / way_f + kPi / way_f;
            angle = sh.angle1 + f + 0.0f;
            speed = speed * (1.0f - std::fabs(float(std::sin(double(f)))) * sh.speed2);
            break;
        }
        default: break;
    }
    b.speed = speed;
    float wrapped = angle + 0.0f;
    wrapped = wrap_angle(wrapped);
    wrapped = wrap_angle(wrapped);
    b.angle = wrapped;

    b.pos = sh.pos;
    if (sh.distance != 0.0f) {
        const Vec3 d = polar(b.angle, sh.distance);
        b.pos.x = b.pos.x + d.x;
        b.pos.y = b.pos.y + d.y;
    }
    b.pos.z = 0.1f;
    b.flags |= Bullet::kFlagActive;
    b.state = 1;
    b.timer.reset();
    b.tick_timer.reset();
    b.cancel_immunity = 0;
    b.scale = 1.0f;
    b.scale_interp.end_time = 0;
    if (safe_radius_sq > 0.0f) {
        const float dx = b.pos.x - player->pos.x;
        const float dy = b.pos.y - player->pos.y;
        if (dx * dx + dy * dy < safe_radius_sq) {
            release_bullet(b);
            return -1;
        }
    }
    b.velocity = polar(angle, speed);
    b.ex_flags = sh.flags;
    b.color = sh.color;
    b.type = sh.type;
    b.unknown_c7c = 0;
    b.life_default = 0x3c;
    b.flags = (b.flags & 0xfffffff3u) | Bullet::kFlagCollidable;
    b.timer1.reset();
    b.timer2.reset();

    const BulletTypeInfo& info = bullet_type_info(b.type);
    bind_appearance(b, b.type, b.color);
    b.layer_bucket = info.default_layer;
    b.step_sfx = sh.step_sfx;
    b.spawn_grace = 5;
    b.hitbox = {info.hitbox, info.hitbox};
    b.initial_ex_flags = sh.flags;
    b.ex_flags = 0;
    b.ex_loop_count = 0;
    b.ex_index = sh.start_ex;
    std::memcpy(b.ex, sh.ex, sizeof b.ex);
    const i32 start = (b.ex_index >= 0 && b.ex_index < 18) ? b.ex_index : 0;
    if (b.ex_index >= 0 && b.ex_index < 18 && b.ex[start].type == kExSpawnFog) {
        const i16 a = i16(b.ex[start].a);
        if (a != 1) b.vm_main.pending_interrupt = i32(a) + 7;
        b.state = 2;
        b.pos.x = b.pos.x - b.velocity.x * 4.0f;
        b.pos.y = b.pos.y - b.velocity.y * 4.0f;
        b.pos.z = b.pos.z - b.velocity.z * 4.0f;
        b.ex_index = b.ex_index + 1;
    } else {
        b.vm_main.pending_interrupt = 2;
    }
    run_ex(b, 1.0f);
    animations->run(b.vm_main);
    if ((b.extra_vm_flags & 1u) != 0) animations->run(b.vm_extra);
    return 0;
}

// ---------------------------------------------------------------------------------------------
// 0x412860

void BulletManager::update(float gs, bool frozen) {
    iterate_current_ = active_head;
    iterate_next_ = active_head ? active_head->active_next : nullptr;
    active_count = 0;
    for (int i = 0; i < kLayerBuckets; ++i) { layer_heads[i] = nullptr; layer_tails[i] = nullptr; }
    for (Bullet* b = iterate_current_; b != nullptr;) {
        bool alive = true;
        if (!frozen) {
            if ((b->flags & Bullet::kFlagForceGraze) != 0 &&
                ((b->state == 2 && b->timer.current > 7) || b->state == 1)) {
                check_player_collision(*b, true, gs);
            } else if (step_bullet(*b, gs) != 0) {
                alive = false;
            }
        }
        if (alive) {
            if ((b->flags & Bullet::kFlagHidden) == 0) {
                const i32 bucket = b->layer_bucket;
                if (layer_heads[bucket] == nullptr) layer_heads[bucket] = b;
                else layer_tails[bucket]->layer_next = b;
                layer_tails[bucket] = b;
                b->layer_next = nullptr;
            }
            ++active_count;
            b->timer.tick(gs);
        }
        iterate_current_ = iterate_next_;
        iterate_next_ = iterate_next_ ? iterate_next_->active_next : nullptr;
        b = iterate_current_;
    }
}

// 0x412a60：按桶链同步 VM 坐标 / 朝向 / 缩放（绘制本体由 AnmRenderer 完成）
void BulletManager::draw(AnmRenderer& renderer) {
    for (int bucket = 0; bucket < kLayerBuckets; ++bucket) {
        for (Bullet* b = layer_heads[bucket]; b != nullptr; b = b->layer_next) {
            auto draw_synced = [&](AnmVm& vm) {
                vm.entity_offset = b->pos;
                if ((vm.flags_hi & 0x80u) != 0) {
                    vm.rotation.z = wrap_angle(b->angle + kHalfPi);
                    vm.flags_lo |= AnmVm::kRotationDirty;
                }
                // 缩放中的敌弹：写的是 VM 的 scale2（子弹 +0x80 = VM +0x58），与脚本自己的 scale 相乘——出生闪光的
                // 5 → 1 插值因此保留在 scale 里（0x412a60：`[bullet + 0x80] = [bullet + 0x141c]`）
                if ((b->flags & Bullet::kFlagScaled) != 0) {
                    vm.flags_lo |= AnmVm::kScaleDirty;
                    vm.scale2 = {b->scale, b->scale};
                }
                renderer.draw_vm(vm);
            };
            if ((b->extra_vm_flags & 1u) != 0) draw_synced(b->vm_extra);
            draw_synced(b->vm_main);
        }
    }
}

// ---------------------------------------------------------------------------------------------
// 0x411e70：返回非 0 表示已回收

int BulletManager::step_bullet(Bullet& b, float gs) {
    b.tick_timer.tick(gs);
    if ((b.flags & Bullet::kFlagReleased) != 0) { release_bullet(b); return -1; }
    if ((b.ex_flags & kExScale) != 0) {
        const float s = step_interp_float(b.scale_interp, gs);
        b.scale = s;
        if (b.scale_interp.end_time == 0) {
            b.ex_flags &= ~kExScale;
            if (s == 1.0f) b.flags &= ~Bullet::kFlagScaled;
        }
    }
    switch (b.state) {
        case 1:
        state_1: {
            int again;
            do {
                if ((b.ex_flags & kExHide) == 0) run_ex(b, gs);
                if (b.ex_flags == 0) break;
                again = 0;
                if ((b.ex_flags & kExInitialBoost) != 0) again += ex_initial_boost(b, gs);
                if ((b.ex_flags & kExAccel) != 0) again += ex_accel(b, gs);
                if ((b.ex_flags & kExSpeedEase) != 0) again += ex_speed_ease(b, gs);
                if ((b.ex_flags & kExAccelTurn) != 0) again += ex_accel_turn(b, gs);
                if ((b.ex_flags & kExStopTurn) != 0) again += ex_stop_turn(b, gs);
                if ((b.ex_flags & kExBounce) != 0) again += ex_bounce(b, gs);
                if ((b.ex_flags & kExMoveTo) != 0) again += ex_move_to(b, gs);
                if ((b.ex_flags & kExExtraVelocity) != 0) again += ex_extra_velocity(b, gs);
                if ((b.ex_flags & kExBoundsGuard) != 0) again += ex_bounds_guard(b, gs);
                if ((b.ex_flags & kExWait) != 0) {
                    if (b.ex_wait_timer.current < 1) { ++again; b.ex_flags ^= kExWait; }
                    else b.ex_wait_timer.untick(gs);
                }
                if ((b.ex_flags & kExHide) != 0) {
                    if (b.ex4000000_timer.current < 1) {
                        b.flags &= ~Bullet::kFlagHidden;
                        b.ex_flags ^= kExHide;
                        ++again;
                    } else {
                        b.flags |= Bullet::kFlagHidden;
                        b.ex4000000_timer.untick(gs);
                    }
                }
                if (b.cancel_immunity != 0) b.cancel_immunity = b.cancel_immunity - 1;
            } while (again != 0);
            if ((b.flags & Bullet::kFlagHidden) == 0) {
                const float vy = b.velocity.y * gs;
                const float vz = b.velocity.z * gs;
                b.pos.x = b.pos.x + gs * b.velocity.x;
                b.pos.y = b.pos.y + vy;
                b.pos.z = b.pos.z + vz;
                check_player_collision(b, false, gs);
            }
            break;
        }
        case 2: {
            const float vz = b.velocity.z * gs;
            const float vx = b.velocity.x * gs;
            b.pos.y = b.velocity.y * gs * 0.5f + b.pos.y;
            b.pos.x = b.pos.x + vx * 0.5f;
            b.pos.z = vz * 0.5f + b.pos.z;
            if ((b.timer.current < 8 || check_player_collision(b, false, gs) != 1) && b.vm_main.int_vars[0] != 0) {
                b.state = 1;
                goto state_1;
            }
            break;
        }
        case 3: {
            const float vx = b.velocity.x * gs;
            const float vy = b.velocity.y * gs;
            b.pos.z = b.velocity.z * gs * 0.5f + b.pos.z;
            b.pos.y = vy * 0.5f + b.pos.y;
            b.pos.x = vx * 0.5f + b.pos.x;
            break;
        }
        case 5: {
            if (b.timer.current > 2) {
                if (b.timer.current == 3) start_hit_dissolve(b, gs);
                const float vx = gs * b.velocity.x;
                const float vy = b.velocity.y * gs;
                b.pos.z = b.velocity.z * gs * 0.5f + b.pos.z;
                b.pos.y = vy * 0.5f + b.pos.y;
                b.pos.x = vx * 0.5f + b.pos.x;
            }
            break;
        }
        default: break;
    }
    if (const AnmRuntimeSprite* sprite = main_sprite(b)) {
        if ((b.ex_flags & kExWrap) != 0) ex_wrap(b);
        if ((b.ex_flags & kExBoundsGuard) == 0 && b.spawn_grace < 1) {
            const float hw = sprite->width * b.scale * 0.5f;
            if (b.pos.x + hw <= -192.0f || 192.0f <= b.pos.x - hw) { release_bullet(b); return -1; }
            const float hh = sprite->height * b.scale * 0.5f;
            if (b.pos.y + hh <= -64.0f || 448.0f <= b.pos.y - hh) { release_bullet(b); return -1; }
        }
    }
    if (b.cancel_immunity != 0) b.cancel_immunity = b.cancel_immunity - 1;
    if (b.spawn_grace > 0) b.spawn_grace = b.spawn_grace - 1;
    if ((b.flags & Bullet::kFlagHidden) != 0 || animations->run(b.vm_main) == 0) {
        if ((b.extra_vm_flags & 1u) != 0) animations->run(b.vm_extra);
        return 0;
    }
    release_bullet(b);
    return -1;
}

// 0x4124b0 命中段 / 0x411e70 状态 5：主 VM 中断 1，消弹脚本 ≥ 0 时生成特效并以 vel×gs×10 为目标飞散
void BulletManager::start_hit_dissolve(Bullet& b, float gs) {
    b.vm_main.pending_interrupt = 1;
    if ((b.extra_vm_flags & 1u) != 0) b.vm_extra.pending_interrupt = 1;
    if (b.cancel_script < 0) return;
    const u32 id = animations->spawn_effect(*anm, b.cancel_script, -1, b.pos, 0.0f);
    AnmVm* vm = animations->find(id);
    if (vm == nullptr) return;
    const Vec3 push{globals->push_x, globals->push_y, globals->push_z};
    const Vec3 goal{gs * b.velocity.x * 10.0f, b.velocity.y * gs * 10.0f, b.velocity.z * gs * 10.0f};
    vm->pos_interp.end_time = 0x1e;
    vm->pos_interp.bezier1 = push;
    vm->pos_interp.bezier2 = push;
    vm->pos_interp.mode = 6;
    vm->pos_interp.initial = push;
    vm->pos_interp.goal = goal;
    vm->pos_interp.timer.reset();
}

// ---------------------------------------------------------------------------------------------
// 0x4124b0：0 无事 / 1 被弹 / 2 擦弹

int BulletManager::check_player_collision(Bullet& b, bool force_graze, float gs) {
    b.vm_main.flags_lo &= 0xfff9ffffu;
    b.vm_main.pos = {globals->push_x, globals->push_y, globals->push_z};
    const u32 flags = b.flags;
    if ((flags & Bullet::kFlagCollidable) == 0 || b.hitbox.x <= 0.0f) return 0;

    // 0x4438c0：矩形判定（判定框 size）；0x4439e0：圆形判定（半径 r）
    auto rect_test = [&](const Vec2& size) -> int {
        const float bx = b.pos.x, by = b.pos.y;
        const float left = bx - size.x * 0.5f;
        const float top = by - size.y * 0.5f;
        const Vec3& rmin = player->rect_min;
        const Vec3& rmax = player->rect_max;
        if (bx + size.x * 0.5f < rmin.x || by + size.y * 0.5f < rmin.y || left > rmax.x || top > rmax.y) {
            if (rmin.x <= bx + 24.0f && rmin.y <= by + 24.0f && bx - 24.0f <= rmax.x && by - 24.0f <= rmax.y) return 2;
            return 0;
        }
        if (world != nullptr && world->dialogue) return 0;
        if (force_graze) return 2;
        const i32 st = player->state;
        if (st != Player::kRespawnWait && st != Player::kDying && st != Player::kState3) {
            if (player->invincible.current < 1) player->hit(*animations, effect_anm);
            return 1;
        }
        return 0;
    };
    auto circle_test = [&](float r) -> int {
        const float dx = player->pos.x - b.pos.x;
        const float dy = player->pos.y - b.pos.y;
        float hr = player->sht.hitbox;
        const float d2 = dx * dx + dy * dy;
        if ((player->flags & Player::kFlagScaled) != 0) hr = hr * player->scale * 3.6f;
        if (hr * hr + r * r <= d2) {
            float g = 40.0f;
            if (40.0f <= r / 2.5f) g = r / 2.5f;
            return ((g + hr) * (g + hr) + r * r <= d2) ? 0 : 2;
        }
        if (world != nullptr && world->dialogue) return 0;
        if (force_graze) return 2;
        const i32 st = player->state;
        if (st != Player::kRespawnWait && st != Player::kDying && st != Player::kState3) {
            if (player->invincible.current < 1) player->hit(*animations, effect_anm);
            return 1;
        }
        return 0;
    };

    int result;
    if ((flags & Bullet::kFlagScaled) == 0) {
        if ((flags & Bullet::kFlagCircle) == 0) result = rect_test(b.hitbox);
        else result = circle_test(b.hitbox.x);
    } else if ((flags & Bullet::kFlagCircle) == 0) {
        result = rect_test({b.hitbox.x * b.scale, b.hitbox.y * b.scale});
    } else {
        result = circle_test(b.hitbox.x * b.scale);
    }

    if (result == 1) {
        if (b.cancel_immunity == 0) {
            b.state = 3;
            start_hit_dissolve(b, gs);
        }
    } else if (result == 2 && (b.flags & Bullet::kFlagGrazed) == 0) {
        graze(b.pos);
        b.flags |= Bullet::kFlagGrazed;
        return 2;
    }
    return result;
}

// 0x444cf0：Player::graze（计数、特效、音效）后生成得点道具 0x10（0x430960）
void BulletManager::graze(const Vec3& pos) {
    player->graze(*animations, effect_anm, *globals, pos);
    const float angle = atan2_f(pos.y - player->pos.y, pos.x - player->pos.x);
    spawn_item(0x10, pos, angle, 1.9f, 0);
}

void BulletManager::spawn_item(i32 type, const Vec3& pos, float angle, float speed, i32 flag) {
    if (items != nullptr) items->spawn(type, pos, angle, speed, flag);
}

// 0x4252d0：沿活动链（0x1403b1c / 0x1403b20 游标）扫描；比较用 radius² 与 dx² + dy²（严格小于）
void BulletManager::trigger_ex_param_near(const Vec3& center, float radius) {
    iterate_current_ = active_head;
    iterate_next_ = active_head ? active_head->active_next : nullptr;
    for (Bullet* b = iterate_current_; b != nullptr;) {
        if (b->ex_param == 1) {
            const float dx = center.x - b->pos.x;
            const float dy = center.y - b->pos.y;
            if (dx * dx + dy * dy < radius * radius) {
                b->ex_index = 8;
                b->ex_param = 2;
                b->ex_flags = 0;
            }
        }
        iterate_current_ = iterate_next_;
        iterate_next_ = iterate_next_ ? iterate_next_->active_next : nullptr;
        b = iterate_current_;
    }
}

// ---------------------------------------------------------------------------------------------
// 0x416840 / 0x416a00 / 0x416c20 / 0x416d20 / 0x416f40

void BulletManager::cancel_bullet(Bullet& b, i32 item_mode) {
    b.vm_main.pending_interrupt = 1;
    animations->run(b.vm_main);
    if ((b.extra_vm_flags & 1u) != 0) b.vm_extra.pending_interrupt = 1;
    if ((b.flags & Bullet::kFlagHidden) == 0) {
        if (b.cancel_script >= 0)
            cancel_effect_ids[u32(b.index)] = animations->spawn_effect(*anm, b.cancel_script, -1, b.pos, 0.0f);
        play_sfx(0x47, b.pos.x);
        convert_cancel_item(b.pos, item_mode);
    }
    const float vy = b.velocity.y * game_speed;
    const float vz = b.velocity.z * game_speed;
    b.pos.x = b.pos.x + b.velocity.x * game_speed * 0.5f;
    b.pos.y = b.pos.y + vy * 0.5f;
    b.pos.z = b.pos.z + vz * 0.5f;
    b.state = 4;
    b.timer.reset();
}

// 0x416a00：屏内（±32 像素）消弹按模式转化道具；模式 2/4/5 消耗 DAT_004a6d88
void BulletManager::convert_cancel_item(const Vec3& pos, i32 mode) {
    if (mode == 0) return;
    if (!(-192.0f < pos.x + 32.0f && pos.x - 32.0f < 192.0f && 0.0f < pos.y + 32.0f && pos.y - 32.0f < 448.0f)) return;
    total_canceled = total_canceled + 1;
    if (mode == 1 || mode == 3) return;
    constexpr float kSpread = 0.17453292f;  // 10°
    if (mode == 2) {
        if (total_canceled % 5 == 0 && !spellcard_active) spawn_item(1, pos, -kHalfPi, 2.2f, 0);
        if (!spellcard_active) {
            const float a = float(rng->signed_unit() * kSpread) - kHalfPi;
            spawn_item(10, pos, a, 2.2f, 0);
        }
        return;
    }
    if (mode == 5) {
        if (cancel_item_counter % 3 == 0) {
            const float a = float(rng->signed_unit() * kSpread) - kHalfPi;
            spawn_item(0x10, pos, a, 2.2f, 1);
        }
        cancel_item_counter = cancel_item_counter + 1;
        return;
    }
    if (mode == 4) {
        const float a = float(rng->signed_unit() * kSpread) - kHalfPi;
        spawn_item(0x10, pos, a, 2.2f, 1);
        if (season_release_active) {
            const float a2 = float(rng->signed_unit() * kSpread) - kHalfPi;
            spawn_item(season_release_item + 8, pos, a2, 2.2f, 0);
        }
    }
}

void BulletManager::cancel_radius(const Vec3& center, float radius, i32 item_mode, bool check_immunity) {
    iterate_current_ = active_head;
    iterate_next_ = active_head ? active_head->active_next : nullptr;
    for (Bullet* b = iterate_current_; b != nullptr;) {
        if ((b->state == 2 || b->state == 1) && (!check_immunity || b->cancel_immunity == 0)) {
            const float dy = b->pos.y - center.y;
            const float reach = b->hitbox.x * 0.5f + radius;
            const float dx = b->pos.x - center.x;
            if (dy * dy + dx * dx <= reach * reach) cancel_bullet(*b, item_mode);
        }
        iterate_current_ = iterate_next_;
        iterate_next_ = iterate_next_ ? iterate_next_->active_next : nullptr;
        b = iterate_current_;
    }
}

void BulletManager::cancel_rect(const Vec3& center, const Vec2& size, float angle, i32 item_mode) {
    // 原版按池下标顺扫全部 2000 槽（不走活动链表），判定半径 = 弹缩放 × 判定宽
    for (int i = 0; i < kMaxBullets; ++i) {
        Bullet& b = bullets[u32(i)];
        if ((b.state != 2 && b.state != 1) || b.cancel_immunity != 0) continue;
        const float r = b.scale * b.hitbox.x;
        if (!rect_hits_circle(center.x, center.y, size.x, size.y, angle, b.pos.x, b.pos.y, r)) continue;
        if (!rect_hits_circle(0.0f, 224.0f, 384.0f, 448.0f, 0.0f, b.pos.x, b.pos.y, r)) continue;
        cancel_bullet(b, item_mode);
    }
}

void BulletManager::cancel_all(i32 item_mode) {
    for (int i = 0; i < kMaxBullets; ++i) {
        Bullet& b = bullets[u32(i)];
        if (b.state != 0 && b.state != 3) cancel_bullet(b, item_mode);
    }
}

// ---------------------------------------------------------------------------------------------
// 0x413860：解释 Ex 步骤直到遇到需等待的步骤

void BulletManager::run_ex(Bullet& b, float gs) {
    (void)gs;
    while (b.ex_index < 18) {
        if (b.ex_index < 0) break;
        BulletExStep& st = b.ex[b.ex_index];
        const u32 t = st.type;
        if (t == 0 || (st.async == 0 && (b.ex_flags & ~kExBoundsGuard) != 0) || (t & b.ex_flags) != 0) break;

        // 角度参数通用解析：≤ -999990 保持当前角度；≥ 999990 自机狙（+ offset）；否则取参数；结果折回
        auto resolve_angle = [&](float v, float offset) -> float {
            if (v <= -999990.0f) return b.angle;
            if (v >= 999990.0f) v = angle_to_player(b.pos) + offset;
            return wrap_angle(v);
        };
        auto step_sfx_if_started = [&]() {
            if (b.ex_index != 0 && b.step_sfx >= 0) play_sfx(b.step_sfx);
        };

        switch (t) {
            case kExInitialBoost:
                b.ex_flags |= kExInitialBoost;
                b.ex1_timer.set(0);
                b.ex1_extra = 0;
                ++b.ex_index;
                break;
            case kExSpawnFog:
                b.vm_main.pending_interrupt = i32(i16(st.a)) + 7;
                b.state = 2;
                b.pos.x = b.pos.x - b.velocity.x * 4.0f;
                b.pos.y = b.pos.y - b.velocity.y * 4.0f;
                b.pos.z = b.pos.z - b.velocity.z * 4.0f;
                ++b.ex_index;
                break;
            case kExAccel:
                b.ex_flags |= kExAccel;
                b.ex4_acc = st.r;
                b.ex4_angle = resolve_angle(st.s, st.m);
                b.ex4_timer.set(0);
                b.ex4_duration = st.a;
                b.ex4_acc_vec = polar(b.ex4_angle, b.ex4_acc);
                step_sfx_if_started();
                ++b.ex_index;
                break;
            case kExAccelTurn:
                b.ex_flags |= kExAccelTurn;
                b.ex8_acc = st.r;
                b.ex8_ang_vel = st.s;
                b.ex8_timer.set(0);
                b.ex8_duration = st.a;
                step_sfx_if_started();
                ++b.ex_index;
                break;
            case kExStopTurn: {
                b.ex_flags |= kExStopTurn;
                b.ex10_target_speed = (st.s > -999990.0f) ? st.s : b.speed;
                float x;
                if (st.r > -999990.0f) {
                    if (st.r >= 9999990.0f) x = wrap_angle(b.saved_angle);
                    else if (st.r >= 999990.0f) x = wrap_angle(angle_to_player(b.pos) + st.m);
                    else x = wrap_angle(st.r);
                } else {
                    x = b.angle;
                }
                switch (st.c) {
                    case 0: case 1: case 4: b.ex10_target_angle = x; break;
                    case 2: b.ex10_target_angle = wrap_angle(angle_to_player(b.saved_pos) + x); break;
                    case 3: b.ex10_target_angle = wrap_angle(b.saved_angle + x); break;
                    case 5: case 6: b.ex10_target_angle = float(rng->signed_unit() * st.r); break;
                    case 7: {
                        float a;
                        if (st.r > -999990.0f) a = wrap_angle(st.r >= 990.0f ? angle_to_player(b.pos) : st.r);
                        else a = b.angle;
                        b.ex10_target_angle = a;
                        b.ex10_target_speed = b.speed + float(rng->signed_unit() * st.s);
                        break;
                    }
                    default: break;
                }
                b.ex10_timer.set(0);
                b.ex10_duration = st.a;
                b.ex10_repeats = st.b;
                b.ex10_count = 0;
                b.ex10_mode = st.c;
                b.ex10_param = st.d;
                ++b.ex_index;
                break;
            }
            case kExBounce:
                b.ex_flags |= kExBounce;
                b.ex40_speed = st.r;
                if ((u32(st.b) & 0x20u) != 0) { b.ex40_width = st.s; b.ex40_height = st.m; }
                else { b.ex40_width = 384.0f; b.ex40_height = 448.0f; }
                b.ex40_max_bounces = st.a;
                b.ex40_count = 0;
                b.ex40_wall_mask = u32(st.b);
                ++b.ex_index;
                break;
            case kExImmunity:
                b.cancel_immunity = st.a;
                ++b.ex_index;
                break;
            case kExBoundsGuard:
                b.ex_flags |= kExBoundsGuard;
                if ((b.ex100_timer.flags & 1u) == 0) b.ex100_timer.set(0);
                b.ex100_timer.current = st.a;
                b.ex100_timer.previous = st.a - 1;
                b.ex100_timer.current_f = float(st.a);
                b.ex100_ray_check = st.b;
                ++b.ex_index;
                break;
            case kExChangeType: {
                const i32 type = i32(i16(st.a));
                const i32 color = i32(u16(st.b) & 0x7fffu);
                b.type = i16(type);
                b.color = i16(color);
                const BulletTypeInfo& info = bullet_type_info(type);
                b.hitbox = {info.hitbox, info.hitbox};
                b.layer_bucket = info.default_layer;
                bind_appearance(b, type, color);
                if ((u32(st.b) & 0x8000u) != 0) b.vm_main.pending_interrupt = 2;
                ++b.ex_index;
                break;
            }
            case kExCancel:
                if (st.a == 1) b.cancel_script = -1;
                cancel_bullet(b, 0);
                ++b.ex_index;
                break;
            case kExSfx:
                play_sfx(st.a, b.pos.x);
                ++b.ex_index;
                break;
            case kExWrap:
                b.ex_flags |= kExWrap;
                b.ex1000_max = st.a;
                b.ex1000_count = 0;
                b.ex1000_mask = u32(st.b);
                ++b.ex_index;
                break;
            case kExReshoot: {
                EtShooter sh;
                std::memset(static_cast<void*>(&sh), 0, sizeof sh);
                sh.pos = b.pos;
                sh.aim_mode = i16(st.a);
                sh.start_ex = st.b;
                sh.way = i16(st.c);
                sh.stack = i16(st.d);
                sh.step_sfx = -1;
                float a1;
                if (st.r > -999990.0f) a1 = wrap_angle(st.r >= 999990.0f ? angle_to_player(b.pos) : st.r);
                else a1 = b.angle;
                sh.angle1 = a1;
                sh.angle2 = st.s;
                sh.speed1 = (st.m <= -999990.0f) ? b.speed : st.m;
                sh.speed2 = st.n;
                const i32 next_index = b.ex_index + 1;
                b.ex_index = next_index;
                const BulletExStep& next = b.ex[(next_index < 18) ? next_index : 17];
                sh.type = i16(next.a);
                sh.color = i16(next.b);
                const i32 cancel_after = next.c;
                sh.flags = 0;
                std::memcpy(sh.ex, b.ex, sizeof sh.ex);
                shoot(sh);
                b.ex_index = b.ex_index + 1;
                if (cancel_after != 0) cancel_bullet(b, 0);
                break;
            }
            case kExParam:
                b.ex_param = st.a;
                ++b.ex_index;
                break;
            case kExLoop:
                if (st.b < 1) {
                    b.ex_index = st.a;
                } else if (b.ex_loop_count == 0) {
                    b.ex_loop_count = st.b;
                    b.ex_index = st.a;
                } else if (b.ex_loop_count == 1) {
                    b.ex_loop_count = 0;
                    ++b.ex_index;
                } else {
                    b.ex_loop_count = b.ex_loop_count - 1;
                    b.ex_index = st.a;
                }
                break;
            case kExMoveTo: {
                b.ex_flags |= kExMoveTo;
                b.ex20000_target.x = st.r;
                b.ex20000_target.y = st.s;
                if ((u32(st.b) & 0x100u) != 0) {
                    b.ex20000_target.x = b.pos.x + b.ex20000_target.x;
                    b.ex20000_target.y = b.pos.y + b.ex20000_target.y;
                    b.ex20000_target.z = b.pos.z + b.ex20000_target.z;
                }
                b.ex20000_saved_speed = b.speed;
                b.ex20000_target.z = 0.0f;
                b.ex20000_duration = st.a;
                b.ex20000_mode = u32(st.b) & 0xffu;
                b.ex20000_timer.reset();
                const Vec3 push{globals->push_x, globals->push_y, globals->push_z};
                b.ex20000_interp.initial = b.pos;
                b.ex20000_interp.goal = b.ex20000_target;
                b.ex20000_interp.bezier1 = push;
                b.ex20000_interp.bezier2 = push;
                b.ex20000_interp.end_time = st.a;
                b.ex20000_interp.mode = i32(u32(st.b) & 0xffu);
                b.ex20000_interp.timer.reset();
                ++b.ex_index;
                break;
            }
            case kExSetAngle:
                if (st.r < 990.0f) {
                    if (st.r >= -990.0f) b.angle = wrap_angle(st.r);
                } else {
                    b.angle = wrap_angle(wrap_angle(angle_to_player(b.pos) + (st.r - 999.0f)));
                }
                if (st.s >= -990.0f) b.speed = st.s;
                b.velocity = polar(b.angle, b.speed);
                ++b.ex_index;
                break;
            case kExExtraVelocity:
                b.ex_flags |= kExExtraVelocity;
                b.ex80000_vel = polar(st.r, st.s);
                b.ex80000_vel.z = 0.0f;
                b.ex80000_angle = st.r;
                b.ex80000_speed = st.s;
                b.ex80000_duration = st.a;
                b.ex80000_timer.set(0);
                ++b.ex_index;
                break;
            case kExBlend:
                if (st.a == 2) b.vm_main.flags_lo = (b.vm_main.flags_lo & 0xfffffe5fu) | 0x40u;
                else if (st.a == 1) b.vm_main.flags_lo = (b.vm_main.flags_lo & 0xfffffe3fu) | 0x20u;
                else b.vm_main.flags_lo = b.vm_main.flags_lo & 0xfffffe1fu;
                ++b.ex_index;
                break;
            case kExSpeedEase:
                b.ex_flags |= kExSpeedEase;
                b.ex200000_acc = (st.r - b.speed) / float(st.a);
                b.ex200000_angle = resolve_angle(st.s, st.m);
                b.ex200000_timer.reset();
                b.ex200000_duration = st.a;
                b.ex200000_acc_vec = polar(b.ex200000_angle, b.ex200000_acc);
                step_sfx_if_started();   // 原版经 LAB_00413a88 与 Ex 4 / 8 共用收尾（含变换音效）
                ++b.ex_index;
                break;
            case kExScale:
                b.ex_flags |= kExScale;
                b.scale_interp.initial = st.r;
                b.scale_interp.goal = st.s;
                b.scale_interp.bezier1 = 0.0f;
                b.scale_interp.bezier2 = 0.0f;
                b.scale_interp.end_time = st.a;
                b.scale_interp.mode = st.b;
                b.scale_interp.timer.reset();
                b.flags |= Bullet::kFlagScaled;
                ++b.ex_index;
                break;
            case kExSavePose:
                b.saved_pos = b.pos;
                b.saved_angle = b.angle;
                b.saved_speed = b.speed;
                ++b.ex_index;
                break;
            case kExSpawnEnemy:
                // 0x41aa70 按 str 生成敌机：th16 全部关卡数据未使用该步骤；EnemyManager 按名生成接口接入后补
                ++b.ex_index;
                break;
            case kExLayer:
                b.layer_bucket = st.a;
                ++b.ex_index;
                break;
            case kExHide:
                if (st.a >= 1) {
                    b.ex_flags |= kExHide;
                    b.ex4000000_timer.set(st.a);
                }
                ++b.ex_index;
                break;
            case kExLaser: {
                if (st.a == 0 || st.a == 1) {
                    BulletLaserRequest req;
                    req.kind = st.a;
                    req.pos = b.pos;
                    if (st.r > -999990.0f) req.angle = wrap_angle(st.r >= 999990.0f ? angle_to_player(b.pos) : st.r);
                    else req.angle = b.angle;
                    req.speed = (st.s <= -999990.0f) ? b.speed : st.s;
                    req.step = st;
                    const i32 next_index = b.ex_index + 1;
                    req.next = b.ex[(next_index < 18) ? next_index : 17];
                    std::memcpy(req.ex, b.ex, sizeof req.ex);
                    if (on_laser_request) on_laser_request(req);
                    b.ex_index = next_index + 1;
                    const bool cancel_after = (st.a == 0) ? (st.d != 0) : ((u32(st.d) & 0x10000u) != 0);
                    if (cancel_after) cancel_bullet(b, 0);
                } else {
                    ++b.ex_index;
                }
                break;
            }
            case kExHitbox: {
                float h = st.r;
                if (h < 0.0f) h = bullet_type_info(b.type).hitbox;
                b.hitbox = {h, h};
                ++b.ex_index;
                break;
            }
            case kExWait:
                if (st.a >= 1) {
                    b.ex_flags |= kExWait;
                    b.ex_wait_timer.set(st.a);
                }
                ++b.ex_index;
                break;
            default:
                ++b.ex_index;
                break;
        }
    }
}

// ---------------------------------------------------------------------------------------------
// Ex 每帧处理：返回 1 表示本步结束（0x411e70 据此再跑一轮 run_ex）

// 0x414ec0
int BulletManager::ex_initial_boost(Bullet& b, float gs) {
    if (b.ex1_timer.current < 0x11) {
        b.velocity = polar(b.angle, (5.0f - b.ex1_timer.current_f * 5.0f * 0.0625f) + b.speed);
        b.ex1_timer.tick(gs);
        return 0;
    }
    b.ex_flags ^= kExInitialBoost;
    return 1;
}

// 0x414fb0
int BulletManager::ex_accel(Bullet& b, float gs) {
    if (b.ex4_duration <= b.ex4_timer.current) { b.ex_flags &= ~kExAccel; return 1; }
    b.speed = b.ex4_acc * gs + b.speed;
    const float ay = b.ex4_acc_vec.y * gs;
    const float az = b.ex4_acc_vec.z * gs;
    b.velocity.x = b.velocity.x + b.ex4_acc_vec.x * gs;
    b.velocity.y = b.velocity.y + ay;
    b.velocity.z = b.velocity.z + az;
    if (0.0001f < std::fabs(b.velocity.x) || 0.0001f < std::fabs(b.velocity.y)) {
        b.angle = wrap_angle(atan2_f(b.velocity.y, b.velocity.x));
        b.speed = float(std::sqrt(double(b.velocity.x * b.velocity.x + b.velocity.y * b.velocity.y)));
    }
    b.ex4_timer.tick(gs);
    return 0;
}

// 0x4151e0
int BulletManager::ex_speed_ease(Bullet& b, float gs) {
    if (b.ex200000_duration <= b.ex200000_timer.current) { b.ex_flags &= ~kExSpeedEase; return 1; }
    b.speed = b.ex200000_acc * gs + b.speed;
    const float ay = b.ex200000_acc_vec.y * gs;
    const float az = b.ex200000_acc_vec.z * gs;
    b.velocity.x = b.velocity.x + b.ex200000_acc_vec.x * gs;
    b.velocity.y = b.velocity.y + ay;
    b.velocity.z = b.velocity.z + az;
    if (0.0001f < std::fabs(b.velocity.x) || 0.0001f < std::fabs(b.velocity.y))
        b.angle = wrap_angle(atan2_f(b.velocity.y, b.velocity.x));
    b.ex200000_timer.tick(gs);
    return 0;
}

// 0x4153e0
int BulletManager::ex_accel_turn(Bullet& b, float gs) {
    if (b.ex8_duration <= b.ex8_timer.current) { b.ex_flags &= ~kExAccelTurn; return 1; }
    float a = b.ex8_ang_vel * gs + b.angle;
    a = wrap_angle(a);
    a = wrap_angle(a);
    b.angle = a;
    b.speed = b.ex8_acc * gs + b.speed;
    b.velocity = polar(b.angle, b.speed);
    b.ex8_timer.tick(gs);
    return 0;
}

// 0x415570
int BulletManager::ex_stop_turn(Bullet& b, float gs) {
    if (b.ex10_timer.current < b.ex10_duration) {
        float spd = b.speed;
        const float drop = (b.ex10_timer.current_f * spd) / float(b.ex10_duration);
        spd = spd - drop;
        b.velocity = polar(b.angle, spd);
        b.ex10_timer.tick(gs);
        return 0;
    }
    if (b.step_sfx >= 0) play_sfx(b.step_sfx);
    b.ex10_count = b.ex10_count + 1;
    switch (b.ex10_mode) {
        case 0: case 5: b.angle = wrap_angle(b.angle + b.ex10_target_angle); break;
        case 1: case 6: b.angle = wrap_angle(angle_to_player(b.pos) + b.ex10_target_angle); break;
        case 2: case 3: case 4: b.angle = wrap_angle(b.ex10_target_angle); break;
        default: break;
    }
    const float spd = b.ex10_target_speed;
    b.speed = spd;
    b.ex10_timer.reset();
    if (b.ex10_count < b.ex10_repeats) {
        b.velocity = polar(b.angle, spd);
        b.ex10_timer.tick(gs);
        return 0;
    }
    b.velocity = polar(b.angle, spd);
    b.ex_flags &= ~kExStopTurn;
    return 1;
}

// 0x415bb0（含 0x415a10 / 0x415ae0 / 0x4158d0 / 0x415790）
int BulletManager::ex_bounce(Bullet& b, float gs) {
    (void)gs;
    const float gw = bounce_width;
    bool outside = false;
    if (gw <= 0.0f) {
        if (b.pos.x + 0.0f <= b.ex40_width * -0.5f || b.ex40_width * 0.5f <= b.pos.x) outside = true;
        else {
            const float hh = b.ex40_height * 0.5f;
            if (b.pos.y + 0.0f <= 224.0f - hh || hh + 224.0f <= b.pos.y) outside = true;
        }
        if (!outside) return 0;
    } else {
        if (gw * -0.5f < b.pos.x + 0.0f && b.pos.x < gw * 0.5f) {
            const float hh = bounce_height * 0.5f;
            if (224.0f - hh < b.pos.y + 0.0f && b.pos.y < hh + 224.0f) return 0;
        }
    }
    const u32 mask = b.ex40_wall_mask;
    const bool keep_angle = (mask & 0x10u) != 0;
    bool bounced = false;
    if ((mask & 1u) != 0) {  // 0x415a10 上边
        float h = bounce_height;
        if (h <= 0.0f) h = b.ex40_height;
        const float limit = 224.0f - h * 0.5f;
        if (!(limit <= b.pos.y)) {
            if (!keep_angle) {
                b.angle = wrap_angle(-b.angle);
                b.pos.y = (448.0f - h) - b.pos.y;
            }
            bounced = true;
        }
    }
    if ((mask & 2u) != 0) {  // 0x415ae0 下边
        float h = bounce_height;
        if (h <= 0.0f) h = b.ex40_height;
        if (!(b.pos.y < h * 0.5f + 224.0f)) {
            if (!keep_angle) {
                b.angle = wrap_angle(-b.angle);
                b.pos.y = (h + 448.0f) - b.pos.y;
            }
            bounced = true;
        }
    }
    if ((mask & 8u) != 0) {  // 0x4158d0 右边
        float w = bounce_width;
        if (w <= 0.0f) w = b.ex40_width;
        if (!(b.pos.x < w * 0.5f)) {
            if (!keep_angle) {
                float a = wrap_angle(-b.angle - kPi);
                b.angle = a;
                a = wrap_angle(a + 0.0f);
                a = wrap_angle(a);
                b.angle = a;
                b.pos.x = w - b.pos.x;
            }
            bounced = true;
        }
    }
    if ((mask & 4u) != 0) {  // 0x415790 左边
        float w = bounce_width;
        if (w <= 0.0f) w = b.ex40_width;
        if (!(w * -0.5f <= b.pos.x)) {
            if (!keep_angle) {
                float a = wrap_angle(-b.angle - kPi);
                b.angle = a;
                a = wrap_angle(a + 0.0f);
                a = wrap_angle(a);
                b.angle = a;
                b.pos.x = -w - b.pos.x;
            }
            bounced = true;
        }
    }
    if (-990.0f < b.ex40_speed) b.speed = b.ex40_speed;
    b.velocity = polar(b.angle, b.speed);
    if (bounced) {
        b.ex40_count = b.ex40_count + 1;
        if (b.step_sfx >= 0) play_sfx(b.step_sfx);
    }
    if (b.ex40_count < b.ex40_max_bounces) return 0;
    b.ex_flags &= ~kExBounce;
    return 1;
}

// 0x415d80
int BulletManager::ex_wrap(Bullet& b) {
    const AnmRuntimeSprite* sprite = main_sprite(b);
    if (sprite == nullptr) return 0;
    const float hw = sprite->width * 0.5f;
    if (-192.0f < b.pos.x + hw && b.pos.x - hw < 192.0f) {
        const float hh = sprite->height * 0.5f;
        if (0.0f < b.pos.y + hh && b.pos.y - hh < 448.0f) return 0;
    }
    const u32 mask = b.ex1000_mask;
    if ((mask & 1u) != 0 && b.pos.y < 0.0f) b.pos.y = sprite->height + 448.0f + b.pos.y;
    else if ((mask & 2u) != 0 && b.pos.y > 448.0f) b.pos.y = b.pos.y - (sprite->height + 448.0f);
    else if ((mask & 4u) != 0 && b.pos.x < -192.0f) b.pos.x = sprite->width + 384.0f + b.pos.x;
    else if ((mask & 8u) != 0 && b.pos.x > 192.0f) b.pos.x = b.pos.x - (sprite->width + 384.0f);
    else return 0;
    b.ex1000_count = b.ex1000_count + 1;
    if (b.step_sfx >= 0) play_sfx(b.step_sfx);
    if (b.ex1000_count < b.ex1000_max) return 0;
    b.ex_flags ^= kExWrap;
    return 1;
}

// 0x415f90
int BulletManager::ex_move_to(Bullet& b, float gs) {
    if (b.ex20000_duration <= b.ex20000_timer.current) {
        b.ex_flags &= ~kExMoveTo;
        b.pos = b.ex20000_target;
        b.speed = b.ex20000_saved_speed;
        b.velocity = polar(b.angle, b.ex20000_saved_speed);
        b.velocity.z = 0.0f;
        return 1;
    }
    if (b.ex20000_timer.current == 0) b.ex20000_interp.initial = b.pos;
    const Vec3 before = b.pos;
    const Vec3 p = step_interp_vec3(b.ex20000_interp, gs);
    b.velocity = {p.x - before.x, p.y - before.y, p.z - before.z};
    if (0.0001f < std::fabs(b.velocity.x) || 0.0001f < std::fabs(b.velocity.y))
        b.angle = wrap_angle(atan2_f(b.velocity.y, b.velocity.x));
    b.velocity.z = 0.0f;
    b.ex20000_timer.tick(gs);
    return 0;
}

// 0x4161f0
int BulletManager::ex_extra_velocity(Bullet& b, float gs) {
    if (b.ex80000_duration <= b.ex80000_timer.current) { b.ex_flags &= 0xfffffff6u; return 1; }
    const float vy = b.ex80000_vel.y * gs;
    const float vz = b.ex80000_vel.z * gs;
    b.pos.x = b.pos.x + b.ex80000_vel.x * gs;
    b.pos.y = b.pos.y + vy;
    b.pos.z = b.pos.z + vz;
    b.ex80000_timer.reset();
    return 0;
}

// 0x4162d0
int BulletManager::ex_bounds_guard(Bullet& b, float gs) {
    b.ex100_timer.untick(gs);
    if (b.ex100_ray_check != 0) {
        const AnmRuntimeSprite* sprite = main_sprite(b);
        const float w = sprite ? sprite->width : 0.0f;
        const float h = sprite ? sprite->height : 0.0f;
        bool inside = false;
        const float hw = w * 0.5f;
        if (-192.0f < b.pos.x + hw && b.pos.x - hw < 192.0f) {
            const float hh = h * 0.5f;
            if (0.0f < hh + b.pos.y && b.pos.y - hh < 448.0f) inside = true;
        }
        if (!inside) {
            const Vec3 dir = polar(b.angle, 1.0f);
            // 四角射线：cross ≤ 0 的取最大 dot 为左界，cross ≥ 0 的为右界；任一侧无解则结束守护
            auto corner = [&](float cx, float cy, float& cross, float& dot) {
                float x = cx - b.pos.x;
                float y = cy - b.pos.y;
                normalize2(x, y);
                cross = dir.x * y - dir.y * x;
                dot = dir.y * y + dir.x * x;
            };
            float c1, d1, c2, d2, c3, d3, c4, d4;
            corner((-384.0f - w) * 0.5f, 224.0f - (h + 448.0f) * 0.5f, c1, d1);
            corner((w + 384.0f) * 0.5f, 224.0f - (h + 448.0f) * 0.5f, c2, d2);
            corner((-384.0f - w) * 0.5f, (h + 448.0f) * 0.5f + 224.0f, c3, d3);
            corner((w + 384.0f) * 0.5f, (h + 448.0f) * 0.5f + 224.0f, c4, d4);
            float left = -999.0f, right = -999.0f;
            if (c1 <= 0.0f && -999.0f < d1 && 0.0f <= d1) left = d1;
            if (c2 <= 0.0f && left < d2 && 0.0f <= d2) left = d2;
            if (c3 <= 0.0f && left < d3 && 0.0f <= d3) left = d3;
            if (c4 <= 0.0f && left < d4 && 0.0f <= d4) left = d4;
            if (0.0f <= c1 && -999.0f < d1 && 0.0f <= d1) right = d1;
            if (0.0f <= c2 && right < d2 && 0.0f <= d2) right = d2;
            if (0.0f <= c3 && right < d3 && 0.0f <= d3) right = d3;
            if (0.0f <= c4 && right < d4 && 0.0f <= d4) right = d4;
            if (left < -998.0f || right < -998.0f) { b.ex_flags ^= kExBoundsGuard; return 1; }
        }
    }
    if (b.ex100_timer.current > 0) return 0;
    b.ex_flags ^= kExBoundsGuard;
    return 1;
}

}  // namespace th16
