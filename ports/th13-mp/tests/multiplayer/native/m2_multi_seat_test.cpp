#include "../../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

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
        if (name == "mp2.ecl") {
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
        return battle.load(provider, "mp2.ecl", 2, host, progress, player, score);
    }
    bool load_sht(const char* name) {
        TestFileReader reader;
        std::vector<u8> sht;
        if (!reader.read(std::string("eagler-touhou/ports/th13-mp/web/sdl-runtime/") +
                         name, sht))
            return false;
        return battle.prepare_player(provider, progress, player, name, false);
    }
    void enable(i32 count, std::array<i32, 3> characters) {
        CHECK(battle.enable_seats(provider, count, characters));
    }
};

float seat_x(const StageBattle& battle, i32 seat) {
    return battle.seats[size_t(seat)].x;
}
float seat_y(const StageBattle& battle, i32 seat) {
    return battle.seats[size_t(seat)].y;
}

u32 held_of(u32 buttons) {
    GameInput input;
    input.update(buttons);
    return input.held;
}

}

TEST(world_tick_once) {
    Fixture single, dual, triple;
    CHECK(single.init()); CHECK(dual.init()); CHECK(triple.init());
    dual.enable(2, {0, 1, 2});
    triple.enable(3, {0, 1, 2});
    GameInput none;
    for (int frame = 0; frame < 120; ++frame) {
        CHECK(single.battle.step(none));
        CHECK(dual.battle.step(none));
        CHECK(triple.battle.step(none));
        CHECK_EQ(dual.battle.sim.frame, single.battle.sim.frame);
        CHECK_EQ(triple.battle.sim.frame, single.battle.sim.frame);
        CHECK_EQ(dual.battle.sim.random.index(), single.battle.sim.random.index());
        CHECK_EQ(triple.battle.sim.random.index(), single.battle.sim.random.index());
        CHECK_EQ(dual.battle.sim.enemies.size(), single.battle.sim.enemies.size());
        CHECK_EQ(triple.battle.sim.enemies.size(), single.battle.sim.enemies.size());
    }
}

TEST(independent_inputs_2_3) {
    {
        Fixture dual;
        CHECK(dual.init());
        dual.enable(2, {0, 0, 0});
        const u32 left = held_of(0x40), right = held_of(0x80), up = held_of(0x10);
        for (int frame = 0; frame < 60; ++frame) {
            dual.battle.seat_inputs[1].update(right);
            GameInput seat0;
            seat0.update(left);
            CHECK(dual.battle.step(seat0));
        }
        CHECK(seat_x(dual.battle, 0) < 0.0f);
        CHECK(seat_x(dual.battle, 1) > 0.0f);
        CHECK(std::fabs(seat_x(dual.battle, 0) + seat_x(dual.battle, 1)) < 1e-4f);
        CHECK(std::fabs(seat_y(dual.battle, 0) - seat_y(dual.battle, 1)) < 1e-4f);
    }
    {
        Fixture triple;
        CHECK(triple.init());
        triple.enable(3, {0, 0, 0});
        const u32 left = held_of(0x40), right = held_of(0x80), up = held_of(0x10);
        for (int frame = 0; frame < 60; ++frame) {
            triple.battle.seat_inputs[1].update(right);
            triple.battle.seat_inputs[2].update(up);
            GameInput seat0;
            seat0.update(left);
            CHECK(triple.battle.step(seat0));
        }
        CHECK(seat_x(triple.battle, 0) < 0.0f);
        CHECK(seat_x(triple.battle, 1) > 0.0f);
        CHECK(std::fabs(seat_x(triple.battle, 0)) > 100.0f);
        CHECK(seat_y(triple.battle, 2) < seat_y(triple.battle, 0));
        CHECK(std::fabs(seat_x(triple.battle, 2)) < 1e-4f);
    }
}

