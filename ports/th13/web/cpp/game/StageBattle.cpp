#include "StageBattle.hpp"
#include "StageMath.hpp"
#include "rebuild/X87.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace th13 {

namespace {

u16 read_u16(const u8* p) { return u16(p[0]) | u16(p[1]) << 8; }
u32 read_u32(const u8* p) { return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24; }

const i32 kDirX[9] = {0, 0, 0, -1, 1, -1, 1, -1, 1};
const i32 kDirY[9] = {0, -1, 1, 0, 0, -1, -1, 1, 1};

int dir_index(u32 held) {
    if ((held & 0x50) == 0x50) return 5;
    if ((held & 0x60) == 0x60) return 7;
    if ((held & 0x90) == 0x90) return 6;
    if ((held & 0xa0) == 0xa0) return 8;
    if ((held & 0x20) == 0) {
        if ((held & 0x10) == 0) {
            if ((held & 0x40) == 0) return (held & 0x80) ? 4 : 0;
            return 3;
        }
        return 1;
    }
    return 2;
}

i32 speed128_from_float(float value) { return i32(value * 128.0f); }

float combat_angle(float angle) {
    constexpr float pi = 3.1415927410125732f;
    while (angle > pi)
        angle -= 2.0f * pi;
    while (angle < -pi)
        angle += 2.0f * pi;
    return angle;
}

StageEnemy* nearest_target(StageSim& sim, float x, float y, float range) {
    StageEnemy* result = nullptr;
    float best = range * range;
    for (auto& enemy : sim.enemies) {
        if (enemy->dead || (enemy->flags & 0x18000021u)) continue;
        const float dx = enemy->position[0] - x, dy = enemy->position[1] - y;
        const float distance = dx * dx + dy * dy;
        if (distance < best) {
            result = enemy.get();
            best = distance;
        }
    }
    return result;
}

bool shot_hits_enemy_box(float sx, float sy, float sw, float sh, float ex, float ey, float ew, float eh, float theta) {
    if (theta == 0.0f) {
        return ex - ew * 0.5f <= sx + sw * 0.5f && sx - sw * 0.5f <= ex + ew * 0.5f &&
               ey - eh * 0.5f <= sy + sh * 0.5f && sy - sh * 0.5f <= ey + eh * 0.5f;
    }
    const float dx = ex - sx, dy = ey - sy;
    const double margin = double(std::fabs(ew) + std::fabs(eh)) * 0.5;
    const double bx = double(std::fabs(sw)) * 0.5 + margin, by = double(std::fabs(sh)) * 0.5 + margin;
    const double reach = (bx * bx + by * by) * 1.0001;
    const double distance = double(dx) * dx + double(dy) * dy;
    if (distance > reach) return false;
    const float a = -theta;
    const float sf = float(std::sin(double(a))), cf = float(std::cos(double(a)));
    const float ox = float(double(dx) * cf - double(dy) * sf);
    const float oy = float(double(dx) * sf + double(dy) * cf);
    const float vx = std::fabs(float(double(ew) * cf - double(eh) * sf));
    const float vy = std::fabs(float(double(ew) * sf + double(eh) * cf));
    return -sw * 0.5f <= ox + double(vx) * 0.5 && ox - double(vx) * 0.5 <= sw * 0.5f &&
           -sh * 0.5f <= oy + double(vy) * 0.5 && oy - double(vy) * 0.5 <= sh * 0.5f;
}

bool parse_sht_shots(const u8* sht, size_t size, StageBattle& battle) {
    if (size < 0xcc) return false;
    const u16 blocks = read_u16(sht + 2);
    if (blocks == 0 || blocks > StageBattle::kShtBlocks) return false;
    for (u32 block = 0; block < blocks; block++) {
        const u32 offset = read_u32(sht + 0xc8 + block * 4);
        size_t cursor = size_t(0xf4) + offset;
        std::vector<ShtShotRecord> records;
        for (u32 record = 0;; record++) {
            const size_t at = cursor + size_t(record) * 0x38;
            if (at >= size) return false;
            if (i8(sht[at]) < 0) break;
            if (at + 0x38 > size) return false;
            ShtShotRecord entry;
            entry.interval = i32(sht[at]);
            entry.phase = i32(sht[at + 1]);
            entry.damage = float(i16(read_u16(sht + at + 2)));
            entry.script = i32(i16(read_u16(sht + at + 0x22))) + 10;
            entry.anchor = i32(static_cast<i8>(sht[at + 0x20]));
            entry.sound = i32(i16(read_u16(sht + at + 0x24)));
            std::memcpy(&entry.x, sht + at + 4, sizeof entry.x);
            std::memcpy(&entry.y, sht + at + 8, sizeof entry.y);
            std::memcpy(&entry.angle, sht + at + 0x14, sizeof entry.angle);
            std::memcpy(&entry.speed, sht + at + 0x18, sizeof entry.speed);
            std::memcpy(&entry.width, sht + at + 0x0c, sizeof entry.width);
            std::memcpy(&entry.height, sht + at + 0x10, sizeof entry.height);
            entry.type = i32(sht[at + 0x21]);
            entry.behavior = i32(read_u32(sht + at + 0x2c));
            entry.initializer = i32(read_u32(sht + at + 0x28));
            entry.hit_behavior = i32(read_u32(sht + at + 0x34));
            std::memcpy(&entry.motion_phase, sht + at + 0x1c, sizeof entry.motion_phase);
            records.push_back(entry);
        }
        battle.sht_shots[block] = std::move(records);
    }
    battle.sht_shots_loaded = true;
    return true;
}

}

bool StageBattle::prepare_player(EclResourceProvider& provider, SessionState& progress, HudPlayerState& player,
                                 const char* sht_name, bool cold_entry) {
    progress_ = &progress;
    player_ = &player;
    character_ = progress.character & 3;
    if (!player_state_.initialized) {
        player_state_ = StagePlayerState{};
        invulnerable = 120;
        player_state_.initialized = true;
    }
    activate_stage_player();
    sim.player_x = player_x;
    sim.player_y = player_y;
    bomb_timer = 0;
    bomb_age = -1;
    bomb_age_accumulator_ = 0;
    bomb_generation = 0;
    bomb_rays = {};
    bomb_duration_ = 0;
    bomb_orbs = {};
    sht_shots = {};
    sht_shots_loaded = false;
    player_hit_size_ = kPlayerHitSize[progress.character & 3];
    if (sht_name) {
        std::vector<u8> sht;
        if (!provider.read(sht_name, sht) || sht.size() < 0x20) {
            error_ = std::string("cannot read ") + sht_name;
            return false;
        }
        float fields[4];
        for (int i = 0; i < 4; ++i)
            std::memcpy(&fields[i], sht.data() + 0x10 + i * 4, sizeof fields[i]);
        speed_orth = speed128_from_float(fields[0]);
        speed_orth_focus = speed128_from_float(fields[1]);
        speed_diag = speed128_from_float(fields[2]);
        speed_diag_focus = speed128_from_float(fields[3]);
        if (sht.size() < 0xc8) {
            error_ = std::string("missing option table in ") + sht_name;
            return false;
        }
        for (int mode = 0; mode < 2; ++mode)
            for (int i = 0; i < 10; ++i)
                for (int axis = 0; axis < 2; ++axis) {
                    float value;
                    std::memcpy(&value, sht.data() + 0x28 + mode * 0x50 + i * 8 + axis * 4, 4);
                    option_offsets_[mode][i][axis] = speed128_from_float(value);
                }
        if (!parse_sht_shots(sht.data(), sht.size(), *this)) {
            error_ = std::string("invalid shot table in ") + sht_name;
            return false;
        }
        if (cold_entry) {
            player.power_step = 100;
            player.max_power = i32(read_u32(sht.data() + 0x20)) * player.power_step;
        }
    }
    if (cold_entry) {
        if (!sht_name) {
            error_ = "Cold player entry requires SHT";
            return false;
        }
        const i32 steps = (progress.stage > 1 || (progress.run_mode_flags & 0x30u) == 0x20u) ? 4 : 1;
        player.power = std::min(steps * player.power_step, player.max_power);
    }
    if (cold_entry) update_options(true);
    player_prepared_ = true;
    return true;
}

bool StageBattle::load(EclResourceProvider& provider, const char* stage_file, i32 difficulty_rank,
                       StageBattleServices& host, SessionState& progress, HudPlayerState& player, HudScoreState& score,
                       const char* event_path, const char* sht_name) {
    host_ = &host;
    progress_ = &progress;
    sim.session = &progress;
    sim.mode_flags = progress.run_mode_flags;
    player_ = &player;
    score_ = &score;
    sim.spell.reset();
    sim.bomb_active = false;
    sim.spell_award = [this](i32 bonus) { score_->score = std::min(999999999, score_->score + bonus); };
    const bool prepared = player_prepared_;
    if (!prepared && !prepare_player(provider, progress, player, sht_name, false)) return false;
    player_prepared_ = false;
    item_manager.bind_random(sim.random);
    item_manager.life_changed = [this](i32 stock, i32 pieces) {
        if (!host_->life_hud(stock, pieces)) error_ = "Item life HUD refresh failed";
    };
    item_manager.bombs_changed = [this](i32 stock, i32 pieces) {
        if (!host_->bomb_hud(stock, pieces)) error_ = "Item bomb HUD refresh failed";
    };
    item_manager.option_power_changed = [this] {
        rebuild_options_for_power();
        if (option_power_changed && !option_power_changed(*this)) error_ = "Player option presentation failed";
    };
    sim.items = &item_manager;
    item_manager.spirit_birth_sound = [this](i32 id, float x) {
        if (!host_->hud_sound(id, x)) {
            sim.error_ = "Spirit birth sound failed";
            return;
        }
    };
    sim.enemy_absorb_spirits = [this](StageEnemy& enemy) {
        for (auto& spirit : item_manager.spirits()) {
            if (!spirit.alive) continue;
            if (spirit.state != 2 && spirit.kind != 3) spirit.state = 5;
            const float dx = spirit.x - enemy.position[0], dy = spirit.y - enemy.position[1];
            if (float(double(dx) * dx + double(dy) * dy) >= 400) continue;
            if (!host_->hud_sound(63, spirit.x) || !host_->hud_sound(67, spirit.x)) {
                sim.error_ = "Spirit absorption sound failed";
                return false;
            }
            if (enemy.spell_active) {
                enemy.spell_life += enemy.i_slots[2];
                enemy.life =
                    float((enemy.spell_life - enemy.phase_life_threshold * 7) / 7 + enemy.phase_life_threshold);
            } else
                enemy.life += float(enemy.i_slots[2]);
            ++enemy.i_slots[3];
            spirit.alive = false;
            spirit.state = 0;
        }
        return true;
    };
    item_manager.bind_session(progress);
    sim.retire_sink = [this](u32 serial) { host_->enemy_retire(serial); };
    sim.enemy_hit = [this](StageEnemy& enemy) {
        collide_shots(enemy);
        if (!error_.empty()) sim.error_ = error_;
    };
    sim.enemy_body_collision = [this](StageEnemy& enemy) {
        collide_enemy_body(enemy);
        if (!error_.empty()) sim.error_ = error_;
    };
    sim.bullet_player_collision = [this](StageBullet& bullet) {
        collide_player_bullet(bullet);
        if (!error_.empty()) sim.error_ = error_;
    };
    sim.laser_collision = [this](const StageLaser& laser) {
        const i32 result = collide_laser(laser);
        if (!error_.empty()) sim.error_ = error_;
        return result;
    };
    if (!sim.begin(provider, stage_file, difficulty_rank, event_path)) {
        error_ = sim.error();
        return false;
    }
    if (!sink_open_) events_ = event_path != nullptr;
    life_probe_ = std::getenv("TH13_LIFE_PROBE") != nullptr;
    if (!sim.start()) {
        error_ = sim.error();
        return false;
    }
    if (!prepared) update_options(true);
    if (events_) sim.sink.set_frame(0);
    return true;
}

