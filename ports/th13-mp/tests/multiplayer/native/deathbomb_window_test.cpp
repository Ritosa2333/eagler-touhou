// RULES R-G: MP doubles the retail eight-frame deathbomb window to sixteen.
// Real collision callbacks, player updates and original SHT data; no main so
// this translation unit links with the existing M4 suite entry point.
#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include <fstream>
#include <iterator>

using namespace th13;
using namespace th13test;

namespace {
struct DeathbombHost final : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};

struct DeathbombProvider final : EclResourceProvider {
    std::vector<u8> stage;
    std::vector<std::string>* animations = nullptr;
    DeathbombProvider() {
        std::vector<u8> main;
        emit(main, 0, 23, 0, 255, 0, {100000});
        emit(main, 0, 10, 0, 255, 0, {});
        stage = build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name == "deathbomb.ecl") { out = stage; return true; }
        const char* root = std::getenv("TH13_NATIVE_ASSET_ROOT");
        std::ifstream file(std::string(root ? root : "/work/artifacts/reference/raw") + "/" + name,
                           std::ios::binary);
        out = {std::istreambuf_iterator<char>(file), {}};
        return !out.empty();
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name);
        return true;
    }
};

struct DeathbombFixture {
    DeathbombHost host;
    DeathbombProvider provider;
    SessionState progress;
    HudPlayerState hud;
    HudScoreState score;
    StagePlayerState window;
    StageBattle battle{nullptr, &window};
    std::array<GameInput, 3> inputs{};
    i32 count = 0;

    bool boot(i32 players, i32 character = 0) {
        count = players;
        progress.character = character;
        provider.animations = &battle.sim.anim_list;
        if (!battle.load(provider, "deathbomb.ecl", 2, host, progress, hud, score)) return false;
        if (players >= 2) {
            if (!battle.enable_seats(provider, players, {character, character, character})) return false;
        } else battle.seat_count = players; // Both public single-seat step forms.
        // Initialize the combat fixture before collision; every later phase,
        // timer, life and Bomb change comes from the production step path.
        for (i32 seat = 0; seat < (players >= 2 ? players : 1); ++seat) {
            auto& player = state(seat);
            player.phase = StagePlayerState::Phase::Active;
            player.phase_frame = 0;
            player.phase_clock.set(0);
            player.invulnerable = 0;
            player.x = float(-120 + seat * 120);
            player.y = 400;
            player.fixed_x = i32(player.x * 128);
            player.fixed_y = 51200;
            resources(seat).power = 300;
        }
        return true;
    }
    StagePlayerState& state(i32 seat) {
        return count >= 2 ? battle.seats[size_t(seat)] : window;
    }
    HudPlayerState& resources(i32 seat) {
        return count >= 2 ? battle.seat_huds[size_t(seat)] : hud;
    }
    void collide(i32 owner) {
        StageBullet hit;
        hit.alive = true;
        hit.collision_ready = true;
        hit.flags = 0x12u;
        hit.hit_size[0] = hit.hit_size[1] = 8;
        hit.x = state(owner).x;
        hit.y = state(owner).y;
        battle.sim.bullet_player_collision(hit);
        CHECK(battle.error().empty());
        CHECK_EQ(state(owner).phase, StagePlayerState::Phase::Deathbomb);
        CHECK_EQ(state(owner).phase_frame, 0);
        CHECK_EQ(resources(owner).deaths, 0);
        CHECK_EQ(resources(owner).extra_lives, 2);
    }
    bool step(i32 bomb_owner = -1) {
        for (i32 seat = 0; seat < (count >= 2 ? count : 1); ++seat) {
            inputs[size_t(seat)].update(seat == bomb_owner ? 2u : 0u);
            battle.seat_inputs[size_t(seat)] = inputs[size_t(seat)];
        }
        return battle.step(inputs[0]) && battle.error().empty();
    }
    void advance(i32 frames) { for (i32 frame = 0; frame < frames; ++frame) CHECK(step()); }
    void peers_unchanged(i32 owner) {
        for (i32 seat = 0; seat < count; ++seat) if (seat != owner) {
            CHECK_EQ(state(seat).phase, StagePlayerState::Phase::Active);
            CHECK_EQ(resources(seat).extra_lives, 2);
            CHECK_EQ(resources(seat).deaths, 0);
            CHECK_EQ(resources(seat).bombs, 2);
            CHECK_EQ(resources(seat).power, 300);
            CHECK_EQ(state(seat).bomb_generation, 0u);
        }
    }
};

