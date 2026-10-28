// StageBattle 实现。ECL 步进与敌机/弹幕数据全部来自 StageSim；自机侧按
// event-contract.md §7.2 定点模型（0x442960 / pl02a.sht / 0x446870 同式）。
#include "StageBattle.hpp"
#include "StageMath.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace th13 {

namespace {

u16 read_u16(const u8* p) {
    return u16(p[0]) | u16(p[1]) << 8;
}
u32 read_u32(const u8* p) {
    return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24;
}

// 0x4abc30 方向表（单位向量，斜向不归一）：0 无 1↑ 2↓ 3← 4→ 5↖ 6↗ 7↙ 8↘。
const i32 kDirX[9] = {0, 0, 0, -1, 1, -1, 1, -1, 1};
const i32 kDirY[9] = {0, -1, 1, 0, 0, -1, -1, 1, 1};

// 0x442960:55-78 的方向判定序（复合方向优先，单一方向按位）。
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

// 0x441eae：speed128 = int(sht 浮点 × 128.0)（截断）。
i32 speed128_from_float(float value) {
    return i32(value * 128.0f);
}

float combat_angle(float angle) {
    constexpr float pi = 3.1415927410125732f;
    while (angle > pi) angle -= 2.0f * pi;
    while (angle < -pi) angle += 2.0f * pi;
    return angle;
}

// 00421c40：距离内最近的可锁定敌机；并非第一个敌机或只追Boss。
StageEnemy* nearest_target(StageSim& sim, float x, float y, float range) {
    StageEnemy* result = nullptr;
    float best = range * range;
    for (auto& enemy : sim.enemies) {
        // 00421c40 checks list membership and target flags, not remaining HP.
        // A defeated boss can remain linked and targetable during its exit.
        if (enemy->dead || (enemy->flags & 0x18000021u)) continue;
        const float dx = enemy->position[0] - x, dy = enemy->position[1] - y;
        const float distance = dx * dx + dy * dy;
        if (distance < best) { result = enemy.get(); best = distance; }
    }
    return result;
}

// 00446870：敌机矩形投影进伤害矩形局部坐标；SHT尺寸是全尺寸。
// ex/ey/ehx/ehy = 敌机位置与受击全尺寸（显式传入：collide_shots 用本帧
// 步进前快照，见 enemy_scan_state_）。
bool rotated_hit(float x, float y, float width, float height, float angle,
                 float ex, float ey, float ehx, float ehy) {
    const float c = std::cos(angle), s = std::sin(angle);
    const float dx = ex - x, dy = ey - y;
    const float local_x = dx * c + dy * s, local_y = dy * c - dx * s;
    const float hx = ehx * 0.5f, hy = ehy * 0.5f;
    return std::abs(local_x) <= width * 0.5f + std::abs(c) * hx + std::abs(s) * hy &&
           std::abs(local_y) <= height * 0.5f + std::abs(s) * hx + std::abs(c) * hy;
}

bool rotated_hit(float x, float y, float width, float height, float angle,
                 const StageEnemy& enemy) {
    return rotated_hit(x, y, width, height, angle, enemy.position[0], enemy.position[1],
                       enemy.hurtbox[0], enemy.hurtbox[1]);
}

// 0x4469c8: transform the enemy center and full size into the shot's local frame.
// 0x4469ff passes the enemy-size argument, not the damage registration's size.
bool shot_hits_enemy_box(float sx, float sy, float sw, float sh,
                         float ex, float ey, float ew, float eh, float theta) {
    if (theta == 0.0f) {
        return ex - ew * 0.5f <= sx + sw * 0.5f && sx - sw * 0.5f <= ex + ew * 0.5f &&
               ey - eh * 0.5f <= sy + sh * 0.5f && sy - sh * 0.5f <= ey + eh * 0.5f;
    }
    const float a = -theta;
    const float sf = float(std::sin(double(a))), cf = float(std::cos(double(a)));
    const float dx = ex - sx, dy = ey - sy;
    const float ox = float(double(dx) * cf - double(dy) * sf);
    const float oy = float(double(dx) * sf + double(dy) * cf);
    const float vx = std::fabs(float(double(ew) * cf - double(eh) * sf));
    const float vy = std::fabs(float(double(ew) * sf + double(eh) * cf));
    return -sw * 0.5f <= ox + double(vx) * 0.5 && ox - double(vx) * 0.5 <= sw * 0.5f &&
           -sh * 0.5f <= oy + double(vy) * 0.5 && oy - double(vy) * 0.5 <= sh * 0.5f;
}

// 0x443e40 的弹型域解析：+2 u16 块数、+0xc8 u32 指针表（+0xf4 基准的
// 相对偏移），每块为 0x38 步长的记录数组、首字节 i8 <0 终止。记录体
// （0x446000/0x445c40 直读）：+0 u8 间隔、+1 u8 相位、+2 i16 伤害、
// +4/+8 f32 出膛偏移、+0x14 f32 角度、+0x18 f32 弹速、+0x20 i8 锚位。
// 00445e0c–00445e47 从 +0x22 读有符号动画编号并加 10 后绑定 ANM。
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
            // 终止记录只占首字节（末块常紧贴文件尾，实测四张 .sht 的末块
            // 终止符落在 size-4 处）；完整记录体仍需 0x38 字节在界内。
            if (at >= size) return false;
            if (i8(sht[at]) < 0) break;
            if (at + 0x38 > size) return false;
            ShtShotRecord entry;
            entry.interval = i32(sht[at]);
            entry.phase = i32(sht[at + 1]);
            entry.damage = float(i16(read_u16(sht + at + 2)));
            entry.script = i32(i16(read_u16(sht + at + 0x22))) + 10;
            entry.anchor = i32(static_cast<i8>(sht[at + 0x20]));
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

}  // namespace

bool StageBattle::load(EclResourceProvider& provider, const char* stage_file, i32 difficulty_rank,
                       StageBattleServices& host, SessionState& progress, HudPlayerState& player,
                       HudScoreState& score, const char* event_path, const char* sht_name) {
    host_ = &host;
    progress_ = &progress;
    sim.session = &progress;
    sim.mode_flags = progress.run_mode_flags;
    player_ = &player;
    score_ = &score;
    sim.spell.reset();
    sim.bomb_active = false;
    sim.spell_award = [this](i32 bonus) {
        score_->score = std::min(999999999, score_->score + bonus);
    };
    character_ = progress.character & 3;
    if (!player_state_.initialized) {
        // 00441dd0 is player construction, not each stage's activation.
        player_state_ = StagePlayerState{};
        invulnerable = 120;
        player_state_.initialized = true;
    }
    activate_stage_player();
    // StageSim's ECL constructor may query the player before the first
    // gameplay tick (the 0x448ec0 restore is later in the replay path).
    // Keep that read side on the persistent player object immediately.
    sim.player_x = player_x;
    sim.player_y = player_y;
    bomb_timer = 0;
    bomb_age = -1;
    bomb_generation = 0;
    bomb_rays = {};
    bomb_duration_ = 0;
    bomb_orbs = {};
    // Player shots belong to the persistent 441dd0 object, not the stage.
    // New StagePlayerState construction already clears these pools.
    sht_shots = {};
    sht_shots_loaded = false;
    // 0x441dd0:65-66：玩家判定尺寸 = 表[be7b8]（.data 0x4c0ba8，实读
    // 2/3/3/3）。常规开局 be7bc=0（回放头 +0x60 另写），本作选择/回放
    // 头只传 character，故下标 = character & 3。
    // 自机判定尺寸 = 0x4c0ba8[be7b8]（2/3/3/3）；道具收集盒全尺寸 =
    // 0x4c0bc0[be7b8]（60/60/100/118，0x441dd0 写 +0x62c/+0x630，半宽 ×0.5）。
    player_hit_size_ = kPlayerHitSize[progress.character & 3];
    // 道具/神霊管理器接线（R/E）：game RNG 共享实例 + StageItemHost 注入。
    item_manager.bind_random(sim.random);
    sim.items = &item_manager;
    sim.enemy_absorb_spirits=[this](StageEnemy& enemy) {
        // 422b20: every live spirit is eligible for absorption; only
        // state2/kind3 retain their existing movement state.
        for(auto& spirit:item_manager.spirits()) {
            if(!spirit.alive)continue;
            if(spirit.state!=2&&spirit.kind!=3)spirit.state=5;
            const float dx=spirit.x-enemy.position[0],dy=spirit.y-enemy.position[1];
            if(float(double(dx)*dx+double(dy)*dy)>=400)continue;
            if(!host_->hud_sound(63)||!host_->hud_sound(67)) {
                sim.error_="Spirit absorption sound failed";return false;
            }
            if(enemy.spell_active) {
                enemy.spell_life+=enemy.i_slots[2];
                enemy.life=float((enemy.spell_life-enemy.phase_life_threshold*7)/7+enemy.phase_life_threshold);
            } else enemy.life+=float(enemy.i_slots[2]);
            ++enemy.i_slots[3];spirit.alive=false;spirit.state=0;
        }
        return true;
    };
    // r4/items：会话绑定（trance 激活期产神霊 bit0=1 的收集双路径门）。
    item_manager.bind_session(progress);
    // 敌机销毁回收默认接线（0x418e00 销毁支路的 ANM 面）：sim 摘链时经
    // retire_sink 通知宿主回收槽位 VM（0x470110 语义）。此前仅 RunScene
    // 手工接线，harness draws-replay 缺线导致被击杀敌机的本体 VM 滞留
    // 层桶继续绘制——对局回放窗 f124 起 op[16] count 6≠4、此后每 30 帧
    // +2 无界增长的实测根因（oracle 侧同帧敌机销毁、VM 同帧出捕获面）。
    // host.enemy_retire 默认空实现（无头事件窗零影响）；RunScene 在 load
    // 之后覆盖 retire_sink，行为不变。
    sim.retire_sink = [this](u32 serial) { host_->enemy_retire(serial); };
    sim.enemy_hit = [this](StageEnemy& enemy) {
        collide_shots(enemy);
        if (!error_.empty()) sim.error_ = error_;
    };
    sim.enemy_body_collision = [this](StageEnemy& enemy) {
        collide_enemy_body(enemy);
        if (!error_.empty()) sim.error_ = error_;
    };
    sim.laser_collision = [this](const StageLaser& laser) {
        const i32 result = collide_laser(laser);
        if (!error_.empty()) sim.error_ = error_;
        return result;
    };
    if (sht_name) {
        // §7.2：移速出自 .sht（+0x10..+0x1c 四浮点），文件缺失即失败（不静默）。
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
        // 00444df0 原版两表：高速 +0x28，低速 +0x78，三角档索引0/1/3/6。
        for (int mode = 0; mode < 2; ++mode)
            for (int i = 0; i < 10; ++i)
                for (int axis = 0; axis < 2; ++axis) {
                    float value;
                    std::memcpy(&value, sht.data() + 0x28 + mode * 0x50 + i * 8 + axis * 4, 4);
                    option_offsets_[mode][i][axis] = speed128_from_float(value);
                }
        // 弹型域（0x443e40）：布局异常按装载失败处理（不静默降级）。
        if (!parse_sht_shots(sht.data(), sht.size(), *this)) {
            error_ = std::string("invalid shot table in ") + sht_name;
            return false;
        }
    }
    if (!sim.begin(provider, stage_file, difficulty_rank, event_path)) {
        error_ = sim.error();
        return false;
    }
    // 观察幂等（r4/harness frame0）：sink 已在 load 之前打开（open_event_sink）
    // 时不清 events_——生产允许首帧前启 sink，load 不得截断已开流。机制零
    // 改动：events_ 只门控 sink 写点；load(event_path) 自开路径不受影响。
    if (!sink_open_) events_ = event_path != nullptr;
    life_probe_ = std::getenv("TH13_LIFE_PROBE") != nullptr;
    if (!sim.start()) {
        error_ = sim.error();
        return false;
    }
    update_options(true);
    if (events_) sim.sink.set_frame(0);
    return true;
}

