// ItemManager 实现——原版道具/神霊管理器行为子集。
// 地址逐条登记在 artifacts/verification/r3/I/INTERFACE.md 与本文件各函数头。
// RNG：共享 game RNG（0x4dc324 位，StageSim.random / ApplicationState::game_random）。
#include "ItemManager.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

namespace th13 {

namespace {

std::string item_trace_rows;
bool item_trace_enabled = false;
i32 item_trace_frame = -1;

constexpr float kPi = 3.1415927410125732f;

// 0x472f30：x87 循环角度 wrap（[-π,π]；步长 2π，0x20 次上限同原版）。
float wrap_angle(float angle) {
    float v = angle;
    int guard = 0;
    while (v > kPi && guard <= 0x20) {
        v -= 2.0f * kPi;
        ++guard;
    }
    guard = 0;
    while (v < -kPi && guard <= 0x20) {
        v += 2.0f * kPi;
        ++guard;
    }
    return v;
}

}  // namespace

void set_item_trace(bool enabled) {
    item_trace_enabled = enabled;
    if (!enabled) item_trace_rows.clear();
}

void set_item_trace_frame(i32 frame) {
    item_trace_frame = frame;
}

void item_trace_note(const char* tag, i32 max_point, i32 score) {
    if (!item_trace_enabled) return;
    char row[192];
    std::snprintf(row, sizeof row, "f=%d note=%s max_point=%d score=%d\n",
                  item_trace_frame, tag, max_point, score);
    item_trace_rows += row;
}

std::string take_item_trace() {
    std::string result;
    result.swap(item_trace_rows);
    return result;
}

// ---- StageItemHost：生成 ----

// 0x0042f020 spawnItem：写单个"待落槽"字段域（x 钳 [-192,192]；vel =
// polar(angle, speed)（0x42f540）；state/type 见头）。type 10..14 → 神霊。
// 0x42f020 体不触 game RNG（勘误后口径：神霊的两次抽取在 spawn_value_item）。
void ItemManager::reset_items() {
    items_ = {};
    ordinary_free_.clear(); cancel_free_.clear();
    for (i32 i = 0; i < 600; ++i) ordinary_free_.push_back(i);
    for (i32 i = 600; i < kItemCapacity; ++i) cancel_free_.push_back(i);
    cancel_total_ = cancel_wave_ = 0;
    events_.clear();
}

void ItemManager::spawn_item(float x, float y, float z, i32 kind, float angle,
                             float speed) {
    if (kind >= 10 && kind <= 14) {
        spawn_value_item(kind - 10, x, y, z, 0.5f);
        return;
    }
    auto& free = kind == 9 ? cancel_free_ : ordinary_free_;
    if (!free.empty()) {
        Item& item = items_[size_t(free.back())];
        free.pop_back();
        if (kind == 9) {
            // 0x42f020 type9 分支：state5 待转、sprite = 波数/累计计数式出场
            // 延迟（0042f020.c:75-107），此后每帧 -1、<0 起跳 state2。
            i32 delay;
            if (cancel_wave_ < 0x100)
                delay = cancel_total_ & 3;
            else if (cancel_wave_ < 0x200)
                delay = (cancel_total_ & 7) + 4;
            else if (cancel_wave_ < 0x400)
                delay = (cancel_total_ & 0xf) + 8;
            else
                delay = (cancel_total_ & 0x1f) + 0x10;
            item.state = 5;
            item.type = 9;
            item.type2 = 9;
            item.sprite = delay;
            // 42f020 reads these counters but never increments them.
        } else {
            item.state = 1;
            item.type = kind;
            item.type2 = 0;
            item.sprite = 0;
        }
        item.x = kind == 9 ? x : std::clamp(x, -192.0f, 192.0f);
        item.y = y;
        item.z = z;
        item.vx = float(std::cos(double(angle)) * double(speed));
        item.vy = float(std::sin(double(angle)) * double(speed));
        item.vz = 0.0f;
        item.homing_speed = 0.0f;
        // anm 域（0x42f020：+0xB78 哨兵 -999999 后 -1（prev）、+0xB7C=0
        // （frame）、+0xB80=0（time））。
        item.anm_frame_prev = -1;
        item.anm_frame = 0;
        item.anm_time = 0.0f;
        item.flags = 1;
        item.alive = true;
        item.target_seat = -1;
        item.tracking_seat = -1;
        return;
    }
}

void ItemManager::spawn_item(const float pos[3], i32 type, float angle,
                             float speed) {
    spawn_item(pos[0], pos[1], pos[2], type, angle, speed);
}

bool ItemManager::spawn_targeted_item(float x, float y, i32 type, i32 target_seat) {
    if (ordinary_free_.empty()) return false;
    const auto slot = ordinary_free_.back();
    spawn_item(x, y, 0, type, 0, 0);
    auto& item = items_[size_t(slot)];
    item.target_seat = target_seat;
    item.state = 3;
    item.homing_speed = 4.f;
    return true;
}

void ItemManager::retire_item(Item& item) {
    // 42f4d0 returns to the head of the slot's own free list.
    const i32 slot = i32(&item - items_.data());
    item.alive = false;
    item.state = 0;
    item.target_seat = -1;
    item.tracking_seat = -1;
    (slot < 600 ? ordinary_free_ : cancel_free_).push_back(slot);
}

// 0x00438ec0 神霊生成：扫描 256 槽（步长 0x22 float），槽 +0x14 起字段。
// RNG（00438ec0.c:94-115，每件恰两次，先角后速）：
//   signed_unit() → angle = su*π*0.25 + 象限基向（id&3：0→-3π/4、1→π/4、
//   2→3π/4、3→-π/4），0x472f30 wrap；
//   unit() → speed = u*3.5 + base（[0xb]=move_speed 初值；[4] 目标速度 = 0）。
// pos 钳 x∈[-192,192]、y∈[0,384]；kind==4 落 1（门用原始 param）。
// flags（0x438ec0:54,67 XOR 化简）：bit0 = DAT_004be830>>8&1（trance 激活位
// ——经由 bind_session 读，未绑定按 0），bit1 = 2（界外反弹门）。
// 尾部溢出（0x438ec0:124-139）：kind∉{3,4} 产件后 mgr+0x8838 计数 +1，
// >9 时同位（未钳制 pos）补一件 kind3（base 0.5，再耗两次 RNG）。
void ItemManager::spawn_value_item(i32 kind, float x, float y, float z,
                                   float base_speed) {
    spawn_value_item(kind, x, y, z, base_speed, true);
}

void ItemManager::spawn_value_item(i32 kind, float x, float y, float z,
                                   float base_speed, bool sound) {
    if (sound && spirit_birth_sound) spirit_birth_sound((kind == 4 ? 1 : kind) + 0x3b, x);
    if (!random_) return;
    for (Spirit& s : spirits_) {
        if (s.alive) continue;
        const i32 kind_resolved = kind == 4 ? 1 : kind;
        s.kind = kind_resolved;
        s.x = x < -192.0f ? -192.0f : (x > 192.0f ? 192.0f : x);
        s.y = y < 0.0f ? 0.0f : (y > 384.0f ? 384.0f : y);
        s.z = z;
        // id = 池计数 +1（0x438ec0：+0x8820 先增再写 [3]）；象限 = id & 3。
        const u32 id = ++spirit_serial_;
        s.serial = id;
        s.fade_requested = false;
        const u32 quadrant = id & 3u;
        float angle = 0.0f;
        switch (quadrant) {
        case 0: GameRandom::tag(9); angle = random_->signed_unit() * kPi * 0.25f - 2.3561945f; break;
        case 1: GameRandom::tag(9); angle = random_->signed_unit() * kPi * 0.25f + 0.7853982f; break;
        case 2: GameRandom::tag(9); angle = random_->signed_unit() * kPi * 0.25f + 2.3561945f; break;
        default: GameRandom::tag(9); angle = random_->signed_unit() * kPi * 0.25f - 0.7853982f; break;
        }
        s.angle = wrap_angle(angle);
        // 0x438ec0：[0xb]（move_speed）= u*3.5 + base；[4]（speed 目标）= 0。
        s.speed = 0.0f;
        GameRandom::tag(10);
        s.move_speed = random_->unit() * 3.5f + base_speed;
        s.state = 1;
        s.anm_frame_prev = -1;
        s.anm_frame = 0;
        s.anm_time = 0.0f;
        s.frame = -1;
        s.lifetime_clock.set(0);
        s.flags = 2u | ((session_ && trance_active(*session_)) ? 1u : 0u);
        s.alive = true;
        s.tracking_seat = -1;
        // 0x438ec0 出生体内同步建 VM（0x438ff9 effect.anm 0x38 →
        // 0x43909d/0x4390be astral kind*6+1/4，均 0x46f3f0 尾插）。
        // 必须在出生当帧、本帧 0x46f170 主链 walk 之前完成：链尾序决定
        // 本帧尾插子 VM 是否被 walk 二次 tick（Pn16 s05 f11697）。
        if (spirit_spawn_animation)
            (void)spirit_spawn_animation(u32(&s - spirits_.data()), s.serial,
                                         kind_resolved, s.flags, s.x, s.y, s.z);
        break;
    }
    // 0x438ec0:124-139：溢出窗/计数块在 256 槽扫描循环结束之后无条件执行
    //（池满未 spawn 时同样执行——这正是 round-19 死亡窗分叉的根因：web 旧
    // 实现把该块放在「找到空槽 spawn 成功」分支内，池满时计数器不前进，
    // sfx id62 少 30 次且溢出 kind3 补件从未发起（f3924 game RNG 差 8 index
    // → seed 10889 vs 34447）。原生证据：/work/artifacts/decomp/jp/functions/
    // 00438ec0.c:124-139，do-while(0x100) 循环之后：
    //   if (param_3 != 3 && != 4) { 置窗 0x3c/60.0；++0x8838；
    //     if (9 < 0x8838) FUN_00438ec0(mgr, x, 3, pos, 0.5, 1.4e-45); }
    // 递归调用 param_6=1.4e-45（非零）会再播音，但 kind3 分支只置窗条件
    // 不满足（param_3==3），不二次累计。
    if (kind != 3 && kind != 4) {
        spirit_window_ = 60;
        spirit_window_clock_.set(60);
        if (++spirit_overflow_ > 9) {
            spawn_value_item(3, x, y, z, 0.5f, true);
        }
    }
}

// 0x4302a0：616 etClear 转换点登记。Stage1 无人消费，先记事件留证。
void ItemManager::on_et_clear(float x, float y, float z) {
    (void)x; (void)y;
    (void)z;
}

// ---- 步进：道具池（0x0042e380）----

// 每帧序（0x42e380 帧头：+0x79dcfc/+0x79dcf4 清零后逐槽步进 0..0xa57）：
//   1 落下：磁门 = (player state 2/4 || player_y >= 128) && !dialog →
//     pos += vel（旧速）；vy += 0.03；vy>=0 时 vx=0；vy 钳 2.0；y>472 回收
//     （无结算）。未磁 → state3 本帧即走 LAB_0042e661 追踪步。
//   2 上升（消弹灵）：pos += vel（旧速）；vy += 0.03；vy>=0 → 转 state3 并
//     当帧走追踪步（LAB_0042e621→e661）；vy<0 且 y>472 → 回收。
//   3 追踪：angle = atan2 向玩家（0x443f60，重合时 π/2）；vel = polar(angle,
//     speed)；speed = min(speed+0.2, 12)；player state 4 → 回 state1 且 vel=0
//     （LAB_0042e7d7 无 state 门，3/4 同门）。
//   4 聚焦吸附追踪：同 3（速度来源 = 玩家表速/3，转态时已除）。
//   5 待转（type9）：sprite 每帧 -1，<0 → state2（重挂 ANM 属表现层）。
//   随后 LAB_0042e7f7 收集尾（见 collect_check：收集盒 → 吸附盒）。
void ItemManager::update(const ItemCollector& player) {
    update_seats(&player, 1);
}

// M3：最近合格席（R-I7）。平距平方严格小于比较——平局保留较小席位。
const ItemCollector& ItemManager::pick_collector(const ItemCollector* seats, i32 seat_count,
                                                  float x, float y,
                                                  const ItemCollector& fallback) {
    const ItemCollector* best = nullptr;
    float best_d2 = 0;
    for (i32 i = 0; i < seat_count; ++i) {
        const ItemCollector& candidate = seats[i];
        if (!candidate.eligible) continue;  // R-I8：幽灵/排除席不参与
        const float dx = candidate.x - x, dy = candidate.y - y;
        const float d2 = dx * dx + dy * dy;
        if (!best || d2 < best_d2) {
            best = &candidate;
            best_d2 = d2;
        }
    }
    return best ? *best : fallback;
}

// M3：多席道具步进（R-I7：每件道具的确定性接收者 = 最近合格玩家，平距
// 平局取小席位；已追踪目标有效时保持；R-I8：幽灵/不合格席不参与选择）。单席调用 = 单元素数组，
// 逐件选择恒返回唯一席——与旧单席路径逐位一致（零行为变化）。
// magnet/追踪/收集盒全部按选中席求值；结算入选中席的 progress/player/score。
void ItemManager::update_seats(const ItemCollector* seats, i32 seat_count) {
    if (!seats || seat_count <= 0) return;
    const ItemCollector& fallback = seats[0];
    last_player_ = fallback;
    const float rate = fallback.progress ? fallback.progress->rate : 1.f;
    bool has_collector = false;
    bool team_autocollect = false;
    for (i32 seat = 0; seat < seat_count; ++seat) {
        const auto& candidate = seats[seat];
        has_collector |= candidate.eligible;
        team_autocollect |= candidate.eligible && !candidate.dialog &&
            candidate.state != 2 && candidate.state != 4 && candidate.y < 128.0f;
    }
    cancel_wave_ = 0;  // 0x42e380:74（type9 出场延迟的每帧波数）
    for (Item& item : items_) {
        if (!item.alive) continue;
        if (item.target_seat >= 0 || (item.state != 3 && item.state != 4))
            item.tracking_seat = -1;
        if (item.tracking_seat >= seat_count ||
            (item.tracking_seat >= 0 && !seats[item.tracking_seat].eligible))
            item.tracking_seat = -1;
        // A gift keeps its named recipient, including while that seat is
        // temporarily unable to collect. It never becomes a broadcast drop.
        if (item.target_seat >= 0 &&
            (item.target_seat >= seat_count || !seats[item.target_seat].eligible)) continue;
        // Gifts retain their explicit transaction recipient. Ordinary drops
        // acquire the nearest receiver when homing starts and keep it while valid.
        const ItemCollector& player = item.target_seat >= 0 ? seats[item.target_seat]
            : item.tracking_seat >= 0 ? seats[item.tracking_seat]
            : pick_collector(seats, seat_count, item.x, item.y, fallback);
        // R-I7 selects the physical collector; R-I3 broadcasts only personal
        // rewards. Shared score/gauge and item retirement still occur once.
        last_player_ = player;
        const bool magnet = !has_collector || (!team_autocollect &&
            player.eligible && !player.dialog && (player.state == 2 || player.state == 4 ||
                               player.y >= 128.0f));
        switch (item.state) {
        case 1: {
            if (magnet) {
                item.x += float(item.vx * rate);
                item.y += float(item.vy * rate);
                item.z += float(item.vz * rate);
                item.vy += float(0.03f * rate);
                if (item.vy >= 0.0f) item.vx = 0.0f;
                if (item.vy > 2.0f) item.vy = 2.0f;
                if (item.y > 472.0f) {
                    retire_item(item);  // 0x42e380：底部回收，无结算
                    break;
                }
            } else {
                item.homing_speed = player.homing_speed;
                item.state = 3;
                home_step(item, player);  // 0x42e380 直落 LAB_0042e661
            }
            collect_check(item, player, seats, seat_count);
            break;
        }
        case 2: {
            item.x += float(item.vx * rate);
            item.y += float(item.vy * rate);
            item.z += float(item.vz * rate);
            item.vy += float(0.03f * rate);
            if (item.vy >= 0.0f && has_collector) {
                item.homing_speed = player.homing_speed;
                item.state = 3;
                if (has_collector) home_step(item, player);  // LAB_0042e621→e661
                collect_check(item, player, seats, seat_count);
                break;
            }
            if (item.y > 472.0f) {
                retire_item(item);  // 0x42f4d0 回收（无结算）
                break;
            }
            collect_check(item, player, seats, seat_count);
            break;
        }
        case 3:
        case 4:
            if (has_collector) home_step(item, player);
            collect_check(item, player, seats, seat_count);
            break;
        case 5: {
            // state5 不进收集尾（0x42e380 的 if 臂直落下一槽）。
            --item.sprite;
            if (item.sprite < 0) item.state = 2;
            continue; // original state5 arm bypasses the timer tail, even after switching to2
        }
        default:
            break;
        }
        item.tracking_seat = item.alive && item.target_seat < 0 && seat_count >= 2 &&
            player.eligible && (item.state == 3 || item.state == 4)
            ? i32(&player - seats) : -1;
        if (item.alive && item.state != 5) {
            item.anm_frame_prev = item.anm_frame;
            item.anm_time = float(item.anm_time + (rate > .99f && rate < 1.01f ? 1.f : rate));
            item.anm_frame = truncate_int(item.anm_time);
        }
    }
}

// state3/4 共用步进（0x42e380 LAB_0042e661 / state4 分支）。
// 追踪初速上限 12、增量 0.2（3/4 同值；「4 慢速」是神霊 0x438730 的语义）。
void ItemManager::home_step(Item& item, const ItemCollector& player) {
    const float rate = player.progress ? player.progress->rate : 1.f;
    float dx = player.x - item.x;
    float dy = player.y - item.y;
    float angle = (dx == 0.0f && dy == 0.0f) ? 1.5707964f
                    : float(std::atan2(double(dy), double(dx))); // 443f60 x87 result -> f32
    angle = wrap_angle(angle);
    // 42f540 stores only after sin/cos times speed, not between them.
    item.vx = float(std::cos(double(angle)) * double(item.homing_speed));
    item.vy = float(std::sin(double(angle)) * double(item.homing_speed));
    item.x += float(item.vx * rate);
    item.y += float(item.vy * rate);
    item.z += float(item.vz * rate);
    if (item.homing_speed < 12.0f) item.homing_speed += 0.2f;
    // LAB_0042e7d7：player state 4（决死窗）→ 任意追踪态回落 state1、vel 清零
    //（原版此处无 state 门，3 与 4 同门；player state 2 不触发）。
    if (player.state == 4) {
        item.state = 1;
        item.vx = 0.0f;
        item.vy = 0.0f;
    }
}

// 收集判定 + type 分派（0x42e380 LAB_0042e7f7 与 0x42e8ad jump 表）。
// 次序：player state==2 → 直接跳过整段（不收集不吸附）。
//   ① 收集盒 box（+0x146EC，单一无聚焦变体）命中 → switch(type) 结算 →
//      合并尾（收集音 0x460c50 → VM 中断 0x46fc30 → 0x42f4d0 出链回收）。
//   ② 未命中且 state∉{3,4}：吸附盒（聚焦持键 DAT_004e4c08&8 → +0x14704..，
//      否则 +0x1471C..，asm 0x42ebb9）命中 → state4、追踪初速 =
//      *(player+0x660 表+8)/3（collector.homing_speed/3）。
void ItemManager::collect_check(Item& item, const ItemCollector& player,
                                 const ItemCollector* seats, i32 seat_count) {
    // 一次性静态读取：逐道具逐帧的 getenv 死调用（wasm 侧恒空）清出热路径。
    static const bool item_dbg = std::getenv("TH13_ITEM_DBG") != nullptr;
    if (item_dbg)
        std::fprintf(stderr, "COLLECTCHK state=%d type=%d pos=(%g,%g) box=[%g,%g,%g,%g]\n",
                     player.state, item.type, item.x, item.y, player.box[0],
                     player.box[1], player.box[2], player.box[3]);
    if (player.state == 2 || !player.eligible) return;
    if (item.x < player.box[0] || item.y < player.box[1] ||
        item.x > player.box[2] || item.y > player.box[3]) {
        // 吸附盒（0x42e380:305-315）：state 3/4 已在追踪，不再触发。
        if (item.state == 3 || item.state == 4) return;
        const std::array<float, 4>& auto_box =
            player.focus ? player.box_auto_focus : player.box_auto;
        if (item.x < auto_box[0] || item.y < auto_box[1] ||
            item.x > auto_box[2] || item.y > auto_box[3])
            return;
        item.homing_speed = player.homing_speed / 3.0f;
        item.state = 4;
        return;
    }
    SessionState& progress = *player.progress;
    HudPlayerState& hud = *player.player;
    HudScoreState& score_state = *player.score;
    if (item.target_seat >= 0) {
        // R-I9: debit happened at the fifth press; only this recipient owns
        // the delivered resource and its HUD update.
        if (item.type == ItemExtraLife) {
            hud.extra_lives = std::min(8, hud.extra_lives + 1);
            if (life_changed)
                life_changed(item.target_seat, std::max(0, hud.extra_lives), hud.life_pieces);
        } else if (collect_power_big(hud.power_step)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y, 0xffffff40u});
        }
        events_.push_back({ItemEvent::Kind::Collected, item.type, item.x, item.y});
        retire_item(item);
        return;
    }
    if (seat_count >= 2 && item.target_seat < 0 &&
        item.type != ItemPoint && item.type != ItemSpiritDrop) {
        ResourceGrant grant;
        switch (item.type) {
        case ItemPower: grant.power = 1; break;
        case ItemBigPower: case ItemBigPowerAlt: grant.power_steps = 1; break;
        case ItemLifePiece: grant.life_pieces = 1; break;
        case ItemBombPiece: grant.bomb_pieces = 2; break;
        case ItemExtraLife: grant.lives = 1; break;
        case ItemBomb: grant.bombs = 1; break;
        default: break;
        }
        const bool power_item = grant.power || grant.power_steps;
        const bool full_before = power_item && all_players_full_power(seats, seat_count);
        const bool level_up = grant_resources(grant, seats, seat_count);
        if (full_before) {
            if (item.type == ItemBigPowerAlt) add_max_point(100.f);
            else score_state.score = std::min(999999999,
                score_state.score + (item.type == ItemPower ? 1000 : 2000));
            events_.push_back({ItemEvent::Kind::ScorePopup,
                item.type == ItemPower ? 10000 : item.type == ItemBigPower ? 20000 : 100,
                item.x, item.y, item.type == ItemPower ? 0xffffffffu :
                item.type == ItemBigPower ? 0xff808080u : 0xff40ff40u});
            if (item.type != ItemPower) push_level_up(item);
        } else if (power_item && all_players_full_power(seats, seat_count)) {
            events_.push_back({ItemEvent::Kind::FullPower, 0, item.x, item.y});
        }
        if (level_up) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y, 0xffffff40u});
        }
        events_.push_back({ItemEvent::Kind::Collected, item.type, item.x, item.y});
        retire_item(item);
        return;
    }
    switch (item.type) {
    case 1:  // 小P（0x42edc0）
        if (collect_power_small()) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            // 0x42edc0: 跨 be7f0 档 → 0x44ad60(-1, 道具位, 0xffffff40)
            // 哨兵弹字（digit10→sprite206）。漏掉会改变 10 槽环形池的
            // 游标，使后续弹字绘制顺序与原生相反（5507 首批次序实证）。
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y,
                               0xffffff40u});
        }
        break;
    case 2: {  // 点（0x42e380 case2）：得分 = 显示值/10（0x413bf0）
        const i32 value = point_value(player.y, item.state == 3);
        score_state.score += value / 10;
        if (score_state.score > 999999999) score_state.score = 999999999;
        ++point_items_;  // 0x4be7d8
        ++progress.point_items;  // 同一计数进会话态（跨关累计；stage 头 +0x34 快照源）
        events_.push_back({ItemEvent::Kind::ScorePopup, value, item.x, item.y,
                           (player.y > 128.0f && item.state != 3)
                               ? 0xffffffffu : 0xffffff00u});
        break;
    }
    case 3: {  // 大P（0x42e959）
        if (hud.power >= hud.max_power) {
            // 0x413bc0(&be7b8, 20000)：score += 20000/10 = 2000 原值
            //（显示 +20000）；popup 0x4e20（无 0x42a430 演出）。
            score_state.score += 20000 / 10;
            if (score_state.score > 999999999) score_state.score = 999999999;
            events_.push_back({ItemEvent::Kind::ScorePopup, 20000, item.x, item.y,
                               0xff808080u});
            events_.push_back({ItemEvent::Kind::PowerLevelUp, 0, item.x, item.y});
        }
        if (collect_power_big(hud.power_step)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            // 0x42e959: FUN_0043d210 返回跨档 → popup -1（0xffffff40）。
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y,
                               0xffffff40u});
        }
        break;
    }
    case 4:  // 残机碎片（0x43d330）
        collect_life_piece();
        break;
    case 5:  // 灵击碎片 +2（0x43d480）
        collect_bomb_piece(2);
        break;
    case 6:  // 1UP（0x43d2f0）
        collect_extra_life();
        break;
    case 7:  // 灵击（0x43d440）
        collect_bomb();
        break;
    case 8: {  // 大P 变体（0x42e8bd）
        if (hud.power >= hud.max_power) {
            // 0042e8ca loads 004aeaf0=100.0; 0042f510 converts to raw x100.
            add_max_point(100.0f);
            events_.push_back({ItemEvent::Kind::ScorePopup, 100, item.x, item.y,
                               0xff40ff40u});
            events_.push_back({ItemEvent::Kind::PowerLevelUp, 0, item.x, item.y});
        }
        if (collect_power_big(hud.power_step)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            // 0x42e8bd: FUN_0043d210 返回跨档 → popup -1（0xffffff40）。
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y,
                               0xffffff40u});
        }
        break;
    }
    case 9: {  // 消弹灵（0x42eb46）：0x42f510(0.1) raw max_point+10；
               // 0x413bf0(&be7b8,10) score += 10/10 = 1 原值（显示 +10）。
        const i32 trace_mp = progress.max_point;
        const i32 trace_sc = score_state.score;
        add_max_point(0.1f);
        score_state.score += 1;
        if (score_state.score > 999999999) score_state.score = 999999999;
        if (item_trace_enabled) {
            char row[192];
            std::snprintf(row, sizeof row,
                          "f=%d item9 max_point=%d->%d score=%d->%d\n",
                          item_trace_frame, trace_mp, progress.max_point,
                          trace_sc, score_state.score);
            item_trace_rows += row;
        }
        break;
    }
    default:
        break;
    }
    events_.push_back({ItemEvent::Kind::Collected, item.type, item.x, item.y});
    retire_item(item);  // 0x42f4d0：出链回收
}

