// M4 原生固定 case（全队灵界射击与唯一死亡责任；RULES R-T1..T10）。
// 被测生产面：
//   - StageBattle 团队灵界状态机（trance_mode/owner_seat/generation）；
//   - update_trance_mp（R-T8 Manual 请求逐席仲裁：席位升序第一个有效者）；
//   - commit_death 的 DeathProtection owner 记录（R-T5/R-T9）；
//   - end_trance owner 定向死亡（R-T5：owner 死一次，队友不进死亡演出）；
//   - Bomb 入口（R-T2：灵界期间全员禁雷）；
//   - 槽消耗全场一次（R-T3：step_mp 每帧恰一次）。
//
// 固定 case 映射（workflow-cases.json m4.*）：
//   team_trance_shooting_four_roles → 全队灵界射击（弹型块 10 全席生效）
//   protection_owner_only_death    → 保护灵界结束只有 owner 死
//   gauge_once_thresholds          → 199/200/599/600 阈值 + 槽每帧 -1 一次
//   all_collision_invulnerability  → 灵界期间全席免疫三类受击
//   simultaneous_requests          → 同帧多席 C 请求仲裁（席位升序）
//   deathbomb_manual_c             → 决死窗内 C 开启记 Manual（结束 -200 不死）
//   generation_respawn_pause_stage → owner 重生/暂停/跨关不清新代灵界
//   peer_bomb_policy               → 灵界期间全席禁雷、结束后恢复
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "multiplayer/WorldState.hpp"
#include <cmath>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32) override { return true; }
};

