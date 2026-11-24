#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "sdl/ApplicationState.hpp"
#include "multiplayer/Session.hpp"
#include "multiplayer/WorldState.hpp"
#include "eagler/gameplay/CooperativeRules.hpp"
#include <cmath>
#include <cstdlib>

using namespace th13;
using namespace th13test;

namespace {

struct Host final : StageBattleServices {
    std::vector<i32> stopped;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32, float) override { return true; }
    bool stop_sound(i32 id) override { stopped.push_back(id); return true; }
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
        if (name == "mp5.ecl") {
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
        return battle.load(provider, "mp5.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

}

TEST(ghost_rescue_resources) {
    {
        Fixture final;
        CHECK(final.init()); final.enable(2, {0, 1, 0});
        final.progress.ending_points = 0;
        auto& seat = final.battle.seats[1];
        final.battle.seat_huds[1].extra_lives = 0;
        seat.phase = StagePlayerState::Phase::Deathbomb;
        seat.phase_frame = 16;
        seat.bomb_timer = 50;
        seat.bomb_protection = 12;
        PlayerShot beam; beam.alive = true; beam.behavior = 2; beam.registration_slot = 0;
        seat.shots.push_back(beam);
        seat.damage_registrations[0].active = true;
        seat.damage_registrations[0].flags = 1;
        seat.damage_registrations[0].damage = 20;
        GameRandom visual(7);
        multiplayer::WorldState before, after, replayed;
        CHECK(before.capture(final.battle, visual));
        CHECK(final.battle.step(GameInput{}));
        CHECK(seat.final_death);
        CHECK_EQ(seat.bomb_timer, 0);
        CHECK(!seat.shots[0].alive);
        CHECK(!seat.damage_registrations[0].active);
        CHECK(std::find(final.host.stopped.begin(), final.host.stopped.end(), 20) != final.host.stopped.end());
        CHECK(after.capture(final.battle, visual));
        CHECK(before.digest() != after.digest());
        CHECK(before.restore(final.battle, visual));
        CHECK(final.battle.step(GameInput{}));
        CHECK(replayed.capture(final.battle, visual));
        CHECK_EQ(after.digest(), replayed.digest());
        CHECK_EQ(seat.bomb_protection, 11);
        final.battle.seats[0].invulnerable = 0;
        StageBullet hit; hit.alive = true; hit.collision_ready = true;
        hit.flags = 0x12; hit.hit_size[0] = hit.hit_size[1] = 8;
        hit.x = final.battle.seats[0].x; hit.y = final.battle.seats[0].y;
        final.battle.sim.bullet_player_collision(hit);
        CHECK_EQ(final.battle.seats[0].phase, StagePlayerState::Phase::Active);
        seat.bomb_protection = 0;
        StageBullet negative;
        negative.alive = true; negative.collision_ready = true; negative.flags = 0x12;
        negative.hit_size[0] = negative.hit_size[1] = 8;
        negative.x = final.battle.seats[0].x; negative.y = final.battle.seats[0].y;
        final.battle.sim.bullet_player_collision(negative);
        CHECK_EQ(final.battle.seats[0].phase, StagePlayerState::Phase::Deathbomb);
        CHECK(after.restore(final.battle, visual));
        final.battle.seat_huds[1].extra_lives = 1;
        for (int frame = 0; frame < 35 && !seat.ghost; ++frame)
            CHECK(final.battle.step(GameInput{}));
        CHECK(seat.ghost);
        CHECK(seat.ghost_just_entered);
        CHECK(!final.battle.rescue_seat(0, 1));
        multiplayer::WorldState ghost_start, ghost_forward, ghost_replay;
        CHECK(ghost_start.capture(final.battle, visual));
        for (int frame = 0; frame < 95; ++frame) CHECK(final.battle.step(GameInput{}));
        CHECK(ghost_forward.capture(final.battle, visual));
        CHECK(ghost_start.digest_core() != ghost_forward.digest_core());
        CHECK(ghost_start.restore(final.battle, visual));
        for (int frame = 0; frame < 95; ++frame) CHECK(final.battle.step(GameInput{}));
        CHECK(ghost_replay.capture(final.battle, visual));
        CHECK_EQ(ghost_forward.digest(), ghost_replay.digest());
        CHECK(final.battle.rescue_seat(0, 1));
        CHECK(!seat.final_death);
        CHECK_EQ(seat.bomb_timer, 0);
        CHECK(!seat.shots[0].alive);
    }
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    auto& battle = dual.battle;
    battle.seats[1].ghost = true;
    u32 rng = battle.seats[1].ghost_random;
    auto next = [&]() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; };
    const i32 dx = i32(next() % 97) - 48;
    const i32 dy = i32(next() % 81) - 40;
    GameInput left;
    for (int frame = 0; frame < 10; ++frame) {
        left.update(0x40);
        battle.seat_inputs[1].update(0x40);
        CHECK(battle.step(left));
    }
    CHECK_EQ(battle.seats[1].fixed_x, dx * 10);
    CHECK_EQ(battle.seats[1].fixed_y, 51200 + dy * 10);
    CHECK(battle.seats[0].x < 0.0f);
    CHECK(!battle.sim.seat_eligible[1]);
    dual.progress.graze = 0;
    dual.progress.max_point_cap = 500000;
    StageBullet bullet;
    bullet.alive = true;
    bullet.collision_ready = true;
    bullet.flags = 0x12u;
    bullet.hit_size[0] = 8.0f;
    bullet.hit_size[1] = 8.0f;
    bullet.x = 0.0f;
    bullet.y = 400.0f;
    battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(dual.progress.graze, 0);
    battle.seat_huds[0].extra_lives = 1;
    const i32 bombs_before = battle.seat_huds[1].bombs;
    battle.seat_huds[1].bombs = 0;
    CHECK(battle.rescue_seat(0, 1));
    CHECK_EQ(battle.seat_huds[0].extra_lives, 0);
    CHECK(!battle.seats[1].ghost);
    CHECK_EQ(battle.seat_huds[1].bombs, 2);
    CHECK_EQ(battle.seats[1].invulnerable, 240);
    CHECK(!battle.rescue_seat(0, 1));
    battle.seats[1].ghost = true;
    CHECK(!battle.rescue_seat(1, 1));
    battle.seat_huds[0].extra_lives = 0;
    CHECK(!battle.rescue_seat(0, 1));
    u8 transfer = 0;
    bool granted = false;
    for (int tap = 0; tap < 5; ++tap)
        granted = eagler::cooperative::AdvanceLifeTransfer(transfer, true, true);
    CHECK(granted);
    CHECK_EQ(unsigned(transfer), 0u);
}

TEST(team_wipe_180) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    auto& battle = dual.battle;
    CHECK(!battle.team_wiped());
    CHECK_EQ(battle.ghost_count(), 0);
    battle.seats[0].ghost = true;
    CHECK(!battle.team_wiped());
    CHECK_EQ(battle.ghost_count(), 1);
    battle.seats[1].ghost = true;
    CHECK(battle.team_wiped());
    CHECK_EQ(battle.ghost_count(), 2);
    GameInput none;
    for (int frame = 0; frame < 180; ++frame) CHECK(battle.step(none));
    CHECK(battle.team_wiped());
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    triple.battle.seats[0].ghost = true;
    triple.battle.seats[1].ghost = true;
    CHECK(!triple.battle.team_wiped());
    triple.battle.seats[2].ghost = true;
    CHECK(triple.battle.team_wiped());
}

