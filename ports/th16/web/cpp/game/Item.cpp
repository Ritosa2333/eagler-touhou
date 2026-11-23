// th16 ItemManager——道具池、生成、每帧运动 / 吸引 / 拾取结算（锚点见 Item.hpp 头注释）。
#include "Item.hpp"

#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "GameGlobals.hpp"
#include "Gui.hpp"
#include "Motion.hpp"
#include "Player.hpp"
#include "ScorePopup.hpp"
#include "Rng.hpp"

#include <cmath>
#include <cstring>

namespace th16 {

namespace {

constexpr float kHalfPi = 1.5707964f;  // 0x494534

// DAT_004917f8：道具种类 → bullet.anm {主脚本, 副脚本}（0 / 0xf 无脚本）
constexpr i32 kItemScripts[17][2] = {
    {-1, -1}, {112, 134}, {113, 135}, {114, 136}, {115, 137}, {116, 138}, {117, 139}, {118, 140}, {119, 141},
    {120, -1}, {121, -1}, {122, -1}, {123, -1}, {124, -1}, {125, -1}, {-1, -1}, {129, -1},
};
// DAT_004917bc[9..0xe]：季节得点值
constexpr float kSeasonValues[6] = {5.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f};
constexpr int kAppearEffectScript = 0x65;   // effect.anm 出现特效
constexpr int kSeasonItemScriptBase = 0x81; // 0x430c90：季节量表道具脚本 = 0x81 + 子季节

inline bool is_season_value(i32 type) { return type >= kItemSeasonValue0 && type <= kItemSeasonValue5; }
inline bool in_rect(const Vec3& p, const Vec3& lo, const Vec3& hi) {
    return lo.x <= p.x && lo.y <= p.y && p.x <= hi.x && p.y <= hi.y;
}

}  // namespace

// ---------------------------------------------------------------------------------------------
// 0x42f440 / 0x42f0b0 / 0x42f260 / 0x4184a0

void ItemManager::create(AnmManager& anm_manager, AnmFile* bullet_anm, AnmFile* effect, GameGlobals& g, Player* pl,
                         Rng& game_rng) {
    animations = &anm_manager;
    anm = bullet_anm;
    effect_anm = effect;
    globals = &g;
    player = pl;
    rng = &game_rng;
    items.assign(size_t(kSlots), Item{});
    for (int i = 0; i < kSlots; ++i) {
        items[size_t(i)].index = i;
        items[size_t(i)].vm.owner = &items[size_t(i)];
        items[size_t(i)].vm2.owner = &items[size_t(i)];
    }
    reset();
}

void ItemManager::reset() {
    // 0x4184a0：memset 全池（VM 亦清零），两条空闲链按下标顺序头插 → 下标最大的槽最先分配
    spawn_count = 0;
    serial = 0;
    point_active = 0;
    active_count = 0;
    normal_free = nullptr;
    point_free = nullptr;
    for (int i = 0; i < kSlots; ++i) {
        Item& it = items[size_t(i)];
        it = Item{};
        // 原版 memset 整池为 0（VM 的精灵号、缩放等亦为 0，与模板复制前的快照一致）
        it.vm.clear();
        it.vm2.clear();
        it.index = i;
        it.vm.owner = &it;
        it.vm2.owner = &it;
        push_front(i < kNormalSlots ? normal_free : point_free, it);
    }
    attract_scale = 1.0f;
    sfx_queue.clear();
}

void ItemManager::push_front(Item*& head, Item& it) {
    it.next = head;
    it.prev = nullptr;
    if (head != nullptr) head->prev = &it;
    head = &it;
}

void ItemManager::unlink(Item*& head, Item& it) {
    // 0x405610（prev 为空即为链头，对应原版 prev 指向哨兵）
    if (it.next != nullptr) it.next->prev = it.prev;
    if (it.prev != nullptr) it.prev->next = it.next;
    else head = it.next;
    it.next = nullptr;
    it.prev = nullptr;
}

void ItemManager::release(Item& it) {
    // 0x430d90：state = 0 并回到所属空闲链头
    it.state = 0;
    push_front(it.index < kNormalSlots ? normal_free : point_free, it);
}

// ---------------------------------------------------------------------------------------------
// 0x430960

Item* ItemManager::spawn(i32 type, const Vec3& pos, float angle, float speed, i32 flag) {
    spawn_count = spawn_count + 1;
    if (!is_season_value(type) && type != kItemSeason) {
        Item* it = normal_free;
        if (it == nullptr) return nullptr;
        it->state = 1;
        it->pos = pos;
        if (it->pos.x <= -192.0f) it->pos.x = -192.0f;
        else if (it->pos.x >= 192.0f) it->pos.x = 192.0f;
        if (type == kItemBombFragmentAlt) globals->bomb_fragment_items = globals->bomb_fragment_items + 1;
        const i32 kind = type == kItemBombFragmentAlt ? i32(kItemBombFragment) : type;
        const Vec3 v = polar(angle, speed);
        it->velocity = {v.x, v.y, 0.0f};
        it->timer.set(0);
        it->speed = 0;
        it->attract_speed = 0;
        it->counter = 0;
        it->type = kind;
        spawn_effect(*it);
        it->type_copy = 0;
        if (anm != nullptr && kind >= 0 && kind <= 16) {
            bind_script(it->vm, kItemScripts[kind][0]);
            bind_script(it->vm2, kItemScripts[kind][1]);
        }
        it->flag = flag;
        it->vm.color[0] = it->vm.color[1] = it->vm.color[2] = it->vm.color[3] = 255;   // +0x530 = -1
        unlink(normal_free, *it);
        return it;
    }
    Item* it = point_free;
    if (it == nullptr) return nullptr;
    it->serial = serial;
    // 出现延迟：按得点池活动数分档，对生成计数取模（+0x1c972e4 原版恒 0 → 常走 % 4）
    if (point_active < 0x100) it->counter = spawn_count % 4;
    else if (point_active < 0x200) it->counter = spawn_count % 8 + 4;
    else if (point_active < 0x400) it->counter = spawn_count % 16 + 8;
    else it->counter = spawn_count % 32 + 0x10;
    it->state = 6;
    it->type = type;
    it->type_copy = type;
    it->pos = pos;
    const Vec3 v = polar(angle, speed);
    it->velocity = {v.x, v.y, 0.0f};
    it->timer.reset();
    it->angle = angle;
    it->speed = speed;
    it->flag = flag;
    unlink(point_free, *it);
    return it;
}

void ItemManager::spawn_effect(Item& it) {
    // 0x430d10
    const i32 t = it.type;
    if (t == kItemLifeFragment || t == kItemBombFragment || t == kItemBombFragmentAlt || t == kItemLife || t == kItemBomb) {
        if (effect_anm != nullptr && animations != nullptr) animations->spawn_effect(*effect_anm, kAppearEffectScript, -1, it.pos, 0.0f);
        play_sfx((t == kItemLifeFragment || t == kItemLife) ? 0x4a : 0x30);
    }
}

void ItemManager::bind_script(AnmVm& vm, int script) {
    // 0x40d460：0x407b20 模板复制 + parent / root 清零 + 0x45f980 首帧
    if (animations == nullptr || anm == nullptr || script < 0 || size_t(script) >= anm->templates.size()) return;
    animations->init_from_template(*anm, vm, script);
    animations->run(vm);
}

void ItemManager::appear(Item& it) {
    // 0x430c90：得点池道具延迟结束
    int script;
    if (it.type == kItemSeason) {
        it.state = 3;
        script = kSeasonItemScriptBase + globals->season;
    } else {
        script = kItemScripts[it.type][0];
        it.state = 2;
    }
    if (animations != nullptr && anm != nullptr && script >= 0 && size_t(script) < anm->templates.size()) {
        animations->init_from_template(*anm, it.vm, script);
        it.vm.parent = nullptr;
        it.vm.root = nullptr;
        animations->run(it.vm);
    }
    it.vm2.flags_lo &= ~1u;   // 副 VM 停更
    it.vm2.ins_offset = -1;   // +0x634 = 副 VM +0x28：停止执行
}

// ---------------------------------------------------------------------------------------------
// 0x42f4e0

float ItemManager::angle_to_player(const Vec3& from) const {
    const float dy = player->pos.y - from.y;
    const float dx = player->pos.x - from.x;
    if (dy == 0.0f && dx == 0.0f) return kHalfPi;
    return atan2_f(dy, dx);
}

i32 ItemManager::point_line() const { return globals->shot == 3 ? 0x94 : 0x80; }

void ItemManager::update(float gs) {
    point_active = 0;
    active_count = 0;
    const float scale = attract_scale;
    const float attract_base = player->sht.graze;   // .sht +0x08：吸引初速（四机体同为 5）
    const bool player_can_collect = player->state != Player::kRespawnWait && player->state != Player::kDying;
    const bool force_attract = bomb_attract || release_attract || dialogue;

    for (Item& it : items) {
        if (it.state == 0) continue;
        if (it.state == 6) {
            it.counter = it.counter - 1;
            if (it.counter >= 0) continue;
            appear(it);
            continue;   // 出现当帧不推进 VM / 计时
        }

        enum Flow { kSkip, kCollide, kFalling, kAttract };
        Flow flow = kCollide;

        // 状态 4 本体（LAB_0042fbda）：吸向自机；之后统一做自机决死检查
        auto attract_step = [&]() {
            if (!is_season_value(it.type)) globals->collect_indicator = 8;
            const float a = angle_to_player(it.pos);
            const Vec3 v = polar(a, it.attract_speed);
            it.velocity.x = v.x;
            it.velocity.y = v.y;
            it.pos.x = it.velocity.x * gs + it.pos.x;
            it.pos.y = it.velocity.y * gs + it.pos.y;
            it.pos.z = it.velocity.z * gs + it.pos.z;
            if (it.attract_speed < kMaxAttractSpeed) it.attract_speed = it.attract_speed + kAttractAccel;
        };
        // 0x42f4e0 状态 1 本体（LAB_0042f981）：自机低于收点线 / 炸弹 / 解放 / 对话 → 吸引；否则下落
        auto falling_step = [&]() -> Flow {
            if (it.counter > 0) {
                it.counter = it.counter - 1;
                if (it.counter < 1) spawn_effect(it);
                return kSkip;
            }
            if (player_can_collect && !(float(point_line()) <= player->pos.y)) return kAttract;
            if (force_attract) return kAttract;
            const float vz = it.velocity.z * gs;
            const float vx = it.velocity.x * gs;
            it.pos.y = it.velocity.y * gs * scale + it.pos.y;
            it.pos.z = vz * scale + it.pos.z;
            it.pos.x = it.pos.x + vx * scale;
            it.velocity.y = gs * kGravity * scale + it.velocity.y;
            if (it.timer.current > 0x1f) {
                if (scale >= 1.0f) {
                    it.vm.color[1] = 0xff;
                    it.vm.color[2] = 0xff;
                } else {
                    // 慢放吸引时主 VM 随机抖动（DAT_004a6d80）；th16 无此路径
                    const float r1 = rng->signed_unit();
                    const float r2 = rng->signed_unit();
                    it.vm.pos = {r2, r1, 0.0f};
                    it.vm.color[1] = 0xa0;
                    it.vm.color[2] = 0xff;
                }
                it.vm.color[3] = 0xff;
                it.vm.color[0] = 0xff;
            }
            if (0.0f <= it.velocity.y) it.velocity.x = 0;
            if (kMaxFallSpeed < it.velocity.y) it.velocity.y = kMaxFallSpeed;
            if (kBottomLimit < it.pos.y || kSideLimit <= std::fabs(it.pos.x)) {
                release(it);
                return kSkip;
            }
            return kCollide;
        };

        switch (it.state) {
        case 1:
            flow = falling_step();
            break;
        case 2: {
            it.pos.x = it.velocity.x * gs + it.pos.x;
            it.pos.y = it.pos.y + it.velocity.y * gs;
            it.pos.z = it.pos.z + it.velocity.z * gs;
            it.velocity.y = gs * kGravity + it.velocity.y;
            if (it.velocity.y < 0.0f) {
                if (it.pos.y <= kBottomLimit && std::fabs(it.pos.x) < kSideLimit) { flow = kCollide; break; }
                release(it);
                flow = kSkip;
                break;
            }
            it.attract_speed = attract_base;
            if (it.type == kItemSeason) {
                it.state = 1;
                flow = falling_step();
            } else {
                flow = kAttract;
            }
            break;
        }
        case 3: {
            const float vz = it.velocity.z * gs;
            const float vy = it.velocity.y * gs;
            it.pos.x = it.velocity.x * gs + it.pos.x;
            it.pos.z = vz + it.pos.z;
            it.pos.y = it.pos.y + vy;
            it.velocity.y = gs * kGravity + it.velocity.y;
            const float speed = it.speed - kGravity;
            it.speed = speed;
            const Vec3 v = polar(it.angle, speed);
            it.velocity.x = v.x;
            it.velocity.y = v.y;
            if (bomb_attract || release_attract) it.flag = 1;
            if (it.speed <= 0.0f) {
                it.velocity = {0.0f, 0.0f, 0.0f};
                it.speed = 0;
                it.angle = kHalfPi;
                it.attract_speed = attract_base;
                it.state = it.flag != 0 ? 4 : 1;
                flow = falling_step();
                break;
            }
            if (it.pos.y <= kBottomLimit && std::fabs(it.pos.x) < kSideLimit) { flow = kCollide; break; }
            release(it);
            flow = kSkip;
            break;
        }
        case 4:
            attract_step();
            if (player->state == Player::kDying) { it.state = 1; it.velocity.x = 0; it.velocity.y = 0; }
            flow = kCollide;
            break;
        case 5: {
            if ((player_can_collect && player->pos.y < float(point_line())) || force_attract) { flow = kAttract; break; }
            const float a = angle_to_player(it.pos);
            const Vec3 v = polar(a, it.attract_speed);
            it.velocity.x = v.x;
            it.velocity.y = v.y;
            it.pos.x = it.velocity.x * gs + it.pos.x;
            it.pos.y = it.velocity.y * gs + it.pos.y;
            it.pos.z = it.velocity.z * gs + it.pos.z;
            if (it.attract_speed < kMaxAttractSpeed) it.attract_speed = it.attract_speed + kAttractAccel;
            if (player->state == Player::kDying) { it.state = 1; it.velocity.x = 0; it.velocity.y = 0; }
            flow = kCollide;
            break;
        }
        default:
            flow = kCollide;
            break;
        }
        if (flow == kAttract) {
            // LAB_0042fbc1 / LAB_0042fbd0：从状态 1/2/5 转入吸引：重置吸引初速、状态 4，并立即走一步
            it.attract_speed = attract_base;
            it.state = 4;
            attract_step();
            if (player->state == Player::kDying) { it.state = 1; it.velocity.x = 0; it.velocity.y = 0; }
            flow = kCollide;
        }
        if (flow == kSkip) continue;

        // LAB_0042fd1d：拾取 / 吸引圈判定
        if (player->state != Player::kRespawnWait) {
            if (in_rect(it.pos, player->item_rect_min, player->item_rect_max)) {
                collect(it);
                play_sfx(0x25, it.pos.x);
                release(it);
                continue;
            }
            if (it.state != 5 && it.state != 4 && it.state != 3) {
                const bool inside = focus_held ? in_rect(it.pos, player->graze_rect_min, player->graze_rect_max)
                                               : in_rect(it.pos, player->item_rect2_min, player->item_rect2_max);
                if (inside && !is_season_value(it.type)) {
                    it.state = 5;
                    it.attract_speed = attract_base / 3.0f;
                }
            }
        }
        if ((it.vm.flags_lo & 1u) != 0) animations->run(it.vm);
        if ((it.vm2.flags_lo & 1u) != 0) animations->run(it.vm2);
        it.timer.tick(gs);
        active_count = active_count + 1;
    }
    if (attract_scale < 1.0f) attract_scale = attract_scale + 0.1f;
}

// ---------------------------------------------------------------------------------------------
// 拾取结算

void ItemManager::popup(const Vec3& pos, i32 value, u32 color) {
    if (score_popups != nullptr) score_popups->push(pos, value, color);
}

void ItemManager::record_collect(i32 value) {
    globals->collect_count = globals->collect_count + 1;
    globals->collect_sum = globals->collect_sum + value;
    globals->collect_pos = player->pos;
}

void ItemManager::add_score(i32 value) {
    globals->add_score(value, [this] {
        if (gui != nullptr) gui->celebrate_extend();
        else play_sfx(0x11);
    });
}

void ItemManager::notify_full_power() {
    if (gui != nullptr) gui->show_notice(Gui::Notice::FullPower, 0);   // 0x42bcf0(0, 2)
}

i32 ItemManager::full_point_value() const {
    const i32 q = globals->point_value / 100;
    i32 v = ((q - q % 10) / 10) * 10;
    return v < 1 ? 10 : v;
}

i32 ItemManager::scaled_point_value(float player_y, i32 line) const {
    // 0x430100 / 0x430620：按自机高度在 [line, line+450] 间线性折算得点值（整数运算，向零取整）
    const i32 q = globals->point_value / 100;
    const i32 a = (q % 10 - q) * 3;
    const i32 b = (q - q % 10) * 3;
    i32 v = (((a / 4) * (i32(player_y) - line)) / 0x1c2 + b / 4) / 10 * 10;
    return v < 1 ? 10 : v;
}

void ItemManager::collect_power(Item& it) {
    // 0x430100
    const float py = player->pos.y;
    const i32 line = point_line();
    i32 value;
    if (globals->power < globals->power_max) {
        globals->power = globals->power + 1;
        if (globals->power > globals->power_max) { globals->power = globals->power_max; notify_full_power(); }
        if ((globals->power - 1) / globals->power_per_level != globals->power / globals->power_per_level) {
            player->setup_options(*animations, *globals);
            popup(it.pos, -1, 0xffffff40u);
            play_sfx(0xd, it.pos.x);
        }
        value = 100;
    } else if (py <= float(line) || it.state == 4) {
        value = full_point_value();
        popup(it.pos, value, 0xffffff00u);
        record_collect(value);
        popup(it.pos, value, 0xffffffffu);
    } else {
        value = scaled_point_value(py, line);
        popup(it.pos, value, 0xffffffffu);
        popup(it.pos, value, 0xffffffffu);
    }
    add_score(value);
    if (py <= float(line) || it.state == 4) record_collect(value);
}

void ItemManager::collect_point(Item& it) {
    // 0x430620
    const i32 line = point_line();
    if (float(line) < player->pos.y && it.state != 4) {
        const i32 value = scaled_point_value(player->pos.y, line);
        popup(it.pos, value, 0xffffffffu);
        add_score(value);
    } else {
        const i32 value = full_point_value();
        popup(it.pos, value, 0xffffff00u);
        record_collect(value);
        add_score(value);
    }
    globals->point_items = globals->point_items + 1;
}

void ItemManager::collect_big_power(Item& it) {
    // 0x4304a0
    const float py = player->pos.y;
    const i32 per = globals->power_per_level;
    i32 value;
    if (globals->power < globals->power_max) {
        globals->power = globals->power + per;
        value = 100;
        if (globals->power > globals->power_max) { globals->power = globals->power_max; notify_full_power(); }
        if ((globals->power - per) / globals->power_per_level != globals->power / globals->power_per_level) {
            player->setup_options(*animations, *globals);
            play_sfx(0xd, it.pos.x);
            popup(it.pos, -1, 0xffffff40u);
        }
    } else {
        value = 20000;
        add_score(20000);
        popup(it.pos, 20000, 0xff808080u);
        play_sfx(0xd, it.pos.x);
    }
    add_score(value);
    if (py <= float(point_line()) || it.state == 4) record_collect(value);
}

void ItemManager::collect_full_power(Item& it) {
    // 0x4303a0
    if (globals->power >= globals->power_max) {
        globals->point_value = globals->point_value + 10000;
        if (globals->point_value > globals->point_value_max) globals->point_value = globals->point_value_max;
        popup(it.pos, 100, 0xff40ff40u);
        play_sfx(0xd, it.pos.x);
        if (globals->power >= globals->power_max) return;
    }
    const i32 max = globals->power_max;
    globals->power = globals->power + max;
    if (globals->power > max) { globals->power = max; notify_full_power(); }
    if ((globals->power - max) / globals->power_per_level != globals->power / globals->power_per_level) {
        player->setup_options(*animations, *globals);
        popup(it.pos, -1, 0xffffff40u);
        play_sfx(0xd, it.pos.x);
    }
}

bool ItemManager::add_life() {
    // 0x43df70
    if (globals->lives > 7) return false;
    globals->lives = globals->lives + 1;
    if (globals->lives > 8) globals->lives = 8;
    if (gui != nullptr) gui->refresh_lives_display(globals->lives, globals->life_fragments);
    return true;
}

void ItemManager::add_bomb_fragment() {
    // 0x43dff0：凑满 5 片进一颗（音效 0x2e），进位时与结束时各刷新一次炸弹显示
    if (globals->bombs > 7) { globals->bomb_fragments = 0; return; }
    globals->bomb_fragments = globals->bomb_fragments + 1;
    if (globals->bomb_fragments > 4) {
        globals->bombs = globals->bombs + 1;
        globals->bomb_fragments = 0;
        if (globals->bombs < 9) play_sfx(0x2e);
        else globals->bombs = 8;
        if (gui != nullptr) gui->refresh_bombs_display(globals->bombs, globals->bomb_fragments);
    }
    if (gui != nullptr) gui->refresh_bombs_display(globals->bombs, globals->bomb_fragments);
}

void ItemManager::add_bomb() {
    // 0x43dfb0
    globals->bombs = globals->bombs + 1;
    if (globals->bombs < 9) play_sfx(0x2e);
    else globals->bombs = 8;
    if (gui != nullptr) gui->refresh_bombs_display(globals->bombs, globals->bomb_fragments);
}

void ItemManager::add_point_value(float value) {
    // 0x430dc0
    globals->point_value = globals->point_value + i32(value * 100.0f);
    if (globals->point_value > globals->point_value_max) globals->point_value = globals->point_value_max;
}

bool ItemManager::add_season_gauge() {
    // 0x43de50：量表 +1（封顶），返回是否跨过子机阈值
    if (globals->season_gauge_max <= globals->season_gauge) return false;
    const i32 before = globals->season_option_count();
    i32 next = globals->season_gauge + 1;
    if (globals->season_gauge_max < next) next = globals->season_gauge_max;
    globals->season_gauge = next;
    return before != globals->season_option_count();
}

void ItemManager::collect(Item& it) {
    switch (it.type) {
    case kItemPower: collect_power(it); break;
    case kItemPoint: collect_point(it); break;
    case kItemBigPower: collect_big_power(it); break;
    case kItemLife:
        if (add_life()) {
            play_sfx(0x11);
            if (gui != nullptr) gui->show_notice(Gui::Notice::Extend, 0);   // 0x42bcf0(0, 4)
        }
        break;
    case kItemBombFragment: add_bomb_fragment(); break;
    case kItemBomb: add_bomb(); break;
    case kItemFullPower: collect_full_power(it); break;
    case kItemSeason:
        if (add_season_gauge()) {
            player->setup_options(*animations, *globals);
            popup(it.pos, -1, 0xffffff40u);
            play_sfx(0x3f, it.pos.x);
        }
        add_score(10);
        if (gui != nullptr) gui->refresh_season_gauge();   // 0x42c600
        break;
    default:
        if (is_season_value(it.type)) {
            const float value = kSeasonValues[it.type - kItemSeasonValue0];
            add_point_value(value);
            if (release_bonus != nullptr) *release_bonus = *release_bonus + value;
            const i32 q = globals->point_value / 100;
            add_score(((q - q % 10) / 100) * 10);
        }
        break;
    }
}

// ---------------------------------------------------------------------------------------------
// 0x4307a0

void ItemManager::draw(AnmRenderer& renderer, bool season_pass) {
    for (Item& it : items) {
        if (it.state == 0 || (it.vm.flags_lo & 1u) == 0 || it.counter >= 1) continue;
        if ((it.type == kItemSeason) != season_pass) continue;
        it.vm.entity_offset = it.pos;
        it.vm2.entity_offset = it.pos;
        if (it.vm.pos.y >= -8.0f) {
            renderer.draw_vm(it.vm);
            it.above_screen = 0;
            continue;
        }
        if ((it.vm2.flags_lo & 1u) != 0) {
            const float y = it.vm2.pos.y;
            it.vm2.pos.y = 8.0f;
            it.vm2.color[3] = (y + 8.0f < 32.0f) ? u8(i32((y + 8.0f) * 0.03125f * 255.0f)) : 0xff;
            renderer.draw_vm(it.vm2);
        }
        it.above_screen = 1;
    }
}

}  // namespace th16