// R-I3/R-I4: this grant contains personal quantities only. Never call the
// complete single-player collector once per seat: it also changes team score,
// gauge and point counts. Ghosts receive bookkeeping without being revived.
bool ItemManager::grant_resources(const ResourceGrant& grant,
                                  const ItemCollector* seats, i32 seat_count) {
    bool level_up = false, extended = false, bomb_gained = false;
    for (i32 seat = 0; seat < seat_count; ++seat) {
        auto& hud = *seats[seat].player;
        const i32 power_before = hud.power;
        hud.power = std::min(hud.max_power,
                            hud.power + grant.power + grant.power_steps * hud.power_step);
        level_up |= power_before / hud.power_step != hud.power / hud.power_step;
        if (grant.life_pieces) {
            hud.life_pieces += grant.life_pieces;
            while (hud.life_pieces >= life_piece_threshold(hud.life_piece_threshold_index)) {
                hud.life_pieces -= life_piece_threshold(hud.life_piece_threshold_index++);
                if (hud.extra_lives < 8) { ++hud.extra_lives; extended = true; }
            }
        }
        if (grant.bomb_pieces) {
            if (hud.bombs >= 9) hud.bomb_pieces = 0;
            else {
                hud.bomb_pieces += grant.bomb_pieces;
                if (hud.bomb_pieces > 7) {
                    hud.bomb_pieces = 0; ++hud.bombs; bomb_gained = true;
                }
            }
        }
        hud.extra_lives = std::min(8, hud.extra_lives + grant.lives);
        if (grant.bombs && hud.bombs < 9) { ++hud.bombs; bomb_gained = true; }
    }
    if (extended) events_.push_back({ItemEvent::Kind::Extend, 1, last_player_.x, last_player_.y});
    if (bomb_gained) events_.push_back({ItemEvent::Kind::BombGained, 1, last_player_.x, last_player_.y});
    // The retail HUD reads the old session address; expose seat 0 there,
    // while each seat's own threshold remains authoritative.
    if (grant.life_pieces)
        seats[0].progress->life_piece_threshold_index = seats[0].player->life_piece_threshold_index;
    // R-I3 广播发放：每席各触发一次，携带该席权威值（幽灵席照发——
    // 团队账面奖励保留，R-I8）。仅在该类资源确实入账时触发，避免无关
    // 席位重发同标签复位图标插值。
    if ((grant.lives || grant.life_pieces) && life_changed)
        for (i32 seat = 0; seat < seat_count; ++seat)
            life_changed(seat, std::max(0, seats[seat].player->extra_lives),
                         seats[seat].player->life_pieces);
    if ((grant.bombs || grant.bomb_pieces) && bombs_changed)
        for (i32 seat = 0; seat < seat_count; ++seat)
            bombs_changed(seat, seats[seat].player->bombs, seats[seat].player->bomb_pieces);
    return level_up;
}

