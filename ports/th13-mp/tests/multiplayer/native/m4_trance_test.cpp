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

}

TEST(gauge_once_thresholds) {
    {
        SessionState state;
        state.ending_points = 599;
        CHECK(!trance_ready(state));
        state.ending_points = 600;
        CHECK(trance_ready(state));
        trance_begin(state);
        CHECK(!trance_ready(state));
        trance_end(state, false);
        CHECK_EQ(state.ending_points, 400);
        trance_add(state, 250);
        CHECK_EQ(state.ending_points, 600);
        trance_add(state, 100);
        CHECK_EQ(state.ending_points, 600);
        state.ending_points = 199;
        trance_end(state, false);
        CHECK_EQ(state.ending_points, 0);
    }
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        CHECK(dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::Manual);
        CHECK_EQ(dual.battle.trance_owner_seat, 0);
        const i32 slot_at_start = dual.progress.ending_points;
        GameInput none;
        for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none));
        CHECK_EQ(dual.progress.ending_points, slot_at_start - 10);
    }
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
            CHECK_EQ(big, 0);
            CHECK_EQ(boundary.battle.seat_huds[1].power, 300);
            for (i32 frame = 0; frame < 110; ++frame) CHECK(boundary.battle.step(idle));
            CHECK_EQ(boundary.battle.seats[1].phase, StagePlayerState::Phase::Active);
            CHECK(!boundary.battle.seats[1].ghost);
            CHECK_EQ(boundary.battle.seat_huds[1].deaths, 1);
            CHECK_EQ(boundary.battle.seat_huds[0].deaths, 0);
        }
    }
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
        CHECK(spirits.battle.end_trance());
        CHECK_EQ(old->frame, old_frame);
        CHECK_EQ(born->frame, born_frame);
        CHECK(spirits.battle.step(idle));
        CHECK_EQ(old->frame, old_frame + 1);
        CHECK_EQ(born->frame, born_frame + 1);
    }
}

