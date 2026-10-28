// 弹-玩家判定边界语义单测（fix-battle-edges-20261028）。
//
// 三处边界均以原版 th13.exe 直调（unicorn）取得的真值为准（探针与输出见
// artifacts/verification/fix-battle-edges-20261028/）：
//  1. 圆形命中（0x444140）：0x4441a1 fcomp + 0x4441a5 test ah,0x41 / jp
//     ——命中分支仅当 p²+r²>D²（严格），等号落擦弹支路（D==p 原版返回 2）。
//  2. 擦弹 span（0x4441ac fdiv qword 2.5 → 0x4441b2 fstp dword）：
//     span 单舍入落 f32 后参与阈值 r²+(span+p)²（0x4441cc-0x4441d7）。
//  3. 矩形擦弹带（0x443fd0）：分离判据严格 <（0x44008-0x44056），而 ±24
//     带四边全部含等号（0x4440d8/0x4440ea test ah,5+jnp、0x4440fc/0x44410e
//     test ah,0x41+je）——四边等号原版均返回 2，越 1ulp 返回 0。
// 本套件把这些边界钉死：任何方向（含把严格改含等号或反之）的漂移都翻红。
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
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};
struct Host final : StageBattleServices {
    int hits = 0, grazes = 0;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32 id) override { hits += id == 2; grazes += id == 42; return true; }
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
        // 角色 0（灵梦）：player_hit_size_ = 2.0f → 矩形半宽 1。
        battle.set_player_pos(100, 300);
        battle.player_phase = StageBattle::PlayerPhase::Active;
        battle.invulnerable = 0;
    }
    StageBullet& circle_bullet(float x, float y, float radius) {
        StageBullet b;
        b.x = x;
        b.y = y;
        b.flags = 0x12;  // bit1 碰撞 + bit4 圆形（0x40e840 出生位）
        b.hit_size[0] = b.hit_size[1] = radius;
        battle.sim.bullets.push_back(b);
        return battle.sim.bullets.back();
    }
    StageBullet& rect_bullet(float x, float y) {
        StageBullet b;
        b.x = x;
        b.y = y;
        b.flags = 0x2;  // bit1 碰撞，bit4=0 → 矩形分支；hit_size 默认 4 → 半宽 2
        battle.sim.bullets.push_back(b);
        return battle.sim.bullets.back();
    }
    bool step() { return battle.step(GameInput{}); }
};

float frombits(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
float nextup(float f) { u32 b; std::memcpy(&b, &f, 4); ++b; std::memcpy(&f, &b, 4); return f; }
float nextdown(float f) { u32 b; std::memcpy(&b, &f, 4); --b; std::memcpy(&f, &b, 4); return f; }

// 1. 圆形命中等号：D==p 原版返回 2（擦弹），D<p 才命中。
TEST(circle_hit_equality_falls_through_to_graze_not_hit) {
    {  // D=1.5 < p=2 → 命中。
        Rig r;
        r.circle_bullet(101.5f, 300.0f, 0.0f);
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 1);
        CHECK_EQ(r.session.graze, 0);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Deathbomb);
        CHECK(!r.battle.sim.bullets[0].alive);
    }
    {  // D=2 == p → 等号落擦弹（原版 0x4441a5 jp 仅 thr>D² 才命中）。
        Rig r;
        r.circle_bullet(102.0f, 300.0f, 0.0f);
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 0);
        CHECK_EQ(r.session.graze, 1);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
        CHECK(r.battle.sim.bullets[0].alive);
    }
}

// 2. 擦弹 span 必须先落 f32（0x4441b2）：r=0x42c8000c、弹 x=0xc10764a8 时
//    D²=f32 网格点 11764.02148438 落在 [double-span 阈值 11764.0213867,
//    f32-span 阈值 11764.0215149) 内——span 保 double 会漏判这次擦弹。
TEST(circle_graze_span_rounds_to_f32_before_threshold) {
    const u32 r_bits = 0x42c8000cu, bx_bits = 0xc10764a8u;
    {  // 原版（f32 span）判定擦弹。
        Rig r;
        r.circle_bullet(frombits(bx_bits), 300.0f, frombits(r_bits));
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 0);
        CHECK_EQ(r.session.graze, 1);
        CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Active);
    }
    {  // 同 r 远 0.5px：超出两种阈值 → 无接触。
        Rig r;
        r.circle_bullet(frombits(bx_bits) - 0.5f, 300.0f, frombits(r_bits));
        CHECK(r.step());
        CHECK_EQ(r.host.hits, 0);
        CHECK_EQ(r.session.graze, 0);
    }
}

// 3. 矩形擦弹带四边含等号：恰好压线 → 擦弹；越 1ulp → 无接触。
TEST(rect_graze_band_all_four_edges_are_inclusive) {
    struct Case { float x, y, ox, oy; };  // 弹位与“越界 1ulp”方向偏移
    const Case edges[4] = {
        {125.0f, 300.0f, 1.0f, 0.0f},  // 边3：bx-24 == right(101)
        {100.0f, 325.0f, 0.0f, 1.0f},  // 边4：by-24 == bottom(301)
        {75.0f, 300.0f, -1.0f, 0.0f},  // 边1：bx+24 == left(99)
        {100.0f, 275.0f, 0.0f, -1.0f}, // 边2：by+24 == top(299)
    };
    for (const auto& e : edges) {
        float beyond_x = e.ox > 0 ? nextup(e.x) : e.ox < 0 ? nextdown(e.x) : e.x;
        float beyond_y = e.oy > 0 ? nextup(e.y) : e.oy < 0 ? nextdown(e.y) : e.y;
        {  // 压线等号 → 擦弹（原版 2）。
            Rig r;
            r.rect_bullet(e.x, e.y);
            CHECK(r.step());
            CHECK_EQ(r.session.graze, 1);
            CHECK_EQ(r.host.hits, 0);
            CHECK(r.battle.sim.bullets[0].alive);
        }
        {  // 越 1ulp → 无接触（原版 0）。
            Rig r;
            r.rect_bullet(beyond_x, beyond_y);
            CHECK(r.step());
            CHECK_EQ(r.session.graze, 0);
            CHECK_EQ(r.host.hits, 0);
            CHECK(r.battle.sim.bullets[0].alive);
        }
    }
}

// 4. 矩形分离判据严格：弹右边 == 玩家左边 → 未分离 → 命中。
TEST(rect_separation_equality_is_overlap_and_hits) {
    Rig r;
    r.rect_bullet(97.0f, 300.0f);  // 弹右缘 99 == left 99 → 命中（原版 1）
    CHECK(r.step());
    CHECK_EQ(r.host.hits, 1);
    CHECK_EQ(r.session.graze, 0);
    CHECK(r.battle.player_phase == StageBattle::PlayerPhase::Deathbomb);
    CHECK(!r.battle.sim.bullets[0].alive);
}
}  // namespace
int main() { return th13test::run_all("bullet collision boundary"); }