class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 0xff, 0, {600});
        emit(main, 0, 10, 0, 0xff, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "mp4.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            static const std::string root = [] {
                std::string path = __FILE__;
                for (int i = 0; i < 8; ++i) {
                    const std::string probe = path + "/artifacts/reference/raw/pl00a.sht";
                    std::FILE* check = std::fopen(probe.c_str(), "rb");
                    if (check) { std::fclose(check); break; }
                    const auto cut = path.find_last_of("/\\");
                    if (cut == std::string::npos) break;
                    path.erase(cut);
                }
                return path;
            }();
            std::FILE* file = std::fopen((root + "/artifacts/reference/raw/" + name).c_str(), "rb");
            if (!file) return false;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            return got == out.size();
        }
        return false;
    }
    bool animation(u32, const std::string& name) override {
        if (animations) animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Fixture {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    GameInput idle;

    bool init() {
        provider.animations = &battle.sim.anim_list;
        return battle.load(provider, "mp4.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
    // 灵界就绪态：满槽 600 + 任务计时 ≥30（零输入推进 30 帧）。
    void arm_trance() {
        progress.ending_points = 600;
        GameInput none;
        for (int frame = 0; frame < 32; ++frame) CHECK(battle.step(none));
    }
};

const u32 kCKey = 0x800;

StageBullet bullet_at(float x, float y) {
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = bullet.hit_size[1] = 8;
    bullet.x = x;
    bullet.y = y;
    return bullet;
}

void separate_seats(Fixture& fixture, i32 target) {
    for (i32 seat = 0; seat < fixture.battle.seat_count; ++seat) {
        auto& player = fixture.battle.seats[size_t(seat)];
        player.x = seat == target ? -120.f : 120.f;
        player.fixed_x = i32(player.x * 128);
        player.invulnerable = 0;
    }
}

}  // namespace

// ---- gauge_once_thresholds：阈值行为 + 槽每帧 -1 恰一次 ----
TEST(gauge_once_thresholds) {
    // ① 阈值单元：trance_ready 600 门槛；trance_add 封顶；trance_end -200。
    {
        SessionState state;
        state.ending_points = 599;
        CHECK(!trance_ready(state));   // 599 不可激活
        state.ending_points = 600;
        CHECK(trance_ready(state));    // 600 可激活
        trance_begin(state);
        CHECK(!trance_ready(state));   // 激活中不可再激活
        trance_end(state, false);      // 正常结束 -200
        CHECK_EQ(state.ending_points, 400);
        trance_add(state, 250);        // 封顶 600
        CHECK_EQ(state.ending_points, 600);
        trance_add(state, 100);
        CHECK_EQ(state.ending_points, 600);
        state.ending_points = 199;
        trance_end(state, false);      // 199 - 200 → 0（无负值）
        CHECK_EQ(state.ending_points, 0);
    }
    // ② MP 槽消耗一次：2 席会话激活灵界后推进 10 帧——槽 -10（不是 -20）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        // seat 0 按 C 激活（经 step 参数注入 C 沿）。
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        CHECK(dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
        CHECK_EQ(dual.battle.trance_owner_seat, 0);
        const i32 slot_at_start = dual.progress.ending_points;
        GameInput none;
        for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none));
        CHECK_EQ(dual.progress.ending_points, slot_at_start - 10);  // R-T3：一次/帧
    }
    // R-T6: 199 must follow actual collision/death/drop/respawn, whereas 200
    // enters protection without immediately charging a life or lowering Power.
    for (i32 gauge : {199, 200}) {
        Fixture boundary;
        CHECK(boundary.init());
        boundary.enable(2, {0, 0, 0});
        GameInput idle;
        for (i32 frame = 0; frame < 32; ++frame) CHECK(boundary.battle.step(idle));
        separate_seats(boundary, 1);
        boundary.progress.ending_points = gauge;
        boundary.battle.seat_huds[1].power = 400;
        const i32 lives = boundary.battle.seat_huds[1].extra_lives;
        auto bullet = bullet_at(-120, 400);
        boundary.battle.sim.bullet_player_collision(bullet);
        CHECK_EQ(boundary.battle.seats[1].phase, StagePlayerState::Phase::Deathbomb);
        for (i32 frame = 0; frame < 20 &&
             boundary.battle.seats[1].phase == StagePlayerState::Phase::Deathbomb; ++frame)
            CHECK(boundary.battle.step(idle));
        if (gauge == 200) {
            CHECK(boundary.battle.team_trance_active());
            CHECK_EQ(boundary.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
            CHECK_EQ(boundary.battle.trance_owner_seat, 1);
            CHECK_EQ(boundary.battle.seat_huds[1].extra_lives, lives);
            CHECK_EQ(boundary.battle.seat_huds[1].power, 400);
        } else {
            CHECK(!boundary.battle.team_trance_active());
            CHECK_EQ(boundary.battle.seats[1].phase, StagePlayerState::Phase::Dying);
            CHECK_EQ(boundary.battle.seat_huds[1].extra_lives, lives - 1);
            CHECK_EQ(boundary.battle.seat_huds[1].deaths, 1);
            for (i32 frame = 0; frame < 4; ++frame) CHECK(boundary.battle.step(idle));
            i32 small = 0, big = 0;
            for (const auto& item : boundary.battle.item_manager.items()) if (item.alive) {
                small += item.type == ItemPower;
                big += item.type == ItemBigPower;
            }
            std::printf("# gauge199 death drops small=%d big=%d power=%d\n",
                small, big, boundary.battle.seat_huds[1].power);
            CHECK_EQ(small, 7);
            CHECK_EQ(big, 1);
            CHECK_EQ(boundary.battle.seat_huds[1].power, 300);
            for (i32 frame = 0; frame < 110; ++frame) CHECK(boundary.battle.step(idle));
            CHECK_EQ(boundary.battle.seats[1].phase, StagePlayerState::Phase::Active);
            CHECK(!boundary.battle.seats[1].ghost);
            CHECK_EQ(boundary.battle.seat_huds[1].deaths, 1);
            CHECK_EQ(boundary.battle.seat_huds[0].deaths, 0);
        }
    }
    // R-T4: actual C activation converts existing spirits, newborn spirits
    // inherit trance, and end_trance resets their lifetime exactly once.
    {
        Fixture spirits;
        CHECK(spirits.init());
        spirits.enable(2, {1, 1, 0});
        spirits.arm_trance();
        auto& manager = spirits.battle.item_manager;
        manager.spawn_value_item(0, -160, 60, 0, 0);
        Spirit* old = nullptr;
        for (auto& spirit : manager.spirits()) if (spirit.alive) old = &spirit;
        CHECK(old != nullptr);
        if (!old) return;
        const u32 old_serial = old->serial;
        CHECK_EQ(old->flags & 1u, 0u);
        old->frame = 77;
        old->lifetime_clock.set(78);
        GameInput c;
        c.update(kCKey);
        CHECK(spirits.battle.step(c));
        CHECK_EQ(old->flags & 1u, 1u);
        CHECK_EQ(old->serial, old_serial);
        CHECK(old->frame < 2);
        manager.spawn_value_item(2, 160, 60, 0, 0);
        Spirit* born = nullptr;
        for (auto& spirit : manager.spirits())
            if (spirit.alive && spirit.serial != old_serial) born = &spirit;
        CHECK(born != nullptr);
        if (!born) return;
        CHECK_EQ(born->flags & 1u, 1u);
        CHECK(spirits.battle.end_trance());
        CHECK_EQ(old->flags & 1u, 0u);
        CHECK_EQ(born->flags & 1u, 0u);
        CHECK_EQ(old->frame, -1);
        CHECK_EQ(born->frame, -1);
        CHECK_EQ(old->lifetime_clock.current, 0);
        CHECK_EQ(born->lifetime_clock.current, 0);
        GameInput idle;
        CHECK(spirits.battle.step(idle));
        const i32 old_frame = old->frame, born_frame = born->frame;
        CHECK(old_frame >= 0);
        CHECK(born_frame >= 0);
        CHECK(spirits.battle.end_trance()); // Already idle: no repeated reset.
        CHECK_EQ(old->frame, old_frame);
        CHECK_EQ(born->frame, born_frame);
        CHECK(spirits.battle.step(idle));
        CHECK_EQ(old->frame, old_frame + 1);
        CHECK_EQ(born->frame, born_frame + 1);
    }
}

// ---- simultaneous_requests：同帧多席 C 请求仲裁 ----
TEST(simultaneous_requests) {
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    triple.arm_trance();
    // 三席同帧按 C：席位升序第一个有效者（seat 0）成为 owner。
    GameInput seat0;
    seat0.update(kCKey);
    triple.battle.seat_inputs[1].update(kCKey);
    triple.battle.seat_inputs[2].update(kCKey);
    CHECK(triple.battle.step(seat0));
    CHECK(triple.battle.team_trance_active());
    CHECK_EQ(triple.battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK_EQ(triple.battle.trance_owner_seat, 0);  // R-T8：升序第一
    CHECK_EQ(triple.battle.trance_generation, 1u);
    // seat 0 Bomb 中时跳过 → seat 1 成为 owner。
    Fixture triple2;
    CHECK(triple2.init());
    triple2.enable(3, {0, 0, 0});
    triple2.arm_trance();
    // seat 0 Bomb 中（无效请求）：bomb_timer 每帧递减，设 1 保证当帧有效。
    triple2.battle.seats[0].bomb_timer = 1;
    GameInput seat0b;
    seat0b.update(kCKey);
    triple2.battle.seat_inputs[1].update(kCKey);
    CHECK(triple2.battle.step(seat0b));
    CHECK(triple2.battle.team_trance_active());
    CHECK_EQ(triple2.battle.trance_owner_seat, 1);  // 跳过 Bomb 席
}

// ---- peer_bomb_policy：主动和保护灵界期间全员禁雷 ----
TEST(peer_bomb_policy) {
    constexpr u32 kBombKey = 0x02;
    using Mode = StageBattle::TranceMode;
    using Phase = StagePlayerState::Phase;
    for (const Mode mode : {Mode::Manual, Mode::DeathProtection}) {
        Fixture triple;
        CHECK(triple.init());
        triple.enable(3, {0, 0, 0});
        triple.arm_trance();

        GameInput host;
        const auto step = [&](u32 seat0, u32 seat1, u32 seat2) {
            host.update(seat0);
            triple.battle.seat_inputs[1].update(seat1);
            triple.battle.seat_inputs[2].update(seat2);
            CHECK(triple.battle.step(host));
        };
        std::array<i32, 3> stock{};
        for (size_t seat = 0; seat < stock.size(); ++seat)
            stock[seat] = triple.battle.seat_huds[seat].bombs;
        const auto check_no_bombs = [&] {
            for (size_t seat = 0; seat < stock.size(); ++seat) {
                CHECK_EQ(triple.battle.seat_huds[seat].bombs, stock[seat]);
                CHECK_EQ(triple.battle.seats[seat].bomb_timer, 0);
            }
            CHECK(!triple.battle.sim.bomb_active);
        };

        separate_seats(triple, 1);
        auto bullet = bullet_at(-120, 400);
        triple.battle.sim.bullet_player_collision(bullet);
        CHECK_EQ(triple.battle.seats[1].phase, Phase::Deathbomb);
        if (mode == Mode::Manual) {
            // 同帧 C 先于 X；发动者在决死窗内也不能扔雷。
            step(kBombKey, kCKey | kBombKey, kBombKey);
        } else {
            for (i32 frame = 0; frame < 20 && !triple.battle.team_trance_active(); ++frame)
                step(0, 0, 0);
        }
        CHECK(triple.battle.team_trance_active());
        CHECK_EQ(triple.battle.trance_mode, mode);
        CHECK_EQ(triple.battle.trance_owner_seat, 1);
        check_no_bombs();
        step(0, 0, 0);
        step(kBombKey, kBombKey, kBombKey);
        check_no_bombs();

        for (i32 frame = 0; frame < 20 && triple.battle.seats[1].phase == Phase::Deathbomb; ++frame)
            step(0, 0, 0);
        CHECK_EQ(triple.battle.seats[1].phase, Phase::Active);
        // 走正常的槽耗尽路径，再验证每个恢复可操作的席位能发动 Bomb。
        triple.progress.ending_points = 1;
        step(0, 0, 0);
        CHECK(!triple.battle.team_trance_active());
        if (mode == Mode::DeathProtection) {
            CHECK_EQ(triple.battle.seats[1].phase, Phase::Dying);
            for (i32 frame = 0; frame < 110; ++frame) step(0, 0, 0);
        }
        for (size_t seat = 0; seat < stock.size(); ++seat) {
            CHECK_EQ(triple.battle.seats[seat].phase, Phase::Active);
            stock[seat] = triple.battle.seat_huds[seat].bombs;
        }
        step(kBombKey, kBombKey, kBombKey);
        for (size_t seat = 0; seat < stock.size(); ++seat) {
            CHECK_EQ(triple.battle.seat_huds[seat].bombs, stock[seat] - 1);
            CHECK(triple.battle.seats[seat].bomb_timer > 0);
        }
    }
}

// ---- team_trance_shooting_four_roles：全队灵界射击 ----
// 灵界激活后每席的弹型块 = 10（trance 块）：开火后 shots 非空、弹型域
// 来自块 10（各角色 SHT 的 trance 弹型）。
TEST(team_trance_shooting_four_roles) {
    for (i32 role = 0; role < 4; ++role) {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {role, role, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        CHECK(dual.battle.team_trance_active());
        // 两席都开火（Z）：灵界弹型生效（shots 生成）。
        GameInput none;
        for (int frame = 0; frame < 5; ++frame) {
            GameInput seat0;
            seat0.update(0x01);
            dual.battle.seat_inputs[1].update(0x01);
            CHECK(dual.battle.step(seat0));
        }
        CHECK(!dual.battle.seats[0].shots.empty());
        CHECK(!dual.battle.seats[1].shots.empty());  // 全队灵界射击（非仅 owner）
    }
    // R-T2/R-T8: observe births on the actual activation frame, including
    // an expired collision window in a later seat. Post-activation firing
    // cannot detect a one-frame ordinary-shot leak from an earlier seat.
    const std::array<i32, 4> shot_counts{2, 4, 2, 1};
    const std::array<std::array<i32, 4>, 4> damages{{
        {{22, 22, 0, 0}}, {{22, 22, 11, 11}},
        {{22, 22, 0, 0}}, {{25, 0, 0, 0}}
    }}; // Independently read raw SHT block10, phase0.
    for (i32 role = 0; role < 4; ++role) for (i32 count : {2, 3})
        for (i32 hit_seat = 0; hit_seat < count; ++hit_seat)
            for (bool manual : {false, true}) for (bool transition : {false, true}) {
                if (manual && transition) continue; // Transition does not arbitrate C.
                Fixture activation;
                CHECK(activation.init());
                activation.enable(count, {role, role, role});
                activation.arm_trance();
                activation.progress.ending_points = manual ? 600 : 200;
                separate_seats(activation, hit_seat);
                auto hit = bullet_at(-120, 400);
                activation.battle.sim.bullet_player_collision(hit);
                CHECK_EQ(activation.battle.seats[size_t(hit_seat)].phase,
                         StagePlayerState::Phase::Deathbomb);
                for (i32 frame = 0; frame < 20 &&
                     activation.battle.seats[size_t(hit_seat)].phase_frame < 16; ++frame)
                    CHECK(activation.battle.step(activation.idle));
                CHECK_EQ(activation.battle.seats[size_t(hit_seat)].phase_frame, 16);
                CHECK(!activation.battle.team_trance_active());
                std::array<std::vector<PlayerShot>, 3> born;
                activation.battle.player_shot_birth = [&](PlayerShot& shot, u32) {
                    born[size_t(shot.owner_seat)].push_back(shot);
                    return true;
                };
                // A peer's Manual C wins before collision expiry is resolved.
                const i32 requester = (hit_seat + 1) % count;
                GameInput local;
                for (i32 seat = 0; seat < count; ++seat) {
                    const u32 keys = 1u | (manual && seat == requester ? kCKey : 0u);
                    if (seat == 0) local.update(keys);
                    else activation.battle.seat_inputs[size_t(seat)].update(keys);
                }
                CHECK(transition ? activation.battle.step_transition(local)
                                 : activation.battle.step(local));
                CHECK_EQ(activation.battle.trance_mode, manual
                    ? StageBattle::TranceMode::Manual : StageBattle::TranceMode::DeathProtection);
                CHECK_EQ(activation.battle.trance_owner_seat, manual ? requester : hit_seat);
                CHECK_EQ(activation.battle.seats[size_t(hit_seat)].phase,
                         StagePlayerState::Phase::Active);
                CHECK_EQ(activation.battle.seats[size_t(hit_seat)].phase_frame, 17);
                CHECK_EQ(activation.battle.seat_huds[size_t(hit_seat)].deaths, 0);
                CHECK_EQ(activation.progress.ending_points, manual ? 600 : 200);
                for (i32 seat = 0; seat < count; ++seat) {
                    const auto& row = born[size_t(seat)];
                    CHECK_EQ(row.size(), size_t(shot_counts[size_t(role)]));
                    for (size_t shot = 0; shot < row.size() &&
                         shot < size_t(shot_counts[size_t(role)]); ++shot) {
                        CHECK_EQ(row[shot].owner_seat, seat);
                        CHECK_EQ(row[shot].damage, float(damages[size_t(role)][shot]));
                        CHECK_EQ(row[shot].script, role == 0 ? 14 : 11);
                    }
                }
            }
}

// ---- protection_owner_only_death：保护灵界 owner 唯一死亡 ----
// 槽 200 时 seat 1 撞弹 → DeathProtection（owner=1）；槽耗尽 → 只有
// seat 1 死亡（seat 0 存活、不进死亡演出）。
TEST(protection_owner_only_death) {
    // R-T5: the observer window is not authoritative. Exercise every owner /
    // observer pairing on a final life, including owner == active observer.
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner)
        for (i32 observer = 0; observer < count; ++observer) {
            Fixture final;
            CHECK(final.init());
            final.enable(count, {0, 0, 0});
            final.progress.ending_points = 200;
            GameInput idle;
            for (i32 frame = 0; frame < 32; ++frame) CHECK(final.battle.step(idle));
            final.battle.active_seat = observer;
            for (i32 seat = 0; seat < count; ++seat) {
                final.battle.seats[size_t(seat)].x = seat == owner ? -120.f : 120.f;
                final.battle.seats[size_t(seat)].fixed_x = i32(final.battle.seats[size_t(seat)].x * 128);
                final.battle.seats[size_t(seat)].invulnerable = 0;
            }
            final.battle.seat_huds[size_t(owner)].extra_lives = 0;
            StageBullet hit;
            hit.alive = true; hit.collision_ready = true; hit.flags = 0x12u;
            hit.hit_size[0] = hit.hit_size[1] = 8;
            hit.x = -120; hit.y = 400;
            final.battle.sim.bullet_player_collision(hit);
            CHECK_EQ(final.battle.seats[size_t(owner)].phase, StagePlayerState::Phase::Deathbomb);
            for (i32 frame = 0; frame < 20 && !final.battle.team_trance_active(); ++frame)
                CHECK(final.battle.step(idle));
            CHECK(final.battle.team_trance_active());
            CHECK_EQ(final.battle.trance_owner_seat, owner);
            for (i32 frame = 0; frame < 210 && final.battle.team_trance_active(); ++frame)
                CHECK(final.battle.step(idle));
            std::printf("# owner-observer count=%d owner=%d observer=%d phase=%d lives=%d deaths=%d\n",
                count, owner, observer, i32(final.battle.seats[size_t(owner)].phase),
                final.battle.seat_huds[size_t(owner)].extra_lives,
                final.battle.seat_huds[size_t(owner)].deaths);
            CHECK(!final.battle.team_trance_active());
            CHECK_EQ(final.battle.seats[size_t(owner)].phase, StagePlayerState::Phase::Dying);
            CHECK_EQ(final.battle.seat_huds[size_t(owner)].extra_lives, -1);
            CHECK_EQ(final.battle.seat_huds[size_t(owner)].deaths, 1);
            for (i32 frame = 0; frame < 35; ++frame) CHECK(final.battle.step(idle));
            CHECK(final.battle.seats[size_t(owner)].ghost);
            CHECK(final.battle.seats[size_t(owner)].dead);
            CHECK_EQ(final.battle.seat_huds[size_t(owner)].deaths, 1);
            for (i32 seat = 0; seat < count; ++seat) if (seat != owner) {
                CHECK_EQ(final.battle.seats[size_t(seat)].phase, StagePlayerState::Phase::Active);
                CHECK_EQ(final.battle.seat_huds[size_t(seat)].extra_lives, 2);
                CHECK_EQ(final.battle.seat_huds[size_t(seat)].deaths, 0);
                CHECK(!final.battle.seats[size_t(seat)].ghost);
            }
        }
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    // 槽设 200（保护灵界可开不可续）；走步进建立任务计时。
    dual.progress.ending_points = 200;
    GameInput none;
    for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
    // seat 0 移开（生产数据流：左移 60 帧 ≈ 270px，远离撞弹点）；
    // seat 1 清无敌撞弹。弹只压 seat 1（一弹消一次：seat 0 不在弹道）。
    GameInput left;
    for (int frame = 0; frame < 60; ++frame) {
        left.update(0x40);
        CHECK(dual.battle.step(left));
    }
    left.update(0x00);
    CHECK(std::fabs(dual.battle.seats[0].x) > 150.0f);  // seat 0 已离场（钳制边界 182）
    dual.battle.seats[1].invulnerable = 0;
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = dual.battle.seats[1].x;
    bullet.y = dual.battle.seats[1].y;
    const i32 lives0 = dual.battle.seat_huds[0].extra_lives;
    const i32 lives1 = dual.battle.seat_huds[1].extra_lives;
    dual.battle.sim.bullet_player_collision(bullet);  // MP 逐席：seat 1 命中
    // R-G doubles the retail eight-frame deathbomb window to sixteen.
    // Observe its actual expiration before checking protection ownership.
    GameInput none2;
    for (int frame = 0; frame < 20 &&
         dual.battle.seats[1].phase == StagePlayerState::Phase::Deathbomb; ++frame)
        CHECK(dual.battle.step(none2));
    CHECK(dual.battle.team_trance_active());
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
    CHECK_EQ(dual.battle.trance_owner_seat, 1);  // 撞弹者 = owner
    CHECK_EQ(dual.battle.trance_generation, 1u);
    // Protection begins with 200 and then consumes once per logical frame.
    for (int frame = 0; frame < 203; ++frame) CHECK(dual.battle.step(none));
    CHECK(!dual.battle.team_trance_active());
    // owner（seat 1）进入死亡演出（phase Dying/Deathbomb 或已扣命）；
    // seat 0 存活且不扣命。
    const auto& s0 = dual.battle.seats[0];
    const auto& s1 = dual.battle.seats[1];
    CHECK_EQ(dual.battle.seat_huds[0].extra_lives, lives0);  // 队友不扣命
    CHECK(s1.phase == StagePlayerState::Phase::Dying ||
          s1.phase == StagePlayerState::Phase::Deathbomb ||
          dual.battle.seat_huds[1].extra_lives < lives1);   // owner 死亡责任
    CHECK(s0.phase == StagePlayerState::Phase::Active);     // 队友不进死亡演出
}

// ---- all_collision_invulnerability：灵界期间全席免疫三类受击 ----
TEST(all_collision_invulnerability) {
    for (i32 character = 0; character < 4; ++character) {
        Fixture startup;
        CHECK(startup.init()); startup.enable(3, {0, 0, character}); startup.arm_trance();
        startup.battle.seat_inputs[2].update(2);
        CHECK(startup.battle.step(GameInput{}));
        CHECK_EQ(startup.battle.seats[2].bomb_protection, character == 0 ? 40 : 120);
        startup.battle.seats[0].invulnerable = 0;
        startup.battle.seats[1].invulnerable = 0;
        for (i32 target : {0, 1}) {
            separate_seats(startup, target);
            auto hit = bullet_at(-120, 400);
            startup.battle.sim.bullet_player_collision(hit);
            CHECK_EQ(startup.battle.seats[size_t(target)].phase, StagePlayerState::Phase::Active);
        }
    }
    // Overlapping sources survive either owner's termination and round-trip
    // independently through the rollback snapshot and digest.
    {
        Fixture overlap;
        CHECK(overlap.init()); overlap.enable(2, {0, 0, 0}); overlap.arm_trance();
        GameInput both; both.update(2);
        overlap.battle.seat_inputs[1].update(2);
        CHECK(overlap.battle.step(both));
        CHECK(overlap.battle.seats[0].bomb_protection > 0);
        CHECK(overlap.battle.seats[1].bomb_protection > 0);
        GameRandom visual(4);
        multiplayer::WorldState saved, changed, restored;
        CHECK(saved.capture(overlap.battle, visual));
        overlap.battle.seats[0].bomb_timer = 0;
        overlap.battle.seats[0].bomb_protection = 0;
        CHECK(changed.capture(overlap.battle, visual));
        CHECK(saved.digest_core() != changed.digest_core());
        CHECK(overlap.battle.seats[1].bomb_protection > 0);
        CHECK(saved.restore(overlap.battle, visual));
        CHECK(restored.capture(overlap.battle, visual));
        CHECK_EQ(restored.digest(), saved.digest());
    }
    {
        Fixture staggered;
        CHECK(staggered.init()); staggered.enable(2, {0, 0, 0}); staggered.arm_trance();
        GameInput first; first.update(2);
        CHECK(staggered.battle.step(first));
        for (int frame = 0; frame < 100; ++frame) CHECK(staggered.battle.step(GameInput{}));
        staggered.battle.seat_inputs[1].update(2);
        CHECK(staggered.battle.step(GameInput{}));
        staggered.battle.seat_inputs[1].update(0);
        for (int frame = 0; frame < 140; ++frame) CHECK(staggered.battle.step(GameInput{}));
        CHECK_EQ(staggered.battle.seats[0].bomb_protection, 0);
        CHECK(staggered.battle.seats[1].bomb_protection > 0);
        separate_seats(staggered, 0);
        auto hit = bullet_at(-120, 400);
        staggered.battle.sim.bullet_player_collision(hit);
        CHECK_EQ(staggered.battle.seats[0].phase, StagePlayerState::Phase::Active);
        staggered.battle.seats[1].bomb_protection = 0;
        auto unprotected = bullet_at(-120, 400);
        staggered.battle.sim.bullet_player_collision(unprotected);
        CHECK_EQ(staggered.battle.seats[0].phase, StagePlayerState::Phase::Deathbomb);
    }
    // Bomb sources apply to every seat after the complete player pass. Test
    // both seat orders, all collision consumers, startup, active and tail;
    // clearing the sole source is the matching vulnerable negative control.
    for (i32 owner : {0, 1}) for (i32 kind : {0, 1, 2})
        for (i32 when : {0, 20, 220, 240}) {
        Fixture bomb;
        CHECK(bomb.init());
        bomb.enable(2, {0, 0, 0});
        bomb.arm_trance();
        bomb.progress.ending_points = 0;
        separate_seats(bomb, 1 - owner);
        bomb.battle.seats[size_t(owner)].invulnerable = 500;
        GameInput local;
        if (owner == 0) local.update(2);
        else bomb.battle.seat_inputs[1].update(2);
        CHECK(bomb.battle.step(local));
        CHECK(bomb.battle.seats[size_t(owner)].invulnerable >= 499);
        bomb.battle.seat_inputs[1].update(0);
        for (i32 frame = 0; frame < when; ++frame) CHECK(bomb.battle.step(GameInput{}));
        const i32 target = 1 - owner;
        bomb.battle.seats[size_t(target)].invulnerable = 0;
        const bool protected_now = bomb.battle.seats[size_t(owner)].bomb_protection > 0;
        CHECK_EQ(protected_now, when < 240);
        if (kind == 0) {
            auto hit = bullet_at(-120, 400);
            bomb.battle.sim.bullet_player_collision(hit);
        } else if (kind == 1) {
            StageLaser laser;
            laser.origin[0] = -168; laser.origin[1] = 400;
            laser.length = laser.target_length = 96; laser.width = 20; laser.phase = 2;
            bomb.battle.sim.laser_collision(laser);
        } else {
            StageEnemy enemy;
            enemy.position[0] = -120; enemy.position[1] = 400;
            enemy.hitbox[0] = enemy.hitbox[1] = 24;
            bomb.battle.sim.enemy_body_collision(enemy);
        }
        CHECK_EQ(bomb.battle.seats[size_t(target)].phase, protected_now
            ? StagePlayerState::Phase::Active : StagePlayerState::Phase::Deathbomb);
    }
    // R-T2: the production shared-world callbacks must traverse every seat.
    // Non-trance controls prove these exact laser/body geometries really hit.
    for (bool trance : {false, true}) for (i32 target = 0; target < 3; ++target)
        for (bool laser_hit : {false, true}) {
            Fixture collision;
            CHECK(collision.init());
            collision.enable(3, {0, 1, 3});
            collision.arm_trance();
            if (trance) {
                GameInput c;
                c.update(kCKey);
                CHECK(collision.battle.step(c));
            }
            separate_seats(collision, target);
            // Synchronize the observation window via the normal frame path.
            CHECK(collision.battle.step(GameInput{}));
            if (laser_hit) {
                StageLaser laser;
                laser.origin[0] = -168;
                laser.origin[1] = 400;
                laser.length = laser.target_length = 96;
                laser.width = 20;
                laser.phase = 2;
                CHECK_EQ(laser.player_contact(-120, 400, 1, 1), 1);
                collision.battle.sim.laser_collision(laser);
            } else {
                StageEnemy enemy;
                enemy.position[0] = -120;
                enemy.position[1] = 400;
                enemy.hitbox[0] = enemy.hitbox[1] = 24;
                collision.battle.sim.enemy_body_collision(enemy);
            }
            std::printf("# collision trance=%d target=%d laser=%d phase=%d\n",
                trance, target, laser_hit, i32(collision.battle.seats[size_t(target)].phase));
            CHECK_EQ(collision.battle.seats[size_t(target)].phase,
                trance ? StagePlayerState::Phase::Active : StagePlayerState::Phase::Deathbomb);
            for (i32 seat = 0; seat < 3; ++seat) if (seat != target)
                CHECK_EQ(collision.battle.seats[size_t(seat)].phase, StagePlayerState::Phase::Active);
        }
    // The manager must split at every actual hit location, even when seat0
    // (the legacy sim.player_x/y) is nowhere near the laser. A second hit
    // must also cut the fragment created by the first hit in this traversal.
    for (i32 hits : {1, 2}) {
        Fixture manager;
        CHECK(manager.init());
        manager.enable(3, {0, 1, 3});
        GameInput idle;
        for (i32 frame = 0; frame < 32; ++frame) CHECK(manager.battle.step(idle));
        const std::array<float, 3> xs = hits == 1
            ? std::array<float, 3>{160, -120, 160}
            : std::array<float, 3>{160, -120, 40};
        for (i32 seat = 0; seat < 3; ++seat) {
            auto& state = manager.battle.seats[size_t(seat)];
            state.x = xs[size_t(seat)];
            state.fixed_x = i32(state.x * 128);
            state.invulnerable = 0;
        }
        CHECK(manager.battle.step(idle));
        CHECK_EQ(manager.battle.sim.player_x, 160.f);
        StageLaser laser;
        laser.phase = 2;
        laser.origin[0] = -168;
        laser.origin[1] = 400;
        laser.length = laser.target_length = hits == 1 ? 96.f : 288.f;
        laser.width = laser.target_width = 20;
        laser.timing[2] = 100;
        manager.battle.sim.append_laser(laser);
        manager.battle.sim.advance_lasers();
        CHECK(manager.battle.sim.error().empty());
        CHECK_EQ(manager.battle.seats[1].phase, StagePlayerState::Phase::Deathbomb);
        CHECK_EQ(manager.battle.seats[0].phase, StagePlayerState::Phase::Active);
        CHECK_EQ(manager.battle.seats[2].phase, hits == 1
            ? StagePlayerState::Phase::Active : StagePlayerState::Phase::Deathbomb);
        const auto& pieces = manager.battle.sim.lasers;
        std::printf("# manager laser hits=%d pieces=%zu\n", hits, pieces.size());
        CHECK_EQ(pieces.size(), size_t(hits + 1));
        if (pieces.size() != size_t(hits + 1)) continue;
        CHECK_EQ(pieces[0].origin[0], -168.f);
        CHECK_EQ(pieces[0].length, 32.f);
        CHECK_EQ(pieces[1].origin[0], -104.f);
        CHECK_EQ(pieces[1].length, hits == 1 ? 32.f : 128.f);
        if (hits == 2) {
            CHECK_EQ(pieces[2].origin[0], 56.f);
            CHECK_EQ(pieces[2].length, 64.f);
        }
        CHECK_EQ(manager.battle.sim.player_x, 160.f);
    }
    // Ending a team trance cannot shorten preexisting personal protection.
    for (i32 owner : {0, 1}) {
        Fixture protection;
        CHECK(protection.init());
        protection.enable(2, {0, 0, 0});
        protection.arm_trance();
        GameInput local;
        if (owner == 0) local.update(kCKey);
        else protection.battle.seat_inputs[1].update(kCKey);
        CHECK(protection.battle.step(local));
        CHECK_EQ(protection.battle.trance_owner_seat, owner);
        protection.battle.seats[0].invulnerable = 500;
        protection.battle.seats[1].invulnerable = 600;
        CHECK(protection.battle.end_trance());
        CHECK_EQ(protection.battle.seats[0].invulnerable, 500);
        CHECK_EQ(protection.battle.seats[1].invulnerable, 600);
    }
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    dual.arm_trance();
    GameInput trigger;
    trigger.update(kCKey);
    CHECK(dual.battle.step(trigger));
    CHECK(dual.battle.team_trance_active());
    // 两席清个人无敌（灵界保护 ≠ 个人无敌）。
    dual.battle.seats[0].invulnerable = 0;
    dual.battle.seats[1].invulnerable = 0;
    // 弹直接压在两席上：不 kill（团队灵界免疫）。
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = dual.battle.seats[0].x;
    bullet.y = dual.battle.seats[0].y;
    dual.battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);
    CHECK_EQ(dual.battle.seats[1].phase, StagePlayerState::Phase::Active);
}

// ---- deathbomb_manual_c：决死窗内 C 开启记 Manual ----
// 灵界已激活时撞弹（早退出口保命）+ 槽耗尽正常 -200 不触发死亡。
TEST(deathbomb_manual_c) {
    // The MP preparation pass resolves phase16 only. Phase15 remains the
    // last valid Bomb input opportunity; expiry at16 keeps the old ordering.
    for (i32 owner : {0, 1}) for (i32 response_frame : {15, 16})
        for (bool transition : {false, true}) {
        Fixture bomb_window;
        CHECK(bomb_window.init());
        bomb_window.enable(2, {0, 0, 0});
        bomb_window.arm_trance();
        bomb_window.progress.ending_points = 200;
        separate_seats(bomb_window, owner);
        auto hit = bullet_at(-120, 400);
        bomb_window.battle.sim.bullet_player_collision(hit);
        for (i32 frame = 0; frame < 20 &&
             bomb_window.battle.seats[size_t(owner)].phase_frame < response_frame; ++frame)
            CHECK(bomb_window.battle.step(bomb_window.idle));
        CHECK_EQ(bomb_window.battle.seats[size_t(owner)].phase_frame, response_frame);
        CHECK_EQ(bomb_window.battle.seats[size_t(owner)].phase,
                 StagePlayerState::Phase::Deathbomb);
        const i32 bombs = bomb_window.battle.seat_huds[size_t(owner)].bombs;
        GameInput local;
        if (owner == 0) local.update(2);
        else bomb_window.battle.seat_inputs[1].update(2);
        CHECK(transition ? bomb_window.battle.step_transition(local)
                         : bomb_window.battle.step(local));
        CHECK_EQ(bomb_window.battle.seats[size_t(owner)].phase,
                 StagePlayerState::Phase::Active);
        CHECK_EQ(bomb_window.battle.seat_huds[size_t(owner)].deaths, 0);
        CHECK_EQ(bomb_window.battle.team_trance_active(), response_frame == 16);
        CHECK_EQ(bomb_window.battle.seat_huds[size_t(owner)].bombs,
                 bombs - (response_frame == 15 ? 1 : 0));
        CHECK_EQ(bomb_window.battle.seats[size_t(owner)].bomb_timer > 0,
                 response_frame == 15);
    }
    // R-T10: enter the genuine collision window first, then press C. Manual
    // is preserved through expiry, and an early normal end subtracts 200 only.
    for (i32 owner : {0, 1}) {
        Fixture window;
        CHECK(window.init());
        window.enable(2, {0, 0, 0});
        window.arm_trance();
        separate_seats(window, owner);
        auto hit = bullet_at(-120, 400);
        window.battle.sim.bullet_player_collision(hit);
        CHECK_EQ(window.battle.seats[size_t(owner)].phase, StagePlayerState::Phase::Deathbomb);
        GameInput local;
        if (owner == 0) local.update(kCKey);
        else window.battle.seat_inputs[1].update(kCKey);
        CHECK(window.battle.step(local));
        CHECK_EQ(window.battle.trance_mode, StageBattle::TranceMode::Manual);
        CHECK_EQ(window.battle.trance_owner_seat, owner);
        GameInput idle;
        for (i32 frame = 0; frame < 20; ++frame) {
            window.battle.seat_inputs[1].update(0);
            CHECK(window.battle.step(idle));
        }
        CHECK_EQ(window.battle.seats[size_t(owner)].phase, StagePlayerState::Phase::Active);
        CHECK_EQ(window.battle.seat_huds[size_t(owner)].deaths, 0);
        const i32 before = window.progress.ending_points;
        CHECK(window.battle.end_trance());
        CHECK_EQ(window.progress.ending_points, before - 200);
        CHECK_EQ(window.battle.seat_huds[size_t(owner)].deaths, 0);
        CHECK_EQ(window.battle.seat_huds[size_t(owner)].extra_lives, 2);
    }
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    dual.arm_trance();
    GameInput trigger;
    trigger.update(kCKey);
    CHECK(dual.battle.step(trigger));
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
    // 灵界中 seat 0 撞弹：团队无敌救下，不改 mode/owner（R-T8）。
    dual.battle.seats[0].invulnerable = 0;
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = dual.battle.seats[0].x;
    bullet.y = dual.battle.seats[0].y;
    dual.battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK_EQ(dual.battle.trance_owner_seat, 0);
    CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);
}