TEST(four_characters_duplicates) {
    static constexpr i32 kSpeedOrth[4] = {576, 640, 576, 576};
    for (i32 a = 0; a < 4; ++a) {
        for (i32 b = 0; b < 4; ++b) {
            Fixture fixture;
            CHECK(fixture.init());
            fixture.enable(2, {a, b, 0});
            CHECK_EQ(fixture.battle.seat_characters[0], a);
            CHECK_EQ(fixture.battle.seat_characters[1], b);
            CHECK_EQ(fixture.battle.seats[0].hit_size, StageBattle::kPlayerHitSize[a]);
            CHECK_EQ(fixture.battle.seats[1].hit_size, StageBattle::kPlayerHitSize[b]);
            CHECK_EQ(fixture.battle.seats[1].character, b);
            const u32 left = held_of(0x40);
            for (int frame = 0; frame < 10; ++frame) {
                GameInput seat0;
                seat0.update(left);
                CHECK(fixture.battle.step(seat0));
            }
            CHECK(seat_x(fixture.battle, 0) < 0.0f);
            CHECK(std::fabs(seat_x(fixture.battle, 1)) < 1e-4f);
        }
    }
}

TEST(damage_owner_ids) {
    Fixture dual;
    CHECK(dual.init());
    dual.enable(2, {0, 0, 0});
    const u32 fire = held_of(0x01);
    for (int frame = 0; frame < 30; ++frame) {
        GameInput seat0;
        seat0.update(fire);
        CHECK(dual.battle.step(seat0));
    }
    CHECK(!dual.battle.seats[0].shots.empty());
    CHECK(dual.battle.seats[1].shots.empty());
    size_t active0 = 0;
    for (const auto& reg : dual.battle.seats[0].damage_registrations)
        if (reg.active) ++active0;
    CHECK(active0 > 0);
    size_t active1 = 0;
    for (const auto& reg : dual.battle.seats[1].damage_registrations)
        if (reg.active) ++active1;
    CHECK_EQ(active1, 0u);
    const size_t shots_before = dual.battle.seats[0].shots.size();
    dual.battle.swap_player_context(1);
    CHECK_EQ(dual.battle.shots.size(), 0u);
    dual.battle.swap_player_context(0);
    CHECK_EQ(dual.battle.shots.size(), shots_before);
    CHECK_EQ(dual.battle.active_seat, 0);
}

TEST(single_seat_sp_equivalence) {
    Fixture a, b;
    CHECK(a.init()); CHECK(b.init());
    Fixture c;
    CHECK(c.init());
    CHECK(c.battle.enable_seats(c.provider, 2, {0, 0, 0}));
    CHECK_EQ(c.battle.player_x, a.battle.player_x);
    CHECK_EQ(c.battle.player_y, a.battle.player_y);
    CHECK_EQ(c.battle.player_phase_frame, a.battle.player_phase_frame);
    CHECK_EQ(c.battle.invulnerable, a.battle.invulnerable);
    CHECK_EQ(c.battle.seat_count, 2);
    CHECK_EQ(c.battle.active_seat, 0);
    const u32 pattern[8] = {0x01, 0x01, 0x41, 0x00, 0x02, 0x08, 0x10, 0x80};
    for (int frame = 0; frame < 90; ++frame) {
        GameInput input;
        input.update(pattern[frame % 8]);
        CHECK(a.battle.step(input));
        CHECK(b.battle.step(input));
        CHECK_EQ(a.battle.player_x, b.battle.player_x);
        CHECK_EQ(a.battle.player_y, b.battle.player_y);
        CHECK_EQ(a.battle.player_phase_frame, b.battle.player_phase_frame);
        CHECK_EQ(a.battle.invulnerable, b.battle.invulnerable);
        CHECK_EQ(a.battle.sim.frame, b.battle.sim.frame);
        CHECK_EQ(a.battle.sim.random.index(), b.battle.sim.random.index());
        CHECK_EQ(a.battle.shots.size(), b.battle.shots.size());
        CHECK(a.battle.error().empty() == b.battle.error().empty());
    }
    Fixture d;
    CHECK(d.init());
    GameInput none;
    for (int frame = 0; frame < 90; ++frame) {
        GameInput input;
        input.update(pattern[frame % 8]);
        CHECK(d.battle.step(input));
    }
    CHECK(d.battle.enable_seats(d.provider, 2, {0, 0, 0}));
    for (int frame = 0; frame < 30; ++frame) {
        CHECK(d.battle.step(none));
        CHECK(a.battle.step(none));
        CHECK_EQ(d.battle.seats[0].x, a.battle.player_x);
        CHECK_EQ(d.battle.seats[0].y, a.battle.player_y);
        CHECK_EQ(d.battle.sim.frame, a.battle.sim.frame);
        CHECK_EQ(d.battle.sim.random.index(), a.battle.sim.random.index());
    }
}

int main() { return th13test::run_all("m2.multi_seat"); }