void StageBattle::activate_stage_player() {
    // 004424d0 resets the active phase, phase/fire timers and movement
    // history, but does not write player+14688 (remaining protection),
    // position or focus. Replay's448ec0 restores the latter separately.
    player_dead = false;
    player_phase = PlayerPhase::Active;
    player_phase_frame = 0;
    player_phase_clock_.set(0);
    player_state_.startup_clock.set(0);
    fire_clock_.set(-1);
    player_state_.youmu_charge_clock.set(0);
    option_power_ = -1;
    option_reset_pending_ = false;
    option_movement_tick_ = false;
    for (auto& position : movement_history_) position = {pos_fx, pos_fy};
}

void StageBattle::move_player(u32 held) {
    option_movement_tick_ = true;
    focus = (held & 0x08) != 0;
    const int dir = dir_index(held);
    i32 speed = dir == 0 ? 0
                                  : focus ? (dir < 5 ? speed_orth_focus : speed_diag_focus)
                                          : (dir < 5 ? speed_orth : speed_diag);
    if (character_ == 1 && bomb_timer > 0)
        speed = i32(float(speed) * 0.2f); // 0040a860 player+0xa2b4
    // 0x442960:223-236：定点累加 + 钳制（±0x5c00 / [0x1000,0xd800]）。
    const float rate = sim.time_rate(); //442d3b stores f32 velocity, then truncates.
    if (touch.mode && !(held & 0xf0u) && std::isfinite(touch.x) && std::isfinite(touch.y)) {
        double x = double(touch.x) * 128 - pos_fx;
        double y = double(touch.y) * 128 - pos_fy;
        if (touch.mode != 2) {
            double limit = double(focus ? speed_orth_focus : speed_orth) * rate;
            if (character_ == 1 && bomb_timer > 0) limit *= .2;
            const double length = std::sqrt(x * x + y * y);
            if (length > limit && length > 0) { x *= limit / length; y *= limit / length; }
        }
        pos_fx += truncate_int(float(x));
        pos_fy += truncate_int(float(y));
    } else {
        pos_fx += truncate_int(float(float(speed * kDirX[dir]) * rate));
        pos_fy += truncate_int(float(float(speed * kDirY[dir]) * rate));
    }
    if (pos_fx < -0x5c00) pos_fx = -0x5c00;
    else if (pos_fx > 0x5c00) pos_fx = 0x5c00;
    if (pos_fy < 0x1000) pos_fy = 0x1000;
    else if (pos_fy > 0xd800) pos_fy = 0xd800;
    player_x = float(pos_fx) * 0.0078125f;
    player_y = float(pos_fy) * 0.0078125f;
}

void StageBattle::restore_replay_player(i32 x,i32 y,bool initial_focus) {
    pos_fx=x;pos_fy=y;
    player_x=float(x)*0.0078125f;player_y=float(y)*0.0078125f;
    focus=initial_focus;
    update_options(true); // fills all33 history positions and resets eight interpolations
    sim.player_x=player_x;sim.player_y=player_y;
}

void StageBattle::update_options(bool reset) {
    const i32 count = std::max(0, std::min(power_level(), 4));
    const bool changed = option_power_ != count;
    if (reset) for (auto& position : movement_history_) position = {pos_fx, pos_fy};
    if (option_movement_tick_ &&
        (movement_history_[0][0] != pos_fx || movement_history_[0][1] != pos_fy)) {
        for (int i = 32; i > 0; --i) movement_history_[i] = movement_history_[i - 1];
        movement_history_[0] = {pos_fx, pos_fy};
    }
    option_count = count;
    const i32 first = count * (count - 1) / 2;
    for (int i = 0; i < 8; ++i) {
        auto& option = options[i];
        option.active = i < count && player_phase != PlayerPhase::Dying && !player_dead;
        if (i >= count) continue;
        // 443240 only calls 442960 in the movement branch. Deathbomb,
        // death and entry must not advance the option positions on their own.
        if (!reset && !option_movement_tick_) continue;
        auto target = option_offsets_[focus ? 1 : 0][first + i];
        // 00445660：妖梦卫星高速取第(卫星+1)*8个移动历史，低速在自机上25px。
        if (character_ == 3) {
            target = focus ? std::array<i32, 2>{0, -3200}
                           : std::array<i32, 2>{
                                 movement_history_[(i + 1) * 8][0] - pos_fx,
                                 movement_history_[(i + 1) * 8][1] - pos_fy};
        }
        target[0] += pos_fx;
        target[1] += pos_fy;
        auto& position = option_fixed_[i];
        if (reset || changed || option_reset_pending_) {
            position = target;
        } else {
            // 00442960：整数插值（+0x14680初值30），双轴都截断为0才整体吸附。
            const i32 dx = (target[0] - position[0]) * 30 / 100;
            const i32 dy = (target[1] - position[1]) * 30 / 100;
            if (dx == 0 && dy == 0) position = target;
            else { position[0] += dx; position[1] += dy; }
        }
        option.x = float(position[0]) * 0.0078125f;
        option.y = float(position[1]) * 0.0078125f;
    }
    option_power_ = count;
    if (reset || option_movement_tick_) option_reset_pending_ = false;
}

void StageBattle::update_firing(bool held) {
    // 0x443240 only enables the firing controller after its startup timer exceeds 19.
    const bool enabled = player_state_.startup_clock.current > 19 &&
                         std::any_of(sim.enemies.begin(), sim.enemies.end(),
                             [](const auto& enemy) { return !enemy->dead; }) &&
                         !(progress_ && (progress_->scene_flags & 0x4000u)) &&
                         !(sim.dialog_host && sim.dialog_host->dialog_active());
    if (!enabled) {
        fire_clock_.set(-1);
        player_state_.youmu_charge_clock.set(0);
        return;
    }
    // 004460d0's non-active branch clears charge feedback, but preserves
    // the firing timer at +146c8..146d8. A deathbomb resumes that same burst.
    if (player_dead || player_phase != PlayerPhase::Active) return;
    if (character_ == 3) {
        // 004460d0: focused Youmu charges instead of running the ordinary
        // 15-frame firing cycle. Release after60 starts a40-frame SHT burst
        // from the saved player position, even if focus is released meanwhile.
        auto& charge = player_state_.youmu_charge_clock;
        const float rate = sim.time_rate();
        if (charge.current < 0) {
            const float saved_x = player_x, saved_y = player_y;
            const bool saved_focus = focus;
            player_x = player_state_.youmu_charge_origin.x;
            player_y = player_state_.youmu_charge_origin.y;
            focus = true;
            fire_shots(charge.current + 40);
            player_x = saved_x; player_y = saved_y; focus = saved_focus;
            charge.tick(&rate);
            if (charge.current >= 0) charge.set(0);
            if (focus) return;
        } else if (focus && held) {
            charge.tick(&rate);
            return;
        } else {
            if (charge.current > 0) {
                const bool ready = charge.current >= 60;
                charge.set(ready ? -40 : 0);
                if (ready) player_state_.youmu_charge_origin = {player_x, player_y, 0};
            }
            if (focus) return;
        }
    }
    if (fire_clock_.current < 0) {
        if (!held) return;
        fire_clock_.set(0);
    }
    // 004460d0 每帧序：先 FUN_00446000(counter) 再计数。计数域 0..13 加按住
    // 到 14 当帧回绕（0x4732c0 -14 → 0），下一帧仍以 counter=0 出膛——计数
    // 周期 15（0..14），相位类记录（帧%间隔==相位）的节奏不被回绕打断。
    // 若先 ++ 再判射（旧序），回绕帧后 0 永远到不了 FUN_00446000，相位逐
    // 14 帧漂移（th13_01 实证：port 弹幕 80,83,88.. vs 原版均匀 79,82,85,88..，
    // 首差 state f93/event f100）。松键尾射保留至本轮 counter=14。
    if(fire_clock_.current!=fire_clock_.previous)fire_shots(fire_clock_.current);
    const float rate=sim.time_rate();
    if(fire_clock_.current<14)fire_clock_.tick(&rate);
    else if(!held)fire_clock_.set(-1);
    else {
        //4460d0 calls4732c0(-14): the rewind itself is rate-scaled.
        fire_clock_.previous=fire_clock_.current;
        const float delta=rate>.99f&&rate<1.01f?-14.f:float(rate*-14.f);
        fire_clock_.fractional=float(fire_clock_.fractional+delta);
        fire_clock_.current=truncate_int(fire_clock_.fractional);
    }
}