TEST(simultaneous_requests) {
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    triple.arm_trance();
    GameInput seat0;
    seat0.update(kCKey);
    triple.battle.seat_inputs[1].update(kCKey);
    triple.battle.seat_inputs[2].update(kCKey);
    CHECK(triple.battle.step(seat0));
    CHECK(triple.battle.team_trance_active());
    CHECK_EQ(triple.battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK_EQ(triple.battle.trance_owner_seat, 0);
    CHECK_EQ(triple.battle.trance_generation, 1u);
    Fixture triple2;
    CHECK(triple2.init());
    triple2.enable(3, {0, 0, 0});
    triple2.arm_trance();
    triple2.battle.seats[0].bomb_timer = 1;
    GameInput seat0b;
    seat0b.update(kCKey);
    triple2.battle.seat_inputs[1].update(kCKey);
    CHECK(triple2.battle.step(seat0b));
    CHECK(triple2.battle.team_trance_active());
    CHECK_EQ(triple2.battle.trance_owner_seat, 1);
}

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
        GameInput none;
        for (int frame = 0; frame < 5; ++frame) {
            GameInput seat0;
            seat0.update(0x01);
            dual.battle.seat_inputs[1].update(0x01);
            CHECK(dual.battle.step(seat0));
        }
        CHECK(!dual.battle.seats[0].shots.empty());
        CHECK(!dual.battle.seats[1].shots.empty());
    }
    const std::array<i32, 4> shot_counts{2, 4, 2, 1};
    const std::array<std::array<i32, 4>, 4> damages{{
        {{22, 22, 0, 0}}, {{22, 22, 11, 11}},
        {{22, 22, 0, 0}}, {{25, 0, 0, 0}}
    }};
    for (i32 role = 0; role < 4; ++role) for (i32 count : {2, 3})
        for (i32 hit_seat = 0; hit_seat < count; ++hit_seat)
            for (bool manual : {false, true}) for (bool transition : {false, true}) {
                if (manual && transition) continue;
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

TEST(protection_owner_only_death) {
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
    dual.progress.ending_points = 200;
    GameInput none;
    for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
    GameInput left;
    for (int frame = 0; frame < 60; ++frame) {
        left.update(0x40);
        CHECK(dual.battle.step(left));
    }
    left.update(0x00);
    CHECK(std::fabs(dual.battle.seats[0].x) > 150.0f);
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
    dual.battle.sim.bullet_player_collision(bullet);
    GameInput none2;
    for (int frame = 0; frame < 20 &&
         dual.battle.seats[1].phase == StagePlayerState::Phase::Deathbomb; ++frame)
        CHECK(dual.battle.step(none2));
    CHECK(dual.battle.team_trance_active());
    CHECK_EQ(dual.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
    CHECK_EQ(dual.battle.trance_owner_seat, 1);
    CHECK_EQ(dual.battle.trance_generation, 1u);
    for (int frame = 0; frame < 203; ++frame) CHECK(dual.battle.step(none));
    CHECK(!dual.battle.team_trance_active());
    const auto& s0 = dual.battle.seats[0];
    const auto& s1 = dual.battle.seats[1];
    CHECK_EQ(dual.battle.seat_huds[0].extra_lives, lives0);
    CHECK(s1.phase == StagePlayerState::Phase::Dying ||
          s1.phase == StagePlayerState::Phase::Deathbomb ||
          dual.battle.seat_huds[1].extra_lives < lives1);
    CHECK(s0.phase == StagePlayerState::Phase::Active);
}

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
    dual.battle.seats[0].invulnerable = 0;
    dual.battle.seats[1].invulnerable = 0;
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

TEST(deathbomb_manual_c) {
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

TEST(generation_respawn_pause_stage) {
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
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        const u32 gen1 = dual.battle.trance_generation;
        GameInput none;
        for (int frame = 0; frame < 700; ++frame) CHECK(dual.battle.step(none));
        CHECK(!dual.battle.team_trance_active());
        dual.progress.ending_points = 600;
        for (int frame = 0; frame < 32; ++frame) CHECK(dual.battle.step(none));
        GameInput trigger2;
        trigger2.update(kCKey);
        CHECK(dual.battle.step(trigger2));
        CHECK_EQ(dual.battle.trance_generation, gen1 + 1);
    }
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
        for (int frame = 0; frame < 203; ++frame) CHECK(dual.battle.step(none));
        CHECK(!dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.seats[1].phase, StagePlayerState::Phase::Dying);
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
        for (int frame = 0; frame < 30; ++frame) CHECK(dual.battle.step(none));
        CHECK(dual.battle.team_trance_active());
        CHECK_EQ(dual.battle.trance_generation, 2u);
        CHECK(dual.progress.ending_points > 0);
        CHECK(dual.progress.ending_points < slot);
        CHECK_EQ(dual.battle.seats[0].phase, StagePlayerState::Phase::Active);
    }
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        dual.arm_trance();
        GameInput trigger;
        trigger.update(kCKey);
        CHECK(dual.battle.step(trigger));
        const i32 slot = dual.progress.ending_points;
        for (int frame = 0; frame < 5; ++frame)
            CHECK(dual.battle.step_transition(GameInput{}));
        CHECK_EQ(dual.progress.ending_points, slot);
    }
}