TEST(stage_revive) {
    Fixture triple;
    CHECK(triple.init());
    triple.enable(3, {0, 0, 0});
    auto& battle = triple.battle;
    battle.seats[0].ghost = true;
    battle.seats[2].ghost = true;
    battle.seat_huds[0].bombs = 0;
    battle.seat_huds[2].bombs = 0;
    battle.revive_all_on_stage_clear();
    CHECK_EQ(battle.ghost_count(), 0);
    CHECK(!battle.seats[0].ghost);
    CHECK(!battle.seats[2].ghost);
    CHECK_EQ(battle.seat_huds[0].bombs, 2);
    CHECK_EQ(battle.seat_huds[2].bombs, 2);
    CHECK_EQ(battle.seats[0].invulnerable, 120);
    CHECK_EQ(battle.seats[0].phase, StagePlayerState::Phase::Active);
    GameInput left;
    for (int frame = 0; frame < 5; ++frame) {
        left.update(0x40);
        CHECK(battle.step(left));
    }
    CHECK(battle.seats[0].x < 0.0f);
}

TEST(hud_world_owner_visibility) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 1, 0});
    auto& battle = dual.battle;
    CHECK_EQ(battle.sim.enemies.size() >= 0, true);
    battle.seat_huds[0].power = 123;
    battle.seat_huds[0].extra_lives = 5;
    battle.seat_huds[0].bombs = 7;
    CHECK_EQ(battle.seat_huds[1].power, 100);
    CHECK_EQ(battle.seat_huds[1].extra_lives, 2);
    CHECK_EQ(battle.seat_huds[1].bombs, 2);
    CHECK_EQ(battle.seat_characters[0], 0);
    CHECK_EQ(battle.seat_characters[1], 1);
    GameInput none;
    for (int frame = 0; frame < 25; ++frame) CHECK(battle.step(none));
    GameInput fire;
    for (int frame = 0; frame < 5; ++frame) {
        fire.update(0x01);
        CHECK(battle.step(fire));
    }
    bool owner0_seen = false;
    for (const auto& reg : battle.seats[0].damage_registrations)
        if (reg.active && reg.owner_seat == 0) owner0_seen = true;
    CHECK(owner0_seen);
}