bool ItemManager::all_players_full_power(const ItemCollector* seats, i32 seat_count) {
    for (i32 seat = 0; seat < seat_count; ++seat)
        if (seats[seat].eligible && seats[seat].player->power < seats[seat].player->max_power)
            return false;
    return true;
}

void ItemManager::push_level_up(const Item& item) {
    events_.push_back({ItemEvent::Kind::PowerLevelUp, 0, item.x, item.y});
}

// ---- 0x4be7b8 进度域写点 ----

// 0x0042edc0 小P：满灵力（be7e8 >= be7ec）→ score += 1000（原值，cap
// 999999999）+ popup 10000（显示）；否则 be7e8 += 1，超 be7ec 钳 + 满灵力
// 演出；跨 be7f0 档（(v-1)/step != v/step）→ 返回 true（0x444df0 options 重排）。
bool ItemManager::collect_power_small() {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    HudScoreState& score_state = *c.score;
    if (hud.power >= hud.max_power) {
        score_state.score += 1000;
        if (score_state.score > 999999999) score_state.score = 999999999;
        events_.push_back({ItemEvent::Kind::ScorePopup, 10000, c.x, c.y,
                           0xffffffffu});
        return false;
    }
    const i32 before = hud.power;
    hud.power = before + 1;
    if (hud.power > hud.max_power) {
        hud.power = hud.max_power;
        events_.push_back({ItemEvent::Kind::FullPower, 0, c.x, c.y});
    }
    return before / hud.power_step != hud.power / hud.power_step;
}