void StageBattle::activate_stage_player() {
    player_dead = false;
    player_phase = PlayerPhase::Active;
    player_phase_frame = 0;
    player_phase_clock_.set(0);
    player_state_.startup_clock.set(0);
    fire_clock_.set(-1);
    player_state_.charge_clock.set(0);
    option_power_ = -1;
    option_reset_pending_ = false;
    option_movement_tick_ = false;
    for (auto& position : movement_history_)
        position = {pos_fx, pos_fy};
}

i32 StageBattle::reserve_damage_registration() {
    i32 slot = -1;
    for (i32 n = 0; n < 256; ++n) {
        const i32 candidate = (damage_registration_cursor + n) & 255;
        if (!damage_registrations[size_t(candidate)].active) {
            slot = candidate;
            break;
        }
    }
    if (slot < 0) {
        slot = damage_registration_cursor & 255;
    }
    auto& registration = damage_registrations[size_t(slot)];
    const float radius = registration.radius, growth = registration.radius_growth;
    const float angle = registration.angle, width = registration.width;
    const float height = registration.height;
    registration = DamageRegistration{};
    registration.radius = radius;
    registration.radius_growth = growth;
    registration.angle = angle;
    registration.width = width;
    registration.height = height;
    registration.active = true;
    registration.flags = 1;
    damage_registration_cursor = slot;
    return slot;
}

i32 StageBattle::register_circle(Vec3 center, float radius, float growth, i32 lifetime, i32 damage, i32 interval,
                                 i32 threshold, i32 orb) {
    const i32 slot = reserve_damage_registration();
    if (slot < 0) return slot;
    auto& registration = damage_registrations[size_t(slot)];
    registration.flags = 3u | (orb >= 0 ? 4u : 0u);
    registration.owner = DamageRegistration::Owner::Area;
    registration.orb = orb;
    registration.center_x = center.x;
    registration.center_y = center.y;
    registration.center_z = center.z;
    registration.radius = radius;
    registration.radius_growth = growth;
    registration.clock.set(lifetime);
    registration.interval = interval;
    registration.damage = float(damage);
    registration.threshold = threshold;
    return slot;
}

i32 StageBattle::register_box(Vec3 center, float width, float height, float angle, i32 damage) {
    const i32 slot = reserve_damage_registration();
    if (slot < 0) return slot;
    auto& registration = damage_registrations[size_t(slot)];
    registration.flags = 5;
    registration.owner = DamageRegistration::Owner::Area;
    registration.center_x = center.x;
    registration.center_y = center.y;
    registration.center_z = center.z;
    registration.width = width;
    registration.height = height;
    registration.angle = angle;
    registration.clock.set(0);
    registration.damage = float(damage);
    return slot;
}

i32 StageBattle::allocate_damage_registration(PlayerShot& shot) {
    const i32 slot = reserve_damage_registration();
    if (slot >= 0) {
        auto& registration = damage_registrations[size_t(slot)];
        registration.center_x = shot.x;
        registration.center_y = shot.y;
        registration.center_z = 0;
        registration.width = shot.width;
        registration.height = shot.height;
        registration.angle = shot.angle;
        registration.clock.set(9999999);
        registration.interval = 1;
        registration.damage = shot.damage;
        registration.threshold = 10000000;
        registration.generation = shot.generation;
        shot.registration_slot = slot;
        return slot;
    }
    shot.registration_slot = -1;
    return -1;
}

void StageBattle::retire_damage_registration(PlayerShot& shot) {
    const i32 slot = shot.registration_slot;
    if (slot >= 0 && slot < 256) {
        auto& registration = damage_registrations[size_t(slot)];
        registration.active = false;
        registration.flags &= ~1u;
        registration.damage = 0;
    }
    shot.registration_slot = -1;
}

void StageBattle::sync_damage_registrations() {
    for (auto& registration : damage_registrations) {
        if (!registration.active || !(registration.flags & 1u)) continue;
        if (registration.owner != DamageRegistration::Owner::Shot) continue;
        PlayerShot* owner = nullptr;
        for (auto& shot : shots) {
            if (shot.generation == registration.generation) {
                owner = &shot;
                break;
            }
        }
        if (!owner || !owner->alive || owner->state != 1) {
            registration.active = false;
            registration.flags &= ~1u;
            const bool anchored_beam = owner && owner->type == 2 && owner->behavior == 2 && owner->anchor > 0;
            if (owner && !anchored_beam && owner->registration_slot == i32(&registration - damage_registrations.data()))
                owner->registration_slot = -1;
            continue;
        }
        if (owner->behavior == 2) {
            registration.center_x = owner->damage_x;
            registration.center_y = owner->damage_y;
            registration.width = owner->width;
        } else {
            registration.center_x = owner->x;
            registration.center_y = owner->y;
            registration.width = owner->width;
            registration.height = owner->height;
            registration.angle = owner->angle;
            registration.damage = owner->damage;
        }
    }
}

void StageBattle::move_player(u32 held, bool bomb_slow) {
    option_movement_tick_ = true;
    focus = (held & 0x08) != 0 && !sim.enemies.empty() && player_state_.startup_clock.current >= 4;
    const int dir = dir_index(held);
    i32 speed = dir == 0 ? 0
                : focus  ? (dir < 5 ? speed_orth_focus : speed_diag_focus)
                         : (dir < 5 ? speed_orth : speed_diag);
    if (character_ == 1 && progress_ && trance_active(*progress_)) speed = speed * 3 / 2;
    if (character_ == 1 && bomb_slow) speed = i32(float(speed) * 0.2f);
    const float rate = sim.time_rate();
    if ((touch.mode == 1 || touch.mode == 2) && !(held & 0xf0u) && std::isfinite(touch.x) && std::isfinite(touch.y)) {
        double x = double(touch.x) * 128.0 - pos_fx;
        double y = double(touch.y) * 128.0 - pos_fy;
        if (touch.mode == 1) {
            double limit = double(focus ? speed_orth_focus : speed_orth) * rate;
            if (character_ == 1 && progress_ && trance_active(*progress_)) limit *= 1.5;
            if (character_ == 1 && bomb_slow) limit *= 0.2;
            limit = std::max(limit, 0.0);
            const double length = std::hypot(x, y);
            if (length > limit && length > 0) {
                x *= limit / length;
                y *= limit / length;
            }
        }
        x = std::clamp(x, double(-0x5c00 - pos_fx), double(0x5c00 - pos_fx));
        y = std::clamp(y, double(0x1000 - pos_fy), double(0xd800 - pos_fy));
        pos_fx += truncate_int(float(x));
        pos_fy += truncate_int(float(y));
    } else {
        pos_fx += truncate_int(float(float(speed * kDirX[dir]) * rate));
        pos_fy += truncate_int(float(float(speed * kDirY[dir]) * rate));
    }
    if (pos_fx < -0x5c00)
        pos_fx = -0x5c00;
    else if (pos_fx > 0x5c00)
        pos_fx = 0x5c00;
    if (pos_fy < 0x1000)
        pos_fy = 0x1000;
    else if (pos_fy > 0xd800)
        pos_fy = 0xd800;
    player_x = float(pos_fx) * 0.0078125f;
    player_y = float(pos_fy) * 0.0078125f;
}

void StageBattle::restore_replay_player(i32 x, i32 y, bool initial_focus) {
    pos_fx = x;
    pos_fy = y;
    player_x = float(x) * 0.0078125f;
    player_y = float(y) * 0.0078125f;
    focus = initial_focus;
    update_options(true);
    sim.player_x = player_x;
    sim.player_y = player_y;
}

void StageBattle::rebuild_options_for_power() {
    const i32 count = std::max(0, std::min(power_level(), 4));
    const i32 first = count * (count - 1) / 2;
    option_count = count;
    for (int i = 0; i < 8; ++i) {
        auto& option = options[i];
        option.active = i < count && player_phase != PlayerPhase::Dying && !player_dead;
        if (i >= count) continue;
        auto offset = option_offsets_[focus ? 1 : 0][first + i];
        if (character_ == 3) {
            offset = focus ? std::array<i32, 2>{0, -3200}
                           : std::array<i32, 2>{movement_history_[(i + 1) * 8 - 1][0] - pos_fx,
                                                movement_history_[(i + 1) * 8 - 1][1] - pos_fy};
        }
        option_fixed_[i] = {pos_fx + offset[0], pos_fy + offset[1]};
        option.x = float(option_fixed_[i][0]) * 0.0078125f;
        option.y = float(option_fixed_[i][1]) * 0.0078125f;
    }
    option_power_ = count;
    option_reset_pending_ = true;
}

void StageBattle::update_options(bool reset) {
    const i32 count = std::max(0, std::min(power_level(), 4));
    const bool changed = option_power_ != count;
    if (reset)
        for (auto& position : movement_history_)
            position = {pos_fx, pos_fy};
    if (option_movement_tick_ &&
        (dir_index(input_state) != 0 || movement_history_[0][0] != pos_fx || movement_history_[0][1] != pos_fy)) {
        const int last = std::min(count * 8, 32);
        for (int i = 32; i > last; --i)
            movement_history_[i] = movement_history_[last];
        for (int i = last; i > 0; --i)
            movement_history_[i] = movement_history_[i - 1];
        movement_history_[0] = {pos_fx, pos_fy};
    }
    option_count = count;
    const i32 first = count * (count - 1) / 2;
    for (int i = 0; i < 8; ++i) {
        auto& option = options[i];
        option.active = i < count && player_phase != PlayerPhase::Dying && !player_dead;
        if (i >= count) continue;
        if (!reset && !option_movement_tick_) continue;
        auto target = option_offsets_[focus ? 1 : 0][first + i];
        if (character_ == 3) {
            target = focus ? std::array<i32, 2>{0, -3200}
                           : std::array<i32, 2>{movement_history_[(i + 1) * 8 - 1][0] - pos_fx,
                                                movement_history_[(i + 1) * 8 - 1][1] - pos_fy};
        }
        target[0] += pos_fx;
        target[1] += pos_fy;
        auto& position = option_fixed_[i];
        if (reset || changed || option_reset_pending_) {
            position = target;
        } else {
            const i32 dx = (target[0] - position[0]) * 30 / 100;
            const i32 dy = (target[1] - position[1]) * 30 / 100;
            if (dx == 0 && dy == 0)
                position = target;
            else {
                position[0] += dx;
                position[1] += dy;
            }
        }
        option.x = float(position[0]) * 0.0078125f;
        option.y = float(position[1]) * 0.0078125f;
    }
    option_power_ = count;
    if (reset || option_movement_tick_) option_reset_pending_ = false;
}