void check_success(DeathbombFixture& f, i32 owner) {
    CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Active);
    CHECK_EQ(f.resources(owner).extra_lives, 2);
    CHECK_EQ(f.resources(owner).deaths, 0);
    CHECK_EQ(f.resources(owner).bombs, 1);
    CHECK_EQ(f.resources(owner).power, 300);
    CHECK_EQ(f.state(owner).bomb_generation, 1u);
    CHECK(f.state(owner).bomb_timer > 0);
    // A held X and a fresh second press while that Bomb is active both keep
    // the single original owner debit and generation.
    CHECK(f.step(owner));
    CHECK(f.step());
    CHECK(f.step(owner));
    CHECK_EQ(f.resources(owner).bombs, 1);
    CHECK_EQ(f.resources(owner).deaths, 0);
    CHECK_EQ(f.state(owner).bomb_generation, 1u);
    f.peers_unchanged(owner);
}
}  // namespace

TEST(mp_deathbomb_last_frame) {
    DeathbombFixture f;
    CHECK(f.boot(2));
    f.collide(1);
    f.advance(15);
    CHECK_EQ(f.state(1).phase, StagePlayerState::Phase::Deathbomb);
    CHECK_EQ(f.state(1).phase_frame, 15);
    CHECK(f.step(1));
    check_success(f, 1);
}

TEST(mp_deathbomb_x_all_valid_frames) {
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner)
        for (i32 character = 0; character < 4; ++character) for (i32 age = 0; age < 16; ++age) {
            DeathbombFixture f;
            CHECK(f.boot(count, character));
            f.collide(owner);
            f.advance(age);
            CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Deathbomb);
            CHECK_EQ(f.state(owner).phase_frame, age);
            CHECK(f.step(owner));
            check_success(f, owner);
        }
}

TEST(mp_deathbomb_x_at_and_after_expiry) {
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner)
        for (i32 age : {16, 17, 20}) {
            DeathbombFixture f;
            CHECK(f.boot(count));
            f.collide(owner);
            f.advance(age);
            CHECK(f.step(owner));
            CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Dying);
            CHECK_EQ(f.resources(owner).extra_lives, 1);
            CHECK_EQ(f.resources(owner).deaths, 1);
            CHECK_EQ(f.resources(owner).bombs, 2);
            CHECK_EQ(f.state(owner).bomb_generation, 0u);
            f.peers_unchanged(owner);
        }
}

TEST(single_seat_deathbomb_eight_frames) {
    // Both public production entry forms must retain the retail window.
    for (i32 count : {0, 1}) for (i32 age = 0; age < 10; ++age) {
        DeathbombFixture f;
        CHECK(f.boot(count));
        f.collide(0);
        f.advance(age);
        CHECK(f.step(0));
        if (age < 8) check_success(f, 0);
        else {
            CHECK_EQ(f.state(0).phase, StagePlayerState::Phase::Dying);
            CHECK_EQ(f.resources(0).extra_lives, 1);
            CHECK_EQ(f.resources(0).deaths, 1);
            CHECK_EQ(f.resources(0).bombs, 2);
            CHECK_EQ(f.state(0).bomb_generation, 0u);
        }
    }
}

TEST(mp_deathbomb_no_x_commits_once) {
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner) {
        DeathbombFixture f;
        CHECK(f.boot(count));
        f.collide(owner);
        f.advance(16);
        CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Deathbomb);
        CHECK_EQ(f.state(owner).phase_frame, 16);
        CHECK_EQ(f.resources(owner).deaths, 0);
        CHECK(f.step());
        CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Dying);
        CHECK_EQ(f.resources(owner).extra_lives, 1);
        CHECK_EQ(f.resources(owner).deaths, 1);
        f.advance(12);
        CHECK_EQ(f.resources(owner).extra_lives, 1);
        CHECK_EQ(f.resources(owner).deaths, 1);
        CHECK_EQ(f.resources(owner).bombs, 2);
        CHECK_EQ(f.state(owner).bomb_generation, 0u);
        f.peers_unchanged(owner);
    }
}

TEST(mp_deathbomb_no_x_enters_owner_protection) {
    for (i32 count : {2, 3}) for (i32 owner = 0; owner < count; ++owner) {
        DeathbombFixture f;
        CHECK(f.boot(count));
        f.progress.ending_points = 200;
        f.collide(owner);
        f.advance(16);
        CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Deathbomb);
        CHECK_EQ(f.state(owner).phase_frame, 16);
        CHECK(!f.battle.team_trance_active());
        CHECK(f.step());
        CHECK(f.battle.team_trance_active());
        CHECK_EQ(f.battle.trance_mode, StageBattle::TranceMode::DeathProtection);
        CHECK_EQ(f.battle.trance_owner_seat, owner);
        CHECK_EQ(f.state(owner).phase, StagePlayerState::Phase::Active);
        CHECK_EQ(f.resources(owner).deaths, 0);
        CHECK_EQ(f.resources(owner).extra_lives, 2);
        CHECK_EQ(f.resources(owner).bombs, 2);
        CHECK_EQ(f.state(owner).bomb_generation, 0u);
        f.advance(8);
        CHECK_EQ(f.resources(owner).deaths, 0);
        CHECK_EQ(f.resources(owner).bombs, 2);
        f.peers_unchanged(owner);
    }
}
