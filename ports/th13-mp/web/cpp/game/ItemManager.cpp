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

}

void set_item_trace(bool enabled) {
    item_trace_enabled = enabled;
    if (!enabled) item_trace_rows.clear();
}

void set_item_trace_frame(i32 frame) { item_trace_frame = frame; }

void item_trace_note(const char* tag, i32 max_point, i32 score) {
    if (!item_trace_enabled) return;
    char row[192];
    std::snprintf(row, sizeof row, "f=%d note=%s max_point=%d score=%d\n", item_trace_frame, tag, max_point, score);
    item_trace_rows += row;
}

std::string take_item_trace() {
    std::string result;
    result.swap(item_trace_rows);
    return result;
}

void ItemManager::reset_items() {
    items_ = {};
    ordinary_free_.clear();
    cancel_free_.clear();
    for (i32 i = 0; i < 600; ++i)
        ordinary_free_.push_back(i);
    for (i32 i = 600; i < kItemCapacity; ++i)
        cancel_free_.push_back(i);
    cancel_total_ = cancel_wave_ = 0;
    events_.clear();
}

void ItemManager::spawn_item(float x, float y, float z, i32 kind, float angle, float speed) {
    if (kind >= 10 && kind <= 14) {
        spawn_value_item(kind - 10, x, y, z, 0.5f);
        return;
    }
    auto& free = kind == 9 ? cancel_free_ : ordinary_free_;
    if (!free.empty()) {
        Item& item = items_[size_t(free.back())];
        free.pop_back();
        if (kind == 9) {
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

void ItemManager::spawn_item(const float pos[3], i32 type, float angle, float speed) {
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
    const i32 slot = i32(&item - items_.data());
    item.alive = false;
    item.state = 0;
    item.target_seat = -1;
    item.tracking_seat = -1;
    (slot < 600 ? ordinary_free_ : cancel_free_).push_back(slot);
}

void ItemManager::spawn_value_item(i32 kind, float x, float y, float z, float base_speed) {
    spawn_value_item(kind, x, y, z, base_speed, true);
}

void ItemManager::spawn_value_item(i32 kind, float x, float y, float z, float base_speed, bool sound) {
    if (sound && spirit_birth_sound) spirit_birth_sound((kind == 4 ? 1 : kind) + 0x3b, x);
    if (!random_) return;
    for (Spirit& s : spirits_) {
        if (s.alive) continue;
        const i32 kind_resolved = kind == 4 ? 1 : kind;
        s.kind = kind_resolved;
        s.x = x < -192.0f ? -192.0f : (x > 192.0f ? 192.0f : x);
        s.y = y < 0.0f ? 0.0f : (y > 384.0f ? 384.0f : y);
        s.z = z;
        const u32 id = ++spirit_serial_;
        s.serial = id;
        s.fade_requested = false;
        const u32 quadrant = id & 3u;
        float angle = 0.0f;
        switch (quadrant) {
        case 0:
            GameRandom::tag(9);
            angle = random_->signed_unit() * kPi * 0.25f - 2.3561945f;
            break;
        case 1:
            GameRandom::tag(9);
            angle = random_->signed_unit() * kPi * 0.25f + 0.7853982f;
            break;
        case 2:
            GameRandom::tag(9);
            angle = random_->signed_unit() * kPi * 0.25f + 2.3561945f;
            break;
        default:
            GameRandom::tag(9);
            angle = random_->signed_unit() * kPi * 0.25f - 0.7853982f;
            break;
        }
        s.angle = wrap_angle(angle);
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
        if (spirit_spawn_animation)
            (void)spirit_spawn_animation(u32(&s - spirits_.data()), s.serial, kind_resolved, s.flags, s.x, s.y, s.z);
        break;
    }
    if (kind != 3 && kind != 4) {
        spirit_window_ = 60;
        spirit_window_clock_.set(60);
        if (++spirit_overflow_ > 9) {
            spawn_value_item(3, x, y, z, 0.5f, true);
        }
    }
}

void ItemManager::on_et_clear(float x, float y, float z) {
    (void)x;
    (void)y;
    (void)z;
}

void ItemManager::update(const ItemCollector& player) { update_seats(&player, 1); }

const ItemCollector& ItemManager::pick_collector(const ItemCollector* seats, i32 seat_count, float x, float y,
                                                 const ItemCollector& fallback) {
    const ItemCollector* best = nullptr;
    float best_d2 = 0;
    for (i32 i = 0; i < seat_count; ++i) {
        const ItemCollector& candidate = seats[i];
        if (!candidate.eligible) continue;
        const float dx = candidate.x - x, dy = candidate.y - y;
        const float d2 = dx * dx + dy * dy;
        if (!best || d2 < best_d2) {
            best = &candidate;
            best_d2 = d2;
        }
    }
    return best ? *best : fallback;
}

i32 ItemManager::power_receiver(const ItemCollector* seats, i32 seat_count, float x, float y) {
    i32 best = -1;
    float best_d2 = 0;
    for (i32 i = 0; i < seat_count; ++i) {
        const ItemCollector& candidate = seats[i];
        if (!candidate.eligible || !candidate.player || candidate.player->power >= candidate.player->max_power)
            continue;
        const float dx = candidate.x - x, dy = candidate.y - y;
        const float d2 = dx * dx + dy * dy;
        if (best < 0 || d2 < best_d2) {
            best = i;
            best_d2 = d2;
        }
    }
    return best;
}

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
        team_autocollect |= candidate.eligible && !candidate.dialog && candidate.state != 2 && candidate.state != 4 &&
                            candidate.y < 128.0f;
    }
    cancel_wave_ = 0;
    for (Item& item : items_) {
        if (!item.alive) continue;
        if (item.target_seat >= 0 || (item.state != 3 && item.state != 4)) item.tracking_seat = -1;
        if (item.tracking_seat >= seat_count || (item.tracking_seat >= 0 && !seats[item.tracking_seat].eligible))
            item.tracking_seat = -1;
        if (item.target_seat >= 0 && (item.target_seat >= seat_count || !seats[item.target_seat].eligible)) continue;
        const bool personal_power = seat_count >= 2 && item.target_seat < 0 && is_power_item(item.type);
        if (personal_power && item.tracking_seat >= 0) {
            const HudPlayerState* hud = seats[item.tracking_seat].player;
            if (hud && hud->power >= hud->max_power) {
                const i32 other = power_receiver(seats, seat_count, item.x, item.y);
                if (other >= 0) item.tracking_seat = other;
            }
        }
        const ItemCollector* chosen = item.target_seat >= 0 ? &seats[item.target_seat]
                                      : item.tracking_seat >= 0
                                          ? &seats[item.tracking_seat]
                                          : &pick_collector(seats, seat_count, item.x, item.y, fallback);
        const auto magnet_for = [&](const ItemCollector& collector) {
            return !has_collector || (!team_autocollect && collector.eligible && !collector.dialog &&
                                      (collector.state == 2 || collector.state == 4 || collector.y >= 128.0f));
        };
        bool magnet = magnet_for(*chosen);
        if (personal_power && item.tracking_seat < 0 && item.state == 1 && !magnet) {
            const i32 receiver = power_receiver(seats, seat_count, item.x, item.y);
            if (receiver >= 0) {
                chosen = &seats[receiver];
                magnet = magnet_for(*chosen);
            }
        }
        const ItemCollector& player = *chosen;
        last_player_ = player;
        redirected_seat_ = -1;
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
                    retire_item(item);
                    break;
                }
            } else {
                item.homing_speed = player.homing_speed;
                item.state = 3;
                home_step(item, player);
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
                if (has_collector) home_step(item, player);
                collect_check(item, player, seats, seat_count);
                break;
            }
            if (item.y > 472.0f) {
                retire_item(item);
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
            --item.sprite;
            if (item.sprite < 0) item.state = 2;
            continue;
        }
        default:
            break;
        }
        if (redirected_seat_ >= 0 && item.alive)
            item.tracking_seat = redirected_seat_;
        else
            item.tracking_seat = item.alive && item.target_seat < 0 && seat_count >= 2 && player.eligible &&
                                         (item.state == 3 || item.state == 4)
                                     ? i32(&player - seats)
                                     : -1;
        if (item.alive && item.state != 5) {
            item.anm_frame_prev = item.anm_frame;
            item.anm_time = float(item.anm_time + (rate > .99f && rate < 1.01f ? 1.f : rate));
            item.anm_frame = truncate_int(item.anm_time);
        }
    }
}