void StageBattle::update_firing(bool held) {
    const bool enabled = player_state_.startup_clock.current > 19 &&
                         !(progress_ && (progress_->scene_flags & 0x4000u)) &&
                         !(sim.dialog_host && sim.dialog_host->dialog_active());
    if (!enabled) {
        fire_clock_.set(-1);
        player_state_.charge_clock.set(0);
        if (host_ && (!host_->stop_sound(30) || !host_->stop_sound(55))) error_ = "Youmu charge sound reset failed";
        if (youmu_charge_effect && !youmu_charge_effect(2, {player_x, player_y, 0}))
            error_ = "Youmu charge reset failed";
        return;
    }
    if (player_dead || player_phase != PlayerPhase::Active) return;
    if (character_ == 3) {
        auto& charge = player_state_.charge_clock;
        const auto effect = [&](i32 action) {
            if (youmu_charge_effect && !youmu_charge_effect(action, {player_x, player_y, 0}))
                error_ = "Youmu charge effect failed";
        };
        const auto release_charge = [&]() {
            if (host_ && (!host_->stop_sound(30) || !host_->stop_sound(55))) error_ = "Youmu charge sound stop failed";
            effect(2);
            if (charge.current < 60)
                charge.set(0);
            else {
                charge.set(-40);
                player_state_.charge_origin = {player_x, player_y, 0};
            }
        };
        const auto fire_tail = [&]() {
            const float saved_x = player_x, saved_y = player_y;
            const bool saved_focus = focus;
            player_x = player_state_.charge_origin.x;
            player_y = player_state_.charge_origin.y;
            focus = true;
            fire_shots(charge.current + 40);
            focus = saved_focus;
            player_x = saved_x;
            player_y = saved_y;
            const float rate = sim.time_rate();
            charge.tick(&rate);
        };
        if (focus) {
            if (held && charge.current >= 0) {
                if (charge.current == 0) {
                    if (host_ && !host_->hud_sound(30, player_x)) error_ = "Youmu charge sound failed";
                    effect(0);
                }
                const float rate = sim.time_rate();
                charge.tick(&rate);
                if (host_ && (!host_->set_voice_pan(30, player_x) || !host_->set_voice_pan(55, player_x)))
                    error_ = "Youmu charge sound pan failed";
                if (charge.current == 60) {
                    if (host_ && (!host_->stop_sound(30) || !host_->hud_sound(55, player_x)))
                        error_ = "Youmu charge ready sound failed";
                    effect(1);
                }
            } else if (charge.current > 0) {
                release_charge();
            } else if (charge.current < 0) {
                fire_tail();
                if (charge.current >= 0) charge.set(0);
            }
            effect(3);
            return;
        }
        if (charge.current > 0)
            release_charge();
        else if (charge.current < 0)
            fire_tail();
        effect(3);
    }
    if (fire_clock_.current < 0) {
        if (!held) return;
        fire_clock_.set(0);
    }
    if (fire_clock_.current != fire_clock_.previous) fire_shots(fire_clock_.current);
    const float rate = sim.time_rate();
    if (fire_clock_.current < 14)
        fire_clock_.tick(&rate);
    else if (!held)
        fire_clock_.set(-1);
    else {
        fire_clock_.previous = fire_clock_.current;
        const float delta = rate > .99f && rate < 1.01f ? -14.f : float(rate * -14.f);
        fire_clock_.fractional = float(fire_clock_.fractional + delta);
        fire_clock_.current = truncate_int(fire_clock_.fractional);
    }
}

void StageBattle::fire_shots(i32 frame) {
    const auto spawn = [&](float anchor_x, float anchor_y, float offset_x, float offset_y, float damage, float angle,
                           i32 script, float speed, i32 behavior) {
        PlayerShot shot;
        shot.damage = damage;
        shot.angle = angle;
        shot.script = script;
        shot.speed = speed;
        shot.behavior = behavior;
        shot.generation = ++shot_generation_;
        float vx, vy;
        polar_xy(angle, float(speed * sim.time_rate()), vx, vy);
        shot.x = float((double(offset_x) - double(vx)) + double(anchor_x));
        shot.y = float((double(offset_y) - double(vy)) + double(anchor_y));
        shot.alive = true;
        shots.push_back(shot);
    };
    if (sht_shots_loaded) {
        i32 level = player_ ? player_->power / player_->power_step : 0;
        if (level < 0) level = 0;
        if (level > 4) level = 4;
        const u32 block = progress_ && trance_active(*progress_) ? 10u : u32(level) + (focus ? 5u : 0u);
        static const bool shot_probe = [] {
            const char* probe = std::getenv("TH13_SHOT_PROBE");
            return probe && probe[0] == '1';
        }();
        for (const auto& record : sht_shots[block]) {
            if (record.interval <= 0) continue;
            if (frame % record.interval != record.phase) continue;
            if (record.type == 2 && std::any_of(shots.begin(), shots.end(), [&](const PlayerShot& shot) {
                    return shot.alive && shot.state == 1 && shot.type == 2 && shot.anchor == record.anchor;
                }))
                continue;
            float x = player_x, y = player_y;
            if (record.anchor != 0) {
                const i32 index = record.anchor - 1;
                if (index < 0 || index >= option_count || !options[index].active) continue;
                x = options[index].x;
                y = options[index].y;
            }
            spawn(x, y, record.x, record.y, record.damage, record.angle, record.script, record.speed, record.behavior);
            auto& shot = shots.back();
            shot.width = record.width;
            shot.height = record.height;
            shot.type = record.type;
            shot.anchor = record.anchor;
            shot.anchor_power = power_level();
            shot.target_angle = record.angle;
            shot.hit_behavior = record.hit_behavior;
            if (player_shot_birth && !player_shot_birth(shot, u32(shots.size() - 1))) {
                error_ = "Player shot animation birth failed";
                return;
            }
            if (allocate_damage_registration(shot) < 0) return;
            if (record.initializer == 2) {
                shot.width = 0;
                damage_registrations[size_t(shot.registration_slot)].width = 0;
                if (host_ && !host_->hud_sound(20, player_x)) {
                    error_ = "Player beam sound failed";
                    return;
                }
            }
            if (record.initializer == 3) {
                shot.sine_moving = true;
                auto& motion = shot.sine_motion;
                motion.mode = 4;
                motion.pos[0] = motion.origin[0] = shot.x;
                motion.pos[1] = motion.origin[1] = shot.y;
                motion.speed = shot.speed;
                motion.angle = shot.angle;
                motion.ellipse_angle = wrap_angle(wrap_angle(record.angle));
                motion.radius = 32.0f;
                motion.growth = 0.5235987901687622f;
                motion.sine_phase = wrap_angle(record.motion_phase);
                polar_xy(shot.angle, float(shot.speed * sim.time_rate()), motion.aux[0], motion.aux[1]);
                motion.accumulate(sim.time_rate());
                shot.x = motion.pos[0];
                shot.y = motion.pos[1];
                shot.angle = motion.angle;
            }
            if (record.sound >= 0 && host_ && !host_->hud_sound(record.sound, shot.x)) {
                error_ = "Player shot sound failed";
                return;
            }
            if (shot_probe && sim.frame <= 1300)
                std::printf("PROBE fire ff=%d sim=%d pos=(%.2f,%.2f) dmg=%.1f\n", frame, sim.frame, player_x + record.x,
                            player_y + record.y, double(record.damage));
        }
        return;
    }
    if (frame % kFireInterval != 0) return;
    for (int i = -1; i <= 1; ++i) {
        spawn(player_x, player_y, kShotOffsetX * float(i), -kShotOffsetY, kShotDamage, -1.5707964f, -1, kShotSpeed, 0);
        allocate_damage_registration(shots.back());
    }
}