void StageBattle::fire_shots(i32 frame) {
    const auto spawn = [&](float anchor_x, float anchor_y, float offset_x, float offset_y,
                           float damage, float angle, i32 script, float speed, i32 behavior) {
        PlayerShot shot;
        shot.damage = damage;
        shot.angle = angle;
        shot.script = script;
        shot.speed = speed;
        shot.behavior = behavior;
        shot.generation = ++shot_generation_;
        // 00445de6–00445e09: every shot starts at (offset - initial velocity)
        // + anchor. Keep the anchor separate until this store, as in the x87 code.
        // 004464d0 then performs callback → polar → move in the birth frame.
        float vx, vy;
        polar_xy(angle, float(speed * sim.time_rate()), vx, vy);
        shot.x = float((double(offset_x) - double(vx)) + double(anchor_x));
        shot.y = float((double(offset_y) - double(vy)) + double(anchor_y));
        shot.alive = true;
        shots.push_back(shot);
    };
    if (sht_shots_loaded) {
        // 原版语义（replay-twin-divergence.md §3）：
        // 块 = 功率/步长（0x446000:13-16，power_step=100 出 sht+0x24；回放
        // 口径功率恒初值 100 → 块 1/6，拾取演化分期），聚焦 +5（sht+0x20=4）。
        i32 level = player_ ? player_->power / player_->power_step : 0;
        if (level < 0) level = 0;
        if (level > 4) level = 4;
        const u32 block = progress_ && trance_active(*progress_) ? 10u
                          : u32(level) + (focus ? 5u : 0u);
        // 临时探针（TH13_SHOT_PROBE=1）：P0 弹迹对拍（与 tools/shot_probe.py 同窗）。
        const char* probe = std::getenv("TH13_SHOT_PROBE");
        const bool shot_probe = probe && probe[0] == '1';
        for (const auto& record : sht_shots[block]) {
            // 0x446000:24：帧 % 间隔 == 相位才出膛（无冷却计数）。
            if (record.interval <= 0) continue;
            if (frame % record.interval != record.phase) continue;
            float x = player_x, y = player_y;
            if (record.anchor != 0) {
                const i32 index = record.anchor - 1;
                if (index < 0 || index >= option_count || !options[index].active) continue;
                x = options[index].x;
                y = options[index].y;
            }
            spawn(x, y, record.x, record.y, record.damage,
                  record.angle, record.script, record.speed, record.behavior);
            auto& shot = shots.back();
            shot.width = record.width;
            shot.height = record.height;
            shot.type = record.type;
            shot.hit_behavior = record.hit_behavior;
            if (record.initializer == 3) {
                // 447420 keeps the spawn's initial velocity and integrates
                // immediately, before the normal birth-frame update.
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
                polar_xy(shot.angle, float(shot.speed * sim.time_rate()),
                         motion.aux[0], motion.aux[1]);
                motion.accumulate(sim.time_rate());
                shot.x = motion.pos[0];
                shot.y = motion.pos[1];
                shot.angle = motion.angle;
            }
            if (shot_probe && sim.frame <= 1300)
                std::printf("PROBE fire ff=%d sim=%d pos=(%.2f,%.2f) dmg=%.1f\n",
                            frame, sim.frame, player_x + record.x, player_y + record.y,
                            double(record.damage));
        }
        return;
    }
    // 无 .sht 回退（交互直入未带 .sht）：三发直线近似，同相位制。
    if (frame % kFireInterval != 0) return;
    for (int i = -1; i <= 1; ++i)
        spawn(player_x, player_y, kShotOffsetX * float(i), -kShotOffsetY, kShotDamage,
              -1.5707964f, -1, kShotSpeed, 0);
}

void StageBattle::step_shots() {
    for (auto& shot : shots) {
        if (!shot.alive) continue;
        // 00445c40 的出生位已先减去一帧初速度；004464d0 随后对所有弹
        // 在年龄0同帧执行回调/位移，并由 00473780 量化到0.01。
        // SHT回调索引1 → 00446cb0（00443e40 的DAT_004bb4d4表）。
        if (shot.behavior == 1) {
            StageEnemy* target = nullptr;
            if (shot.target_serial) {
                for (auto& enemy : sim.enemies)
                    if (enemy->serial == shot.target_serial && !enemy->dead &&
                        !(enemy->flags & 0x18000021u)) // 00446cb0 retains the same flag gate.
                        target = enemy.get();
                // 00446cb0:目标失效当帧只清 +0x74 并走 speed+0.1，
                // 下一帧才重新 00421c40 锁定（oracle f100→f101 实证）。
                if (!target) shot.target_serial = 0;
            } else {
                target = nearest_target(sim, shot.x, shot.y, 256.0f);
                shot.target_serial = target ? target->serial : 0;
            }
            if (target) {
                const float wanted = float(std::atan2(double(float(target->position[1]-shot.y)),
                                                      double(float(target->position[0]-shot.x))));
                //447740 returns a shortest difference with extended
                // arithmetic;446e5a multiplies by f64 .08 before storingf32.
                const double delta=double(wanted)-shot.angle;
                const float difference=float(delta>double(3.1415927f)
                    ? double(wanted)-(double(shot.angle)+double(6.2831855f))
                    : -delta>double(3.1415927f)
                        ? double(wanted)-(double(shot.angle)-double(6.2831855f)) : delta);
                if (shot.age > 59) shot.speed += 0.2f;
                else {
                    if (std::abs(difference) >= 0.7853982f)
                        shot.speed = std::max(4.0f, shot.speed - 0.2f);
                    else if (std::abs(difference) < 0.2617994f)
                        shot.speed = std::min(16.0f, shot.speed + 0.2f);
                    shot.angle = wrap_angle(wrap_angle(float(shot.angle+float(double(difference)*double(.08f)))));
                }
            } else {
                shot.speed = std::min(16.0f, shot.speed + 0.1f);
            }
        }
        // 4736a0 ->474f00 stores velocity only after sin/cos * speed.
        // Rounding sin/cos first changes subsequent0.01 coordinate floors.
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
        // The center bounds belong only to the animation-free diagnostic path.
        // 004464d0 clips real SHT shots by ANM geometry, with an age gate.
        if (shot.script < 0 &&
            (shot.y < -32.0f || shot.y > 480.0f || shot.x < -224.0f || shot.x > 224.0f))
            shot.alive = false;
        if (shot.age_clock.current != shot.age) shot.age_clock.set(shot.age);
        shot.age_clock.tick(&rate);
        shot.age = shot.age_clock.current;
    }
    // Keep previous-sync slot identity until all culling has finished.
    consume_player_shot_geometry();
    size_t keep = 0;
    for (size_t i = 0; i < shots.size(); ++i)
        if (shots[i].alive) shots[keep++] = shots[i];
    shots.resize(keep);
}

void StageBattle::begin_player_shot_geometry_snapshot() {
    for (auto& snapshot : shot_geometry_) snapshot.valid = false;
}

void StageBattle::set_player_shot_geometry(i32 slot, i32 age, i32 script, u32 generation,
                                           const std::array<Vec3, 4>& quad) {
    if (slot < 0) return;
    if (shot_geometry_.size() <= size_t(slot))
        shot_geometry_.resize(size_t(slot) + 1);
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
        if (!shot.alive || shot.type == 2 || slot >= shot_geometry_.size()) continue;
        const auto& snapshot = shot_geometry_[slot];
        if (!snapshot.valid || snapshot.slot != i32(slot) ||
            snapshot.generation != shot.generation || snapshot.script != shot.script ||
            snapshot.age != shot.age - 1 || snapshot.age <= 9) continue;
        // 004464d0 culls at 00446531 before LAB_00446619 writes the current
        // node translation into its ANM VM. Therefore these are the
        // previous sync's screen-space corners; do not transform them through
        // the post-step battle position a second time.
        bool outside = true;
        for (const auto& vertex : snapshot.quad) {
            const bool vertex_outside = vertex.x <= 128.0f || vertex.x >= 512.0f ||
                                        vertex.y <= 16.0f || vertex.y >= 464.0f;
            outside = outside && vertex_outside;
        }
        if (outside) shot.alive = false;
    }
}

