#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

#include <cstdio>
#include <cstdlib>
#include <string>

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
        if (name == "challenge.ecl") {
            out = stage;
            return true;
        }
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".sht") == 0) {
            const char* env = std::getenv("TH13_NATIVE_ASSET_ROOT");
            if (env) {
                std::FILE* file = std::fopen((std::string(env) + "/" + name).c_str(), "rb");
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
    bool init() {
        provider.animations = &battle.sim.anim_list;
        return battle.load(provider, "challenge.ecl", 2, host, progress, player, score);
    }
    void enable(i32 count) {
        CHECK(battle.enable_seats(provider, count, {0, 1, 2}));
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

TEST(challenge_blocks_bombs_counts_misses_and_locks_trance) {
    Fixture f;
    CHECK(f.init());
    f.battle.challenge = true;
    f.battle.item_manager.challenge = true;
    f.enable(3);
    f.battle.apply_challenge_locks();
    for (i32 seat = 0; seat < 3; ++seat) {
        CHECK_EQ(f.battle.seat_huds[size_t(seat)].bombs, 0);
        CHECK_EQ(f.battle.seat_huds[size_t(seat)].bomb_pieces, 0);
    }
    CHECK_EQ(f.progress.ending_points, 0);
    CHECK(!trance_active(f.progress));

    f.battle.seat_huds[0].bombs = 4;
    GameInput bomb;
    bomb.update(0x02);
    CHECK(f.battle.step(bomb));
    CHECK_EQ(f.battle.seat_huds[0].bombs, 0);
    CHECK_EQ(f.battle.seats[0].bomb_timer, 0);
    CHECK_EQ(f.battle.seats[0].bomb_generation, 0u);

    ItemCollector collectors[2] = {};
    for (i32 seat = 0; seat < 2; ++seat) {
        auto& collector = collectors[seat];
        collector.x = 0;
        collector.y = 400;
        collector.state = 1;
        collector.eligible = true;
        collector.seat = seat;
        collector.player = &f.battle.seat_huds[size_t(seat)];
        collector.progress = &f.progress;
        collector.score = &f.score;
        collector.box = {-30, 370, 30, 430};
        collector.box_auto = collector.box;
    }
    f.battle.item_manager.spawn_item(0, 400, 0, ItemBomb, 0, 0);
    f.battle.item_manager.update_seats(collectors, 2);
    f.battle.item_manager.spawn_item(0, 400, 0, ItemBombPiece, 0, 0);
    f.battle.item_manager.update_seats(&collectors[0], 1);
    CHECK_EQ(f.battle.seat_huds[0].bombs, 0);
    CHECK_EQ(f.battle.seat_huds[1].bombs, 0);
    CHECK_EQ(f.battle.seat_huds[0].bomb_pieces, 0);
    CHECK_EQ(f.progress.ending_points, 0);

    f.progress.ending_points = 600;
    GameInput trance;
    trance.update(kCKey);
    f.battle.seat_inputs[1].update(kCKey);
    f.battle.seat_inputs[2].update(kCKey);
    CHECK(f.battle.step(trance));
    CHECK(!trance_active(f.progress));
    CHECK_EQ(f.progress.ending_points, 0);
    CHECK_EQ(f.battle.trance_mode, StageBattle::TranceMode::Idle);

    GameInput idle;
    for (i32 frame = 0; frame < 32; ++frame) CHECK(f.battle.step(idle));
    separate_seats(f, 1);
    f.progress.ending_points = 600;
    const i32 lives = f.battle.seat_huds[1].extra_lives;
    auto bullet = bullet_at(-120, 400);
    f.battle.sim.bullet_player_collision(bullet);
    CHECK_EQ(f.battle.seats[1].phase, StagePlayerState::Phase::Deathbomb);
    f.battle.seat_inputs[1].update(0x02);
    for (i32 frame = 0; frame < 20 &&
         f.battle.seats[1].phase == StagePlayerState::Phase::Deathbomb; ++frame)
        CHECK(f.battle.step(idle));
    CHECK(!trance_active(f.progress));
    CHECK_EQ(f.progress.ending_points, 0);
    CHECK_EQ(f.battle.seats[1].phase, StagePlayerState::Phase::Dying);
    CHECK_EQ(f.battle.seat_huds[1].extra_lives, lives);
    CHECK_EQ(f.battle.seat_huds[1].deaths, 1);
    CHECK_EQ(f.battle.seat_huds[1].bombs, 0);
    CHECK(!f.battle.seats[1].ghost);
    CHECK(!f.battle.seats[1].final_death);
    for (i32 frame = 0; frame < 40; ++frame) CHECK(f.battle.step(idle));
    CHECK(!f.battle.seats[1].ghost);
    CHECK(f.battle.seats[1].phase == StagePlayerState::Phase::Entering ||
          f.battle.seats[1].phase == StagePlayerState::Phase::Active);
    CHECK_EQ(f.battle.seat_huds[1].deaths, 1);
    CHECK_EQ(f.battle.seat_huds[1].bombs, 0);
    CHECK(!f.battle.team_wiped());

    f.battle.seat_huds[1].deaths = 0x7fffffff;
    f.battle.seats[1].phase = StagePlayerState::Phase::Deathbomb;
    f.battle.seats[1].phase_frame = 16;
    f.battle.seats[1].dead = false;
    f.battle.seats[1].ghost = false;
    CHECK(f.battle.step(idle));
    CHECK_EQ(f.battle.seat_huds[1].deaths, 0x7fffffff);
    CHECK_EQ(f.battle.seat_huds[1].extra_lives, lives);
    CHECK(!f.battle.seats[1].ghost);
}

int main() { return th13test::run_all("challenge-mode"); }