// 0x0043d210 大P：be7e8 >= be7ec → false（上限钳制在 case3/8 的满灵力分支）；
// 否则 power += amount，超 be7ec 钳 + 满灵力演出；返回是否跨 be7f0 档。
bool ItemManager::collect_power_big(i32 amount) {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    if (hud.power >= hud.max_power) return false;
    const i32 before = hud.power;
    hud.power = before + amount;
    if (hud.power > hud.max_power) {
        hud.power = hud.max_power;
        events_.push_back({ItemEvent::Kind::FullPower, amount, c.x, c.y});
    }
    return before / hud.power_step != hud.power / hud.power_step;
}

// 0x0043d330 adds EAX pieces; while (0x4bb994[be7fc] <= be7f8):
// be7f8 -= 阈值；be7f4 < 8 → +1 命（cap 8，sound 0x11）+ 0x42a430 演出；
// be7fc += 1（下标推进）。be7f4 > 8 → be7f8 = 0 直接收。
void ItemManager::collect_life_piece(i32 amount) {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    SessionState& progress = *c.progress;
    if (hud.extra_lives > 8) {
        hud.life_pieces = 0;
        return;
    }
    hud.life_pieces += amount;
    while (life_piece_threshold(progress.life_piece_threshold_index) <=
           hud.life_pieces) {
        hud.life_pieces -= life_piece_threshold(progress.life_piece_threshold_index);
        if (hud.extra_lives < 8) {
            hud.extra_lives += 1;
            if (hud.extra_lives > 8) hud.extra_lives = 8;
            if (life_changed) life_changed(0, hud.extra_lives, hud.life_pieces);
            events_.push_back({ItemEvent::Kind::Extend, 1, c.x, c.y});
        }
        progress.life_piece_threshold_index += 1;
    }
    if (life_changed) life_changed(0, hud.extra_lives, hud.life_pieces);
}