TEST(trance_exit_protection) {
    constexpr i32 kExitFrames = 70;
    const std::array<i32, 3> roles = {0, 1, 3};
    const auto place = [](Fixture& f, i32 seat) {
        auto& state = f.battle.seats[size_t(seat)];
        state.x = -150.f + 150.f * float(seat);
        state.fixed_x = i32(state.x * 128);
        state.y = 400;
        state.fixed_y = 51200;
    };
    const auto activate = [](Fixture& f, i32 owner) {
        GameInput local;
        if (owner == 0) local.update(kCKey);
        else f.battle.seat_inputs[size_t(owner)].update(kCKey);
        CHECK(f.battle.step(local));
        CHECK(f.battle.team_trance_active());
        CHECK_EQ(f.battle.trance_owner_seat, owner);
        for (i32 seat = 0; seat < f.battle.seat_count; ++seat) {
            f.battle.seat_inputs[size_t(seat)].update(0);
            f.battle.seats[size_t(seat)].invulnerable = 0;
        }
    };
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner) {
        Fixture f;
        CHECK(f.init());
        f.enable(count, roles);
        f.arm_trance();
        for (i32 seat = 0; seat < count; ++seat) place(f, seat);
        activate(f, owner);
        GameInput none;
        i32 guard = 0;
        while (f.battle.team_trance_active() && guard++ < 700) CHECK(f.battle.step(none));
        CHECK(!f.battle.team_trance_active());
        for (i32 seat = 0; seat < count; ++seat)
            CHECK_EQ(f.battle.seats[size_t(seat)].invulnerable, kExitFrames - 1);
        for (i32 frame = 0; frame < kExitFrames - 2; ++frame) {
            for (i32 seat = 0; seat < count; ++seat) {
                auto hit = bullet_at(f.battle.seats[size_t(seat)].x, f.battle.seats[size_t(seat)].y);
                f.battle.sim.bullet_player_collision(hit);
                CHECK(f.battle.seats[size_t(seat)].phase == StagePlayerState::Phase::Active);
            }
            CHECK(f.battle.step(none));
        }
        for (i32 frame = 0; frame < 3; ++frame) CHECK(f.battle.step(none));
        for (i32 seat = 0; seat < count; ++seat) {
            CHECK_EQ(f.battle.seats[size_t(seat)].invulnerable, 0);
            auto hit = bullet_at(f.battle.seats[size_t(seat)].x, f.battle.seats[size_t(seat)].y);
            f.battle.sim.bullet_player_collision(hit);
            CHECK(f.battle.seats[size_t(seat)].phase == StagePlayerState::Phase::Deathbomb);
        }
    }
    {
        Fixture f;
        CHECK(f.init());
        f.enable(3, roles);
        f.arm_trance();
        activate(f, 1);
        f.battle.seats[0].invulnerable = 10;
        f.battle.seats[1].invulnerable = 500;
        f.battle.seats[2].ghost = true;
        f.battle.seats[2].invulnerable = 0;
        CHECK(f.battle.end_trance());
        CHECK_EQ(f.battle.seats[0].invulnerable, kExitFrames);
        CHECK_EQ(f.battle.seats[1].invulnerable, 500);
        CHECK_EQ(f.battle.seats[2].invulnerable, 0);
        GameInput none;
        CHECK(f.battle.step(none));
        CHECK_EQ(f.battle.seats[0].invulnerable, kExitFrames - 1);
        CHECK_EQ(f.battle.seats[1].invulnerable, 499);
    }
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner) {
        Fixture f;
        CHECK(f.init());
        f.enable(count, roles);
        GameInput none;
        for (i32 frame = 0; frame < 32; ++frame) CHECK(f.battle.step(none));
        separate_seats(f, owner);
        for (i32 seat = 0; seat < count; ++seat) place(f, seat);
        f.progress.ending_points = 200;
        auto hit = bullet_at(f.battle.seats[size_t(owner)].x, f.battle.seats[size_t(owner)].y);
        f.battle.sim.bullet_player_collision(hit);
        for (i32 frame = 0; frame < 20 && !f.battle.team_trance_active(); ++frame)
            CHECK(f.battle.step(none));
        CHECK_EQ(f.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
        for (i32 seat = 0; seat < count; ++seat) f.battle.seats[size_t(seat)].invulnerable = 0;
        i32 guard = 0;
        while (f.battle.team_trance_active() && guard++ < 300) CHECK(f.battle.step(none));
        CHECK(!f.battle.team_trance_active());
        CHECK(f.battle.seats[size_t(owner)].phase == StagePlayerState::Phase::Dying);
        for (i32 seat = 0; seat < count; ++seat) {
            if (seat == owner) CHECK(f.battle.seats[size_t(seat)].invulnerable >= kExitFrames - 1);
            else CHECK_EQ(f.battle.seats[size_t(seat)].invulnerable, kExitFrames - 1);
        }
    }
}

int main() { return th13test::run_all("m4.trance"); }