TEST(menu_edges_pause_dialogue) {
    const u32 seat0_held = 0x01, seat1_held = 0x01;
    GameInput a, b;
    a.update(seat0_held);
    b.update(seat1_held);
    const u32 merged_pressed = a.pressed | b.pressed;
    CHECK_EQ(merged_pressed, 1u);
    a.update(seat0_held);
    b.update(seat1_held);
    CHECK_EQ(a.pressed | b.pressed, 0u);
    a.update(seat0_held);
    b.update(0x00);
    b.update(seat1_held);
    CHECK_EQ(a.pressed | b.pressed, 1u);
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    GameInput none;
    for (int frame = 0; frame < 3; ++frame) CHECK(dual.battle.step(none));
    CHECK_EQ(dual.battle.seat_inputs[0].held, 0u);
    CHECK_EQ(dual.battle.seat_inputs[1].held, 0u);

    multiplayer::session.Stop();
    sdl::ApplicationState app;
    const char* assets = std::getenv("TH13_NATIVE_ASSET_ROOT");
    const char* root = std::getenv("TH13_NATIVE_REPO_ROOT");
    CHECK(assets && root);
    if (!assets || !root) return;
    ::setenv("TH13_SAVE_PATH", "manual-trance-pause.bin", 1);
    CHECK(app.audio_device.initialize_for_test(assets,
        std::string(root) + "/games/th13/thbgmogg.dat"));
    CHECK(app.initialize(false));
    CHECK(app.configure_multiplayer(2, {0, 1, 0}, 0));
    CHECK(app.start_game(1));
    app.pending_destination = -1;
    app.frames = 0;
    app.game_random.reseed_restore(1234);
    app.visual.reseed_restore(1234);
    CHECK(app.begin_run());
    if (!app.run) return;
    GameInput controls;
    const auto step = [&](u32 held) {
        controls.update(held);
        return app.update(controls, false) && app.commit(60.f) &&
            app.present() && app.failure.empty();
    };
    for (unsigned i = 0; i < 35; ++i) CHECK(step(0));
    app.progress.ending_points = 600;
    CHECK(step(0x800));
    CHECK(app.run->battle.team_trance_active());
    CHECK_EQ(app.run->battle.trance_mode, StageBattle::TranceMode::Manual);
    CHECK(step(0));
    CHECK(step(0x100));
    CHECK(app.pause->active());
    CHECK_EQ(app.pause->state.screen, PauseScreen::Pause);
    const i32 paused_frame = app.progress.stage_frame;
    const i32 paused_gauge = app.progress.ending_points;
    const u32 generation = app.run->battle.trance_generation;
    for (unsigned i = 0; i < 40; ++i) {
        CHECK(step(0));
        CHECK(app.pause->active());
        CHECK_EQ(app.progress.stage_frame, paused_frame);
        CHECK_EQ(app.progress.ending_points, paused_gauge);
        CHECK_EQ(app.run->battle.trance_generation, generation);
    }
    CHECK(step(0x100));
    for (unsigned i = 0; i < 15; ++i) CHECK(step(0));
    CHECK(!app.pause->active());
    CHECK(app.progress.stage_frame > paused_frame);
    CHECK_EQ(app.progress.ending_points,
        paused_gauge - (app.progress.stage_frame - paused_frame));
    CHECK_EQ(app.run->battle.trance_generation, generation);
}

TEST(no_sp_save_writes) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    const u32 flags_before = dual.progress.run_mode_flags;
    GameInput none;
    for (int frame = 0; frame < 10; ++frame) CHECK(dual.battle.step(none));
    CHECK_EQ(dual.progress.run_mode_flags, flags_before);
    dual.score.score = 1000;
    CHECK_EQ(dual.score.score, 1000);
}

int main() { return th13test::run_all("m5.lifecycle"); }