// 0x0043d480 灵击碎片 +amount：be800(+0x48) > 8 → be804=0 收；
// be804 += amount；be804 > 7 → 归零、be800 = min(be800+1, 9)（先 +1 后
// ≥10 钳 9，0043d480.c:11-17；sound 0x2e 在 <10 支路）。
void ItemManager::collect_bomb_piece(i32 amount) {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    if (hud.bombs > 8) {
        hud.bomb_pieces = 0;
        return;
    }
    hud.bomb_pieces += amount;
    if (hud.bomb_pieces > 7) {
        hud.bomb_pieces = 0;
        hud.bombs += 1;
        if (hud.bombs > 9) hud.bombs = 9;
        else events_.push_back({ItemEvent::Kind::BombGained, hud.bombs, c.x, c.y});
        if (bombs_changed) bombs_changed(0, hud.bombs, hud.bomb_pieces);
    }
    if (bombs_changed) bombs_changed(0, hud.bombs, hud.bomb_pieces);
}

// 0x0043d2f0 1UP：be7f4 > 7 → 收；否则 +1、>8 钳 8。
void ItemManager::collect_extra_life() {
    HudPlayerState& hud = *last_player_.player;
    if (hud.extra_lives > 7) return;
    hud.extra_lives += 1;
    if (hud.extra_lives > 8) hud.extra_lives = 8;
    if (life_changed) life_changed(0, hud.extra_lives, hud.life_pieces);
}