void StageBattle::step_shots() {
    for (auto& shot : shots) {
        if (!shot.alive) continue;
        if (shot.animation_finished) {
            retire_damage_registration(shot);
            shot.alive = false;
            shot.state = 0;
            continue;
        }
        if (shot.behavior == 2 && shot.anchor > 0 && shot.anchor <= i32(options.size())) {
            const auto& option = options[u32(shot.anchor - 1)];
            shot.x = option.x;
            shot.y = option.y;
            if (shot.angle != shot.target_angle) {
                const float delta = wrap_angle(shot.target_angle - shot.angle);
                shot.angle = wrap_angle(wrap_angle(float(shot.angle + float(delta * .1f))));
            }
            if (shot.state == 1) {
                if (shot.width < 512.f) shot.width += 18.f;
                shot.beam_visual_width = shot.width;
                float dx, dy;
                polar_xy(shot.angle, shot.width * .5f, dx, dy);
                shot.damage_x = shot.x + dx;
                shot.damage_y = shot.y + dy;
                const bool invalid = fire_clock_.current < 0 || shot.anchor > option_count ||
                                     shot.anchor_power != power_level() || focus ||
                                     (progress_ && trance_active(*progress_)) ||
                                     (sim.dialog_host && sim.dialog_host->dialog_active());
                if (invalid) {
                    shot.state = 2;
                    if (host_ && !host_->stop_sound(20)) {
                        error_ = "Player beam stop sound failed";
                        return;
                    }
                }
                if (shot.state == 1 && !shot.beam_contact_seen && shot.beam_contact) {
                    shot.beam_interrupt = 3;
                    shot.beam_contact = false;
                }
                shot.beam_contact_seen = false;
            }
        } else if (shot.state == 1 && shot.behavior == 3) {
            shot.speed = float(double(shot.speed) + 1.0);
        } else if (shot.state == 1 && shot.behavior == 5) {
            shot.speed = float(double(shot.speed) * 0.800000011920928955078125);
        } else if (shot.state == 1 && shot.behavior == 1) {
            StageEnemy* target = nullptr;
            if (shot.target_serial) {
                for (auto& enemy : sim.enemies)
                    if (enemy->serial == shot.target_serial && !enemy->dead && !(enemy->flags & 0x18000021u))
                        target = enemy.get();
                if (!target) shot.target_serial = 0;
            } else {
                target = nearest_target(sim, shot.x, shot.y, 256.0f);
                shot.target_serial = target ? target->serial : 0;
            }
            if (target) {
                const float wanted = float(std::atan2(double(float(target->position[1] - shot.y)),
                                                      double(float(target->position[0] - shot.x))));
                const double delta = double(wanted) - shot.angle;
                const float difference =
                    float(delta > double(3.1415927f)    ? double(wanted) - (double(shot.angle) + double(6.2831855f))
                          : -delta > double(3.1415927f) ? double(wanted) - (double(shot.angle) - double(6.2831855f))
                                                        : delta);
                if (shot.age > 59)
                    shot.speed += 0.2f;
                else {
                    if (std::abs(difference) >= 0.7853982f)
                        shot.speed = std::max(4.0f, shot.speed - 0.2f);
                    else if (std::abs(difference) < 0.2617994f)
                        shot.speed = std::min(16.0f, shot.speed + 0.2f);
                    shot.angle = wrap_angle(wrap_angle(float(shot.angle + float(double(difference) * double(.08f)))));
                }
            } else {
                shot.speed = std::min(16.0f, shot.speed + 0.1f);
            }
        }
        float vx, vy;
        const float rate = sim.time_rate();
        if (shot.sine_moving) {
            shot.sine_motion.advance(rate);
            shot.x = shot.sine_motion.pos[0];
            shot.y = shot.sine_motion.pos[1];
            shot.angle = shot.sine_motion.angle;
        } else {
            polar_xy(shot.angle, float(shot.speed * rate), vx, vy);
            shot.x = round001(float(shot.x + vx));
            shot.y = round001(float(shot.y + vy));
        }
        if (shot.script < 0 && (shot.y < -32.0f || shot.y > 480.0f || shot.x < -224.0f || shot.x > 224.0f))
            shot.alive = false;
        if (shot.age_clock.current != shot.age) shot.age_clock.set(shot.age);
        shot.age_clock.tick(&rate);
        shot.age = shot.age_clock.current;
    }
    consume_player_shot_geometry();
    sync_damage_registrations();
    size_t keep = 0;
    for (size_t i = 0; i < shots.size(); ++i)
        if (shots[i].alive) shots[keep++] = shots[i];
    shots.resize(keep);
}

void StageBattle::begin_player_shot_geometry_snapshot() {
    for (auto& snapshot : shot_geometry_)
        snapshot.valid = false;
}

void StageBattle::set_player_shot_geometry(i32 slot, i32 age, i32 script, u32 generation,
                                           const std::array<Vec3, 4>& quad) {
    if (slot < 0) return;
    if (shot_geometry_.size() <= size_t(slot)) shot_geometry_.resize(size_t(slot) + 1);
    auto& snapshot = shot_geometry_[size_t(slot)];
    snapshot.valid = true;
    snapshot.slot = slot;
    snapshot.age = age;
    snapshot.script = script;
    snapshot.generation = generation;
    snapshot.quad = quad;
}

void StageBattle::consume_player_shot_geometry() {
    for (size_t slot = 0; slot < shots.size(); ++slot) {
        auto& shot = shots[slot];
        if (!shot.alive || shot.type == 2) continue;
        bool matched = false;
        if (slot < shot_geometry_.size()) {
            const auto& snapshot = shot_geometry_[slot];
            if (snapshot.valid && snapshot.slot == i32(slot) && snapshot.generation == shot.generation &&
                snapshot.script == shot.script && snapshot.age == shot.age - 1 && snapshot.age > 9) {
                matched = true;
                bool outside = true;
                for (const auto& vertex : snapshot.quad) {
                    const bool vertex_outside =
                        vertex.x <= 128.0f || vertex.x >= 512.0f || vertex.y <= 16.0f || vertex.y >= 464.0f;
                    outside = outside && vertex_outside;
                }
                if (outside) shot.alive = false;
            }
        }
        if (!matched && shot.alive && shot.age > 10 &&
            (shot.y < -32.0f || shot.y > 480.0f || shot.x < -224.0f || shot.x > 224.0f))
            shot.alive = false;
    }
}

bool damage_probe_enabled = false;
u32 damage_probe_first_frame = 7970;
u32 damage_probe_last_frame = 7985;
bool rng_probe_on = false;

float StageBattle::query_enemy_damage(StageEnemy& state, float width, float height) {
    if (player_phase_clock_.current == player_phase_clock_.previous) return 0;
    static const bool shot_probe = [] {
        const char* probe = std::getenv("TH13_SHOT_PROBE");
        return probe && probe[0] == '1';
    }();
    StageEnemy* enemy = &state;
    float damage = 0;
    const float ex = enemy->position[0], ey = enemy->position[1];
    for (auto& registration : damage_registrations) {
        if (!registration.active || !(registration.flags & 1u)) continue;
        if (registration.clock.current == registration.clock.previous) continue;
        if (registration.interval <= 0 || registration.clock.current % registration.interval != 0) continue;
        if (registration.owner == DamageRegistration::Owner::Area) {
            if (registration.flags & 2u) {
                const double dx = double(ex) - registration.center_x;
                const double dy = double(ey) - registration.center_y;
                if (dx * dx + dy * dy > double(registration.radius) * registration.radius) continue;
            } else if (!shot_hits_enemy_box(registration.center_x, registration.center_y, registration.width,
                                            registration.height, ex, ey, width, height, registration.angle))
                continue;
            state.last_hit_position[0] = registration.center_x;
            state.last_hit_position[1] = registration.center_y;
            if (damage_probe_enabled && sim.frame >= damage_probe_first_frame && sim.frame <= damage_probe_last_frame)
                std::printf("DMG f=%d e=%u q=(%.1f,%.1f) reg=%u dmg=%.1f c=(%.1f,%.1f) box=(%.1f,%.1f)\n", sim.frame,
                            state.serial, ex, ey, unsigned(&registration - damage_registrations.data()),
                            double(registration.damage), double(registration.center_x), double(registration.center_y),
                            double(registration.width), double(registration.height));
            damage += registration.damage;
            registration.accumulated_damage = wrapping_add(registration.accumulated_damage, i32(registration.damage));
            if (registration.orb >= 0 && registration.orb < i32(bomb_orbs.size()))
                bomb_orbs[size_t(registration.orb)].accumulated_damage = float(registration.accumulated_damage);
            if (registration.threshold < 9999999 && registration.accumulated_damage >= registration.threshold) {
                registration.active = false;
                registration.flags &= ~1u;
                registration.damage = 0;
            }
            continue;
        }
        const float dx = registration.center_x - ex;
        const float dy = registration.center_y - ey;
        if (shot_probe && sim.frame <= 130)
            std::printf("HITCHK f=%d reg=(%.2f,%.2f) enemy=(%.2f,%.2f) "
                        "size=(%.2f,%.2f) dx=%.2f dy=%.2f\n",
                        sim.frame, registration.center_x, registration.center_y, ex, ey, registration.width,
                        registration.height, dx, dy);
        if (!shot_hits_enemy_box(registration.center_x, registration.center_y, registration.width, registration.height,
                                 ex, ey, width, height, registration.angle))
            continue;
        PlayerShot* shot = nullptr;
        for (auto& candidate : shots)
            if (candidate.generation == registration.generation) {
                shot = &candidate;
                break;
            }
        if (!shot || !shot->alive || shot->state != 1) {
            registration.active = false;
            registration.flags &= ~1u;
            continue;
        }
        state.last_hit_position[0] = registration.center_x;
        state.last_hit_position[1] = registration.center_y;
        registration.accumulated_damage = wrapping_add(registration.accumulated_damage, i32(registration.damage));
        if (shot->hit_behavior == 1 && shot->behavior == 2) {
            const float beam_dx = ex - shot->x, beam_dy = ey - shot->y;
            const float distance_squared = float(double(beam_dx) * beam_dx + double(beam_dy) * beam_dy);
            const float distance = std::sqrt(distance_squared);
            const float shortened = std::max(0.0f, distance - 24.0f);
            registration.width = shortened;
            shot->width = shortened;
            shot->beam_contact_seen = true;
            if (!shot->beam_contact) {
                shot->beam_contact = true;
                shot->beam_interrupt = 2;
            }
            if (shot->age_clock.current != shot->age_clock.previous && (shot->age_clock.current & 1) == 0 &&
                beam_hit_effect) {
                float tip_x, tip_y, travel_x, travel_y;
                polar_xy(shot->angle, shortened, tip_x, tip_y);
                polar_xy(shot->angle, 64.f, travel_x, travel_y);
                if (!beam_hit_effect({float(shot->x + tip_x), float(shot->y + tip_y), 0}, {travel_x, travel_y, 0},
                                     shot->angle)) {
                    error_ = "Player beam hit effect failed";
                    return 0;
                }
            }
            if (shot->age_clock.current != shot->age_clock.previous && (shot->age_clock.current & 3) == 0) {
                if (damage_probe_enabled && sim.frame >= damage_probe_first_frame &&
                    sim.frame <= damage_probe_last_frame)
                    std::printf("DMGS f=%d e=%u reg=%u dmg=%.1f c=(%.1f,%.1f) box=(%.1f,%.1f)\n", sim.frame,
                                state.serial, unsigned(&registration - damage_registrations.data()),
                                double(registration.damage), double(registration.center_x),
                                double(registration.center_y), double(registration.width), double(registration.height));
                damage += registration.damage;
            }
            if (registration.threshold < 9999999 && registration.accumulated_damage >= registration.threshold) {
                registration.active = false;
                registration.flags &= ~1u;
                registration.damage = 0;
            }
            continue;
        }
        if (shot->hit_behavior == 4) {
            if (shot->damage > 1.f) shot->damage -= 1.f;
            damage += shot->damage;
            if (registration.threshold < 9999999 && registration.accumulated_damage >= registration.threshold) {
                registration.active = false;
                registration.flags &= ~1u;
                registration.damage = 0;
            }
            continue;
        }
        if (shot->hit_behavior == 2) {
            const float hit_damage = registration.damage;
            const i32 area = register_circle({shot->x, shot->y, 0}, 24.f, 1.f, 20, i32(shot->damage), 4);
            if (area < 0) return 0;
            auto& blast = damage_registrations[size_t(area)];
            blast.travel_speed = 2.f;
            blast.travel_angle = shot->angle;
            shot->state = 2;
            shot->speed = 2.f;
            shot->registration_slot = -1;
            registration.active = false;
            registration.flags &= ~1u;
            damage += hit_damage;
            if (host_ && !host_->hud_sound(65, shot->x)) {
                error_ = "Player missile explosion sound failed";
                return 0;
            }
            continue;
        }
        if (damage_probe_enabled && sim.frame >= damage_probe_first_frame && sim.frame <= damage_probe_last_frame)
            std::printf("DMGH f=%d e=%u reg=%u gen=%u age=%d dmg=%.1f c=(%.1f,%.1f) box=(%.1f,%.1f) ebox=(%.1f,%.1f)\n",
                        sim.frame, state.serial, unsigned(&registration - damage_registrations.data()),
                        registration.generation, i32(shot->age), double(registration.damage),
                        double(registration.center_x), double(registration.center_y), double(registration.width),
                        double(registration.height), double(width), double(height));
        damage += registration.damage;
        if (shot->hit_behavior == 3) {
            shot->sine_motion.radius = 16.0f;
            shot->sine_motion.speed = shot->speed = 3.0f;
            shot->hit_fade = false;
        } else {
            shot->speed *= .125f;
            shot->hit_fade = true;
        }
        shot->state = 2;
        registration.active = false;
        registration.flags &= ~1u;
        registration.damage = 0;
        shot->registration_slot = -1;
    }
    for (auto& shot : shots) {
        if (!shot.alive || shot.state != 1 || shot.registration_slot >= 0 || shot.behavior == 2 ||
            shot.damage_clock.current == shot.damage_clock.previous)
            continue;
        if (!shot_hits_enemy_box(shot.x, shot.y, shot.width, shot.height, ex, ey, width, height, shot.angle)) continue;
        state.last_hit_position[0] = shot.x;
        state.last_hit_position[1] = shot.y;
        damage += shot.damage;
        shot.state = 2;
        if (shot.hit_behavior == 3) {
            shot.sine_motion.radius = 16.0f;
            shot.sine_motion.speed = shot.speed = 3.0f;
            shot.hit_fade = false;
        } else {
            shot.speed *= .125f;
            shot.hit_fade = true;
        }
    }
    if (damage_probe_enabled && sim.frame >= damage_probe_first_frame && sim.frame <= damage_probe_last_frame)
        std::printf("DMGT f=%d e=%u q=(%.1f,%.1f) sum=%.1f\n", sim.frame, state.serial, ex, ey, double(damage));
    if (damage <= 0) return 0;
    const float capped = std::min(damage, 80.0f);
    const i32 hit_score = (i32(capped) / 10 + 10) / 10;
    score_->score += hit_score;
    if (score_->score > 999999999) score_->score = 999999999;
    if (damage_probe_enabled && sim.frame >= damage_probe_first_frame && sim.frame <= damage_probe_last_frame)
        std::printf("DMGS f=%d e=%u hit=%d score=%d\n", sim.frame, state.serial, hit_score, score_->score);
    return capped;
}