void ItemManager::home_step(Item& item, const ItemCollector& player) {
    const float rate = player.progress ? player.progress->rate : 1.f;
    float dx = player.x - item.x;
    float dy = player.y - item.y;
    float angle = (dx == 0.0f && dy == 0.0f) ? 1.5707964f : float(std::atan2(double(dy), double(dx)));
    angle = wrap_angle(angle);
    item.vx = float(std::cos(double(angle)) * double(item.homing_speed));
    item.vy = float(std::sin(double(angle)) * double(item.homing_speed));
    item.x += float(item.vx * rate);
    item.y += float(item.vy * rate);
    item.z += float(item.vz * rate);
    if (item.homing_speed < 12.0f) item.homing_speed += 0.2f;
    if (player.state == 4) {
        item.state = 1;
        item.vx = 0.0f;
        item.vy = 0.0f;
    }
}

void ItemManager::collect_check(Item& item, const ItemCollector& player, const ItemCollector* seats, i32 seat_count) {
    static const bool item_dbg = std::getenv("TH13_ITEM_DBG") != nullptr;
    if (item_dbg)
        std::fprintf(stderr, "COLLECTCHK state=%d type=%d pos=(%g,%g) box=[%g,%g,%g,%g]\n", player.state, item.type,
                     item.x, item.y, player.box[0], player.box[1], player.box[2], player.box[3]);
    if (player.state == 2 || !player.eligible) return;
    const auto pass_power_on = [&]() {
        if (seat_count < 2 || item.target_seat >= 0 || !is_power_item(item.type) || !player.player ||
            player.player->power < player.player->max_power)
            return false;
        const i32 receiver = power_receiver(seats, seat_count, item.x, item.y);
        if (receiver < 0) return false;
        item.state = 3;
        item.homing_speed = seats[receiver].homing_speed;
        redirected_seat_ = receiver;
        return true;
    };
    if (item.x < player.box[0] || item.y < player.box[1] || item.x > player.box[2] || item.y > player.box[3]) {
        if (item.state == 3 || item.state == 4) return;
        const std::array<float, 4>& auto_box = player.focus ? player.box_auto_focus : player.box_auto;
        if (item.x < auto_box[0] || item.y < auto_box[1] || item.x > auto_box[2] || item.y > auto_box[3]) return;
        if (pass_power_on()) return;
        item.homing_speed = player.homing_speed / 3.0f;
        item.state = 4;
        return;
    }
    if (pass_power_on()) return;
    SessionState& progress = *player.progress;
    HudPlayerState& hud = *player.player;
    HudScoreState& score_state = *player.score;
    if (item.target_seat >= 0) {
        if (item.type == ItemExtraLife) {
            hud.extra_lives = std::min(8, hud.extra_lives + 1);
            if (life_changed) life_changed(item.target_seat, std::max(0, hud.extra_lives), hud.life_pieces);
        } else if (collect_power_big(hud.power_step)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y, 0xffffff40u});
        }
        events_.push_back({ItemEvent::Kind::Collected, item.type, item.x, item.y});
        retire_item(item);
        return;
    }
    const i32 power_multiplier = !legacy_pickup_rewards && seat_count >= 2 ? seat_count : 1;
    if (seat_count >= 2 && item.target_seat < 0 && !is_power_item(item.type) && item.type != ItemPoint &&
        item.type != ItemSpiritDrop) {
        ResourceGrant grant;
        switch (item.type) {
        case ItemLifePiece:
            grant.life_pieces = 1;
            break;
        case ItemBombPiece:
            grant.bomb_pieces = 2;
            break;
        case ItemExtraLife:
            grant.lives = 1;
            break;
        case ItemBomb:
            grant.bombs = 1;
            break;
        default:
            break;
        }
        grant_resources(grant, seats, seat_count);
        events_.push_back({ItemEvent::Kind::Collected, item.type, item.x, item.y});
        retire_item(item);
        return;
    }
    switch (item.type) {
    case 1:
        if (collect_power_small(power_multiplier)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y, 0xffffff40u});
        }
        break;
    case 2: {
        const i32 value = point_value(player.y, item.state == 3);
        score_state.score += value / 10;
        if (score_state.score > 999999999) score_state.score = 999999999;
        ++point_items_;
        ++progress.point_items;
        events_.push_back({ItemEvent::Kind::ScorePopup, value, item.x, item.y,
                           (player.y > 128.0f && item.state != 3) ? 0xffffffffu : 0xffffff00u});
        break;
    }
    case 3: {
        if (hud.power >= hud.max_power) {
            score_state.score += 20000 / 10;
            if (score_state.score > 999999999) score_state.score = 999999999;
            events_.push_back({ItemEvent::Kind::ScorePopup, 20000, item.x, item.y, 0xff808080u});
            events_.push_back({ItemEvent::Kind::PowerLevelUp, 0, item.x, item.y});
        }
        if (collect_power_big(hud.power_step * power_multiplier)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y, 0xffffff40u});
        }
        break;
    }
    case 4:
        collect_life_piece();
        break;
    case 5:
        collect_bomb_piece(2);
        break;
    case 6:
        collect_extra_life();
        break;
    case 7:
        collect_bomb();
        break;
    case 8: {
        if (hud.power >= hud.max_power) {
            add_max_point(100.0f);
            events_.push_back({ItemEvent::Kind::ScorePopup, 100, item.x, item.y, 0xff40ff40u});
            events_.push_back({ItemEvent::Kind::PowerLevelUp, 0, item.x, item.y});
        }
        if (collect_power_big(hud.power_step * power_multiplier)) {
            if (option_power_changed) option_power_changed();
            push_level_up(item);
            events_.push_back({ItemEvent::Kind::ScorePopup, -1, item.x, item.y, 0xffffff40u});
        }
        break;
    }
    case 9: {
        const i32 trace_mp = progress.max_point;
        const i32 trace_sc = score_state.score;
        add_max_point(0.1f);
        score_state.score += 1;
        if (score_state.score > 999999999) score_state.score = 999999999;
        if (item_trace_enabled) {
            char row[192];
            std::snprintf(row, sizeof row, "f=%d item9 max_point=%d->%d score=%d->%d\n", item_trace_frame, trace_mp,
                          progress.max_point, trace_sc, score_state.score);
            item_trace_rows += row;
        }
        break;
    }
    default:
        break;
    }
    events_.push_back({ItemEvent::Kind::Collected, item.type, item.x, item.y});
    retire_item(item);
}