float StageBattle::query_enemy_damage(StageEnemy& state, float width, float height) {
    // 446870 returns before bomb/shot queries when443240's phase clock
    // has not advanced an integer tick (including half-speed Boss deaths).
    if (player_phase_clock_.current == player_phase_clock_.previous) return 0;
    // 临时探针（TH13_SHOT_PROBE=1）：P0 命中检测对拍（与 stage1_oracle HITCHK 同窗）。
    const char* probe = std::getenv("TH13_SHOT_PROBE");
    const bool shot_probe = probe && probe[0] == '1';
    // 0041a8a0 resolves this enemy after its ECL and before the next enemy.
    StageEnemy* enemy = &state;
    float damage = bomb_damage(*enemy, width, height);
    //4456e0(respawn): circular damage registration, radius32 +16/update,
    // life30, damage150;446870 tests enemy center independently of its box.
    if(respawn_damage.active && respawn_damage.remaining.current!=respawn_damage.remaining.previous) {
        const double dx=double(enemy->position[0])-respawn_damage.center.x;
        const double dy=double(enemy->position[1])-respawn_damage.center.y;
        if(dx*dx+dy*dy<=double(respawn_damage.radius)*respawn_damage.radius) damage+=150.f;
    }
    for (auto& shot : shots) {
        if (!shot.alive) continue;
        //446870 requires both the player clock and this registration's
        // countdown to change integer value. Its large f32 lifetime can
        // remain unchanged at fractional time rates.
        if (shot.damage_clock.current == shot.damage_clock.previous) continue;
        // 0x419a90:123 门（flags & 0x21 == 0 才可伤）+ 0x446870 AABB。
        const float ex = enemy->position[0], ey = enemy->position[1];
        const float dx = shot.x - ex, dy = shot.y - ey;
        if (shot_probe && sim.frame <= 130)
            std::printf("HITCHK f=%d enemy=? shot=(%.2f,%.2f) enemy=(%.2f,%.2f) "
                        "hb=(%.2f,%.2f) dx=%.2f dy=%.2f\n",
                        sim.frame, shot.x, shot.y, ex, ey,
                        width, height, dx, dy);
        // 0x446bf0 retires a standard shot through its hit callback even though
        // the damage registration's own HP threshold does not expire it.
        if (!shot_hits_enemy_box(shot.x, shot.y, shot.width, shot.height,
                                 ex, ey, width, height,
                                 shot.angle))
            continue;
        if (shot.hit_behavior == 4) {
            // 00447530: the slash remains active; each hit returns its
            // decremented damage, bounded below by1. ANM completion retires it.
            if (shot.damage > 1.0f) shot.damage -= 1.0f;
            damage += shot.damage;
            continue;
        }
        damage += shot.damage;
        if (shot.hit_behavior == 3) {
            // 4474b0 disables damage but keeps mode4 for the dying ANM.
            shot.sine_motion.radius = 16.0f;
            shot.sine_motion.speed = shot.speed = 3.0f;
        }
        shot.alive = false;
    }
    if (damage <= 0) return 0;
    // 00446870 尾：命中计分 = (min(sum,80)/10 + 10)/10（原值域，逐命中
    // 帧计分；G 状态流实证 f96 +1、f98 +1）。炸弹/出生/死亡演出除5同序。
    const float capped = std::min(damage, 80.0f);
    score_->score += (i32(capped) / 10 + 10) / 10;
    if (score_->score > 999999999) score_->score = 999999999;
    return capped;
}

void StageBattle::collide_shots(StageEnemy& state) {
    StageEnemy* enemy = &state;
    //419a90 gates the query by flags, not HP. BossDead keeps its entity
    // alive with bit80/10 and still consumes shots/awards hit points/spirits.
    if (enemy->dead || (enemy->flags & 0x21u)) return;
    float damage = enemy->hurtbox[0] > 0
        ? query_enemy_damage(*enemy, enemy->hurtbox[0], enemy->hurtbox[1]) : 0;
    if (enemy->damage_function == 1) {
        // 422cd0 queries a160x32 rectangle at the ship, writes (incoming+
        // query) to boss+519c, and returns that same sum to419a90.
        const float transferred = damage + query_enemy_damage(*enemy, 160, 32);
        if (auto* boss = sim.boss()) boss->pending_damage = i32(transferred);
        damage += transferred;
    }
    if (enemy->pending_damage > 0) {
        damage += enemy->pending_damage;
        enemy->pending_damage = 0;
    }
    // 00419a90 zeros the queried damage while stage+198 holds a dialogue.
    // Shots still retire and award their hit score, but neither life nor
    // the spirit accumulator/threshold/timer sees that damage.
    if (sim.dialog_host && sim.dialog_host->dialog_active()) return;
    if (damage <= 0) return;
    // 00419a90：出生/死亡演出期间伤害除5。
    float applied = damage;
    if (player_phase == PlayerPhase::Entering || player_phase == PlayerPhase::Dying)
        applied = float(i32(applied) / 5);
    // 419c76..419ccc: Miko's final spell (102..105) reduces damage
    // below 1651 HP while a bomb, player protection or Trance is active.
    // This precedes both the life update and the spirit accumulator.
    const auto& spell = sim.spell.state();
    if (spell.active() && spell.id >= 102 && spell.id <= 105 &&
        enemy->life < 1651.f &&
        (sim.bomb_active || invulnerable > 0 || (progress_ && trance_active(*progress_))))
        applied = float(i32(applied) / 5);
    if (applied <= 0) return;
    // 00419a90 / 004179e0：伤害同趟立即入命，消除次帧延迟
    frame_damage_total += applied;
    enemy->spirit_accum += applied;
    // 0x419a90 积分灵门（伤害帧判定，读累加后的累计）：
    // 00419a90:166-188: 先 4179e0 (spirit_accum += applied)，再检查
    // spirit_accum >= spirit_threshold；阈值按距离递增不看 hit timer，
    // 只有 spawn 本身要求 +0x4004>19 且 +0x4034 bit1==0。
    if (enemy->spirit_accum >= enemy->spirit_threshold) {
        const float sdx = enemy->position[0] - player_x;
        const float sdy = enemy->position[1] - player_y;
        const float dist2 = sdx * sdx + sdy * sdy;
        i32 step = 1000;
        if (dist2 < 2304.0f) step = 200;
        else if (dist2 < 9216.0f) step = 400;
        else if (dist2 < 36864.0f) step = 600;
        if (enemy->hit_anim_timer > 19 && !enemy->damage_spirits_disabled)
            item_manager.spawn_value_item(1, enemy->position[0], enemy->position[1],
                                          enemy->position[2], 2.0f);
        enemy->spirit_threshold += step;
    }
    enemy->hit_anim_timer = 30;
    sim.damage(*enemy, applied);
    // 419a90 marks effective damage after phase/death processing. Its final
    // feedback branch chooses the hit voice and ANM tint once per enemy,
    // subject to the original four-update cooldown, including bomb damage.
    if (applied > 0 && !enemy->dead) enemy->flags |= 0x400000u;
    if (enemy->dead) reward_kill(*enemy);
}

void StageBattle::explode_orb(PlayerBombOrb& orb) {
    if (!orb.active) return;
    orb.active = false;
    orb.exploding = true;
    orb.explosion_frame = 0;
    orb.explosion_damage_clock.set(11);
    // 0040b2a0/0040b350：半径128立即消弹，另建64起每帧扩8的伤害圆。
    sim.cancel_bullets_radius(Vec3{orb.x,orb.y,0},128,true,true);
    sim.cancel_lasers_radius({orb.x, orb.y, 0}, 128);
}