bool StageBattle::query_enemy_bomb_hit(const StageEnemy& enemy) const {
    if (player_phase_clock_.current == player_phase_clock_.previous) return false;
    i32 damage = 0;
    bool bomb_hit = false;
    for (const auto& registration : damage_registrations) {
        if (!registration.active || !(registration.flags & 1u) ||
            registration.clock.current == registration.clock.previous || registration.interval <= 0 ||
            registration.clock.current % registration.interval != 0)
            continue;
        bool hit;
        if (registration.flags & 2u) {
            const double dx = double(enemy.position[0]) - registration.center_x;
            const double dy = double(enemy.position[1]) - registration.center_y;
            hit = dx * dx + dy * dy <= double(registration.radius) * registration.radius;
        } else {
            hit = shot_hits_enemy_box(registration.center_x, registration.center_y, registration.width,
                                      registration.height, enemy.position[0], enemy.position[1], enemy.hurtbox[0],
                                      enemy.hurtbox[1], registration.angle);
        }
        if (!hit) continue;
        damage = wrapping_add(damage, i32(registration.damage));
        if (registration.flags & 4u) bomb_hit = true;
    }
    return damage != 0 && bomb_hit;
}

void StageBattle::collide_shots(StageEnemy& state) {
    StageEnemy* enemy = &state;
    if (!enemy->dead && (enemy->flags & 0x800u) && query_enemy_bomb_hit(*enemy)) {
        if (sim.death_effects_and_drops(*enemy)) enemy->dead = true;
        return;
    }
    if (enemy->dead || (enemy->flags & 0x21u)) return;
    float damage = enemy->hurtbox[0] > 0 ? query_enemy_damage(*enemy, enemy->hurtbox[0], enemy->hurtbox[1]) : 0;
    if (enemy->damage_function == 1) {
        const i32 returned = i32(damage) + i32(query_enemy_damage(*enemy, 160, 32));
        if (auto* boss = sim.boss()) boss->pending_damage = returned;
        damage = float(i32(damage) + returned);
    }
    if (enemy->pending_damage > 0) {
        damage = float(i32(damage) + enemy->pending_damage);
        enemy->pending_damage = 0;
    }
    if (sim.dialog_host && sim.dialog_host->dialog_active()) return;
    if (damage <= 0) return;
    float applied = damage;
    if (player_phase == PlayerPhase::Entering || player_phase == PlayerPhase::Dying) applied = float(i32(applied) / 5);
    const auto& spell = sim.spell.state();
    if (spell.active() && spell.id >= 102 && spell.id <= 105 && enemy->life < 1651.f &&
        (sim.bomb_active || invulnerable > 0 || (progress_ && trance_active(*progress_))))
        applied = float(i32(applied) / 5);
    if (applied <= 0) return;
    if (spell.active() && spell.id >= 109 && spell.id <= 118 && sim.bomb_active && (enemy->flags & 0x1000000u) != 0) {
        if (host_ && !host_->hud_sound(36, enemy->position[0])) error_ = "EX bomb immunity sound failed";
        return;
    }
    if (spell.active() && (spell.flags & kSpellBombProtection) != 0) applied = float(i32(applied) / 30);
    if (damage_probe_enabled) {
        std::fprintf(stderr,
                     "[enemy-damage] f=%u serial=%u dmg_in=%d applied=%d flags=%08x "
                     "life=%d accum=%d\n",
                     sim.frame, enemy->serial, i32(damage), i32(applied), enemy->flags, i32(enemy->life),
                     i32(enemy->spirit_accum));
    }
    frame_damage_total += applied;
    enemy->spirit_accum += applied;
    if (enemy->spirit_accum >= enemy->spirit_threshold) {
        const float sdx = enemy->position[0] - player_x;
        const float sdy = enemy->position[1] - player_y;
        const float dist2 = sdx * sdx + sdy * sdy;
        i32 step = 1000;
        if (dist2 < 2304.0f)
            step = 200;
        else if (dist2 < 9216.0f)
            step = 400;
        else if (dist2 < 36864.0f)
            step = 600;
        if (enemy->hit_anim_timer > 19 && !enemy->damage_spirits_disabled)
            item_manager.spawn_value_item(1, enemy->position[0], enemy->position[1], enemy->position[2], 2.0f);
        enemy->spirit_threshold += step;
    }
    enemy->hit_anim_timer = 30;
    sim.damage(*enemy, applied);
    if (!enemy->dead) enemy->flags |= 0x400000u;
    if (enemy->dead) reward_kill(*enemy);
}

void StageBattle::explode_orb(PlayerBombOrb& orb) {
    if (!orb.active) return;
    if (host_ && !host_->hud_sound(27, orb.x)) error_ = "Reimu bomb orb explosion sound failed";
    const i32 explosion = register_circle({orb.x, orb.y, 0}, 64, 8, 11, 60);
    if (explosion >= 0) damage_registrations[size_t(explosion)].flags |= 4u;
    if (orb.registration_slot >= 0) {
        auto& old = damage_registrations[size_t(orb.registration_slot)];
        old.active = false;
        old.flags &= ~1u;
        orb.registration_slot = -1;
    }
    orb.active = false;
    orb.exploding = true;
    orb.explosion_frame = 0;
    orb.explosion_damage_clock.set(11);
    sim.cancel_bullets_radius(Vec3{orb.x, orb.y, 0}, 128, true, true);
    sim.cancel_lasers_radius({orb.x, orb.y, 0}, 128);
}

