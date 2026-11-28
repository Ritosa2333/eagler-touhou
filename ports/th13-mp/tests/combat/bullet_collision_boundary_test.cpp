#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include <cstring>
using namespace th13;
namespace {
struct Provider final : EclResourceProvider {
    std::vector<u8> data;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> body;
        th13test::emit(body, 0, 23, 0, 255, 0, {1000});
        th13test::emit(body, 0, 10, 0, 255, 0, {});
        data = th13test::build_scpt({{"main", body}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "boundary.ecl") return false;
        out = data;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        names->push_back(name);
        return true;
    }
};
struct Host final : StageBattleServices {
    int hits = 0, grazes = 0;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32 id) override {
        hits += id == 2;
        grazes += id == 42;
        return true;
    }
};
struct Rig {
    Provider provider;
    Host host;
    SessionState session;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    Rig() {
        provider.names = &battle.sim.anim_list;
        CHECK(battle.load(provider, "boundary.ecl", 2, host, session, player, score));
        battle.sim.enemies.clear();
        battle.set_player_pos(100, 300);
        battle.player_phase = StageBattle::PlayerPhase::Active;
        battle.invulnerable = 0;
    }
    StageBullet& circle_bullet(float x, float y, float radius) {
        StageBullet b;
        b.x = x;
        b.y = y;
        b.flags = 0x12;
        b.hit_size[0] = b.hit_size[1] = radius;
        battle.sim.bullets.push_back(b);
        return battle.sim.bullets.back();
    }
    StageBullet& rect_bullet(float x, float y) {
        StageBullet b;
        b.x = x;
        b.y = y;
        b.flags = 0x2;
        battle.sim.bullets.push_back(b);
        return battle.sim.bullets.back();
    }
    bool step() { return battle.step(GameInput{}); }
};

float frombits(u32 b) {
    float f;
    std::memcpy(&f, &b, 4);
    return f;
}
float nextup(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    ++b;
    std::memcpy(&f, &b, 4);
    return f;
}
float nextdown(float f) {
    u32 b;
    std::memcpy(&b, &f, 4);
    --b;
    std::memcpy(&f, &b, 4);
    return f;
}

TEST(circle_hit_equality_falls_through_to_graze_not_hit) {
    {
        Rig r;
        r.circle_bullet(101.5f, 300.0f, 0.0f);
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 1);
        CHECK_EQ(r.session.graze, 0);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Deathbomb);
        CHECK(r.battle.sim.bullets[0].alive);
        CHECK_EQ(r.battle.sim.bullets[0].state, 3);
    }
    {
        Rig r;
        r.circle_bullet(102.0f, 300.0f, 0.0f);
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 0);
        CHECK_EQ(r.session.graze, 1);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
        CHECK(r.battle.sim.bullets[0].alive);
    }
}

TEST(circle_graze_span_rounds_to_f32_before_threshold) {
    const u32 r_bits = 0x42c8000cu, bx_bits = 0xc10764a8u;
    {
        Rig r;
        r.circle_bullet(frombits(bx_bits), 300.0f, frombits(r_bits));
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 0);
        CHECK_EQ(r.session.graze, 1);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
    }
    {
        Rig r;
        r.circle_bullet(frombits(bx_bits) - 0.5f, 300.0f, frombits(r_bits));
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 0);
        CHECK_EQ(r.session.graze, 0);
    }
}

TEST(rect_graze_band_all_four_edges_are_inclusive) {
    struct Case {
        float x, y, ox, oy;
    };
    const Case edges[4] = {
        {125.0f, 300.0f, 1.0f, 0.0f},
        {100.0f, 325.0f, 0.0f, 1.0f},
        {75.0f, 300.0f, -1.0f, 0.0f},
        {100.0f, 275.0f, 0.0f, -1.0f},
    };
    for (const auto& e : edges) {
        float beyond_x = e.ox > 0 ? nextup(e.x) : e.ox < 0 ? nextdown(e.x) : e.x;
        float beyond_y = e.oy > 0 ? nextup(e.y) : e.oy < 0 ? nextdown(e.y) : e.y;
        {
            Rig r;
            r.rect_bullet(e.x, e.y);
            CHECK(r.step());
            CHECK_EQ(r.session.graze, 1);
            CHECK_EQ(r.host.hits, 0);
            CHECK(r.battle.sim.bullets[0].alive);
        }
        {
            Rig r;
            r.rect_bullet(beyond_x, beyond_y);
            CHECK(r.step());
            CHECK_EQ(r.session.graze, 0);
            CHECK_EQ(r.host.hits, 0);
            CHECK(r.battle.sim.bullets[0].alive);
        }
    }
}

TEST(rect_separation_equality_is_overlap_and_hits) {
    Rig r;
    r.rect_bullet(97.0f, 300.0f);
    CHECK(r.step());
    CHECK_EQ(r.host.hits, 1);
    CHECK(r.battle.sim.bullets[0].alive);
    CHECK_EQ(r.battle.sim.bullets[0].state, 3);
}

}
int main() { return th13test::run_all("bullet collision boundary"); }