// ---- generation_respawn_pause_stage：代数防串 + 暂停不耗槽 ----
TEST(generation_respawn_pause_stage) {
    // Transition advances players but neither accepts Manual C nor drains
    // an already active team gauge, including a real protection activation.
    for (i32 owner : {0, 1, 2}) {
        Fixture transition;
        CHECK(transition.init());
        transition.enable(3, {0, 0, 0});
        transition.arm_trance();
        GameInput local;
        if (owner == 0) local.update(kCKey);
        else transition.battle.seat_inputs[size_t(owner)].update(kCKey);
        CHECK(transition.battle.step_transition(local));
        CHECK(!transition.battle.team_trance_active());
        CHECK_EQ(transition.progress.ending_points, 600);
        separate_seats(transition, owner);
        transition.progress.ending_points = 200;
        auto hit = bullet_at(-120, 400);
        transition.battle.sim.bullet_player_collision(hit);
        for (i32 frame = 0; frame < 20 &&
             !transition.battle.team_trance_active(); ++frame)
            CHECK(transition.battle.step_transition(GameInput{}));
        CHECK_EQ(transition.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
        CHECK_EQ(transition.battle.trance_owner_seat, owner);
        CHECK_EQ(transition.battle.seats[size_t(owner)].phase,
                 StagePlayerState::Phase::Active);
        CHECK_EQ(transition.battle.seats[size_t(owner)].phase_frame, 17);
        CHECK_EQ(transition.progress.ending_points, 200);
        for (i32 frame = 0; frame < 5; ++frame)
            CHECK(transition.battle.step_transition(GameInput{}));
        CHECK_EQ(transition.progress.ending_points, 200);
        CHECK_EQ(transition.battle.seat_huds[size_t(owner)].deaths, 0);
    }
    // ① 代数单元：两次激活 generation 递增；owner 重生只清当前代
    //（R-T9 的 generation 检查面——重生清槽路径在 update_player 第 30
    // 帧，M4 记录代数；清槽语义由 SP 路径保留）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        const u32 gen1 = dual.battle.trance_generation;
        // 槽耗尽 → 结束。
        GameInput none;
        for (int frame = 0; frame < 700; ++frame) CHECK(dual.battle.step(none));
        CHECK(!dual.battle.team_trance_active());
        // 再充再开：generation 递增（新代）。
        dual.progress.ending_points = 600;
        for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
        GameInput trigger2;
        trigger2.update(kCKey);
        CHECK(dual.battle.step(trigger2));
        CHECK_EQ(dual.battle.trance_generation, gen1 + 1);
    }
    // ①-2 R-T9 跨代守卫（负控）：owner 保护灵界耗尽死亡的演出期间，
    // 队友撞弹开启的新一代保护灵界不得被 owner 重生第 30 帧的清槽路径
    // 清掉——否则新代槽瞬间归零、新 owner 被错误判定死亡。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.progress.ending_points = 200;
        GameInput none;
        for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
        GameInput left;
        for (int frame = 0; frame < 60; ++frame) {
            left.update(0x40);
            CHECK(dual.battle.step(left));
        }
        left.update(0x00);
        dual.battle.seats[1].invulnerable = 0;
        StageBullet bullet;
        bullet.alive = true;
        bullet.collision_ready = true;
        bullet.flags = 0x12u;
        bullet.hit_size[0] = 8.0f;
        bullet.hit_size[1] = 8.0f;
        bullet.x = dual.battle.seats[1].x;
        bullet.y = dual.battle.seats[1].y;
        dual.battle.sim.bullet_player_collision(bullet);
        for (int frame = 0; frame < 20 &&
             dual.battle.seats[1].phase == StagePlayerState::Phase::Deathbomb; ++frame)
            CHECK(dual.battle.step(none));
        CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
        // 槽耗尽：owner（seat 1）进入死亡演出。
        for (int frame = 0; frame < 203; ++frame) CHECK(dual.battle.step(none));
        CHECK(!dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.seats[1].phase, StagePlayerState::Phase::Dying);
        // 演出期间充槽（神霊拾取值域）+ seat 0 撞弹 → 新一代保护灵界。
        dual.progress.ending_points = 600;
        dual.battle.seats[0].invulnerable = 0;
        StageBullet second;
        second.alive = true;
        second.collision_ready = true;
        second.flags = 0x12u;
        second.hit_size[0] = 8.0f;
        second.hit_size[1] = 8.0f;
        second.x = dual.battle.seats[0].x;
        second.y = dual.battle.seats[0].y;
        dual.battle.sim.bullet_player_collision(second);
        for (int frame = 0; frame < 20 &&
             dual.battle.seats[0].phase == StagePlayerState::Phase::Deathbomb; ++frame)
            CHECK(dual.battle.step(none));
        CHECK(dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.trance_generation, 2u);
        CHECK_EQ(dual.battle.trance_owner_seat, 0);
        const i32 slot = dual.progress.ending_points;
        // 推进越过 seat 1 重生第 30 帧清槽点：新代槽只按每帧 −1 消耗。
        for (int frame = 0; frame < 30; ++frame) CHECK(dual.battle.step(none));
        CHECK(dual.battle.team_trance_active());  // 新代未被旧重生清槽打断
        CHECK_EQ(dual.battle.trance_generation, 2u);
        CHECK(dual.progress.ending_points > 0);
        CHECK(dual.progress.ending_points < slot);  // 仍在正常消耗（非清零）
        CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);  // 新 owner 未被误判死亡
    }
    // ② 暂停不耗槽：灵界中 defer 模式（step_transition 不走 update_trance
    // 扣槽——转场帧保持槽值）。
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        const i32 slot = dual.progress.ending_points;
        // 转场帧（step_transition）不扣槽（暂停语义：不推进灵界计时）。
        for (int frame = 0; frame < 5; ++frame)
            CHECK(dual.battle.step_transition(GameInput{}));
        CHECK_EQ(dual.progress.ending_points, slot);
    }
}

int main() { return th13test::run_all("m4.trance"); }