void StageBattle::step_bomb() {
    youmu_slash_spawns_.clear();  // 本帧生成事件（表现层读后即弃）
    for (auto& orb : bomb_orbs) {
        if (orb.exploding) ++orb.explosion_frame;
    }
    if (bomb_timer <= 0) return;
    ++bomb_age;
    bomb_timer = std::max(0, bomb_duration_ - bomb_age);
    // 四角色更新器0040b590/0040a860/0040c2c0/0040cb50均每帧刷新40无敌，
    // 不是原有统一240帧倒计时。
    invulnerable = 40;
    if(character_!=0&&bomb_timer==0) {
        if(character_==2&&!host_->hud_sound(0x1b))error_="Sanae Bomb end sound";
        return; // 40a860/40c2c0/40cb50 terminate before geometry/cancellation.
    }
    if (character_ == 0) {
        for (int i = 0; i < 8; ++i) {
            auto& orb = bomb_orbs[i];
            if (!orb.active) continue;
            if (bomb_age >= 200) {
                explode_orb(orb);
                continue;
            }
            const auto* target=bomb_age>90+i*10?nearest_target(sim,orb.x,orb.y,512.f):nullptr;
            orb.step(bomb_age,i,player_x,player_y,target);
            // 40b590 calls40ae30 before checking damage-area+0x88 >=300.
            // The final movement determines the explosion's cancel center.
            if (orb.accumulated_damage >= 300.0f) {
                explode_orb(orb);
                continue;
            }
            // 0040b900：8珠轮流每帧一个消半径64，不是每帧全屏消弹。
            if (bomb_age % 8 == i) {
                sim.cancel_bullets_radius(Vec3{orb.x,orb.y,0},64,true,true);
                sim.cancel_lasers_radius({orb.x, orb.y, 0}, 64);
            }
        }
    } else if (character_ == 1) {
        bomb_visual_angle_=bomb_angle_; // 40a860 writes the VM before steering.
        bomb_center_x_=player_x; bomb_center_y_=player_y;
        // 0040a860：Master Spark移动系数0.2；左右移动使激光每帧转0.15度。
        if (input_state & 0x40) bomb_angle_ -= 0.0026179939f;
        else if (input_state & 0x80) bomb_angle_ += 0.0026179939f;
        for (auto& bullet : sim.bullets) {
            // 0040ab90：三个消弹矩形，共用激光轴。
            for (int part = 0; part < 3; ++part) {
                static constexpr float distances[] = {208, 240, 304};
                static constexpr float heights[] = {32, 128, 256};
                StageEnemy point;
                point.position[0] = bullet.x; point.position[1] = bullet.y;
                point.hurtbox[0] = point.hurtbox[1] = 0;
                const float d = distances[part];
                if (rotated_hit(player_x + std::cos(bomb_angle_) * d,
                                player_y + std::sin(bomb_angle_) * d,
                                512, heights[part], bomb_angle_, point))
                    bullet.alive = false;
            }
        }
        // 40ab90 -> 4300f0: same three rectangles also split live lasers.
        for (int part = 0; part < 3; ++part) {
            static constexpr float distances[] = {208, 240, 304};
            static constexpr float heights[] = {32, 128, 256};
            sim.cancel_lasers_box({player_x + std::cos(bomb_angle_) * distances[part],
                                   player_y + std::sin(bomb_angle_) * distances[part], 0},
                                  {512, heights[part], 0}, bomb_angle_, true);
        }
    } else if (character_ == 2) {
        // 0040c2c0：五芒星中心缓动向(0,160)，转速每帧减0.03度。
        // QWORD 4aee80 is the widened float 0.01, not double literal 0.01.
        bomb_center_x_ = float(double(bomb_center_x_)+(0.0-double(bomb_center_x_))*double(0.01f));
        bomb_center_y_ = float(double(bomb_center_y_)+(160.0-double(bomb_center_y_))*double(0.01f));
        bomb_angle_ = combat_angle(bomb_angle_ + bomb_rotation_speed_);
        bomb_rotation_speed_ -= 0.0005235988f;
        float a=bomb_angle_;
        for(auto& ray:bomb_rays) {
            float x,y;polar_xy(a,168.23f,x,y);
            ray.origin={float(bomb_center_x_+x),float(bomb_center_y_+y),0};
            // 40c4ce..40c521 wraps the x87 difference BEFORE its f32 store.
            // Rounding the subtraction first changes the star's ray angles.
            double angle=double(a)-double(2.8274333f);
            if(angle>double(3.1415927f))angle-=double(6.2831855f);
            else if(angle<-double(3.1415927f))angle+=double(6.2831855f);
            ray.angle=wrap_angle(float(angle));
            polar_xy(ray.angle,160.f,x,y);
            ray.center={float(ray.origin.x+x),float(ray.origin.y+y),0};
            a=wrap_angle(float(a+1.2566371f));
        }
        for (auto& bullet : sim.bullets) {
            StageEnemy point;
            point.position[0] = bullet.x; point.position[1] = bullet.y;
            point.hurtbox[0] = point.hurtbox[1] = 0;
            for (const auto& ray:bomb_rays)
                if (rotated_hit(ray.center.x,ray.center.y,320,32,ray.angle,point))
                    bullet.alive=false;
        }
        // 40c600: five independent rectangle cancellations, in ray order.
        for (const auto& ray:bomb_rays)
            sim.cancel_lasers_box(ray.center,{320,32,0},ray.angle,true);
    } else {
        // 0040cb50（BombYoumuInf vtable@0x4aa610 slot1，r4/bomb 模块）：
        // 生成支路每帧恰一斩（age<261；lane 抖动 1 次 signed_unit 抽自共享
        // game RNG）；命中/消弹经 youmu_bomb_ 的斩矩形查询（0x4457e0 当帧
        // 一次性 + 0x4119c0 场内消弹）。set_vm 由表现层回填（无头路径无
        // VM，槽状态保持——r4/bomb 测试 A1-A7 覆盖）。生成帧矩形高 0
        // （scale=0）→ 无消弹无伤害，次帧起生效。
        if (bomb_age < YoumuBomb::kActiveFrames) {
            if (auto spawn = youmu_bomb_.step(bomb_age, sim.random))
                youmu_slash_spawns_.push_back(*spawn);
        }
        for (auto& bullet : sim.bullets) {
            if (!bullet.alive) continue;
            // 弹半径 = hit_size 半宽（0x4119c0 含弹半径语义）。
            if (youmu_bomb_.clears_bullet(bullet.x, bullet.y, bullet.hit_size[0] * 0.5f))
                bullet.alive = false;
        }
    }
}

float StageBattle::bomb_damage(StageEnemy& enemy, float width, float height) {
    float damage = 0;
    if (character_ == 0) {
        for (auto& orb : bomb_orbs) {
            const float dx = enemy.position[0] - orb.x, dy = enemy.position[1] - orb.y;
            const float distance = dx * dx + dy * dy;
            if (orb.active && bomb_age % 3 == 0 && distance <= 56.0f * 56.0f &&
                orb.accumulated_damage < 300.0f) {
                damage += 10.0f;
                orb.accumulated_damage += 10.0f;
            }
            if (orb.exploding && orb.explosion_damage_clock.current !=
                                     orb.explosion_damage_clock.previous) {
                const float radius = 64.0f + float(orb.explosion_frame) * 8.0f;
                if (distance <= radius * radius) damage += 60.0f;
            }
        }
    } else if (character_ == 1 && bomb_timer > 0 && bomb_age % 4 == 0) {
        const float c = std::cos(bomb_angle_), s = std::sin(bomb_angle_);
        static constexpr float distances[] = {208, 240, 304};
        static constexpr float heights[] = {32, 128, 256};
        static constexpr float damages[] = {60, 20, 20};
        for (int i = 0; i < 3; ++i)
            if (rotated_hit(player_x + c * distances[i], player_y + s * distances[i],
                            512, heights[i], bomb_angle_, enemy.position[0], enemy.position[1], width, height))
                damage += damages[i];
    } else if (character_ == 2 && bomb_timer > 0) {
        for (const auto& ray:bomb_rays)
            if(rotated_hit(ray.center.x,ray.center.y,320,32,ray.angle,enemy.position[0],enemy.position[1],width,height))damage+=10;
    } else if (character_ == 3 && bomb_timer > 0) {
        // 0x40cb50 斩矩形伤害：每敌每帧恰一次 30×覆盖斩数（cover_count 为
        // 数量，勿再乘；多斩重叠按槽叠加）。敌半径取受击半宽（r4/bomb
        // integration §伤害口径）。
        damage += YoumuBomb::kDamagePerRect *
                  float(youmu_bomb_.cover_count(enemy.position[0], enemy.position[1],
                                                width * 0.5f));
    }
    return damage;
}