void StageBattle::step_bomb() {
    youmu_slash_spawns_.clear();
    for (auto& orb : bomb_orbs) {
        if (orb.exploding) ++orb.explosion_frame;
    }
    if (bomb_timer <= 0) return;
    const i32 age_before = bomb_age;
    if (bomb_age < 0) {
        bomb_age = 0;
    } else {
        const float rate = sim.time_rate();
        if (rate > 0.99f && rate < 1.01f) {
            bomb_age += 1;
            bomb_age_accumulator_ += 1.0f;
        } else {
            bomb_age_accumulator_ += rate;
            bomb_age = i32(bomb_age_accumulator_);
        }
    }
    const bool age_advanced = bomb_age != age_before;
    bomb_timer = std::max(0, bomb_duration_ - bomb_age);
    invulnerable = 40;
    if (character_ != 0 && bomb_timer == 0) {
        if (character_ == 2 && !host_->hud_sound(0x1b)) error_ = "Sanae Bomb end sound";
        return;
    }
    if (character_ == 0) {
        if (age_advanced && bomb_age == 0)
            for (int i = 0; i < 8; ++i) {
                auto& orb = bomb_orbs[i];
                if (reimu_bomb_orb_birth && !reimu_bomb_orb_birth(*this, i)) {
                    error_ = "Reimu bomb orb animation birth failed";
                    return;
                }
                orb.registration_slot = register_circle({orb.x, orb.y, 0}, 56, 0, 9999, 10, 3, 300, i);
                if (orb.registration_slot < 0) return;
            }
        for (int i = 0; i < 8; ++i) {
            auto& orb = bomb_orbs[i];
            if (!orb.active) continue;
            if (bomb_age >= 200) {
                explode_orb(orb);
                continue;
            }
            const auto* target = bomb_age > 90 + i * 10 ? nearest_target(sim, orb.x, orb.y, 512.f) : nullptr;
            orb.step(bomb_age, i, player_x, player_y, target);
            if (orb.registration_slot >= 0 &&
                damage_registrations[size_t(orb.registration_slot)].accumulated_damage >= 300) {
                explode_orb(orb);
                if (host_ && !host_->hud_sound(27, orb.x)) error_ = "Reimu bomb orb damage explosion sound failed";
                if (reimu_explosion_shake && !reimu_explosion_shake())
                    error_ = "Reimu explosion screen shake registration failed";
                continue;
            }
            if (orb.registration_slot >= 0) {
                auto& registration = damage_registrations[size_t(orb.registration_slot)];
                registration.center_x = orb.x;
                registration.center_y = orb.y;
                registration.center_z = 0;
            }
            if (bomb_age % 8 == i) {
                sim.cancel_bullets_radius(Vec3{orb.x, orb.y, 0}, 64, true, true);
                sim.cancel_lasers_radius({orb.x, orb.y, 0}, 64);
            }
        }
        if (bomb_age >= 200 && reimu_explosion_shake && !reimu_explosion_shake())
            error_ = "Reimu end screen shake registration failed";
    } else if (character_ == 1) {
        bomb_visual_angle_ = bomb_angle_;
        bomb_center_x_ = player_x;
        bomb_center_y_ = player_y;
        if (input_state & 0x40)
            bomb_angle_ -= 0.0026179939f;
        else if (input_state & 0x80)
            bomb_angle_ += 0.0026179939f;
        if (age_advanced && bomb_age % 4 == 0) {
            static constexpr float distances[] = {208, 240, 304};
            static constexpr float heights[] = {32, 128, 256};
            static constexpr i32 damages[] = {60, 20, 20};
            for (int part = 0; part < 3; ++part) {
                float x, y;
                polar_xy(bomb_angle_, distances[part], x, y);
                if (register_box({float(player_x + x), float(player_y + y), 0}, 512, heights[part], bomb_angle_,
                                 damages[part]) < 0)
                    return;
            }
        }
        for (int part = 0; part < 3; ++part) {
            static constexpr float distances[] = {208, 240, 304};
            static constexpr float heights[] = {32, 128, 256};
            sim.cancel_bullets_box({player_x + std::cos(bomb_angle_) * distances[part],
                                    player_y + std::sin(bomb_angle_) * distances[part], 0},
                                   {512, heights[part], 0}, bomb_angle_, 0.0f, true, true, true);
        }
        const bool laser_items = !sim.spell.state().active();
        for (int part = 0; part < 3; ++part) {
            static constexpr float distances[] = {208, 240, 304};
            static constexpr float heights[] = {32, 128, 256};
            sim.cancel_lasers_box({player_x + std::cos(bomb_angle_) * distances[part],
                                   player_y + std::sin(bomb_angle_) * distances[part], 0},
                                  {512, heights[part], 0}, bomb_angle_, laser_items);
        }
    } else if (character_ == 2) {
        bomb_center_x_ = float(double(bomb_center_x_) + (0.0 - double(bomb_center_x_)) * double(0.01f));
        bomb_center_y_ = float(double(bomb_center_y_) + (160.0 - double(bomb_center_y_)) * double(0.01f));
        bomb_angle_ = combat_angle(bomb_angle_ + bomb_rotation_speed_);
        bomb_rotation_speed_ -= 0.0005235988f;
        float a = bomb_angle_;
        for (auto& ray : bomb_rays) {
            float x, y;
            polar_xy(a, 168.23f, x, y);
            ray.origin = {float(bomb_center_x_ + x), float(bomb_center_y_ + y), 0};
            double angle = double(a) - double(2.8274333f);
            if (angle > double(3.1415927f))
                angle -= double(6.2831855f);
            else if (angle < -double(3.1415927f))
                angle += double(6.2831855f);
            ray.angle = wrap_angle(float(angle));
            polar_xy(ray.angle, 160.f, x, y);
            ray.center = {float(ray.origin.x + x), float(ray.origin.y + y), 0};
            if (register_box(ray.center, 320, 32, ray.angle, 10) < 0) return;
            a = wrap_angle(float(a + 1.2566371f));
        }
        const bool laser_items = !sim.spell.state().active();
        for (const auto& ray : bomb_rays) {
            sim.cancel_bullets_box(ray.center, {320, 32, 0}, ray.angle, 0.0f, true, true, true);
            sim.cancel_lasers_box(ray.center, {320, 32, 0}, ray.angle, laser_items);
        }
    } else {
        if (age_advanced && bomb_age < YoumuBomb::kActiveFrames) {
            GameRandom::tag(15);
            if (auto spawn = youmu_bomb_.step(bomb_age, sim.random)) {
                youmu_slash_spawns_.push_back(*spawn);
                if (host_ && (!host_->hud_sound(44) || !host_->hud_sound(27))) {
                    error_ = "Youmu bomb slash sound failed";
                    return;
                }
            }
        }
        if (youmu_bomb_update && !youmu_bomb_update(*this)) {
            error_ = "Youmu Bomb animation update failed";
            return;
        }
        const bool laser_items = !sim.spell.state().active();
        for (int slot = 0; slot < YoumuBomb::kSlots; ++slot) {
            if (!youmu_bomb_.active(slot)) continue;
            const auto rect = youmu_bomb_.rect(slot);
            if (register_box({rect.cx, rect.cy, 0}, rect.w, rect.h, rect.angle, 30) < 0) return;
            sim.cancel_bullets_box({rect.cx, rect.cy, 0}, {rect.w, rect.h, 0}, rect.angle, 0.0f, true, true, true);
            if (!sim.error().empty()) return;
            sim.cancel_lasers_box({rect.cx, rect.cy, 0}, {rect.w, rect.h, 0}, rect.angle, laser_items);
        }
    }
}

void StageBattle::collide_player_bullet(StageBullet& bullet) {
    if (player_dead) return;
    const float p = player_hit_size_;
    const float half = p * 0.5f;
    const float left = player_x - half, right = player_x + half;
    const float top = player_y - half, bottom = player_y + half;
    const bool dialog_gate = !(sim.dialog_host && sim.dialog_host->dialog_active());
    const bool in_trance = progress_ && trance_active(*progress_);
    if (in_trance) return;
    if (!bullet.alive || !bullet.collision_ready || (bullet.flags & 0x2u) == 0) return;
    const bool scaled = (bullet.flags & 0x40u) != 0;
    const float sx = scaled ? bullet.hit_scale * bullet.hit_size[0] : bullet.hit_size[0];
    const float sy = scaled ? bullet.hit_scale * bullet.hit_size[1] : bullet.hit_size[1];
    int result = 0;
    if ((bullet.flags & 0x10u) != 0) {
        const double dx = double(player_x) - double(bullet.x);
        const double dy = double(player_y) - double(bullet.y);
        const float distance2 = rebuild::to_float(rebuild::square_sum_80(dx, dy));
        if (rebuild::below_square_sum(distance2, p, sx)) {
            result = 1;
        } else {
            const float span = std::max(float(double(sx) / 2.5), 40.0f);
            if (rebuild::below_square_sum(distance2, double(sx), double(span) + double(p))) result = 2;
        }
    } else {
        const float hx = sx * 0.5f, hy = sy * 0.5f;
        const bool separated =
            bullet.x + hx < left || bullet.y + hy < top || right < bullet.x - hx || bottom < bullet.y - hy;
        if (separated) {
            if (left <= bullet.x + 24.0f && top <= bullet.y + 24.0f && bullet.x - 24.0f <= right &&
                bullet.y - 24.0f <= bottom)
                result = 2;
        } else {
            result = 1;
        }
    }
    if (result == 1) {
        if (!dialog_gate || player_phase != PlayerPhase::Active) return;
        if (invulnerable <= 0) kill_player();
        if (bullet.cancel_protection == 0 && !sim.cancel_bullet(bullet, bullet.state != 2, false)) error_ = sim.error();
        return;
    }
    if (result == 2 && !bullet.grazed) {
        bullet.grazed = true;
        if (progress_) {
            if (progress_->graze < 99999999) progress_->graze++;
            progress_->max_point += 100;
            if (progress_->max_point > progress_->max_point_cap) progress_->max_point = progress_->max_point_cap;
        }
        if (sim.graze_effect && !sim.graze_effect(Vec3{bullet.x, bullet.y, bullet.z})) {
            error_ = sim.error();
            return;
        }
        host_->hud_sound(42, bullet.x);
    }
}

i32 StageBattle::collide_laser(const StageLaser& laser) {
    if (progress_ && trance_active(*progress_)) return 0;
    const i32 contact = laser.player_contact(player_x, player_y, player_hit_size_ * .5f, player_hit_size_ * .5f);
    if (contact == 1) {
        if (player_dead || player_phase != PlayerPhase::Active || invulnerable > 0 ||
            (sim.dialog_host && sim.dialog_host->dialog_active()))
            return 0;
        kill_player();
    } else if (contact == 2 && laser.kind == StageLaser::Kind::Line && laser.graze_timer % 3 == 0) {
        if (progress_) {
            if (progress_->graze < 99999999) ++progress_->graze;
            progress_->max_point = std::min(progress_->max_point_cap, progress_->max_point + 100);
        }
        if (sim.graze_effect && !sim.graze_effect(Vec3{player_x, player_y, 0})) {
            error_ = sim.error();
            return contact;
        }
        if (!host_->hud_sound(42, player_x)) error_ = "Laser graze sound failed";
    }
    return contact;
}

void StageBattle::collide_enemy_body(StageEnemy& enemy) {
    if (enemy.dead || (enemy.flags & (0x22u | 0x8000000u)) || enemy.body_invulnerable > 0 ||
        (progress_ && trance_active(*progress_)))
        return;
    const float p = player_hit_size_;
    const float half = p * 0.5f;
    int contact = 0;
    if ((enemy.flags & 0x8000u) == 0) {
        const float sx = enemy.hitbox[0] * 0.5f;
        const double dx = double(player_x) - double(enemy.position[0]);
        const double dy = double(player_y) - double(enemy.position[1]);
        const float distance2 = float(dx * dx + dy * dy);
        const double radius2 = double(sx) * sx;
        if (double(distance2) < double(p) * p + radius2)
            contact = 1;
        else {
            const double span = std::max(double(sx) / 2.5, 40.0) + double(p);
            if (double(distance2) < radius2 + span * span) contact = 2;
        }
    } else {
        const float angle = enemy.body_angle;
        const float half_h = enemy.hitbox[1] * 0.5f;
        const float base_x = enemy.position[0] - std::cos(angle) * half_h;
        const float base_y = enemy.position[1] - std::sin(angle) * half_h;
        const float dx = player_x - base_x, dy = player_y - base_y;
        const float ca = std::cos(-angle), sa = std::sin(-angle);
        const float longitudinal = ca * dx - sa * dy;
        const float lateral = sa * dx + ca * dy;
        const float half_w = enemy.hitbox[0] * 0.5f;
        const float length = enemy.hitbox[1];
        const float expanded = half * 16.0f;
        if (longitudinal - expanded <= length && longitudinal + expanded >= 0.0f && lateral + expanded >= -half_w &&
            lateral - expanded <= half_w) {
            if (longitudinal - half <= length && longitudinal + half >= 0.0f && lateral + half >= -half_w &&
                lateral - half <= half_w)
                contact = 1;
            else
                contact = 2;
        }
    }
    if (contact == 1) {
        if (!player_dead && player_phase == PlayerPhase::Active && invulnerable <= 0 &&
            !(sim.dialog_host && sim.dialog_host->dialog_active()))
            kill_player();
    } else if (contact == 2 && (enemy.flags & 0x200u) && i32(enemy.age) % 6 == 0) {
        if (progress_) {
            if (progress_->graze < 99999999) ++progress_->graze;
            progress_->max_point = std::min(progress_->max_point_cap, progress_->max_point + 100);
        }
        if (sim.graze_effect && !sim.graze_effect(Vec3{player_x, player_y, 0})) {
            error_ = sim.error();
            return;
        }
        if (!host_->hud_sound(42, player_x)) error_ = "Enemy body graze sound failed";
    }
}