void ItemManager::grant_resources(const ResourceGrant& grant, const ItemCollector* seats, i32 seat_count) {
    bool extended = false, bomb_gained = false;
    for (i32 seat = 0; seat < seat_count; ++seat) {
        auto& hud = *seats[seat].player;
        if (grant.life_pieces) {
            hud.life_pieces += grant.life_pieces;
            while (hud.life_pieces >= life_piece_threshold(hud.life_piece_threshold_index)) {
                hud.life_pieces -= life_piece_threshold(hud.life_piece_threshold_index++);
                if (hud.extra_lives < 8) {
                    ++hud.extra_lives;
                    extended = true;
                }
            }
        }
        if (challenge) {
            hud.bombs = 0;
            hud.bomb_pieces = 0;
        } else if (grant.bomb_pieces) {
            if (hud.bombs >= 9)
                hud.bomb_pieces = 0;
            else {
                hud.bomb_pieces += grant.bomb_pieces + (seat == grant.bonus_bomb_piece_seat);
                if (hud.bomb_pieces > 7) {
                    hud.bomb_pieces = 0;
                    ++hud.bombs;
                    bomb_gained = true;
                }
            }
        }
        hud.extra_lives = std::min(8, hud.extra_lives + grant.lives);
        if (!challenge && grant.bombs && hud.bombs < 9) {
            ++hud.bombs;
            bomb_gained = true;
        }
    }
    if (extended) events_.push_back({ItemEvent::Kind::Extend, 1, last_player_.x, last_player_.y});
    if (bomb_gained) events_.push_back({ItemEvent::Kind::BombGained, 1, last_player_.x, last_player_.y});
    if (grant.life_pieces) seats[0].progress->life_piece_threshold_index = seats[0].player->life_piece_threshold_index;
    if ((grant.lives || grant.life_pieces) && life_changed)
        for (i32 seat = 0; seat < seat_count; ++seat)
            life_changed(seat, std::max(0, seats[seat].player->extra_lives), seats[seat].player->life_pieces);
    if ((grant.bombs || grant.bomb_pieces) && bombs_changed)
        for (i32 seat = 0; seat < seat_count; ++seat)
            bombs_changed(seat, seats[seat].player->bombs, seats[seat].player->bomb_pieces);
}

