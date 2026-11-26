#include "Player.hpp"

#include "AnmManager.hpp"
#include "AnmGeometry.hpp"
#include "AnmRenderer.hpp"
#include "Bomb.hpp"
#include "Bullet.hpp"
#include "Effect.hpp"
#include "Enemy.hpp"
#include "GameGlobals.hpp"
#include "Gui.hpp"
#include "Input.hpp"
#include "Item.hpp"
#include "Laser.hpp"
#include "ScorePopup.hpp"
#include "Spellcard.hpp"

#include <cmath>

namespace th16 {

namespace {

// DAT_00492c20：方向 → (dx, dy)
constexpr i32 kDirectionTable[9][2] = {{0, 0}, {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
// DAT_00492c00 / DAT_00492c10：子机脚本 / 满火力附加脚本（按机体）；DAT_00492be0：子季节子机脚本（按季节）
constexpr float kPi = 3.1415927410125732f;      // 0x494588
constexpr float kHalfPi = 1.5707963705062866f;  // 0x494534
constexpr int kOptionScript[4] = {8, 7, 11, 11};
constexpr int kOptionFrontScript[4] = {9, 8, 12, 13};
constexpr int kSubOptionScript[5] = {1, 1, 1, 18, 1};
// DAT_004a5e4c（机体 × 等级 → 子机位置起始下标）/ DAT_004a5dac（季节 × 子机数）
constexpr i32 kOptionSlotBase[4][8] = {
    {28, 0, 1, 3, 6, 10, 11, 13}, {16, 0, 1, 3, 6, 10, 11, 13}, {16, 0, 1, 3, 6, 10, 11, 13}, {16, 0, 1, 3, 6, 10, 11, 13}};
constexpr i32 kSubOptionSlotBase[5][8] = {
    {0, 0, 1, 3, 6, 10, 15, 21}, {28, 0, 1, 3, 6, 10, 15, 21}, {28, 0, 1, 3, 6, 10, 15, 21},
    {28, 0, 1, 3, 6, 10, 15, 21}, {28, 0, 1, 3, 6, 10, 15, 21}};
// DAT_00492c98 / DAT_00492c78 / DAT_00492c88 / DAT_00492c68：判定 / 擦弹 / 收点 / 擦弹框半径（四机体同值）
constexpr float kHitboxRadius = 3.0f, kGrazeRadius = 5.0f, kItemRadius = 60.0f, kGrazeBoxRadius = 100.0f;
constexpr i32 kSeasonSteps[8] = {0, 100, 0x82, 0xa0, 200, 0xfa, 300, 0};
constexpr float kFixedToFloat = 0.0078125f;
constexpr int kFocusEffectScript = 0x1a, kSecondEffectScript = 0x1b;

inline Vec3 sub3(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 add3(const Vec3& a, const Vec3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

}  // namespace

void Player::set_position(float x, float y) {
    // 0x440e40
    fixed_x = i32(x * 128.0f);
    fixed_y = i32(y * 128.0f);
    pos.x = float(fixed_x) * kFixedToFloat;
    pos.y = float(fixed_y) * kFixedToFloat;
    for (PlayerOption& o : options) o.snap = 1;
}

void Player::recompute_rects_init() {
    // 0x440fb0 尾部：第二组矩形用整幅半径，且 +0x2c80c 组用擦弹框半径
    rect_min = sub3(pos, hit_radius);
    rect_max = add3(pos, hit_radius);
    item_rect_min = sub3(pos, item_radius);
    item_rect_max = add3(pos, item_radius);
    graze_rect_min = sub3(pos, graze_radius);
    graze_rect_max = add3(pos, graze_radius);
    item_rect2_min = sub3(pos, graze_radius);
    item_rect2_max = add3(pos, graze_radius);
}

void Player::reset_shot_timers() {
    // 0x165cc / 0x165e0：previous = -2, current = -1, current_f = -1.0
    for (Timer* t : {&shot_timer, &shot_timer2}) {
        if ((t->flags & 1) == 0) { t->current = 0; t->previous = i32(0xfff0bdc1); t->current_f = 0; t->speed_slot = 0; t->flags |= 1; }
        t->current = -1;
        t->current_f = -1.0f;
        t->previous = -2;
    }
}

bool Player::init(AnmManager& manager, GameGlobals& globals, AnmFile* player_anm, AnmFile* sub_player_anm,
                  const u8* sht_bytes, u32 sht_size, const u8* sub_sht_bytes, u32 sub_sht_size) {
    // 0x441c60：new + 0x4093f0 VM 构造 + 两实体 0x440ec0 + memset 0；0x440fb0 初始化
    anm = player_anm;
    sub_anm = sub_player_anm;
    if (anm == nullptr || sub_anm == nullptr) { error = "player anm missing"; return false; }
    if (!sht.parse(sht_bytes, sht_size)) { error = "pl sht: " + sht.error; return false; }
    if (!sub_sht.parse(sub_sht_bytes, sub_sht_size)) { error = "pl sub sht: " + sub_sht.error; return false; }

    // 0x407b20 脚本 0 → 自机 VM；回调清零；0x45f980 跑一帧
    manager.init_from_template(*anm, vm, 0);
    manager.run(vm);
    set_position(0.0f, 400.0f);
    speed = i32(sht.speed * 128.0f);
    speed_focus = i32(sht.speed_focus * 128.0f);
    speed_diag = i32(sht.speed_diag * 128.0f);
    speed_diag_focus = i32(sht.speed_diag_focus * 128.0f);
    sht.power_per_level = 100;
    globals.power_max = sht.power_per_level * sht.max_level;
    globals.power_per_level = sht.power_per_level;
    for (int i = 0; i < 8; ++i) globals.set_season_step(i, kSeasonSteps[i]);
    globals.season_gauge_max = globals.season_threshold[7];
    reset_shot_timers();
    sht.hitbox = kHitboxRadius;
    sht.item = kItemRadius;
    sht.graze = kGrazeRadius;
    hit_radius = {sht.hitbox * 0.5f, sht.hitbox * 0.5f, 5.0f};
    item_radius = {kItemRadius * 0.5f, kItemRadius * 0.5f, 5.0f};
    graze_radius = {kGrazeBoxRadius * 0.5f, kGrazeBoxRadius * 0.5f, 5.0f};
    recompute_rects_init();
    state_timer.reset();
    timer3.reset();
    invincible.reset();
    option_lag = kOptionLag;
    option_count = 0;
    speed_mult = 1.0f;
    for (PlayerOption& o : options) o.y = i32(0xffff3800);
    for (PlayerOption& o : sub_options) o.y = i32(0xffff3800);
    flags &= ~kFlagNoShot;
    scale_active = 0;
    scale = 1.0f;
    return true;
}

void Player::kill_vm(AnmManager& manager, u32& id) {
    manager.kill(id);
    id = 0;
}

void Player::interrupt_options(AnmManager& manager) {
    // 0x440dc0
    flags &= ~kFlagOptionsGathered;
    for (PlayerOption& o : options) { manager.interrupt(o.vm, 2); manager.interrupt(o.vm_front, 2); }
    for (PlayerOption& o : sub_options) { manager.interrupt(o.vm, 2); manager.interrupt(o.vm_front, 2); }
    gather_counter = 0;
}

void Player::gather_options(AnmManager& manager) {
    // 0x42ca80
    flags |= kFlagOptionsGathered;
    for (PlayerOption& o : options) { manager.interrupt(o.vm, 3); manager.interrupt(o.vm_front, 3); }
    for (PlayerOption& o : sub_options) { manager.interrupt(o.vm, 3); manager.interrupt(o.vm_front, 3); }
    gather_counter = 0;
}

void Player::reset(AnmManager& manager, GameGlobals& globals) {
    // 0x441740
    state = kNormal;
    reset_shot_timers();
    state_timer.reset();
    timer2.reset();
    timer3.reset();
    flags &= ~0x9u;
    kill_vm(manager, focus_vm);
    kill_vm(manager, effect_vm);
    if (gui != nullptr) gui->refresh_lives_display(globals.lives, globals.life_fragments);   // 0x42c280
    interrupt_options(manager);
    setup_options(manager, globals);
    flags &= ~kFlagNoShot;
    speed_mult = 1.0f;
    last_damage_index = 0;
    scale_active = 0;
    scale = 1.0f;
    scale2 = 1.0f;
}

u32 Player::spawn_option_vm(AnmManager& manager, AnmFile& file, int script) {
    // 0x4440e0 内联：0x46f600 取池 → 0x407b20 → flags_hi = (hi & ~0x80000) | 0x40400 → entity_offset 0、layer 0xe、rot.z 0
    //            → 0x45f980 → creation_flags 0 → 0x46e7d0 世界链表尾；与 0x406380（layer < 0x18）路径逐位相同
    AnmVm* vm_ptr = nullptr;
    const u32 id = manager.spawn(file, script, 0xe, &vm_ptr);
    AnmVm* found = manager.find(id);
    if (found == nullptr) return 0;
    found->entity_offset.y = -32.0f;
    return id;
}

void Player::setup_options(AnmManager& manager, GameGlobals& globals) {
    // 0x4440e0（this = 实体 A）
    const int shot = globals.shot + globals.shot_aux;
    const i32 level = globals.power_per_level != 0 ? globals.power / globals.power_per_level : 0;
    power_level = level;
    if (globals.power < globals.power_max) {
        for (PlayerOption& o : options) { manager.interrupt(o.vm_front, 1); o.vm_front = 0; }
    } else if (level > 0) {
        for (int i = 0; i < level; ++i) {
            PlayerOption& o = options[i];
            manager.kill(o.vm_front);
            o.vm_front = 0;
            AnmVm* front = nullptr;
            const u32 id = manager.spawn_front(*anm, kOptionFrontScript[globals.shot], -1, &front);
            o.vm_front = manager.find(id) != nullptr ? id : 0;
            if (AnmVm* v = manager.find(id)) v->entity_offset.y = -32.0f;
        }
    }
    if (option_count != level) {
        int i = 0;
        for (; i < level && i < kOptionCount; ++i) {
            PlayerOption& o = options[i];
            o.x = fixed_x;
            o.y = fixed_y;
            manager.kill(o.vm);
            o.vm = 0;
            o.index = i;
            const i32 slot = kOptionSlotBase[shot][level] + i;
            o.offset_x = i32(sht.option_unfocused[slot].x * 128.0f);
            o.offset_y = i32(sht.option_unfocused[slot].y * 128.0f);
            o.focus_offset_x = i32(sht.option_focused[slot].x * 128.0f);
            o.focus_offset_y = i32(sht.option_focused[slot].y * 128.0f);
            const i32 ox = focus != 0 ? o.focus_offset_x : o.offset_x;
            const i32 oy = focus != 0 ? o.focus_offset_y : o.offset_y;
            o.target_x = o.x = fixed_x + ox;
            o.target_y = o.y = fixed_y + oy;
            o.vm = spawn_option_vm(manager, *anm, kOptionScript[globals.shot]);
            o.active = 2;
        }
        for (; i < kOptionCount; ++i) {
            PlayerOption& o = options[i];
            o.active = 0;
            manager.interrupt(o.vm, 1);
        }
        option_count = level;
        for (PlayerOption& o : options) o.snap = 1;
    }

    const i32 sub = globals.season_option_count();
    if (sub_count == sub) return;
    int i = 0;
    for (; i < sub && i < kSubOptionCount; ++i) {
        PlayerOption& o = sub_options[i];
        o.x = fixed_x;
        o.y = fixed_y;
        manager.kill(o.vm);
        o.vm = 0;
        o.index = i;
        const i32 slot = kSubOptionSlotBase[globals.season][sub] + i;
        o.offset_x = i32(sub_sht.option_unfocused[slot].x * 128.0f);
        o.offset_y = i32(sub_sht.option_unfocused[slot].y * 128.0f);
        o.focus_offset_x = i32(sub_sht.option_focused[slot].x * 128.0f);
        o.focus_offset_y = i32(sub_sht.option_focused[slot].y * 128.0f);
        // 原版这一段沿用主子机循环的游标（+0xbc 起、步长 0xe4，未重定位到 +0x44c）：
        // 本帧 x/y 取"第 i 个子机槽"（主子机 0..3，再接季节子机 0..）的偏移，target 也写回该槽；
        // 下一帧 0x442380 按 snap 吸附到正确目标，故只影响激活当帧。
        PlayerOption& cursor = i < kOptionCount ? options[i] : sub_options[i - kOptionCount];
        const i32 ox = focus != 0 ? cursor.focus_offset_x : cursor.offset_x;
        const i32 oy = focus != 0 ? cursor.focus_offset_y : cursor.offset_y;
        cursor.target_x = o.x = fixed_x + ox;
        cursor.target_y = o.y = fixed_y + oy;
        o.vm = spawn_option_vm(manager, *sub_anm, kSubOptionScript[globals.season]);
        o.active = 2;
    }
    for (; i < kSubOptionCount; ++i) {
        PlayerOption& o = sub_options[i];
        o.active = 0;
        manager.interrupt(o.vm, 1);
    }
    sub_count = sub;
    for (PlayerOption& o : sub_options) o.snap = 1;
}

void Player::graze(AnmManager& manager, AnmFile* effect_anm, GameGlobals& globals, const Vec3& bullet_pos) {
    globals.graze = globals.graze + 1;
    globals.graze_total = globals.graze_total + 1;
    if (globals.graze > 99999999) globals.graze = 99999999;
    if (globals.graze_total > 99999999) globals.graze_total = 99999999;
    const Vec3 mid{(bullet_pos.x + pos.x) * 0.5f, (pos.y + bullet_pos.y) * 0.5f, 0.0f};
    if (effect_anm != nullptr) manager.spawn_effect(*effect_anm, 0x18, -1, mid, 0.0f);
    if (score_popups != nullptr) score_popups->push(mid, globals.graze_total, 0xffc0c0ffu);   // 0x44a460
    play_sfx(0x2a, bullet_pos.x);
}

void Player::hit(AnmManager& manager, AnmFile* effect_anm) {
    if ((flags & kFlagSilentHit) == 0) play_sfx(2);
    if (effect_anm != nullptr) manager.spawn_effect(*effect_anm, 0x1d, -1, pos, 0.0f);
    if (spellcard != nullptr && (spellcard->flags & Spellcard::kActive) != 0) {
        // 开始 60 帧内被弹不算失败（炸弹中则记 kBombUsed）；之后奖励清零、取得失败
        if (spellcard->timer.current < 0x3c) {
            if (enemies != nullptr && enemies->bomb_active) spellcard->flags |= Spellcard::kBombUsed;
        } else {
            spellcard->bonus = 0;
            spellcard->flags &= ~(Spellcard::kBonusAvailable | Spellcard::kBombUsed);
        }
    }
    state_timer.reset();
    state = kDying;
    invincible.set(0);
    invincible.current = 6;
    invincible.current_f = 6.0f;
    invincible.previous = 5;
    manager.init_from_template(*anm, vm, 0);
    vm.update_callback = 0;
    vm.time_callback = 0;
    manager.run(vm);
}

void Player::die(AnmManager& manager, GameGlobals& globals, AnmFile* effect_anm) {
    // 0x443cd0
    if (effects != nullptr && effect_anm != nullptr) {
        const i32 slot = effects->allocate_slot();
        if (slot != -1) effects->ids[slot] = manager.spawn_effect(*effect_anm, 0x1c, -1, pos, 0.0f);
    }
    globals.lives = globals.lives - 1;
    globals.bombs = 3;
    if (gui != nullptr) {
        gui->refresh_bombs_display(3, globals.bomb_fragments);
        if (globals.lives >= 0) gui->refresh_lives_display(globals.lives, globals.life_fragments);
        gui->refresh_bombs_display(globals.bombs, globals.bomb_fragments);
    }
    state = kRespawnWait;
    state_timer.reset();
    invincible.set(0xb4);
    manager.init_from_template(*anm, vm, 0);
    vm.update_callback = 0;
    vm.time_callback = 0;
    manager.run(vm);
    for (PlayerOption& o : options) {
        o.active = 0;
        manager.interrupt(o.vm, 1);
        manager.interrupt(o.vm_front, 1);
    }
    option_count = 0;
    if (spellcard != nullptr && (spellcard->flags & Spellcard::kActive) != 0) {
        if (spellcard->timer.current < 0x3c) {
            if (enemies != nullptr && enemies->bomb_active) spellcard->flags |= Spellcard::kBombUsed;
        } else {
            spellcard->bonus = 0;
            spellcard->flags &= ~(Spellcard::kBonusAvailable | Spellcard::kBombUsed);
        }
    }
    if (enemies != nullptr) {
        enemies->counters[0] = enemies->counters[0] + 1;   // DAT_004a6dc0+0x3c（ECL -9949）
        enemies->counters[2] = 0;                          // +0x44（ECL -9947）
    }
    if (globals.miss_count < 999999) globals.miss_count = globals.miss_count + 1;
}

void Player::scatter_death_items(AnmManager& manager, GameGlobals& globals) {
    // 0x442560 case 2、第 3 帧：火力 -= 每级 / 2（不低于一级），自机正上方起 7 枚火力道具（角 base-π/8 起每枚 +π/28）速度 3
    globals.power = globals.power - globals.power_per_level / 2;
    if (globals.power < globals.power_per_level) globals.power = globals.power_per_level;
    const float dy = (pos.y - 224.0f) - pos.y;
    const float dx = 0.0f - pos.x;
    const float base = (dy == 0.0f && dx == 0.0f) ? kHalfPi : atan2_f(dy, dx);
    for (int i = 0; i < 7; ++i) {
        const float angle = ((float(i) * kPi) / 28.0f + base) - 0.39269909f;
        if (items != nullptr) items->spawn(kItemPower, pos, angle, 3.0f, 0);
    }
    setup_options(manager, globals);
}

void Player::respawn(AnmManager& manager, GameGlobals& globals) {
    // 0x442560 case 2 尾：状态 0（入场），DAT_004a5788 = 1.0，复活圆伤害源（半径 32、每帧 +16、30 帧、伤害 0x96），
    // Bomb 回 3，pos 副本 = pos，回到 (0, 480)，无敌 0x118、状态计时清零
    (void)manager;
    state = kEnter;
    game_speed_ = 1.0f;
    if (global_game_speed != nullptr) *global_game_speed = game_speed_;
    alloc_circle_damage_source(pos, 32.0f, 16.0f, 0x1e, 0x96);
    globals.bombs = 3;
    if (gui != nullptr) gui->refresh_bombs_display(3, globals.bomb_fragments);
    pos_copy = pos;
    set_position(0.0f, 480.0f);
    invincible.set(0x118);
    state_timer.reset();
}

int Player::collide_body(AnmManager& manager, AnmFile* effect_anm, const Vec3& enemy_pos, float half_size,
                         bool dialogue, bool probe) {
    // 0x4439e0
    const float dx = pos.x - enemy_pos.x;
    const float dy = pos.y - enemy_pos.y;
    float r = sht.hitbox;
    const float d2 = dx * dx + dy * dy;
    if ((flags & 0x10u) != 0) r = r * scale * 3.6f;
    if (r * r + half_size * half_size <= d2) {
        int result = 2;
        float g = 40.0f;
        if (40.0f <= half_size / 2.5f) g = half_size / 2.5f;
        if ((g + r) * (g + r) + half_size * half_size <= d2) result = 0;
        return result;
    }
    if (!dialogue) {
        if (probe) return 2;
        if (state != kRespawnWait && state != kDying && state != kState3) {
            if (invincible.current < 1) hit(manager, effect_anm);
            return 1;
        }
    }
    return 0;
}

void Player::play_sprite(AnmManager& manager, int script) {
    // 0x407b20(anm, &vm, script) + 回调清零 + 0x45f980
    manager.init_from_template(*anm, vm, script);
    manager.run(vm);
}

void Player::update_options(AnmManager& manager, PlayerOption* list, int count) {
    // 0x442380
    for (int i = 0; i < count; ++i) {
        PlayerOption& o = list[i];
        if (o.active == 0) continue;
        if ((flags & kFlagOptionsGathered) == 0) {
            const bool foc = focus != 0;
            o.target_x = fixed_x + (foc ? o.focus_offset_x : o.offset_x);
            o.target_y = fixed_y + (foc ? o.focus_offset_y : o.offset_y);
            // +0xdc 移动回调（函数指针表）：th16 的 .sht 不使用，恒 0
        } else {
            o.target_x = fixed_x;
            o.target_y = fixed_y;
            if (gather_counter > 0x1d) {
                o.active = 0;
                manager.interrupt(o.vm, 1);
                manager.interrupt(o.vm_front, 1);
                continue;
            }
        }
        bool snap_now = false;
        if (o.snap == 0) {
            const i32 lag = option_lag;
            if (lag > 0x1d) {
                const i32 dx = ((o.target_x - o.x) * lag) / 100;
                const i32 dy = ((o.target_y - o.y) * lag) / 100;
                if (dx == 0 && dy == 0) snap_now = true;
                else { o.x += dx; o.y += dy; }
            }
        } else {
            o.snap = 0;
            snap_now = true;
        }
        if (snap_now) { o.x = o.target_x; o.y = o.target_y; }
        const float fx = float(o.x) * kFixedToFloat;
        const float fy = float(o.y) * kFixedToFloat;
        if (AnmVm* v = manager.find(o.vm)) v->entity_offset = {fx, fy, 0.0f};
        if (AnmVm* v = manager.find(o.vm_front)) v->entity_offset = {fx, fy, 0.0f};
    }
}

void Player::move(AnmManager& manager, const InputState& game_input, const PlayerWorld& world, AnmFile* effect_anm,
                  float game_speed) {
    // 0x441cf0
    const u32 held = game_input.held;
    const u8 lo = u8(held);
    if ((lo & 0x50) == 0x50) direction = 5;
    else if ((lo & 0x60) == 0x60) direction = 7;
    else if ((lo & 0x90) == 0x90) direction = 6;
    else if ((lo & 0xa0) == 0xa0) direction = 8;
    else if ((held & 0x20) != 0) direction = 2;
    else if ((held & 0x10) != 0) direction = 1;
    else if ((held & 0x40) != 0) direction = 3;
    else if ((lo & 0x80) != 0) direction = 4;
    else direction = 0;

    if (world.enemy_count == 0 || timer2.current < 4) {
        focus = 0;
        option_lag = kOptionLag;
    } else {
        focus = i32((held >> 3) & 1);
    }

    const i32 dir_x = kDirectionTable[direction][0];
    const i32 dir_y = kDirectionTable[direction][1];
    i32 spd;
    if (focus == 0) {
        if (manager.find(focus_vm) != nullptr) manager.interrupt(focus_vm, 1);
        focus_vm = 0;
        spd = direction < 5 ? speed : speed_diag;
    } else {
        if (focus_vm == 0 && effect_anm != nullptr) focus_vm = manager.spawn(*effect_anm, kFocusEffectScript, 0xe);
        AnmVm* fv = manager.find(focus_vm);
        if (fv == nullptr) {
            focus_vm = 0;
        } else {
            if ((flags & kFlagScaled) == 0) {
                fv->scale2 = {1.0f, 1.0f};
            } else {
                const float grown = (scale - 1.0f) + (scale - 1.0f);
                fv->scale2 = {grown + 1.0f, grown + 1.0f};
            }
            fv->flags_lo |= 8u;
        }
        spd = direction < 5 ? speed_focus : speed_diag_focus;
    }
    const i32 vx = i32(float(spd * dir_x - i32(push.x * -128.0f)) * speed_mult);
    const i32 vy = i32(float(spd * dir_y - i32(push.y * -128.0f)) * speed_mult);

    // 精灵切换：1 开始向左 / 3 开始向右 / 2 左→正 / 4 右→正
    if (vx < 0 && last_move_x >= 0) play_sprite(manager, 1);
    else if (vx > 0 && last_move_x <= 0) play_sprite(manager, 3);
    else if (vx == 0 && last_move_x < 0) play_sprite(manager, 2);
    else if (vx == 0 && last_move_x > 0) play_sprite(manager, 4);

    last_move_x = vx;
    last_move_y = vy;
    velocity.x = float(vx) * game_speed;
    velocity.y = float(vy) * game_speed;
    if (direction != 0) last_velocity = velocity;
    velocity_iy = i32(velocity.y);
    velocity_ix = i32(velocity.x);
    fixed_x += i32(velocity.x);
    fixed_y += i32(velocity.y);
    if (fixed_x < -0x5c00) fixed_x = -0x5c00;
    else if (fixed_x > 0x5c00) fixed_x = 0x5c00;
    if (fixed_y < 0x1000) fixed_y = 0x1000;
    else if (fixed_y > 0xd800) fixed_y = 0xd800;
    pos.x = float(fixed_x) * kFixedToFloat;
    pos.y = float(fixed_y) * kFixedToFloat;
    if (AnmVm* fv = manager.find(focus_vm)) fv->entity_offset = pos;
    else focus_vm = 0;

    if ((flags & kFlagOptionsGathered) != 0) ++gather_counter;
    update_options(manager, options, kOptionCount);
    update_options(manager, sub_options, kSubOptionCount);
    if (gather_counter > 0x1d) { option_count = 0; sub_count = 0; }

    if (effect_timer.current > 0) {
        if (manager.find(effect_vm) == nullptr) {
            effect_vm = 0;
            if (effect_anm != nullptr) effect_vm = manager.spawn(*effect_anm, kSecondEffectScript, 0xe);
        }
        if (AnmVm* ev = manager.find(effect_vm)) ev->entity_offset = pos;
        effect_timer.untick(game_speed);
        if (effect_timer.current < 1) {
            manager.interrupt(effect_vm, 1);
            effect_timer.reset();
            effect_vm = 0;
        }
    }
}

void Player::update(AnmManager& manager, GameGlobals& globals, const InputState& game_input, const PlayerWorld& world,
                    AnmFile* effect_anm, const AnmGeometry& geometry, float game_speed) {
    // 0x442560
    game_speed_ = game_speed;
    switch (state) {
    case kEnter:
        // 入场：60 帧内从 y=480 升到 y=400；子机吸附
        fixed_y = 0xf000 - (state_timer.current * 0x2800) / 0x3c;
        pos.y = float(fixed_y) * kFixedToFloat;
        for (PlayerOption& o : options) o.snap = 1;
        if (state_timer.current < 0x1e) {
            // 前 30 帧：以 pos_copy 为中心、半径从 64 线性扩到 576 切断激光（先尊重免疫，再按 1/4 半径无视免疫）
            const float radius = (float(state_timer.current) * 512.0f) / 30.0f + 64.0f;
            if (laser_manager != nullptr) {
                laser_manager->cancel_radius(pos_copy, radius, 0, true);          // 0x431a70
                laser_manager->cancel_radius(pos_copy, radius * 0.25f, 0, false);
            }
        } else {
            if (bullet_manager != nullptr) bullet_manager->cancel_radius(pos, 640.0f, 0, true);   // 0x416d20
            if (laser_manager != nullptr) laser_manager->cancel_radius(pos, 640.0f, 0, false); // 0x431a70
        }
        if (state_timer.current < 0x3c) break;
        state = kNormal;
        state_timer.set(0);
        // 0x4426a9：入场完成的当帧处理炸弹与季节解放的输入边沿。
        [[fallthrough]];
    case kNormal:
        // 0x40dda0 / 0x40db20：炸弹（X 键）与季节解放（C 键）按边沿触发
        if (bomb != nullptr && bomb->usable() && (game_input.pressed & InputState::kBomb) != 0) bomb->start();
        if (season_release != nullptr && season_release->usable() && (game_input.pressed & InputState::kRelease) != 0) {
            season_release->start();
        }
        move(manager, game_input, world, effect_anm, game_speed);
        break;
    case kDying:
        if (state_timer.current <= 7) {
            // 决死猶予：炸弹可用时发动并回到状态 1（0x444070：计时 0x3c）。原版把解放的判定嵌在炸弹成功之后，
            // 而炸弹已在发动中使解放永远不可用——保留同样的嵌套以免日后误改
            if (bomb != nullptr && (game_input.pressed & InputState::kBomb) != 0 && bomb->usable()) {
                bomb->start();
                recover_from_deathbomb();
                if (season_release != nullptr && season_release->usable() &&
                    (game_input.pressed & InputState::kRelease) != 0) {
                    season_release->start();
                    recover_from_deathbomb();
                }
            }
            break;
        }
        die(manager, globals, effect_anm);
        [[fallthrough]];
    case kRespawnWait:
        if (state_timer.current == 3) scatter_death_items(manager, globals);
        if (state_timer.current > 0x1d) {
            if (globals.lives < 0 && state_timer.current == 0x1e) {
                // 0x43f350：续关画面（回放中 DAT_004a6f08+0xc == 1 不调用）；此分支额外推进一次状态计时，
                // 使续关后下一帧不再落入 == 0x1e 的判断
                if (on_game_over) on_game_over();
                state_timer.tick(game_speed);
            } else {
                respawn(manager, globals);
                // 0x4428fd：复活当帧后续的计时器和伤害源更新使用恢复后的速度。
                game_speed = game_speed_;
            }
        }
        break;
    case kState3:
        // 第 0xf 帧：激光全部终止并转得点道具
        if (state_timer.current == 0xf && laser_manager != nullptr) laser_manager->cancel_all(1);  // 0x431af0(1)
        break;
    default:
        break;
    }

    // 伤害源逐帧：运动推进、+0x04 累加、角度折回、+0x84 清零、寿命回退，< 1 失活
    for (DamageSource& d : damage_sources) {
        if ((d.flags & 1u) == 0) continue;
        d.motion.update_velocity(game_speed);
        d.motion.integrate(game_speed);
        d.f04 += d.f08;
        d.angle = wrap_angle(d.angle + d.angular_speed);
        d.f84 = 0;
        d.timer.untick(game_speed);
        if (d.timer.current < 1) d.flags &= ~1u;
    }

    if (invincible.current < 1) {
        vm.flags_lo &= ~0x60000u;
        // 残影：秋季解放（移动倍率 1.5）每帧留一枚黄色残影；土用解放（kFlagBlinkMode）留红色残影
        if ((flags & kFlagBlinkMode) == 0) {
            if (speed_mult > 1.01f) {
                if (state_timer.current % 8 < 4) {
                    vm.color2[0] = 0x00; vm.color2[1] = 0xff; vm.color2[2] = 0xff; vm.color2[3] = 0xff;
                    vm.flags_lo = (vm.flags_lo & ~0x40000u) | 0x20000u;
                }
                spawn_afterimage(manager, globals, false);
            }
        } else {
            if (state_timer.current % 8 < 4) {
                vm.color2[0] = 0x00; vm.color2[1] = 0x00; vm.color2[2] = 0xff; vm.color2[3] = 0xff;
                vm.flags_lo = (vm.flags_lo & ~0x40000u) | 0x20000u;
            }
            spawn_afterimage(manager, globals, true);
        }
    } else {
        invincible.untick(game_speed);
        if (state_timer.current == state_timer.previous || state_timer.current % 3 != 0) {
            vm.flags_lo &= ~0x60000u;
        } else {
            vm.color2[0] = 0xff; vm.color2[1] = 0x00; vm.color2[2] = 0x00; vm.color2[3] = 0xff;
            vm.flags_lo = (vm.flags_lo & ~0x40000u) | 0x20000u;
        }
    }
    speed_mult = 1.0f;
    push = {globals.push_x, globals.push_y, globals.push_z};
    manager.run(vm);

    if ((flags & kFlagScaled) == 0) {
        vm.flags_lo |= 8u;
        vm.scale = {1.0f, 1.0f};
        rect_min = sub3(pos, hit_radius);
        rect_max = add3(pos, hit_radius);
        item_rect_min = {pos.x - item_radius.x * 0.5f, pos.y - item_radius.y * 0.5f, pos.z - item_radius.z * 0.5f};
        item_rect_max = {pos.x + item_radius.x * 0.5f, pos.y + item_radius.y * 0.5f, pos.z + item_radius.z * 0.5f};
        graze_rect_min = sub3(pos, graze_radius);
        graze_rect_max = add3(pos, graze_radius);
        item_rect2_min = sub3(pos, item_radius);
        item_rect2_max = add3(pos, item_radius);
    } else {
        if (scale_active != 0) {
            // 0x4171c0 推进 +0x2c798 并把返回值写入 scale；每 3 帧一帧回到 1.0（闪烁）
            scale = step_interp_float(scale_interp, game_speed);
            if (state_timer.current == (state_timer.current / 3) * 3) vm.scale = {1.0f, 1.0f};
            else vm.scale = {scale, scale};
        } else {
            vm.scale = {scale, scale};
        }
        vm.flags_lo |= 8u;
        const float s = scale;
        rect_min = {pos.x - s * hit_radius.x, pos.y - hit_radius.y * s, pos.z - hit_radius.z * s};
        rect_max = {pos.x + hit_radius.x * s, pos.y + hit_radius.y * s, pos.z + hit_radius.z * s};
        item_rect_min = {pos.x - s * item_radius.x * 0.5f, pos.y - s * item_radius.y * 0.5f, pos.z - s * item_radius.z * 0.5f};
        item_rect_max = {pos.x + s * item_radius.x * 0.5f, pos.y + s * item_radius.y * 0.5f, pos.z + s * item_radius.z * 0.5f};
        graze_rect_min = {pos.x - s * graze_radius.x, pos.y - graze_radius.y * s, pos.z - graze_radius.z * s};
        graze_rect_max = {pos.x + s * graze_radius.x, pos.y + graze_radius.y * s, pos.z + graze_radius.z * s};
        item_rect2_min = {pos.x - s * item_radius.x, pos.y - item_radius.y * s, pos.z - item_radius.z * s};
        item_rect2_max = {pos.x + s * item_radius.x, pos.y + item_radius.y * s, pos.z + item_radius.z * s};
    }

    state_timer.tick(game_speed);
    timer2.tick(game_speed);
    timer3.tick(game_speed);

    const bool may_shoot = !world.dialogue && world.enemy_count != 0 && !world.stage_clear && timer3.current > 0x13 &&
                           (flags & kFlagNoShot) == 0 && (flags & kFlagScaled) == 0;
    if (may_shoot) {
        shoot(manager, globals, game_input, game_speed);
    } else {
        reset_shot_timers();
        shot_misc = 0;
        shot_misc_b = 0;
        // DAT_004d9e2c 音效队列：登记 0x1e / 0x37 的停止标记（循环射击音）
        stop_sfx(0x1e);
        stop_sfx(0x37);
    }
    tick_bullets(manager, globals, world, geometry, game_speed);
}

// 0x444070：决死中成功发动炸弹——回到状态 1，状态计时 0x3c
void Player::recover_from_deathbomb() {
    state_timer.set(0x3c);
    state = kNormal;
}

// 0x442560 残影段：按机体在自机位置生成 pl0N.anm 脚本 {10, 9, 0xd, 0x10}[shot]，精灵取自机当前帧；red 时整体染红
void Player::spawn_afterimage(AnmManager& manager, const GameGlobals& globals, bool red) {
    if (anm == nullptr) return;
    static constexpr int kAfterimageScript[4] = {10, 9, 0xd, 0x10};
    const u32 id = manager.spawn_effect(*anm, kAfterimageScript[globals.shot & 3], -1, pos);
    AnmVm* ghost = manager.find(id);
    if (ghost == nullptr) return;
    anm_set_sprite(*anm, *ghost, vm.sprite_index);   // 0x45edb0
    if (red) { ghost->color[0] = 0x00; ghost->color[1] = 0x00; ghost->color[2] = 0xff; ghost->color[3] = 0xff; }
}

void Player::shoot(AnmManager& manager, GameGlobals& globals, const InputState& game_input, float game_speed) {
    // 0x4455d0：主射击计时 0..0xd 环绕（0x43ac80 回退 0xe），第二计时 0..0x76 环绕（回退 0x77）
    if (state != kNormal) {
        shot_misc = 0;
        shot_misc_b = 0;
        return;
    }
    const bool shot_held = (game_input.held & InputState::kShot) != 0;
    bool skip_main = false;
    if (shot_timer.current < 0) {
        if (!shot_held) skip_main = true;
        else {
            if (shot_timer2.current < 0) shot_timer2.set(0);
            start_shot_timer(0);
        }
    }
    if (!skip_main) {
        if (shot_timer.current != shot_timer.previous) fire_sets(manager, globals, shot_timer.current, shot_timer2.current);
        if (shot_timer.current < 0xe) shot_timer.tick(game_speed);
        else if (!shot_held) shot_timer.set(-1);
        else shot_timer.rewind(0xe, game_speed);
    }
    if (shot_timer2.current < 0) return;
    if (shot_timer2.current > 0x76) {
        if (!shot_held) shot_timer2.set(-1);
        else shot_timer2.rewind(0x77, game_speed);
        return;
    }
    shot_timer2.tick(game_speed);
}

void Player::start_shot_timer(i32 t) {
    // 0x440d50
    shot_timer.set(t);
}

const ShotEntry* Player::shot_entry(u32 shot_id) const {
    const bool sub = (shot_id & 0xf0000u) != 0;
    const ShotTable& table = sub ? sub_sht : sht;
    const u32 set = sub ? ((shot_id >> 8) & 0xffu) : (shot_id >> 8);
    const u32 entry = shot_id & 0xffu;
    if (set >= table.sets.size() || entry >= table.sets[set].size()) return nullptr;
    return &table.sets[set][entry];
}

void Player::fire_sets(AnmManager& manager, GameGlobals& globals, i32 frame, i32 frame2) {
    // 0x445470：主表组 = 火力等级（低速时 + 1 + 最大等级 → 组 5..9）；子季节表组 = 量表阈值计数
    i32 set = globals.power_per_level != 0 ? globals.power / globals.power_per_level : 0;
    if (focus != 0) set = set + 1 + sht.max_level;
    auto matches = [&](const ShotEntry& e) {
        if (e.period2() == 0) return frame % i32(e.period()) == i32(e.phase());
        return frame2 % i32(e.period2()) == i32(e.phase2());
    };
    if (u32(set) < sht.sets.size()) {
        const std::vector<ShotEntry>& entries = sht.sets[size_t(set)];
        for (u32 i = 0; i < entries.size(); ++i) {
            if (matches(entries[i])) spawn_bullet(manager, globals, (u32(set) << 8) | i);
        }
    }
    const u32 sub = u32(globals.season_option_count());
    if (sub < sub_sht.sets.size()) {
        const std::vector<ShotEntry>& entries = sub_sht.sets[sub];
        for (u32 i = 0; i < entries.size(); ++i) {
            if (matches(entries[i])) spawn_bullet(manager, globals, ((sub | 0x100u) << 8) | i);
        }
    }
}

i32 Player::alloc_damage_source(const Vec3& pos, float angle, i32 lifetime, i32 damage, float size_x, float size_y) {
    // 0x444b20：从上次下标起环形找空位；满池时仍返回下一下标（不初始化）
    i32 i = last_damage_index;
    for (int tries = 0;; ++tries) {
        i = i + 1 < kDamageSourceCount ? i + 1 : 0;
        if ((damage_sources[i].flags & 1u) == 0) break;
        if (tries >= 0xff) {
            last_damage_index = i;
            return i + 1;
        }
    }
    DamageSource& d = damage_sources[i];
    d.flags = (d.flags & ~6u) | 1u;
    d.motion = MotionState{};
    d.motion.pos = pos;
    d.size_x = size_x;
    d.size_y = size_y;
    d.angle = wrap_angle(angle);
    d.angular_speed = 0.0f;
    d.timer.set(lifetime);
    d.damage = damage;
    d.f78 = 0;
    d.f7c = 9999999;
    d.f80 = 1;
    d.kind = 0;
    d.f84 = 0;
    last_damage_index = i;
    return i + 1;
}

i32 Player::alloc_circle_damage_source(const Vec3& pos, float radius, float growth, i32 lifetime, i32 damage) {
    // 0x4449b0：与 0x444b20 同样环形找空位；flags |= 3（bit1 = 圆判定，半径 f04 每帧 += f08）；角度 / 尺寸字段不写
    i32 i = last_damage_index;
    for (int tries = 0;; ++tries) {
        i = i + 1 < kDamageSourceCount ? i + 1 : 0;
        if ((damage_sources[i].flags & 1u) == 0) break;
        if (tries >= 0xff) {
            last_damage_index = i;
            return i + 1;
        }
    }
    DamageSource& d = damage_sources[i];
    d.flags = (d.flags & ~4u) | 3u;
    d.motion = MotionState{};
    d.motion.pos = pos;
    d.f04 = radius;
    d.f08 = growth;
    d.timer.set(lifetime);
    d.damage = damage;
    d.f78 = 0;
    d.f7c = 9999999;
    d.f80 = 1;
    d.kind = 0;
    d.f84 = 0;
    last_damage_index = i;
    return i + 1;
}

void Player::release_bullet(PlayerBullet& b) {
    b.active = 0;
    if (b.damage_source != 0) damage_sources[b.damage_source - 1].flags &= ~1u;
}

bool Player::spawn_bullet(AnmManager& manager, GameGlobals& globals, u32 shot_id) {
    // 0x445360：类型 2 弹按子机限一发（+0x165f8 表非 0 即跳过）；池内首个空位 → 0x444e10
    const ShotEntry* e = shot_entry(shot_id);
    if (e == nullptr) return false;
    const bool sub = (shot_id & 0xf0000u) != 0;
    int cooldown_slot = -1;
    if (e->source() != 0) {
        int k = int(e->source()) - 1;
        if (k >= 100) k -= 100;
        cooldown_slot = k + (sub ? 8 : 0);
    }
    if (e->kind() == 2 && cooldown_slot >= 0 && cooldown_slot < 16 && option_cooldown[cooldown_slot] != 0) return false;
    PlayerBullet* b = nullptr;
    for (int i = 0; i < kBulletCount; ++i) {
        if (bullets[i].active == 0) { b = &bullets[i]; b->index = i; break; }
    }
    if (b == nullptr) return false;

    // ---- 0x444e10 ----
    b->active = 1;
    b->shot_id = shot_id;
    if ((b->timer.flags & 1u) == 0) b->timer.set(0);
    b->timer.current = 0;
    b->timer.current_f = 0.0f;
    b->timer.previous = -1;
    b->damage = e->damage();
    b->hit_w = e->hit_w();
    b->hit_h = e->hit_h();
    b->flags = b->flags ^ ((u32(focus) * 2u ^ b->flags) & 2u);
    b->motion = MotionState{};
    const PlayerOption* opt = nullptr;
    if (e->source() == 0) {
        b->motion.pos = pos;
    } else {
        int k = int(e->source()) - 1;
        opt = k < 100 ? &options[k] : &sub_options[k - 100];
        b->motion.pos = {float(opt->x) * kFixedToFloat, float(opt->y) * kFixedToFloat, 0.0f};
    }
    if (e->kind() == 2 && cooldown_slot >= 0 && cooldown_slot < 16) {
        option_cooldown[cooldown_slot] = globals.power_per_level != 0 ? globals.power / globals.power_per_level : 0;
    }
    b->motion.speed = e->speed();
    float angle = e->angle();
    if (angle < 1000.0f) {
        if (angle < 995.0f || opt == nullptr) angle = wrap_angle(angle);
        else angle = wrap_angle(opt->angle);
        b->motion.angle = angle;
    } else if (opt != nullptr) {
        // 角度 ≥ 1000：子机朝向 + 随机 ±π/12，速度 + 随机 ±2（视觉 RNG DAT_004a6d88）
        const float r1 = manager.visual_rng.signed_unit();
        b->motion.angle = wrap_angle(r1 * 3.1415927f / 12.0f + opt->angle);
        const float r2 = manager.visual_rng.signed_unit();
        b->motion.speed = (r2 + r2) + e->speed();
    } else {
        b->motion.angle = wrap_angle(angle);
    }
    b->motion.update_velocity(game_speed_);
    b->motion.pos.x = (e->offset_x() - b->motion.velocity.x) + b->motion.pos.x;
    b->motion.pos.y = (e->offset_y() - b->motion.velocity.y) + b->motion.pos.y;

    AnmFile* file = sub ? sub_anm : anm;
    const int script = sub ? int(e->script()) : int(e->script()) + 5;
    b->vm = manager.spawn(*file, script, -1);
    AnmVm* vmp = manager.find(b->vm);
    if (vmp == nullptr) b->vm = 0;
    if (vmp != nullptr && (vmp->flags_hi & 0x80u) != 0) {
        vmp->flags_lo |= 4u;
        vmp->rotation.z = e->angle();
    }
    b->damage_source = alloc_damage_source(b->motion.pos, b->motion.angle, 9999999, b->damage, b->hit_w, b->hit_h);
    DamageSource& d = damage_sources[b->damage_source - 1];
    d.kind = 1;
    d.f7c = 10000000;
    d.owner = b->index;
    b->flags |= 1u;
    // 生成回调（DAT_004919c0）：1 清追踪目标；2 激光（0x446200）；4 伤害源类型 2（文低速充能弹）
    switch (e->create_callback()) {
    case 1: b->target = 0; break;
    case 2: start_laser(*b); break;
    case 4: d.kind = 2; break;
    default: break;
    }
    if (e->sfx() >= 0) play_sfx(e->sfx(), b->motion.pos.x);
    if (vmp != nullptr) vmp->entity_offset = b->motion.pos;
    return true;
}

void Player::tick_bullets(AnmManager& manager, const GameGlobals& globals, const PlayerWorld& world,
                          const AnmGeometry& geometry, float game_speed) {
    // 0x4456d0
    const float field_left = geometry.field_offset_x - 192.0f;
    const float field_top = geometry.field_offset_y;
    const float field_right = geometry.field_offset_x + 192.0f;
    const float field_bottom = field_top + 448.0f;
    for (int i = 0; i < kBulletCount; ++i) {
        PlayerBullet& b = bullets[i];
        if (b.active == 0) continue;
        const ShotEntry* e = shot_entry(b.shot_id);
        if (e == nullptr) { release_bullet(b); continue; }
        // 逐帧回调（DAT_004919a0）：1 追踪（0x445ee0 + 0x425240）；2 激光（0x446260）；5 加速 0.5/帧（0x447480）
        switch (e->tick_callback()) {
        case 2:
            tick_laser(manager, globals, world, b, *e);
            break;
        case 1:
            if (b.active != 2) {
                if (enemies == nullptr) {
                    b.target = 0;
                } else if (b.target == 0) {
                    b.target = i32(enemies->nearest_targetable_id(b.motion.pos, 256.0f));  // 0x425240
                }
                Enemy* target_enemy = (b.target != 0 && enemies != nullptr) ? enemies->find_id(u32(b.target)) : nullptr;
                if (target_enemy == nullptr) {
                    b.target = 0;
                } else if ((target_enemy->flags & 0xc000021u) == 0) {
                    const float target_a = float(std::atan2(double(target_enemy->pos.y - b.motion.pos.y),
                                                            double(target_enemy->pos.x - b.motion.pos.x)));
                    const float cur_a = b.motion.angle;
                    float diff = target_a - cur_a;
                    if (diff > 3.1415927f) diff = target_a - (cur_a + 6.2831855f);
                    else if (cur_a - target_a > 3.1415927f) diff = target_a - (cur_a - 6.2831855f);
                    if (b.timer.current >= 0x3c) {
                        b.motion.speed += 0.2f;
                        break;
                    }
                    float spd = b.motion.speed;
                    const float adiff = std::fabs(diff);
                    if (adiff < 0.7853981852531433f) {
                        if (adiff < 0.2617993950843811f) {
                            spd = (spd + 0.2f <= 16.0f) ? (spd + 0.2f) : 16.0f;
                        }
                    } else {
                        spd = (spd - 0.2f >= 4.0f) ? (spd - 0.2f) : 4.0f;
                    }
                    b.motion.angle = wrap_angle(wrap_angle(cur_a + diff * 0.07999999821186066f));
                    b.motion.speed = spd;
                    break;
                }
                const float s = b.motion.speed + 0.1f;
                b.motion.speed = s <= 16.0f ? s : 16.0f;
            }
            break;
        case 5:
            if (b.active == 1) b.motion.speed += 0.5f;
            break;
        default: break;
        }
        b.motion.update_velocity(game_speed);
        b.motion.integrate(game_speed);
        AnmVm* vmp = manager.find(b.vm);
        if (vmp == nullptr) {
            b.vm = 0;
            release_bullet(b);
            continue;
        }
        bool inside = e->kind() == 2;
        if (!inside) {
            float c[4][3] = {};
            geometry.vm_corners(*vmp, c);
            if (b.timer.current < 0xf) inside = true;
            else {
                // 0x4457df：四角已处于屏幕坐标，场地边界从 DAT_004d9d38/3c 起算。
                for (int k = 0; k < 4 && !inside; ++k) {
                    inside = field_left < c[k][0] && c[k][0] < field_right &&
                             field_top < c[k][1] && c[k][1] < field_bottom;
                }
            }
        }
        if (!inside) {
            manager.kill(b.vm);
            b.vm = 0;
            release_bullet(b);
            continue;
        }
        if (b.damage_source != 0 && (b.flags & 1u) != 0) {
            DamageSource& d = damage_sources[b.damage_source - 1];
            d.motion.pos = b.motion.pos;
            d.angle = b.motion.angle;
            d.size_x = b.hit_w;
            d.size_y = b.hit_h;
            d.damage = b.damage;
        }
        vmp->entity_offset = b.motion.pos;
        if ((vmp->flags_hi & 0x80u) != 0) {
            vmp->flags_lo |= 4u;
            vmp->rotation.z = b.motion.angle;
        }
        b.timer.tick(game_speed);
    }
}

i32 Player::hit_bullet_default(AnmManager& manager, PlayerBullet& b) {
    // 0x445e20
    AnmVm* vmp = manager.find(b.vm);
    if (vmp == nullptr) b.vm = 0;
    b.motion.pos.z = 0.1f;
    if (vmp != nullptr) {
        vmp->pending_interrupt = 1;
        vmp->entity_offset = b.motion.pos;
    }
    b.active = 2;
    b.motion.speed *= 0.125f;
    if (b.damage_source != 0) {
        damage_sources[b.damage_source - 1].flags &= ~1u;
        b.damage_source = 0;
    }
    return b.damage;
}

// ---------------------------------------------------------------------------------------------
// 魔理沙激光（.sht 类型 2）。一条激光占一个弹池项：位置每帧贴到所属子机、角度跟随 .sht 角（低速时竖直向上），
// 长度 hit_w 每帧 +18 至 512，命中敌机时截到敌机表面；伤害源是一条以激光中点为心、长 hit_w × 宽 hit_h 的旋转矩形。
// 自机弹的 flags bit0 被清掉，所以 0x4456d0 不再替它同步伤害源——同步由逐帧回调自己做。

// 0x446200
void Player::start_laser(PlayerBullet& b) {
    b.hit_w = 0.0f;
    play_sfx(0x14, pos.x);
    if (DamageSource* d = damage_source(b.damage_source)) {
        d->f80 = 1;
        d->size_x = 0.0f;
    }
    b.flags &= ~1u;
}

// 0x446260
void Player::tick_laser(AnmManager& manager, const GameGlobals& globals, const PlayerWorld& world, PlayerBullet& b,
                        const ShotEntry& e) {
    const bool sub = (b.shot_id & 0xf0000u) != 0;
    const int option_index = int(e.source()) - 1;                                    // 0 起；子季节子机为 100 起
    if (option_index < 0) return;   // .sht 里的激光都挂在子机上；无子机的激光原版会读到越界内存
    const PlayerOption& opt = option_index < 100 ? options[option_index] : sub_options[option_index - 100];
    b.motion.pos = {float(opt.x) * kFixedToFloat, float(opt.y) * kFixedToFloat, 0.0f};

    // 朝向：低速时竖直向上，否则取 .sht 角。与目标角相差 ≥ 0.001 直接对齐，否则以 0.1 比例逼近
    //（原版两分支写反了直觉，但只影响 0.001 弧度以内的尾差，照搬以保持位精确）
    const float current = b.motion.angle;
    const float target = focus == 0 ? wrap_angle(e.angle()) : -1.5707964f;
    float away = current - target;
    if (away > 3.1415927f) away = current - (target + 6.2831855f);
    else if (target - current > 3.1415927f) away = current - (target - 6.2831855f);
    away = wrap_angle(away);
    if (std::fabs(away) >= 0.001f) {
        b.motion.angle = wrap_angle(target);
    } else {
        float toward = target - current;
        if (toward > 3.1415927f) toward = target - (current + 6.2831855f);
        else if (current - target > 3.1415927f) toward = target - (current - 6.2831855f);
        toward = wrap_angle(wrap_angle(toward) * 0.1f);
        b.motion.angle = wrap_angle(wrap_angle(current + toward));
    }
    if (b.active == 2) return;   // 收束段：ANM 走完后由 0x4456d0 回收

    // 原版此处按自机 x 调激光循环音的声像（IDirectSoundBuffer::SetPan），无逻辑副作用
    DamageSource* d = damage_source(b.damage_source);
    if (b.hit_w < 512.0f) {
        b.hit_w += 18.0f;
        if (d != nullptr) d->size_x = b.hit_w;
    }
    const Vec3 half = polar(b.motion.angle, b.hit_w * 0.5f);
    if (AnmVm* vm = manager.find(b.vm)) {
        vm->flags_lo |= 8u;
        vm->sprite_size.x = b.hit_w;
        vm->flags_lo |= 0x10u;
        vm->uv_scale.x = b.hit_w * 0.001953125f;
    } else {
        b.vm = 0;
    }
    if (d != nullptr) d->motion.pos = {b.motion.pos.x + half.x, b.motion.pos.y + half.y, 0.0f};

    if (b.laser_hit == 0) {
        if (b.active != 1) { b.laser_hit = 0; return; }
        if (b.laser_hitting == 1) {
            manager.interrupt(b.vm, 3);   // 离开敌机：收起命中火花
            b.laser_hitting = 0;
        }
    }
    if (b.active == 1) {
        // 持续条件：仍按着射击、所属子机仍在、非复活等待 / 决死、发射时的火力等级未变、无对话、敌机管理器在
        const int cooldown_slot = (option_index < 100 ? option_index : option_index - 100) + (sub ? 8 : 0);
        const bool option_alive = sub ? option_index - 100 < sub_count : option_index < option_count;
        const i32 power_level_now = globals.power_per_level != 0 ? globals.power / globals.power_per_level : 0;
        const bool keep = shot_timer.current >= 0 && option_alive && state != kRespawnWait && state != kDying &&
                          option_cooldown[cooldown_slot] == power_level_now && !world.dialogue && enemies != nullptr;
        if (!keep) {
            if (d != nullptr) d->flags &= ~1u;
            b.active = 2;
            manager.interrupt(b.vm, 1);   // 收束动画
            option_cooldown[cooldown_slot] = 0;
            stop_sfx(0x14);
        }
    }
    b.laser_hit = 0;
}

// 0x446870（圆形判定路径；原版另有 flags & 0x1000 的矩形路径，th16 ECL 无设置点，未移植）
// 返回本次结算的伤害：激光每 4 帧结算一次伤害源的伤害值
i32 Player::hit_laser(AnmManager& manager, const GameGlobals& globals, PlayerBullet& b, const Vec3& enemy_pos,
                      float radius) {
    b.laser_hit = 1;
    if (b.laser_hitting == 0) {
        manager.interrupt(b.vm, 2);   // 贴上敌机：命中火花
        b.laser_hitting = 1;
    }
    DamageSource* d = damage_source(b.damage_source);
    // 把敌机位置转到激光坐标系：along 沿激光、perp 垂直激光；判定半宽 = 激光半宽 + 敌机半径
    const float half_width = (d != nullptr ? d->size_y : 0.0f) * 0.5f + radius;
    const float dx = enemy_pos.x - b.motion.pos.x;
    const float dy = enemy_pos.y - b.motion.pos.y;
    const float s = sin_f(-b.motion.angle);
    const float c = cos_f(-b.motion.angle);
    const float perp = dy * c + dx * s;
    float along = dx * c - dy * s;
    const bool inside = std::fabs(perp) <= half_width && -half_width <= along &&
                        (0.0f <= along || along * along + perp * perp <= half_width * half_width);
    if (inside) {
        // 截到敌机圆的近表面：沿激光方向回退 sqrt(1 - (perp / r)²) × r
        const float t = perp / half_width;
        along -= float(std::sqrt(double(1.0f - t * t))) * half_width;
    }
    b.hit_w = along + 8.0f;
    if (b.hit_w < 0.0f) b.hit_w = 0.0f;
    if (d != nullptr) d->size_x = b.hit_w;

    // 每隔一帧在激光末端放一枚命中火花（主射击 pl01 脚本 8 / 冬季子机 pl01sub 脚本 2），沿激光方向飘出 64
    if (b.timer.current != b.timer.previous && (b.timer.current & 1) == 0) {
        const Vec3 reach = polar(b.motion.angle, b.hit_w);
        const Vec3 tip{b.motion.pos.x + reach.x, b.motion.pos.y + reach.y, 0.0f};
        const Vec3 drift = polar(b.motion.angle, 64.0f);
        const bool sub = (b.shot_id & 0xf0000u) != 0;
        AnmFile* file = sub ? sub_anm : anm;
        if (file != nullptr) {
            const u32 id = manager.spawn_effect(*file, sub ? 2 : 8, -1, tip, 0.0f);
            if (effects != nullptr) effects->register_id(id);
            if (AnmVm* spark = manager.find(id)) {
                spark->flags_lo |= 4u;
                spark->rotation.z = b.motion.angle;
                const Vec3 push{globals.push_x, globals.push_y, globals.push_z};
                spark->pos_interp.end_time = 0x14;
                spark->pos_interp.bezier1 = push;
                spark->pos_interp.bezier2 = push;
                spark->pos_interp.mode = 4;
                spark->pos_interp.initial = push;
                spark->pos_interp.goal = {drift.x, drift.y, 0.0f};
                spark->pos_interp.timer.reset();
            }
        }
    }
    if (b.timer.current != b.timer.previous && b.timer.current % 4 == 0) return d != nullptr ? d->damage : 0;
    return 0;
}

i32 Player::apply_damage_sources(AnmManager& manager, GameGlobals& globals, const Vec3& enemy_pos, float radius,
                                 u32 enemy_id, Vec3* hit_pos, bool* bomb_hit, bool probe) {
    // 0x445a30
    if (timer3.current == timer3.previous) return 0;
    // Bomb（DAT_004a6da8）vtable+0xc 的炸弹伤害回调：Bomb 接入前为 0
    if (bomb_hit != nullptr) *bomb_hit = false;

    i32 total = 0;
    for (int i = 0; i < kDamageSourceCount; ++i) {
        DamageSource& d = damage_sources[i];
        if ((d.flags & 1u) == 0 || d.timer.current == d.timer.previous) continue;
        if (d.f80 == 0 || (d.timer.current % d.f80) != 0) continue;
        bool hit = false;
        if ((d.flags & 2u) == 0) {
            hit = rect_hits_circle(d.motion.pos.x, d.motion.pos.y, d.size_x, d.size_y, d.angle,
                                   enemy_pos.x, enemy_pos.y, radius);
        } else {
            const float sum_r = d.f04 + radius;
            const float dx = d.motion.pos.x - enemy_pos.x;
            const float dy = d.motion.pos.y - enemy_pos.y;
            hit = dx * dx + dy * dy <= sum_r * sum_r;
        }
        if (!hit) continue;
        if (enemy_id != 0) {
            if (d.f84 == i32(enemy_id)) continue;
            d.f84 = i32(enemy_id);
        }
        if (bomb_hit != nullptr && (d.flags & 4u) != 0) *bomb_hit = true;
        i32 dmg = d.damage;
        if (!probe && d.kind != 0) {
            d.f8c = i32(enemy_id);
            if (d.kind == 1 && d.owner >= 0 && d.owner < kBulletCount) {
                // 0x445d40：按 .sht 项 hit_callback（DAT_00491980 表）分派：
                //   0 → 0x445e20 默认消散；2 → 0x446870 激光截断；5 → 0x447270 文命中特效 + 0x445e20
                PlayerBullet& b = bullets[d.owner];
                const ShotEntry* se = shot_entry(b.shot_id);
                if (se != nullptr && se->hit_callback() == 2) {
                    dmg = hit_laser(manager, globals, b, enemy_pos, radius);
                } else {
                    if (se != nullptr && se->hit_callback() == 5) {
                        const float jitter = manager.game_rng.signed_unit() * 0.34906584f;
                        const float eff_angle = wrap_angle(b.motion.angle + jitter);
                        if (AnmFile* eff = manager.file(8))
                            manager.spawn_effect(*eff, 0x98, -1, b.motion.pos, eff_angle);
                    }
                    dmg = hit_bullet_default(manager, b);
                }
            } else if (d.kind == 2 && d.owner >= 0 && d.owner < kBulletCount) {
                // 0x4474a0：文低速充能弹返回当前伤害并将弹伤害降为 1
                PlayerBullet& b = bullets[d.owner];
                dmg = b.damage;
                b.damage = 1;
            }
        }
        if (!probe) d.f78 += d.damage;
        total += dmg;
        if (hit_pos != nullptr) *hit_pos = d.motion.pos;
        if (d.f7c < 9999999 && d.f7c <= d.f78) {
            d.flags &= ~1u;
            d.damage = 0;
        }
    }
    if (total > sht.damage_cap) total = sht.damage_cap;
    if (!probe && total != 0) {
        // 0x43e080：extend 时的残机显示 / 音效 0x11 / 弹出都在 Gui 侧（无 Gui 时只有音效）
        globals.add_score(total / 10 + 10, [this] {
            if (gui != nullptr) gui->celebrate_extend();
            else play_sfx(0x11);
        });
    }
    return total;
}

void Player::draw(AnmRenderer& renderer) {
    // 0x443730（绘制链 0x1d）：决死待机（state 2）不画；其余把 pos 抄进 VM entity_offset，flags_hi = (hi & ~0x80000) | 0x40000，
    // 再 0x468490 绘制。子机 / 自弹 / 判定点 VM 都挂在图层链上，由层绘制链负责
    if (state == kRespawnWait) return;
    vm.entity_offset = pos;
    vm.flags_hi = (vm.flags_hi & 0xfff7ffffu) | 0x40000u;
    renderer.draw_vm(vm);
}

}  // namespace th16