bool StageBattle::kill_player() {
    if (player_dead || player_phase == PlayerPhase::Dying || player_phase == PlayerPhase::Deathbomb) return true;
    player_phase = PlayerPhase::Deathbomb;
    if (sim.player_effect && !sim.player_effect(25, {player_x, player_y, 0}, -1)) {
        error_ = "Player miss effect failed";
        return false;
    }
    sim.spell.player_hit(bomb_timer > 0);
    player_phase_frame = 0;
    player_phase_clock_.set(0);
    invulnerable = 6;
    if (!host_->hud_sound(2)) {
        error_ = "Player hit sound failed";
        return false;
    }
    return true;
}

bool StageBattle::commit_death() {
    sim.spell.player_hit(bomb_timer > 0);
    if (!progress_ || (progress_->run_mode_flags & 0x30u) == 0x20u ||
        (sim.dialog_host && sim.dialog_host->dialog_active())) {
        return commit_death_normal();
    }
    if (!trance_active(*progress_)) {
        if (progress_->ending_points < 200) return commit_death_normal();
        trance_protection_ = true;
        trance_begin(*progress_);
        item_manager.set_spirit_trance(true);
        if (!(progress_->stage == 6 && progress_->chapter >= 6) && !host_->trance_music(true)) {
            error_ = "Protective Trance music failed";
            return false;
        }
        trance_task_frame_ = 0;
    }
    player_phase = PlayerPhase::Active;
    return true;
}

bool StageBattle::commit_death_normal() {
    player_->deaths++;
    --player_->extra_lives;
    player_phase = PlayerPhase::Dying;
    player_phase_frame = 0;
    player_phase_clock_.set(0);
    invulnerable = 180;
    if (player_->extra_lives >= 0 && !host_->life_hud(player_->extra_lives, player_->life_pieces)) {
        error_ = "Player death HUD failed";
        return false;
    }
    return true;
}

bool StageBattle::update_player(const GameInput& input) {
    if (player_dead) return true;
    const bool bomb_slow = bomb_timer > 0;
    const float startup_rate = sim.time_rate();
    player_state_.startup_clock.tick(&startup_rate);
    if (player_phase_clock_.current != player_phase_frame) player_phase_clock_.set(player_phase_frame);
    const bool deathbomb_expired = player_phase == PlayerPhase::Deathbomb && player_phase_frame >= 8;
    if (deathbomb_expired && !commit_death()) return false;
    if (player_phase == PlayerPhase::Dying || deathbomb_expired) {
        if (player_phase_frame == 3) {
            const i32 step = player_->power_step;
            player_->power = std::max(player_->power - step, step);
            death_option_rebuild = true;
            const float base = float(std::atan2(-224.0, double(-player_x)));
            for (i32 i = 0; i < 7; ++i) {
                const float angle = float(double(i) * double(3.1415927f) / 28.0 + base - double(.3926991f));
                item_manager.spawn_item(player_x, player_y, 0, 1, angle, 3.f);
            }
        }
        item_manager.spawn_value_item(3, player_x, player_y, 0, 2.f, false);
        if (player_phase_frame >= 30) {
            if (player_->extra_lives < 0) {
                player_dead = true;
                if (!host_->request_player_game_over()) {
                    error_ = "Player game-over request failed";
                    return false;
                }
            } else {
                if (player_->bombs < 2) {
                    player_->bombs = 2;
                    if (!host_->bomb_hud(player_->bombs, player_->bomb_pieces)) {
                        error_ = "Player respawn bomb HUD failed";
                        return false;
                    }
                }
                progress_->ending_points = 0;
                if (!host_->trance_gauge_refresh()) {
                    error_ = "Trance gauge refresh failed";
                    return false;
                }
                sim.set_time_rate(1.f);
                respawn_damage.center = {player_x, player_y, 0};
                respawn_origin = {player_x, player_y, 0};
                respawn_damage.radius = 32.f;
                respawn_damage.remaining.set(30);
                respawn_damage.active = true;
                respawn_damage.registration_slot = register_circle(respawn_damage.center, 32, 16, 30, 150);
                if (respawn_damage.registration_slot < 0) return false;
                player_phase = PlayerPhase::Entering;
                player_phase_frame = 0;
                player_phase_clock_.set(0);
                invulnerable = 280;
                pos_fx = 0;
                pos_fy = 0xf000;
                player_x = 0;
                player_y = 480;
            }
        }
    } else if (player_phase == PlayerPhase::Entering) {
        pos_fy = 0xf000 - player_phase_frame * 0x2800 / 60;
        player_y = float(pos_fy) * 0.0078125f;
        option_reset_pending_ = true;
        for (auto& position : movement_history_)
            position = {pos_fx, pos_fy};
        if (player_phase_frame < 30) {
            const float radius = float(double(player_phase_frame) * 512.0 / 30.0 + 64.0);
            sim.cancel_lasers_radius(respawn_origin, radius, true);
            sim.cancel_lasers_radius(respawn_origin, float(radius * .25f), false);
        } else {
            if (!sim.cancel_respawn_bullets({player_x, player_y, 0}, 640.f)) return false;
            sim.cancel_lasers(false);
        }
        if (!sim.error().empty()) return false;
        if (player_phase_frame >= 60) {
            player_phase = PlayerPhase::Active;
            player_phase_frame = 0;
            player_phase_clock_.set(0);
            if (input.pressed & 0x02) on_bomb_key();
            move_player(input.held, bomb_slow);
        }
    } else {
        const bool move_this_frame = player_phase == PlayerPhase::Active;
        if (input.pressed & 0x02) on_bomb_key();
        if (move_this_frame && player_phase == PlayerPhase::Active) move_player(input.held, bomb_slow);
    }
    const float rate = sim.time_rate();
    for (auto& registration : damage_registrations)
        if (registration.active) {
            if (registration.owner == DamageRegistration::Owner::Area && (registration.flags & 2u)) {
                float vx = 0, vy = 0;
                polar_xy(registration.travel_angle, float(registration.travel_speed * rate), vx, vy);
                registration.center_x = round001(float(registration.center_x + vx));
                registration.center_y = round001(float(registration.center_y + vy));
            }
            registration.radius = float(registration.radius + registration.radius_growth);
            registration.clock.decrement(&rate);
            if (registration.clock.current < 1) {
                registration.active = false;
                registration.flags &= ~1u;
            }
        }
    for (auto& shot : shots)
        if (shot.alive && shot.registration_slot < 0) shot.damage_clock.decrement(&rate);
    for (auto& orb : bomb_orbs) {
        if (!orb.exploding) continue;
        orb.explosion_damage_clock.decrement(&rate);
        if (orb.explosion_damage_clock.current < 1) orb.exploding = false;
    }
    if (respawn_damage.active) {
        const i32 slot = respawn_damage.registration_slot;
        if (slot < 0 || slot >= i32(damage_registrations.size())) {
            respawn_damage.active = false;
        } else {
            const auto& registration = damage_registrations[size_t(slot)];
            respawn_damage.center = {registration.center_x, registration.center_y, registration.center_z};
            respawn_damage.radius = registration.radius;
            respawn_damage.remaining = registration.clock;
            respawn_damage.active = registration.active;
        }
    }
    if (invulnerable > 0) --invulnerable;
    if (player_phase_clock_.current != player_phase_frame) player_phase_clock_.set(player_phase_frame);
    player_state_.phase_frame_previous = player_phase_frame;
    player_phase_clock_.tick(&rate);
    player_phase_frame = player_phase_clock_.current;
    return error_.empty();
}

void StageBattle::reward_kill(StageEnemy& enemy) { (void)enemy; }

ItemCollector StageBattle::make_item_collector() {
    static constexpr float kCollectBase[4] = {60.0f, 70.0f, 60.0f, 60.0f};
    static constexpr float kAutoBase[4] = {100.0f, 118.0f, 100.0f, 100.0f};
    static constexpr float kHomingSpeed[4] = {5.0f, 7.0f, 5.0f, 6.0f};
    const i32 c = character_ & 3;
    ItemCollector collector;
    collector.x = player_x;
    collector.y = player_y;
    collector.state = player_dead ? 2
                                  : (player_phase == PlayerPhase::Active      ? 1
                                     : player_phase == PlayerPhase::Dying     ? 2
                                     : player_phase == PlayerPhase::Deathbomb ? 4
                                                                              : 1);
    collector.focus = (input_state & 0x08u) != 0;
    if (const auto* boss = sim.boss()) {
        collector.boss_present = true;
        collector.boss_x = boss->position[0];
        collector.boss_y = boss->position[1];
    }
    collector.dialog = sim.dialog_host && sim.dialog_host->dialog_active();
    collector.homing_speed = kHomingSpeed[c];
    const float half = kCollectBase[c] * 0.25f;
    collector.box = {player_x - half, player_y - half, player_x + half, player_y + half};
    const float auto_half = kCollectBase[c] * 0.5f;
    collector.box_auto = {player_x - auto_half, player_y - auto_half, player_x + auto_half, player_y + auto_half};
    const float focus_half = kAutoBase[c] * 0.5f;
    collector.box_auto_focus = {player_x - focus_half, player_y - focus_half, player_x + focus_half,
                                player_y + focus_half};
    collector.progress = progress_;
    collector.player = player_;
    collector.score = score_;
    return collector;
}

void StageBattle::emit_item_events() {
    for (const auto& event : item_manager.events()) {
        i32 sound = -1;
        switch (event.kind) {
        case ItemEvent::Kind::Collected:
            sound = event.value >= 10 ? 0x3f : 0x25;
            break;
        case ItemEvent::Kind::PowerLevelUp:
            sound = 0x0d;
            break;
        case ItemEvent::Kind::Extend:
            sound = 0x11;
            break;
        case ItemEvent::Kind::SpiritGained:
            sound = 0x44;
            break;
        case ItemEvent::Kind::BombGained:
            sound = 0x2e;
            break;
        case ItemEvent::Kind::TranceGained:
            if (!host_->trance_gauge_refresh()) {
                error_ = "Trance gauge refresh failed";
                item_manager.clear_events();
                return;
            }
            break;
        case ItemEvent::Kind::ScorePopup:
            if (!host_->score_popup(event.value, event.x, event.y, event.color)) {
                error_ = "Score popup failed";
                item_manager.clear_events();
                return;
            }
            break;
        case ItemEvent::Kind::FullPower:
            break;
        }
        const bool positional = event.kind == ItemEvent::Kind::Collected || event.kind == ItemEvent::Kind::PowerLevelUp;
        if (sound >= 0) {
            const bool played = positional ? host_->hud_sound(sound, event.x) : host_->hud_sound(sound);
            if (!played && error_.empty()) error_ = "Item sound service failed";
        }
        switch (event.kind) {
        case ItemEvent::Kind::FullPower:
            if (!host_->hud_notice(61)) error_ = "Full power notice failed";
            break;
        case ItemEvent::Kind::Extend:
            if (!host_->hud_notice(63)) error_ = "Extend notice failed";
            break;
        case ItemEvent::Kind::SpiritGained:
            if (!host_->hud_spirit_notice(event.value, event.x, event.y, 0.f)) error_ = "Spirit notice failed";
            break;
        default:
            break;
        }
    }
    item_manager.clear_events();
}