// 0x0043d440 灵击：be7f4(+0x48) += 1；< 10 → sound 0x2e；否则钳 9。
void ItemManager::collect_bomb() {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    hud.bombs += 1;
    if (hud.bombs < 10) {
        events_.push_back({ItemEvent::Kind::BombGained, hud.bombs, c.x, c.y});
    } else {
        hud.bombs = 9;
    }
    if (bombs_changed) bombs_changed(0, hud.bombs, hud.bomb_pieces);
}

// 0x0043d530 灵界值：be808 += amount（cap 600）；跨 200 段 → sound 0x44 +
// 段位演出（事件面）。与 SessionState::trance_add 同式（含 0x42e380 路径）。
void ItemManager::collect_trance(i32 amount) {
    SessionState& progress = *last_player_.progress;
    const i32 before = progress.ending_points;
    trance_add(progress, amount);
    if (before / 200 != progress.ending_points / 200) {
        events_.push_back({ItemEvent::Kind::SpiritGained,
                           progress.ending_points / 200, last_player_.x,
                           last_player_.y});
    }
    // 0x43d530:33-35：每次加值后（ANM 管理器非空）都调 0x42b030，与是否
    // 跨 200 段无关；封顶 600 时也一样。
    events_.push_back({ItemEvent::Kind::TranceGained, progress.ending_points,
                       last_player_.x, last_player_.y});
}

// 点值（0x42e380 case2，asm 0x42e9e5-0x42ea94 直读；A = be7dc/100）：
//   高位（player_y < 128 或 state==3）→ D = A - A%10（<1 → 10，popup 色
//   0xffffff00）；
//   低位（player_y >= 128 且 state!=3）→
//     value = (( floor4(3*(A%10-A)) * (ftol(y)-128) / 450 + floor4(3*D) )
//              / 10) * 10，< 1 → 10（popup 色 0xffffffff）。
//   floor4(v) = (v + (v>>31 & 3)) >> 2（负值向下取整的移位式）；
//   /450 与 /10 均为 C 截断除；ftol = 0x4971f0 向零截断。
//   得分 = value/10（0x413bf0；显示值 = popup value，原值域 ×10）。
void ItemManager::add_max_point(float displayed_increment) {
    // 0042f513..0042f52f: f32 argument * f64 100, truncate to signed integer,
    // then add to DAT_004be7dc and cap against DAT_004be7e4.
    SessionState& progress = *last_player_.progress;
    progress.max_point += i32(double(displayed_increment) * 100.0);
    if (progress.max_point > progress.max_point_cap)
        progress.max_point = progress.max_point_cap;
}

i32 ItemManager::point_value(float player_y, bool homing) const {
    const SessionState& progress = *last_player_.progress;
    const i32 a = progress.max_point / 100;
    if (player_y < 128.0f || homing) {
        const i32 d = a - a % 10;
        return d >= 1 ? d : 10;
    }
    const i32 neg3 = (a % 10 - a) * 3;
    const i32 d3 = (a - a % 10) * 3;
    const i32 neg4 = (neg3 + (neg3 >> 31 & 3)) >> 2;
    const i32 d4 = (d3 + (d3 >> 31 & 3)) >> 2;
    const i32 h = static_cast<i32>(player_y) - 128;  // ftol(player_y) - 0x80
    const i32 pre = neg4 * h / 450 + d4;
    const i32 value = pre / 10 * 10;
    return value >= 1 ? value : 10;
}

// ---- 神霊步进（0x00438730，每槽每帧次序）----
// 字段（float 索引 ×4）：[0] state、[1] kind、[4] speed（目标）、
// [0xb] move_speed（位移快照）、[5..7] pos、[0xc] angle、[0x18] frame、
// [0x21] flags（bit0 = kind/trance 门、bit1 = 界外反弹门）。
//   ① 反弹门（0x438730:41-51）：bit1 且出场地盒（0x40bb70(pos,0,224,384,448)
//      = x∈[-192,192]、y∈[0,448]）→ x 越（<=-192 或 >192）angle=wrap(-angle-π)、
//      y 越（<=0 或 >=448）angle=wrap(-angle)。
//   ② 衰减：move_speed > 0.13000001 → -= 0.12666667。
//   ③ 状态机（均不位移，只改角/速度/态）：
//      1 游离：dist²>=9216(96²) 或 player dying → （选人光标==0 且 bit0）
//        转 4 慢速追踪，否则保持；dist²<9216 且非 dying → 转 2。
//      2 追踪：angle=wrap(0x443f60)；move_speed=speed；speed<12→+0.3；
//        dist²>=16384(128²) → 回 1；player 4/2 → 回 1 且 move_speed=0。
//      4 慢速追踪：angle 同上；move_speed=speed；speed<4→+0.05；
//        player 4 或 !bit0 → 回 1 且 move_speed=0；dist²<12544(112²) → 转 2。
//      5 场景门控（0x4c2188+0x5c；Stage1 无消费恒 0）→ 直接回 1。
//   ④ 公共尾：bit0==0 时 frame 每帧 +1（出生 -1），==480（0x1e0）清 bit1
//      （反弹门关）；此后越界盒 0x40bb70(pos,0,224,448,512) =
//      x∈[-224,224]、y∈[-32,480] 外即回收。bit0==1（trance 期产件）无回收。
//   ⑤ 收集/位移（用步进前坐标的 dist²）：player dying 或 dist²>=400(20²)
//      → 直线位移 polar(angle, move_speed)（0x4736a0/0x473780）；否则收集
//      （收集帧不位移，见 collect_spirit）。
void ItemManager::set_spirit_trance(bool active) {
    // The original keeps position, velocity, kind and identity; resets the
    // lifetime clock and replaces the ANM with kind*6+(active?4:1).
    // 0x439280/0x4393a0（灵界开/关）无条件写 Timer：previous=-1
    //（0xffffffff）、current=0、fractional=0（004392f5/0043930b 与
    // 004393xx 的 mov [ebx+0x5c],0xffffffff / [ebx+0x60],0 /
    // [ebx+0x64],0）。frame 是「本帧尾检查值」的暴露位；重置后必须等于
    // Timer.previous（-1），否则 update_spirits 尾部的 set(frame+1) 同步
    // 守卫会把 current 提前推到 1，随后 tick 再进到 2——比原生快 1 帧，
    // 使 0x1e0（480）反弹门提前一帧关闭（S4 f8188 y 反弹 / S6 f2456
    // x 反弹方向分叉根因；原生 f7708 重置帧 raw state frame=1）。
    for (auto& spirit : spirits_) {
        if (!spirit.alive) continue;
        spirit.flags = (spirit.flags & ~1u) | u32(active);
        spirit.frame = -1;
        spirit.lifetime_clock.set(0);
        spirit.fade_requested = false;
    }
}