void StageBattle::collide_player() {
    if (player_dead) return;
    // 0x40db10 普通弹态：行为队列 → pos+=vel → 碰撞。
    // 0x40e840:183/188（asm 0x40ed43 or [ebx+0x20],0x10）出生置 bit1|bit4，
    // 标准弹走 0x444140 圆形分支；bit4=0 走 0x443fd0 矩形分支；bit6 时
    // 尺寸先乘 +0x1324。两分支回 2 才由 0x40db10:228-231/286-288 调
    // 0x445900（且 +0x20 bit2 防重复）；回 1 消弹；已擦弹弹仍可命中。
    // 0x444140:13 另有 be830 bit0x100（灵界）整段绕过门；本作未实现灵界，
    // 正常 Stage1 该位为 0（设置点 0x405a20 需 be808>=200）。
    // 玩家门 = 0x2190 dialog/state 2/3/4/14698 bit1；本作无真实状态机：
    // player_dead 近似 state4、invulnerable>0 近似 state2/0x14688。此映射
    // 未闭合（0x444a10 置 state2+0x14688=180，0x444ca0 置 state4+6），
    // 由 Application/HUD owner 提供真实玩家状态后替换。
    const float p = player_hit_size_;
    const float half = p * 0.5f;  // 0x620/0x624（0x441dd0:70-84）
    const float left = player_x - half, right = player_x + half;
    const float top = player_y - half, bottom = player_y + half;
    const bool dialog_gate = !(sim.dialog_host && sim.dialog_host->dialog_active());
    const bool in_trance = progress_ && trance_active(*progress_);
    for (auto& bullet : sim.bullets) {
        if (!bullet.alive || !bullet.collision_ready || (bullet.flags & 0x2u) == 0) continue;
        const bool scaled = (bullet.flags & 0x40u) != 0;
        const float sx = scaled ? bullet.hit_scale * bullet.hit_size[0] : bullet.hit_size[0];
        const float sy = scaled ? bullet.hit_scale * bullet.hit_size[1] : bullet.hit_size[1];
        int result = 0;
        if ((bullet.flags & 0x10u) != 0) {
            // 00444140 的灵界门覆盖整个圆形判定（命中和擦弹）。
            // 00443fd0 矩形分支没有这个门，不能把两分支合并成全局 return。
            if (in_trance) continue;
            // 0x444140：0x444181 fstp dword [ebp-8] 把 D 落为 f32；
            // 阈值在 x87 80 位内比较。此处 D 落 f32、阈值用 double 近似，
            // x87 尾差未认证，不得据此宣称逐位一致。
            const double dx = double(player_x) - double(bullet.x);
            const double dy = double(player_y) - double(bullet.y);
            const float distance2 = float(dx * dx + dy * dy);
            const double p2 = double(p) * p;
            // 0x4441a1 fcomp + 0x4441a5 test ah,0x41 / jp：命中分支仅当
            // p²+r²>D²（严格），等号落擦弹支路（unicorn 边界实证：D==p
            // →返回 2）。生产 < 与原版同式，不得改成 ≤。
            if (double(distance2) < p2 + double(sx) * sx) {
                result = 1;
            } else {
                // 0x4441ac fdiv qword 2.5 → 0x4441b2 fstp dword：span 先单舍入
                // 落 f32，0x4441b5 起与 40.0f 取 max、以 f32 回读参与 80 位
                // 阈值 r²+(span+p)²。span 保 double 会在 D² 恰落两种阈值差
                // （~1e-4 px²）内的 f32 网格点时错判（unicorn 边界实证）。
                const float span = std::max(float(double(sx) / 2.5), 40.0f);
                const double graze2 =
                    double(sx) * sx + (double(span) + double(p)) * (double(span) + double(p));
                if (double(distance2) < graze2) result = 2;
            }
        } else {
            // 0x443fd0：分离判据严格 <；中心 ±24 四边（0x4440d8/0x4440ea 为
            // test ah,5+jnp、0x4440fc/0x44410e 为 test ah,0x41+je，两组判向
            // 皆为含等号）才擦弹。unicorn 边界实证：四边等号均返回 2，
            // 越 1ulp 返回 0——生产 <= 与原版同式，3/4 边不得改成 <。
            const float hx = sx * 0.5f, hy = sy * 0.5f;
            const bool separated = bullet.x + hx < left || bullet.y + hy < top ||
                                   right < bullet.x - hx || bottom < bullet.y - hy;
            if (separated) {
                if (left <= bullet.x + 24.0f && top <= bullet.y + 24.0f &&
                    bullet.x - 24.0f <= right && bullet.y - 24.0f <= bottom)
                    result = 2;
            } else {
                result = 1;
            }
        }
        if (result == 1) {
            //444140/443fd0 suppress only the hit branch in states2/4.
            // Their graze branch remains live during the deathbomb window.
            if (!dialog_gate || player_phase == PlayerPhase::Dying ||
                player_phase == PlayerPhase::Deathbomb) continue;
            // 00444140/00443fd0 可先触发玩家受击；0040db10 随后才用
            // bullet+0x24 判断是否消弹，保护不等于玩家无敌。
            if (invulnerable <= 0 && !in_trance) kill_player();
            if (bullet.cancel_protection == 0) bullet.alive = false;
            continue;
        }
        if (result == 2 && !bullet.grazed) {
            bullet.grazed = true;  // +0x20 bit2
            if (progress_) {
                if (progress_->graze < 99999999) progress_->graze++;
                progress_->max_point += 100;
                if (progress_->max_point > progress_->max_point_cap)
                    progress_->max_point = progress_->max_point_cap;
            }
            // 0x445900 尾：0x46f820(...,100,bullet pos,...) 效果 +
            // 0x460c50(...,0x2a) 声音。声音还缺 bullet.x 参数通道。
            sim.anm_effects.push_back({1, 100, bullet.x, bullet.y, bullet.z});
            host_->hud_sound(42);
        }
    }
}

i32 StageBattle::collide_laser(const StageLaser& laser) {
    // 444260 bypasses the entire query in trance. Dialog/invulnerability
    // suppress a hit return, so those states do not split the laser.
    if (progress_ && trance_active(*progress_)) return 0;
    const i32 contact = laser.player_contact(player_x, player_y,
                                            player_hit_size_ * .5f, player_hit_size_ * .5f);
    if (contact == 1) {
        if (player_dead || player_phase == PlayerPhase::Dying ||
            player_phase == PlayerPhase::Deathbomb ||
            invulnerable > 0 || (sim.dialog_host && sim.dialog_host->dialog_active())) return 0;
        kill_player();
    } else if (contact == 2 && laser.kind == StageLaser::Kind::Line && laser.graze_timer % 3 == 0) {
        // 431957: line fragments alone award graze, every third graze query.
        if (progress_) {
            if (progress_->graze < 99999999) ++progress_->graze;
            progress_->max_point = std::min(progress_->max_point_cap, progress_->max_point + 100);
        }
        sim.anm_effects.push_back({1, 100, player_x, player_y, 0});
        if (!host_->hud_sound(42)) error_ = "Laser graze sound failed";
    }
    return contact;
}

void StageBattle::collide_enemy_body(StageEnemy& enemy) {
    if (enemy.dead || (enemy.flags & (0x22u | 0x8000000u)) || enemy.body_invulnerable > 0 ||
        (progress_ && trance_active(*progress_))) return;
    const float p = player_hit_size_;
    const float half = p * 0.5f;
    int contact = 0;
    if ((enemy.flags & 0x8000u) == 0) {
        const float sx = enemy.hitbox[0] * 0.5f;
        const double dx = double(player_x) - double(enemy.position[0]);
        const double dy = double(player_y) - double(enemy.position[1]);
        const float distance2 = float(dx * dx + dy * dy);
        const double radius2 = double(sx) * sx;
        if (double(distance2) < double(p) * p + radius2) contact = 1;
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
        if (longitudinal - expanded <= length && longitudinal + expanded >= 0.0f &&
            lateral + expanded >= -half_w && lateral - expanded <= half_w) {
            if (longitudinal - half <= length && longitudinal + half >= 0.0f &&
                lateral + half >= -half_w && lateral - half <= half_w) contact = 1;
            else contact = 2;
        }
    }
    if (contact == 1) {
        if (!player_dead && player_phase != PlayerPhase::Dying &&
            player_phase != PlayerPhase::Deathbomb && invulnerable <= 0 &&
            !(sim.dialog_host && sim.dialog_host->dialog_active())) kill_player();
    } else if (contact == 2 && (enemy.flags & 0x200u) && i32(enemy.age) % 6 == 0) {
        if (progress_) {
            if (progress_->graze < 99999999) ++progress_->graze;
            progress_->max_point = std::min(progress_->max_point_cap, progress_->max_point + 100);
        }
        sim.anm_effects.push_back({1, 100, player_x, player_y, 0});
        if (!host_->hud_sound(42)) error_ = "Enemy body graze sound failed";
    }
}