void StageBattle::update_items_phase() {
    ItemCollector collector = make_item_collector();
    if (progress_ && score_) item_trace_note("side-begin", progress_->max_point, score_->score);
    item_manager.update(collector);
    if (progress_ && score_) item_trace_note("side-items", progress_->max_point, score_->score);
    emit_item_events();
}

void StageBattle::update_spirits_phase() {
    ItemCollector collector = make_item_collector();
    item_manager.update_spirits(collector);
    if (progress_ && score_) item_trace_note("side-spirits", progress_->max_point, score_->score);
    emit_item_events();
}

void StageBattle::update_item_side() {
    ItemCollector collector = make_item_collector();
    if (progress_ && score_) item_trace_note("side-begin", progress_->max_point, score_->score);
    item_manager.update(collector);
    if (progress_ && score_) item_trace_note("side-items", progress_->max_point, score_->score);
    item_manager.update_spirits(collector);
    if (progress_ && score_) item_trace_note("side-spirits", progress_->max_point, score_->score);
    emit_item_events();
}

bool StageBattle::end_trance() {
    if (!progress_ || !trance_active(*progress_)) return true;
    const bool protection = trance_protection_;
    trance_end(*progress_, protection);
    item_manager.set_spirit_trance(false);
    if (!protection && !host_->trance_gauge_refresh()) {
        error_ = "Trance gauge refresh failed";
        return false;
    }
    trance_protection_ = false;
    if (protection) {
        if ((progress_->scene_flags & 0x200u) == 0 && sim.player_effect &&
            !sim.player_effect(26, {player_x, player_y, 0}, -1)) {
            error_ = "Trance release effect failed";
            return false;
        }
        if (!host_->hud_sound(66, player_x)) {
            error_ = "Player death sound failed";
            return false;
        }
        if (!commit_death_normal()) return false;
    }
    invulnerable = 70;
    trance_task_frame_ = 0;
    if (!(progress_->stage == 6 && progress_->chapter >= 6) && !host_->trance_music(false)) {
        error_ = "Trance music restore failed";
        return false;
    }
    return true;
}

bool StageBattle::update_trance(const GameInput& input) {
    if (!progress_) return true;
    if (trance_active(*progress_)) {
        ++trance_task_frame_;
        if (!host_->trance_gauge_refresh()) {
            error_ = "Trance gauge refresh failed";
            return false;
        }
        if (progress_->ending_points > 0) {
            --progress_->ending_points;
            if (progress_->ending_points < 0) progress_->ending_points = 0;
        }
        if (progress_->ending_points < 1) {
            if (!end_trance()) return false;
        }
        return true;
    }
    ++trance_task_frame_;
    if (!trance_ready(*progress_)) return true;
    if ((input.pressed & 0x800) == 0) return true;
    if (trance_task_frame_ < kTranceActivateFrame) return true;
    if (bomb_timer > 0) return true;
    if (sim.dialog_host && sim.dialog_host->dialog_active()) return true;
    if ((progress_->run_mode_flags & 0x30) == 0x20) return true;
    if (!host_->hud_sound(0x34, player_x)) {
        error_ = "Trance activation sound failed";
        return false;
    }
    trance_begin(*progress_);
    item_manager.set_spirit_trance(true);
    if (!(progress_->stage == 6 && progress_->chapter >= 6) && !host_->trance_music(true)) {
        error_ = "Trance music start failed";
        return false;
    }
    trance_task_frame_ = 0;
    return true;
}

bool StageBattle::open_event_sink(const char* path) {
    if (sink_open_) {
        events_ = true;
        sim.replay_events = true;
        sim.sink.set_frame(sim.frame);
        return true;
    }
    if (!path || !sim.sink.open(path)) {
        error_ = "open_event_sink failed";
        return false;
    }
    sink_open_ = true;
    events_ = true;
    sim.replay_events = true;
    sim.sink.set_frame(sim.frame);
    return true;
}

void StageBattle::on_bomb_key() {
    if (!player_ || !progress_ || trance_active(*progress_)) return;
    if (sim.dialog_host && sim.dialog_host->dialog_active()) return;
    if (player_->bombs <= 0 || bomb_timer > 0 || player_dead ||
        (player_phase != PlayerPhase::Active && player_phase != PlayerPhase::Deathbomb))
        return;
    if (player_phase == PlayerPhase::Deathbomb && player_phase_frame >= 8) return;
    player_->bombs--;
    if (!host_->bomb_hud(player_->bombs, player_->bomb_pieces)) error_ = "bomb hud";
    static constexpr i32 durations[4] = {200, 301, 261, 261};
    bomb_duration_ = durations[character_];
    bomb_timer = bomb_duration_;
    sim.bomb_active = true;
    sim.spell.bomb_started();
    bomb_age = -1;
    bomb_age_accumulator_ = 0;
    ++bomb_generation;
    if (host_ && !host_->hud_sound(44, player_x)) error_ = "Bomb start slash sound";
    if (!host_->hud_sound(0x31)) error_ = "Bomb start sound";
    if (bomb_screen_shake) {
        bool ok = true;
        if (character_ == 1)
            ok = bomb_screen_shake(3, 60, 240, 30);
        else if (character_ == 2)
            ok = bomb_screen_shake(3, 200, 60, 30);
        else if (character_ == 3)
            ok = bomb_screen_shake(1, 6, 230, 30);
        if (!ok) error_ = "Bomb screen shake registration failed";
    }
    bomb_orbs = {};
    if (character_ == 3) youmu_bomb_.reset();
    youmu_slash_spawns_.clear();
    bomb_angle_ = -1.5707964f;
    bomb_visual_angle_ = bomb_angle_;
    bomb_rotation_speed_ = 0;
    bomb_center_x_ = player_x;
    bomb_center_y_ = player_y;
    invulnerable = character_ == 0 ? 40 : 120;
    if (character_ == 0) {
        float angle = 0;
        for (auto& orb : bomb_orbs) {
            orb.active = true;
            orb.x = player_x;
            orb.y = player_y;
            orb.angle = angle;
            angle = combat_angle(angle + 0.7853982f);
        }
    }
    if (player_phase == PlayerPhase::Deathbomb) {
        player_phase = PlayerPhase::Active;
        player_phase_frame = 60;
        player_phase_clock_.set(60);
    }
}

bool StageBattle::continue_player() {
    if (!progress_ || !player_) return false;
    player_dead = false;
    option_reset_pending_ = true;
    update_options(true);
    return error_.empty();
}

bool StageBattle::step_transition(const GameInput& input) {
    input_state = input.held;
    option_movement_tick_ = false;
    if (!update_player(input)) return false;
    sim.player_x = player_x;
    sim.player_y = player_y;
    update_options();
    update_firing((input.held & 1u) != 0);
    step_shots();
    return error_.empty();
}

bool StageBattle::step(const GameInput& input) {
    if (!error_.empty()) return false;
    rng_probe_on = damage_probe_enabled;
    ++sim.frame;
    set_item_trace_frame(sim.frame);
    if (progress_) ++progress_->chapter_frame;
    if (progress_ && score_) item_trace_note("step-begin", progress_->max_point, score_->score);
    frame_damage_total = 0.0f;
    if (events_) sim.sink.set_frame(sim.frame);
    input_state = input.held;
    if (!update_trance(input)) return false;
    option_movement_tick_ = false;
    if (!update_player(input)) return false;
    if (events_) sim.sink.player(player_x, player_y, input_state);
    sim.player_x = player_x;
    sim.player_y = player_y;
    update_options();
    update_firing((input.held & 0x01) != 0);
    step_shots();
    step_bomb();
    sim.bomb_active = bomb_timer > 0;
    if (progress_ && score_) item_trace_note("after-shots", progress_->max_point, score_->score);
    if (!sim.step_frame()) {
        error_ = sim.error();
        return false;
    }
    if (progress_ && score_) item_trace_note("after-sim", progress_->max_point, score_->score);
    if (!defer_item_side) update_item_side();
    if (damage_probe_enabled && sim.frame >= 9800 && sim.frame <= 9860) {
        std::printf("RNGP f=%d idx=%u", sim.frame, sim.random.index());
        for (int i = 0; i < 20; ++i) {
            std::printf(" t%d=%u", i, GameRandom::probe[i]);
            GameRandom::probe[i] = 0;
        }
        std::printf("\n");
    }
    static const bool bomb_probe = std::getenv("TH13_BOMB_PROBE") != nullptr;
    if (bomb_probe) {
        std::printf("BOMB f=%d age=%d timer=%d stock=%d invuln=%d rng=%u", sim.frame, bomb_age, bomb_timer,
                    player_ ? player_->bombs : -1, invulnerable, sim.random.index());
        for (int i = 0; i < 8; ++i)
            if (bomb_orbs[i].active || bomb_orbs[i].exploding)
                std::printf(" orb%d=%c(%.2f,%.2f)", i, bomb_orbs[i].active ? 'A' : 'E', bomb_orbs[i].x, bomb_orbs[i].y);
        for (const auto& enemy : sim.enemies)
            if (!enemy->dead && enemy->life > 0) std::printf(" e%u=%g", enemy->serial, enemy->life);
        u32 alive_bullets = 0;
        for (const auto& bullet : sim.bullets)
            if (bullet.alive) ++alive_bullets;
        std::printf(" bullets=%u\n", alive_bullets);
    }
    if (life_probe_) {
        std::printf("SCORE f=%d score=%d power=%d\n", sim.frame, score_ ? score_->score : -1,
                    player_ ? player_->power : -1);
        for (const auto& shot : shots)
            if (shot.alive)
                std::printf("SHOT f=%d x=%.9g y=%.9g dmg=%.9g\n", sim.frame, double(shot.x), double(shot.y),
                            double(shot.damage));
        for (const auto& enemy : sim.enemies)
            std::printf("LIFE f=%d e=%d life=%d pos=(%.9g,%.9g) hb=(%.9g,%.9g) "
                        "flags=%08x\n",
                        sim.frame, int(enemy->serial), int(enemy->life), double(enemy->position[0]),
                        double(enemy->position[1]), double(enemy->hurtbox[0]), double(enemy->hurtbox[1]), enemy->flags);
    }
    return error_.empty();
}

}