void ItemManager::update_spirits(const ItemCollector& player) {
    update_spirits_seats(&player, 1);
}

// M3：神霊多席步进（R-I7：每只按最近合格席——灵界角色门按实际拾取者
// 的角色；单元素 = 单席零变化）。
void ItemManager::update_spirits_seats(const ItemCollector* seats, i32 seat_count) {
    if (!seats || seat_count <= 0) return;
    const ItemCollector& fallback = seats[0];
    last_player_ = fallback;
    const float rate = fallback.progress ? fallback.progress->rate : 1.f;
    for (Spirit& s : spirits_) {
        if (!s.alive) continue;
        if (s.state != 2 && s.state != 4) s.tracking_seat = -1;
        if (s.tracking_seat >= seat_count ||
            (s.tracking_seat >= 0 && !seats[s.tracking_seat].eligible))
            s.tracking_seat = -1;
        const ItemCollector& player = s.tracking_seat >= 0 ? seats[s.tracking_seat]
            : pick_collector(seats, seat_count, s.x, s.y, fallback);
        // R-I3：结算宿主跟随选中席（同道具侧）。
        last_player_ = player;
        // R-I5：角色门按实际拾取者的席位角色（-1 = 单席回落
        // progress->character——DAT_004be7b8 语义）。
        const i32 character =
            player.character >= 0 ? player.character
                                  : (player.progress ? player.progress->character : 0);
        s.anm_frame_prev = s.anm_frame;
        s.anm_time = float(s.anm_time + (rate > .99f && rate < 1.01f ? 1.f : rate));
        s.anm_frame = truncate_int(s.anm_time);
        // ① 反弹门。
        if ((s.flags & 2u) != 0 &&
            (s.x < -192.0f || s.x > 192.0f || s.y < 0.0f || s.y > 448.0f)) {
            // 43878b–438798 also reflects at x==192 when another axis
            // has already put the spirit outside the 384x448 box.
            if (s.x <= -192.0f || s.x >= 192.0f)
                s.angle = wrap_angle(-s.angle - kPi);
            if (s.y <= 0.0f || s.y >= 448.0f) s.angle = wrap_angle(-s.angle);
        }
        // ② 衰减。
        if (s.move_speed > 0.13000001f) s.move_speed -= 0.12666667f;
        // ③ 状态机。
        switch (s.state) {
        case 1: {
            if (!player.eligible) break;
            const float dx = player.x - s.x, dy = player.y - s.y;
            const float dist2 = dx * dx + dy * dy;
            if (dist2 >= 9216.0f || player.state == 2) {
                if (character == 0 && (s.flags & 1u) != 0) s.state = 4;
            } else {
                s.state = 2;
            }
            break;
        }
        case 2: {
            if (!player.eligible) { s.move_speed = 0.0f; break; }
            s.angle = wrap_angle(angle_to_player(s, player));
            s.move_speed = s.speed;
            if (s.speed < 12.0f) s.speed += 0.3f;
            const float dx = player.x - s.x, dy = player.y - s.y;
            if (dx * dx + dy * dy >= 16384.0f) s.state = 1;
            if (player.state == 4 || player.state == 2) {
                s.state = 1;
                s.move_speed = 0.0f;  // 0x438730:102 清 [0xb]，不清 [4]
            }
            break;
        }
        case 4: {
            if (!player.eligible) { s.move_speed = 0.0f; break; }
            s.angle = wrap_angle(angle_to_player(s, player));
            s.move_speed = s.speed;
            if (s.speed < 4.0f) s.speed += 0.05f;
            if (player.state == 4 || (s.flags & 1u) == 0) {
                s.state = 1;
                s.move_speed = 0.0f;
                break;
            }
            const float dx = player.x - s.x, dy = player.y - s.y;
            if (dx * dx + dy * dy < 12544.0f) s.state = 2;
            break;
        }
        case 5:
            // 438730 ->439550: homing toward boss slot0, while a nearby
            // living player may still reclaim the spirit (30px threshold).
            if(!player.boss_present)s.state=1;
            else s.angle=wrap_angle(float(std::atan2(double(player.boss_y-s.y),double(player.boss_x-s.x))));
            s.move_speed=s.speed;
            if(s.speed<4)s.speed=float(double(s.speed)+.035f);
            if(player.eligible && player.state!=2) {
                const float dx=s.x-player.x,dy=s.y-player.y;
                if(float(double(dx)*dx+double(dy)*dy)<900)s.state=2;
            }
            break;
        default:
            continue;
        }
        s.tracking_seat = seat_count >= 2 && player.eligible && (s.state == 2 || s.state == 4)
            ? i32(&player - seats) : -1;
        // ④⑤ 公共尾。
        if ((s.flags & 1u) == 0) {
            // `frame` exposes the last tested lifetime value. Keep explicit
            // restored/debug state writes coherent with its fractional clock.
            if (s.frame != s.lifetime_clock.previous)
                s.lifetime_clock.set(wrapping_add(s.frame,1));
            // 438730 tests lifetime before401400 at the movement tail.
            s.frame = s.lifetime_clock.current;
            if (s.frame == 480) {
                s.flags &= ~2u;
                s.fade_requested = true; // 00438a77: label1, consumed by ANM tick.
            }
            if (s.x < -224.0f || s.x > 224.0f || s.y < -32.0f || s.y > 480.0f) {
                s.alive = false;
                s.tracking_seat = -1;
                continue;
            }
            if (spirit_animation_alive &&
                !spirit_animation_alive(u32(&s - spirits_.data()), s.serial)) {
                s.alive = false; // 00438aca: missing ANM retires before scoring.
                s.tracking_seat = -1;
                continue;
            }
        }
        const float dx = player.x - s.x, dy = player.y - s.y;
        if (!player.eligible || player.state == 2 || dx * dx + dy * dy >= 400.0f) {
            // 0x4736a0 先落 f32 速度，0x473780 累加后逐轴向下量化到 0.01。
            const float speed = float(s.move_speed * rate);
            const float vx = float(std::cos(double(s.angle)) * double(speed));
            const float vy = float(std::sin(double(s.angle)) * double(speed));
            s.x += vx;
            s.y += vy;
            s.x = float(std::floor(double(s.x * 100.0f))) / 100.0f;
            s.y = float(std::floor(double(s.y * 100.0f))) / 100.0f;
            s.lifetime_clock.tick(&rate);
        } else {
            collect_spirit(s, player, seats, seat_count);
            if (!s.alive) s.tracking_seat = -1;
        }
    }
    // 438660 decrements the production window after traversing spirits.
    if (spirit_window_ > 0) {
        spirit_window_clock_.decrement(&rate);
        spirit_window_ = spirit_window_clock_.current;
    } else spirit_overflow_ = 0;
}