bool StageBattle::kill_player() {
    // 00444ca0：先进入 state4 的8帧决死窗口。成功 Bomb 不扣命、不扣灵力。
    if (player_dead || player_phase == PlayerPhase::Dying ||
        player_phase == PlayerPhase::Deathbomb) return true;
    player_phase = PlayerPhase::Deathbomb;
    if (sim.player_effect && !sim.player_effect(25,{player_x,player_y,0},-1)) {
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
    // 0x444a10。死亡自动灵界保护：be808 >= 200 → 0x405a20(1)（激活位 +
    // 保护位），玩家存活（state 1），灵界结束时 0x405b80 经 0x444a10
    // 直接确定死亡，不再进入新的决死窗口；不满足 → 普通死亡。
    if (!progress_ || trance_active(*progress_) ||
        (progress_->run_mode_flags & 0x30u) == 0x20u ||
        (sim.dialog_host && sim.dialog_host->dialog_active()) ||
        progress_->ending_points < 200) {
        return commit_death_normal();
    }
    trance_protection_ = true;
    trance_begin(*progress_);
    item_manager.set_spirit_trance(true);
    if (!(progress_->stage == 6 && progress_->chapter >= 6) && !host_->trance_music(true)) {
        error_ = "Protective Trance music failed"; return false;
    }
    trance_task_frame_ = 0;
    player_phase = PlayerPhase::Active;
    //444a10's successful405a20 path only changes state, preserving timer.
    return true;
}

bool StageBattle::commit_death_normal() {
    player_->deaths++;
    --player_->extra_lives;  // -1 是最后一命死亡；不能过早打开 GameOver。
    player_phase = PlayerPhase::Dying;
    player_phase_frame = 0;
    player_phase_clock_.set(0);
    invulnerable = 180;
    if (player_->extra_lives >= 0 &&
        !host_->life_hud(player_->extra_lives, player_->life_pieces)) {
        error_ = "Player death HUD failed";
        return false;
    }
    return true;
}

bool StageBattle::update_player(const GameInput& input) {
    if (player_dead) return true;
    const float startup_rate = sim.time_rate();
    player_state_.startup_clock.tick(&startup_rate);
    if (player_phase_clock_.current != player_phase_frame)
        player_phase_clock_.set(player_phase_frame);
    // 00443240 case4 在 timer>=8 时先确定死亡，不再接受该帧的迟到 Bomb。
    const bool deathbomb_expired = player_phase == PlayerPhase::Deathbomb && player_phase_frame >= 8;
    if (deathbomb_expired && !commit_death()) return false;
    //443240 case4 jumps into case2 even when protection changed state to1.
    // No same-frame movement; that shared body still creates a kind3 spirit.
    if (player_phase == PlayerPhase::Dying || deathbomb_expired) {
        if (player_phase_frame == 3) {
            // 00445a20：发生在死亡演出第3帧，而不是碰撞的同帧。
            const i32 step = player_->power_step;
            player_->power = std::max(player_->power - step, step);
            //44343e: seven small power items aimed toward(0,player_y-224).
            const float base=float(std::atan2(-224.0,double(-player_x)));
            for(i32 i=0;i<7;++i) {
                const float angle=float(double(i)*double(3.1415927f)/28.0+base-double(.3926991f));
                item_manager.spawn_item(player_x,player_y,0,1,angle,3.f);
            }
        }
        item_manager.spawn_value_item(3,player_x,player_y,0,2.f);
        if (player_phase_frame >= 30) {
            if (player_->extra_lives < 0) {
                player_dead = true;
                if (!host_->request_player_game_over()) {
                    error_ = "Player game-over request failed";
                    return false;
                }
                // 00443240 continues through the registration/protection
                // epilogue after opening Replay End or Game Over. The overlay
                // freezes subsequent updates, not this callback's final tick.
            } else {
                if (player_->bombs < 2) {
                    player_->bombs = 2;
                    if (!host_->bomb_hud(player_->bombs, player_->bomb_pieces)) {
                        error_ = "Player respawn bomb HUD failed";
                        return false;
                    }
                }
                progress_->ending_points = 0;
                sim.set_time_rate(1.f);
                respawn_damage.center={player_x,player_y,0};
                respawn_origin={player_x,player_y,0};
                respawn_damage.radius=32.f;
                respawn_damage.remaining.set(30);
                respawn_damage.active=true;
                player_phase = PlayerPhase::Entering;
                player_phase_frame = 0;
                player_phase_clock_.set(0);
                invulnerable = 280;  // 004435f9 / player+0x14688
                pos_fx = 0;
                pos_fy = 0xf000;
                player_x = 0;
                player_y = 480;
            }
        }
    } else if (player_phase == PlayerPhase::Entering) {
        // 00443273：60帧内从屏下480回到400，整数定点路径。
        pos_fy = 0xf000 - player_phase_frame * 0x2800 / 60;
        player_y = float(pos_fy) * 0.0078125f;
        // 443240 case0 calls441c30 before the possible fallthrough to case1.
        // It arms all eight option reset latches;442960 consumes them on the
        // first movement update, using that update's focus and player position.
        option_reset_pending_ = true;
        for (auto& position : movement_history_) position = {pos_fx, pos_fy};
        //44331c: first30 updates clear lasers around the saved death point,
        // with distinct protected/unprotected radii.4432f6 thereafter clears
        // bullets around the returning player and fully cancels all lasers.
        if(player_phase_frame<30) {
            const float radius=float(double(player_phase_frame)*512.0/30.0+64.0);
            sim.cancel_lasers_radius(respawn_origin,radius,true);
            sim.cancel_lasers_radius(respawn_origin,float(radius*.25f),false);
        } else {
            if(!sim.cancel_respawn_bullets({player_x,player_y,0},640.f))return false;
            sim.cancel_lasers(false);
        }
        if(!sim.error().empty())return false;
        if (player_phase_frame >= 60) {
            player_phase = PlayerPhase::Active;
            player_phase_frame = 0;
            player_phase_clock_.set(0);
            if (input.pressed & 0x02) on_bomb_key();
            move_player(input.held);
        }
    } else {
        // 00443240 case4 ends after a successful deathbomb restores state1;
        // it does not fall through to case1's 00442960 movement this frame.
        const bool move_this_frame = player_phase == PlayerPhase::Active;
        if (input.pressed & 0x02) on_bomb_key();
        if (move_this_frame && player_phase == PlayerPhase::Active) move_player(input.held);
    }
    //443240 updates registrations and protection after phase-specific work,
    // including the frame on which the respawn registration is created.
    const float rate=sim.time_rate();
    //443240 updates existing registrations before4460d0 creates new ones.
    for (auto& shot : shots)
        if (shot.alive) shot.damage_clock.decrement(&rate);
    // 40b2a0/40b350 create11-tick circular registrations. 443240 expands
    // them each update but446870 queries only changed countdown integers.
    for (auto& orb : bomb_orbs) {
        if (!orb.exploding) continue;
        orb.explosion_damage_clock.decrement(&rate);
        if (orb.explosion_damage_clock.current < 1) orb.exploding = false;
    }
    if(respawn_damage.active) {
        //473780 rounds the registration's stationary mover as well.
        respawn_damage.center.x=round001(respawn_damage.center.x);
        respawn_damage.center.y=round001(respawn_damage.center.y);
        respawn_damage.radius+=16.f;
        respawn_damage.remaining.decrement(&rate);
        if(respawn_damage.remaining.current<1)respawn_damage.active=false;
    }
    if(invulnerable>0)--invulnerable;
    if (player_phase_clock_.current != player_phase_frame)
        player_phase_clock_.set(player_phase_frame);
    player_phase_clock_.tick(&rate);
    player_phase_frame = player_phase_clock_.current;
    return error_.empty();
}

void StageBattle::reward_kill(StageEnemy& enemy) {
    // 0x419a90 击破：无即时分/灵力（G 状态流实证 score/power 击破帧不变，
    // power 恒 100）。掉落与积分灵经 0x417b10/0x438ec0（E 的 drop_table 与
    // 本模块积分灵门）；此钩子保留给表现层（ANM 回收已走 retire_sink）。
    (void)enemy;
}

// 道具/神霊侧逐帧步进（0x42e380/0x438730 的玩家只读面装配）。
// 三盒（0x443240:264-282 活动路径逐帧写，0x441dd0:80-100 为入场同式）：
//   玩家 +0x62c = 0x4c0bb8[c]×0.5（收集基），+0x638 = 0x4c0bc8[c]×0.5
//   （吸附基，0x441dd0:77-78 自 .data 实读）。
//   box            = ±(0x62c×0.5) = ±bb8[c]×0.25（收集判定盒）
//   box_auto       = ±0x62c       = ±bb8[c]×0.5（非聚焦吸附盒）
//   box_auto_focus = ±0x638       = ±bc8[c]×0.5（聚焦吸附盒）
// 追踪初速 = *(player+0x660 表+8) = 0x4c0bd8[c]（0x441dd0:66-68 挂表，
// .data 实读 5/7/5/6；旧定标 3.0 已废）。
void StageBattle::update_item_side() {
    // .data 0x4c0bb8 / 0x4c0bc8 / 0x4c0bd8（EXE 实读，四角色序）。
    static constexpr float kCollectBase[4] = {60.0f, 70.0f, 60.0f, 60.0f};
    static constexpr float kAutoBase[4] = {100.0f, 118.0f, 100.0f, 100.0f};
    static constexpr float kHomingSpeed[4] = {5.0f, 7.0f, 5.0f, 6.0f};
    const i32 c = character_ & 3;
    ItemCollector collector;
    collector.x = player_x;
    collector.y = player_y;
    collector.state = player_dead ? 2
                                  : (player_phase == PlayerPhase::Active ? 1
                                     : player_phase == PlayerPhase::Dying ? 2
                                     : player_phase == PlayerPhase::Deathbomb ? 4
                                     : 1);
    // 42ebb9 reads held Shift directly, including the respawn entry phase
    // where move_player has not refreshed the movement/option focus cache.
    collector.focus = (input_state & 0x08u) != 0;
    if(const auto* boss=sim.boss()) {
        collector.boss_present=true;collector.boss_x=boss->position[0];collector.boss_y=boss->position[1];
    }
    collector.dialog = sim.dialog_host && sim.dialog_host->dialog_active();
    collector.homing_speed = kHomingSpeed[c];
    const float half = kCollectBase[c] * 0.25f;      // 收集判定盒 ±bb8×0.25
    collector.box = {player_x - half, player_y - half,
                     player_x + half, player_y + half};
    const float auto_half = kCollectBase[c] * 0.5f;  // 非聚焦吸附 ±bb8×0.5
    collector.box_auto = {player_x - auto_half, player_y - auto_half,
                          player_x + auto_half, player_y + auto_half};
    const float focus_half = kAutoBase[c] * 0.5f;    // 聚焦吸附 ±bc8×0.5
    collector.box_auto_focus = {player_x - focus_half, player_y - focus_half,
                                player_x + focus_half, player_y + focus_half};
    collector.progress = progress_;
    collector.player = player_;
    collector.score = score_;
    const i32 old_lives = player_ ? player_->extra_lives : 0;
    const i32 old_life_pieces = player_ ? player_->life_pieces : 0;
    const i32 old_bombs = player_ ? player_->bombs : 0;
    const i32 old_bomb_pieces = player_ ? player_->bomb_pieces : 0;
    const i32 old_life_threshold = progress_ ? progress_->life_piece_threshold_index : 0;
    item_manager.update(collector);
    item_manager.update_spirits(collector);
    if (player_) {
        if ((player_->extra_lives != old_lives || player_->life_pieces != old_life_pieces ||
             (progress_ && progress_->life_piece_threshold_index != old_life_threshold)) &&
            !host_->life_hud(player_->extra_lives, player_->life_pieces))
            error_ = "Item collection life HUD failed";
        if ((player_->bombs != old_bombs || player_->bomb_pieces != old_bomb_pieces) &&
            !host_->bomb_hud(player_->bombs, player_->bomb_pieces))
            error_ = "Item collection bomb HUD failed";
    }
    // 收集音（0x42e380 合并尾 0x460c50(...,0x25)=37）；其余事件留给表现层。
    for (const auto& event : item_manager.events()) {
        if (event.kind == ItemEvent::Kind::Collected && event.value > 0)
            host_->hud_sound(37);
    }
    item_manager.clear_events();
}

// 灵界（0x4057f0/0x405a20/0x405b80）：
//   未激活：满 600（trance_ready）+ C 键（0x4e4c14 & 0x800）+ 任务计时
//   ≥30 + 非 Bomb 中 + 无对话 + 非符卡练习位 → 激活（0x405a20(0)）。
//   激活中：42c550 →43d4f0 每帧 be808 -1；600 点持续600帧。
//   be808 < 1 → 结束：保护灵界走 0x444a10 直接确定死亡，常规 -200。
bool StageBattle::end_trance() {
    if (!progress_ || !trance_active(*progress_)) return true;
    const bool protection = trance_protection_;
    trance_end(*progress_, protection);
    item_manager.set_spirit_trance(false);
    trance_protection_ = false;
    if (protection) {
        // 405b80 ->444a10, gauge0 makes405a20 fail: subtract a life
        // and enter state2 immediately, without another Bomb window.
        if (!commit_death_normal()) return false;
    }
    // 405b80 always finishes with405440(0x46), including normal exit.
    invulnerable = 70;
    trance_task_frame_ = 0;
    if (!(progress_->stage == 6 && progress_->chapter >= 6) && !host_->trance_music(false)) {
        error_ = "Trance music restore failed"; return false;
    }
    return true;
}

bool StageBattle::update_trance(const GameInput& input) {
    if (!progress_) return true;
    if (trance_active(*progress_)) {
        ++trance_task_frame_;
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
    if (bomb_timer > 0) return true;                       // 0x4c2170+0x40
    if (sim.dialog_host && sim.dialog_host->dialog_active()) return true;
    if ((progress_->run_mode_flags & 0x30) == 0x20) return true;  // 符卡练习
    if (!host_->hud_sound(0x34)) { error_ = "Trance activation sound failed"; return false; }
    trance_begin(*progress_);
    item_manager.set_spirit_trance(true);
    if (!(progress_->stage == 6 && progress_->chapter >= 6) && !host_->trance_music(true)) {
        error_ = "Trance music start failed"; return false;
    }
    trance_task_frame_ = 0;
    return true;
}

bool StageBattle::open_event_sink(const char* path) {
    // 幂等（r4/harness）：sink 已开时不重开/截断，仅对齐帧号并启用观察。
    if (sink_open_) {
        events_ = true;
        sim.sink.set_frame(sim.frame);
        return true;
    }
    if (!path || !sim.sink.open(path)) {
        error_ = "open_event_sink failed";
        return false;
    }
    sink_open_ = true;
    events_ = true;
    sim.sink.set_frame(sim.frame);
    return true;
}

void StageBattle::on_bomb_key() {
    // 0x4433dc/0x443525 的 Bomb 分派在扣弹前调用 0x40a540；其 :8 明确要求
    // (DAT_004be830 & 0x100)==0，即灵界激活时拒绝 Bomb（不扣弹、不启动）。
    if (!player_ || !progress_ || trance_active(*progress_)) return;
    // 同一 0040a540 共享门还要求 stagectx+0x198 == 0。对话期间按 X
    // 不扣弹、不启 Bomb，也不以清屏或 HUD 更新消耗这次输入。
    if (sim.dialog_host && sim.dialog_host->dialog_active()) return;
    if (player_->bombs <= 0 || bomb_timer > 0 || player_dead ||
        (player_phase != PlayerPhase::Active && player_phase != PlayerPhase::Deathbomb)) return;
    if (player_phase == PlayerPhase::Deathbomb && player_phase_frame >= 8) return;
    player_->bombs--;
    if (!host_->bomb_hud(player_->bombs, player_->bomb_pieces)) error_ = "bomb hud";
    // Bomb vtables @004aa5bc：Reimu200、Marisa301、Sanae/Youmu261。
    static constexpr i32 durations[4] = {200, 301, 261, 261};
    bomb_duration_ = durations[character_];
    bomb_timer = bomb_duration_;
    sim.bomb_active = true;
    sim.spell.bomb_started();
    bomb_age = -1;
    ++bomb_generation;
    if(!host_->hud_sound(0x31))error_="Bomb start sound"; // four native constructors
    bomb_orbs = {};
    if (character_ == 3) youmu_bomb_.reset();  // 0x40c910 cursor/槽复位
    youmu_slash_spawns_.clear();
    bomb_angle_ = -1.5707964f;
    bomb_visual_angle_=bomb_angle_;
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
        // 00443525：决死成功返回 active，状态timer设置60。
        player_phase = PlayerPhase::Active;
        player_phase_frame = 60;
        player_phase_clock_.set(60);
    }
    // 原版各角色以自己的圆/旋转矩形消弹，不是按X即全屏清除。
}

bool StageBattle::continue_player() {
    // 43fa90 resumes the existing dying phase after replenishing lives.
    // 443240 then executes its ordinary respawn branch on the next tick.
    if (!progress_ || !player_) return false;
    player_dead = false;
    option_reset_pending_ = true;
    update_options(true);
    return error_.empty();
}

bool StageBattle::step_transition(const GameInput& input) {
    // 42bfa0 retains player update/draw while enemy/bullet callbacks stop.
    // Input remains the old Replay slot until 448ec0 at startup age 30.
    input_state = input.held;
    option_movement_tick_ = false;
    if (!update_player(input)) return false;
    sim.player_x = player_x; sim.player_y = player_y;
    update_options();
    update_firing((input.held & 1u) != 0);
    step_shots();
    return error_.empty();
}

bool StageBattle::step(const GameInput& input) {
    if (!error_.empty()) return false;
    // 0 帧是 start() 装载趟，此后每 step 一帧；这也是 Boss 非符锚点的
    // 生产时钟，不能只在事件落盘时推进。捕获只观察同一游戏帧。
    ++sim.frame;
    if (progress_) ++progress_->chapter_frame; //42c550 precedes ECL524 in this update.
    frame_damage_total = 0.0f;
    if (events_) sim.sink.set_frame(sim.frame);
    input_state = input.held;
    // Retail schedule:42c550/43d4f0 drain ->4057f0 activation ->443240
    // player ->418e00 enemies ->42e380/438660 items/spirits. In particular,
    // existing spirits and this frame's shots must see a new Trance now.
    if (!update_trance(input)) return false;
    option_movement_tick_ = false;
    if (!update_player(input)) return false;
    if (events_) sim.sink.player(player_x, player_y, input_state);
    // ECL 步进吃实时自机位置（etOn 瞄准/追弹用）。
    sim.player_x = player_x;
    sim.player_y = player_y;
    update_options();
    update_firing((input.held & 0x01) != 0);
    step_shots();
    step_bomb();
    sim.bomb_active = bomb_timer > 0;
    for (auto& enemy : sim.enemies)
        if (enemy->hit_anim_timer > 0) --enemy->hit_anim_timer;
    if (!sim.step_frame()) {
        error_ = sim.error();
        return false;
    }
    collide_player();
    // 道具/神霊侧（0x42e380/0x438730）：装配玩家只读面后步进。
    update_item_side();
    // 临时探针（TH13_BOMB_PROBE=1）：Bomb 全窗逐帧对照（r4/combat 权威
    // 对拍）：age/timer/库存/无敌、8 珠态、敌 HP、存活弹数、game RNG 计数。
    if (std::getenv("TH13_BOMB_PROBE")) {
        std::printf("BOMB f=%d age=%d timer=%d stock=%d invuln=%d rng=%u",
                    sim.frame, bomb_age, bomb_timer, player_ ? player_->bombs : -1,
                    invulnerable, sim.random.index());
        for (int i = 0; i < 8; ++i)
            if (bomb_orbs[i].active || bomb_orbs[i].exploding)
                std::printf(" orb%d=%c(%.2f,%.2f)", i,
                            bomb_orbs[i].active ? 'A' : 'E', bomb_orbs[i].x,
                            bomb_orbs[i].y);
        for (const auto& enemy : sim.enemies)
            if (!enemy->dead && enemy->life > 0)
                std::printf(" e%u=%g", enemy->serial, enemy->life);
        u32 alive_bullets = 0;
        for (const auto& bullet : sim.bullets)
            if (bullet.alive) ++alive_bullets;
        std::printf(" bullets=%u\n", alive_bullets);
    }
    // 临时探针（TH13_LIFE_PROBE=1）：敌机生命轨迹对拍（与 stage1_oracle
    // TH13_LIFE_PROBE 同窗，帧/序号/生命三字段同格式）。
    if (life_probe_) {
        std::printf("SCORE f=%d score=%d power=%d\n", sim.frame,
                    score_ ? score_->score : -1, player_ ? player_->power : -1);
        for (const auto& shot : shots)
            if (shot.alive)
                std::printf("SHOT f=%d x=%.9g y=%.9g dmg=%.9g\n", sim.frame,
                            double(shot.x), double(shot.y), double(shot.damage));
        for (const auto& enemy : sim.enemies)
            std::printf("LIFE f=%d e=%d life=%d pos=(%.9g,%.9g) hb=(%.9g,%.9g) "
                        "flags=%08x\n",
                        sim.frame, int(enemy->serial), int(enemy->life),
                        double(enemy->position[0]), double(enemy->position[1]),
                        double(enemy->hurtbox[0]), double(enemy->hurtbox[1]),
                        enemy->flags);
    }
    return error_.empty();
}

}  // namespace th13