void ItemManager::push_level_up(const Item& item) {
    events_.push_back({ItemEvent::Kind::PowerLevelUp, 0, item.x, item.y});
}

bool ItemManager::collect_power_small(i32 amount) {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    HudScoreState& score_state = *c.score;
    if (hud.power >= hud.max_power) {
        score_state.score += 1000;
        if (score_state.score > 999999999) score_state.score = 999999999;
        events_.push_back({ItemEvent::Kind::ScorePopup, 10000, c.x, c.y, 0xffffffffu});
        return false;
    }
    const i32 before = hud.power;
    hud.power = before + amount;
    if (hud.power > hud.max_power) {
        hud.power = hud.max_power;
        events_.push_back({ItemEvent::Kind::FullPower, 0, c.x, c.y});
    }
    return before / hud.power_step != hud.power / hud.power_step;
}

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

void ItemManager::collect_life_piece(i32 amount) {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    SessionState& progress = *c.progress;
    if (hud.extra_lives > 8) {
        hud.life_pieces = 0;
        return;
    }
    hud.life_pieces += amount;
    while (life_piece_threshold(progress.life_piece_threshold_index) <= hud.life_pieces) {
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

void ItemManager::collect_bomb_piece(i32 amount) {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    if (challenge) {
        hud.bombs = 0;
        hud.bomb_pieces = 0;
        return;
    }
    if (hud.bombs > 8) {
        hud.bomb_pieces = 0;
        return;
    }
    hud.bomb_pieces += amount;
    if (hud.bomb_pieces > 7) {
        hud.bomb_pieces = 0;
        hud.bombs += 1;
        if (hud.bombs > 9)
            hud.bombs = 9;
        else
            events_.push_back({ItemEvent::Kind::BombGained, hud.bombs, c.x, c.y});
        if (bombs_changed) bombs_changed(0, hud.bombs, hud.bomb_pieces);
    }
    if (bombs_changed) bombs_changed(0, hud.bombs, hud.bomb_pieces);
}

void ItemManager::collect_extra_life() {
    HudPlayerState& hud = *last_player_.player;
    if (hud.extra_lives > 7) return;
    hud.extra_lives += 1;
    if (hud.extra_lives > 8) hud.extra_lives = 8;
    if (life_changed) life_changed(0, hud.extra_lives, hud.life_pieces);
}

void ItemManager::collect_bomb() {
    ItemCollector& c = last_player_;
    HudPlayerState& hud = *c.player;
    if (challenge) {
        hud.bombs = 0;
        hud.bomb_pieces = 0;
        return;
    }
    hud.bombs += 1;
    if (hud.bombs < 10) {
        events_.push_back({ItemEvent::Kind::BombGained, hud.bombs, c.x, c.y});
    } else {
        hud.bombs = 9;
    }
    if (bombs_changed) bombs_changed(0, hud.bombs, hud.bomb_pieces);
}

void ItemManager::collect_trance(i32 amount) {
    if (challenge) return;
    SessionState& progress = *last_player_.progress;
    const i32 before = progress.ending_points;
    trance_add(progress, amount);
    if (before / 200 != progress.ending_points / 200) {
        events_.push_back(
            {ItemEvent::Kind::SpiritGained, progress.ending_points / 200, last_player_.x, last_player_.y});
    }
    events_.push_back({ItemEvent::Kind::TranceGained, progress.ending_points, last_player_.x, last_player_.y});
}

void ItemManager::add_max_point(float displayed_increment) {
    SessionState& progress = *last_player_.progress;
    progress.max_point += i32(double(displayed_increment) * 100.0);
    if (progress.max_point > progress.max_point_cap) progress.max_point = progress.max_point_cap;
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
    const i32 h = static_cast<i32>(player_y) - 128;
    const i32 pre = neg4 * h / 450 + d4;
    const i32 value = pre / 10 * 10;
    return value >= 1 ? value : 10;
}

void ItemManager::set_spirit_trance(bool active) {
    for (auto& spirit : spirits_) {
        if (!spirit.alive) continue;
        spirit.flags = (spirit.flags & ~1u) | u32(active);
        spirit.frame = -1;
        spirit.lifetime_clock.set(0);
        spirit.fade_requested = false;
    }
}

void ItemManager::update_spirits(const ItemCollector& player) { update_spirits_seats(&player, 1); }

void ItemManager::update_spirits_seats(const ItemCollector* seats, i32 seat_count) {
    if (!seats || seat_count <= 0) return;
    const ItemCollector& fallback = seats[0];
    last_player_ = fallback;
    const float rate = fallback.progress ? fallback.progress->rate : 1.f;
    for (Spirit& s : spirits_) {
        if (!s.alive) continue;
        if (s.state != 2 && s.state != 4) s.tracking_seat = -1;
        if (s.tracking_seat >= seat_count || (s.tracking_seat >= 0 && !seats[s.tracking_seat].eligible))
            s.tracking_seat = -1;
        const ItemCollector& player =
            s.tracking_seat >= 0 ? seats[s.tracking_seat] : pick_collector(seats, seat_count, s.x, s.y, fallback);
        last_player_ = player;
        const i32 character =
            player.character >= 0 ? player.character : (player.progress ? player.progress->character : 0);
        s.anm_frame_prev = s.anm_frame;
        s.anm_time = float(s.anm_time + (rate > .99f && rate < 1.01f ? 1.f : rate));
        s.anm_frame = truncate_int(s.anm_time);
        if ((s.flags & 2u) != 0 && (s.x < -192.0f || s.x > 192.0f || s.y < 0.0f || s.y > 448.0f)) {
            if (s.x <= -192.0f || s.x >= 192.0f) s.angle = wrap_angle(-s.angle - kPi);
            if (s.y <= 0.0f || s.y >= 448.0f) s.angle = wrap_angle(-s.angle);
        }
        if (s.move_speed > 0.13000001f) s.move_speed -= 0.12666667f;
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
            if (!player.eligible) {
                s.move_speed = 0.0f;
                break;
            }
            s.angle = wrap_angle(angle_to_player(s, player));
            s.move_speed = s.speed;
            if (s.speed < 12.0f) s.speed += 0.3f;
            const float dx = player.x - s.x, dy = player.y - s.y;
            if (dx * dx + dy * dy >= 16384.0f) s.state = 1;
            if (player.state == 4 || player.state == 2) {
                s.state = 1;
                s.move_speed = 0.0f;
            }
            break;
        }
        case 4: {
            if (!player.eligible) {
                s.move_speed = 0.0f;
                break;
            }
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
            if (!player.boss_present)
                s.state = 1;
            else
                s.angle = wrap_angle(float(std::atan2(double(player.boss_y - s.y), double(player.boss_x - s.x))));
            s.move_speed = s.speed;
            if (s.speed < 4) s.speed = float(double(s.speed) + .035f);
            if (player.eligible && player.state != 2) {
                const float dx = s.x - player.x, dy = s.y - player.y;
                if (float(double(dx) * dx + double(dy) * dy) < 900) s.state = 2;
            }
            break;
        default:
            continue;
        }
        s.tracking_seat =
            seat_count >= 2 && player.eligible && (s.state == 2 || s.state == 4) ? i32(&player - seats) : -1;
        if ((s.flags & 1u) == 0) {
            if (s.frame != s.lifetime_clock.previous) s.lifetime_clock.set(wrapping_add(s.frame, 1));
            s.frame = s.lifetime_clock.current;
            if (s.frame == 480) {
                s.flags &= ~2u;
                s.fade_requested = true;
            }
            if (s.x < -224.0f || s.x > 224.0f || s.y < -32.0f || s.y > 480.0f) {
                s.alive = false;
                s.tracking_seat = -1;
                continue;
            }
            if (spirit_animation_alive && !spirit_animation_alive(u32(&s - spirits_.data()), s.serial)) {
                s.alive = false;
                s.tracking_seat = -1;
                continue;
            }
        }
        const float dx = player.x - s.x, dy = player.y - s.y;
        if (!player.eligible || player.state == 2 || dx * dx + dy * dy >= 400.0f) {
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
    if (spirit_window_ > 0) {
        spirit_window_clock_.decrement(&rate);
        spirit_window_ = spirit_window_clock_.current;
    } else
        spirit_overflow_ = 0;
}

float ItemManager::angle_to_player(const Spirit& s, const ItemCollector& player) {
    const float dx = player.x - s.x, dy = player.y - s.y;
    if (dx == 0.0f && dy == 0.0f) return 1.5707964f;
    return static_cast<float>(std::atan2(static_cast<double>(dy), static_cast<double>(dx)));
}

void ItemManager::collect_spirit(Spirit& s, const ItemCollector& player, const ItemCollector* seats, i32 seat_count) {
    if (!player.eligible) return;
    SessionState& progress = *player.progress;
    const i32 character = player.character >= 0 ? player.character : progress.character;
    const bool easy_difficulty = progress.difficulty < 5;
    events_.push_back({ItemEvent::Kind::Collected, 10 + s.kind, s.x, s.y});
    const bool spirit_trace = item_trace_enabled;
    const i32 trace_max_point = progress.max_point;
    const i32 trace_score_before = player.score->score;
    if ((s.flags & 1u) == 0) collect_trance(2);
    switch (s.kind) {
    case 0:
        if (seat_count >= 2) {
            ResourceGrant grant;
            grant.life_pieces = (s.flags & 1u) == 0 ? 1 : 2;
            grant_resources(grant, seats, seat_count);
        } else
            collect_life_piece((s.flags & 1u) == 0 ? 1 : 2);
        break;
    case 1:
        if (easy_difficulty) {
            add_max_point((s.flags & 1u) == 0 ? 10.0f : 100.0f);
        }
        if (character == 2 && (s.flags & 1u) == 0) collect_trance(1);
        break;
    case 2:
        if (seat_count >= 2) {
            ResourceGrant grant;
            const bool doubled = (s.flags & 1u) != 0;
            grant.bomb_pieces = doubled && legacy_pickup_rewards ? 2 : 1;
            if (doubled && !legacy_pickup_rewards) grant.bonus_bomb_piece_seat = player.seat;
            grant_resources(grant, seats, seat_count);
        } else
            collect_bomb_piece((s.flags & 1u) == 0 ? 1 : 2);
        break;
    case 3: {
        if ((s.flags & 1u) == 0) collect_trance(1);
        const i32 a = progress.max_point / 100;
        const i32 v = a - a % 10;
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
        std::snprintf(row, sizeof row, "f=%d spirit kind=%d flags=%u max_point=%d score=%d->%d\n", item_trace_frame,
                      s.kind, s.flags, trace_max_point, trace_score_before, player.score->score);
        item_trace_rows += row;
    }
    static constexpr i32 kPickup[2][4] = {{96, 102, 99, -1}, {97, 103, 100, -1}};
    const i32 pickup_kind = s.kind >= 0 && s.kind < 4 ? s.kind : -1;
    const i32 pickup_script = pickup_kind >= 0 ? kPickup[(s.flags & 1u) ? 1 : 0][pickup_kind] : -1;
    if (pickup_script >= 0 && spirit_pickup_effect) (void)spirit_pickup_effect(pickup_script, s.x, s.y, s.z);
    s.alive = false;
}

}