float ItemManager::angle_to_player(const Spirit& s, const ItemCollector& player) {
    // 0x443f60：dx/dy 各自 fsub 后 fstp dword 落 f32（0x443f6e-0x443f76 /
    // 0x443f7a-0x443f83），再 fld 回 x87 调双精度 CRT atan2（0x443fbb
    // call 0x4977aa），结果一次 fstp dword（0x443fc0）。f32 重载 atan2f
    // 在 S3 f8187（serial 1052）差 1 ULP，越 0.01 量化边界翻向。
    const float dx = player.x - s.x, dy = player.y - s.y;
    if (dx == 0.0f && dy == 0.0f) return 1.5707964f;
    return static_cast<float>(std::atan2(static_cast<double>(dy), static_cast<double>(dx)));
}

// 0x438730 收集段：按出生 flags bit0（trance 激活位）分两套（0x438730:140-245）。
//   bit0==0（常规产件，LAB_00438b3d 起）：公共 trance +2（0x43d530，跨 200 段
//     sound 0x44+段位演出）；kind0 → 残机碎片（0x43d330）；kind1 → 难度<5 时
//     max_point += 1000（0x42f510(10.0)），选人光标==2 再 trance +1（0x43d530）；
//     kind2 → 灵击碎片 +1（0x43d480(1)）；kind3 → trance 再 +1 + 点值弹出
//     （A-A%10，无下限钳）+ score += 值/10（0x413bf0）。
//   bit0==1（trance 期产件）：无公共 trance；kind0/2 分别给残机/灵击碎片+2；
//     kind3 → 点值 + score（无 trance）；其余同。
void ItemManager::collect_spirit(Spirit& s, const ItemCollector& player,
                                  const ItemCollector* seats, i32 seat_count) {
    if (!player.eligible) return;
    SessionState& progress = *player.progress;
    // R-I5：角色门按实际拾取者的席位角色（-1 = 单席回落 DAT_004be7b8）。
    const i32 character = player.character >= 0 ? player.character : progress.character;
    const bool easy_difficulty = progress.difficulty < 5;  // DAT_004be7c4 < 5
    events_.push_back({ItemEvent::Kind::Collected,
                       10 + s.kind, s.x, s.y});
    const bool spirit_trace = item_trace_enabled;
    const i32 trace_max_point = progress.max_point;
    const i32 trace_score_before = player.score->score;
    if ((s.flags & 1u) == 0) collect_trance(2);
    switch (s.kind) {
    case 0:
        // 438b5d passes EAX=1 for ordinary-born spirits; 438d09 passes
        // EAX=2 for trance-born spirits. Both invoke the same 43d330.
        if (seat_count >= 2) {
            ResourceGrant grant; grant.life_pieces = (s.flags & 1u) == 0 ? 1 : 2;
            grant_resources(grant, seats, seat_count);
        } else collect_life_piece((s.flags & 1u) == 0 ? 1 : 2);
        break;
    case 1:
        if (easy_difficulty) {
            // 00438bbe (ordinary) uses 10.0; 00438d5e (trance) uses 100.0.
            add_max_point((s.flags & 1u) == 0 ? 10.0f : 100.0f);
        }
        // 438bea's Sanae bonus belongs only to the ordinary-born branch.
        // The trance-born kind1 branch at438d5e awards max point, then retires.
        if (character == 2 && (s.flags & 1u) == 0) collect_trance(1);
        break;
    case 2:
        if (seat_count >= 2) {
            ResourceGrant grant; grant.bomb_pieces = (s.flags & 1u) == 0 ? 1 : 2;
            grant_resources(grant, seats, seat_count);
        } else collect_bomb_piece((s.flags & 1u) == 0 ? 1 : 2);
        break;
    case 3: {
        if ((s.flags & 1u) == 0) collect_trance(1);
        const i32 a = progress.max_point / 100;
        const i32 v = a - a % 10;  // 原版无 min 钳（0x438c74 直读）
        HudScoreState& score_state = *player.score;
        score_state.score += v / 10;
        if (score_state.score > 999999999) score_state.score = 999999999;
        events_.push_back({ItemEvent::Kind::ScorePopup, v, s.x, s.y, 0xffffff00u});
        break;
    }
    default:
        break;
    }
    if (spirit_trace) {
        char row[224];
        std::snprintf(row, sizeof row,
                      "f=%d spirit kind=%d flags=%u max_point=%d score=%d->%d\n",
                      item_trace_frame, s.kind, s.flags, trace_max_point,
                      trace_score_before, player.score->score);
        item_trace_rows += row;
    }
    // 0x438730 收集瞬间的 front.anm 拾取动画：普通/灵界两套脚本表
    // （0x438e3c / 0x438e4c 两张 jump table 的 0x46f820 调用点实证）：
    //   ordinary: kind0→96(0x60) kind1→102(0x66) kind2→99(0x63) kind3→无
    //   trance  : kind0→97(0x61) kind1→103(0x67) kind2→100(0x64) kind3→无
    // 创建为 flags=0 尾插、layer=-1、位置=神霊位；句柄原版即丢弃（ESI=0）。
    static constexpr i32 kPickup[2][4] = {{96, 102, 99, -1}, {97, 103, 100, -1}};
    const i32 pickup_kind = s.kind >= 0 && s.kind < 4 ? s.kind : -1;
    const i32 pickup_script =
        pickup_kind >= 0 ? kPickup[(s.flags & 1u) ? 1 : 0][pickup_kind] : -1;
    if (pickup_script >= 0 && spirit_pickup_effect)
        (void)spirit_pickup_effect(pickup_script, s.x, s.y, s.z);
    s.alive = false;
}

}  // namespace th13
