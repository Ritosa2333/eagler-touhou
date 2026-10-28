#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Provider : EclResourceProvider {
    std::vector<u8> data; std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> code;
        th13test::emit(code, 0, 502, 0, 255, 0, {0x20});
        th13test::emit(code, 0, 23, 0, 255, 0, {1000});
        data = th13test::build_scpt({{"main", code}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& out) override { out = data; return true; }
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};
struct Host : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Rig {
    Provider provider; Host host; SessionState progress; HudPlayerState player; HudScoreState score;
    StageBattle battle; StageEnemy ship; StageEnemy* boss = nullptr;
    bool init() {
        provider.names = &battle.sim.anim_list;
        if (!battle.load(provider, "ship.ecl", 2, host, progress, player, score)) return false;
        auto b = std::make_unique<StageEnemy>(); boss = b.get();
        boss->script_id = 99; boss->life = 1000; boss->position[1] = 80;
        boss->hurtbox[0] = boss->hurtbox[1] = 0;
        boss->spirit_threshold = 10000;
        battle.sim.enemies.push_back(std::move(b)); battle.sim.boss_slot[0] = 99;
        ship.life = 200000; ship.position[1] = 200; ship.spirit_threshold = 10000;
        ship.hurtbox[0] = ship.hurtbox[1] = 0;
        ship.damage_function = 1;
        return true;
    }
    void shot(float x, float y, float damage) {
        PlayerShot s; s.alive = true; s.x = x; s.y = y; s.width = s.height = 2; s.angle = 0; s.damage = damage;
        battle.shots.push_back(s);
    }
};
TEST(ship_queries_160_by_32_and_defers_transfer_until_boss_update) {
    Rig r; CHECK(r.init()); r.shot(70, 200, 27);
    r.battle.sim.enemy_hit(r.ship);
    CHECK_EQ(r.ship.life, 199973.f); CHECK_EQ(r.boss->life, 1000.f);
    CHECK_EQ(r.boss->pending_damage, 27); CHECK_EQ(r.score.score, 1);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->life, 973.f); CHECK_EQ(r.boss->pending_damage, 0);
    CHECK_EQ(r.score.score, 1); // consuming redirected damage awards no second query score
    r.battle.sim.enemy_hit(*r.boss); CHECK_EQ(r.boss->life, 973.f);
}
TEST(zero_life_boss_death_animation_still_queries_hits_without_losing_health) {
    Rig r;CHECK(r.init());r.boss->life=0;r.boss->flags=0x100009cu;
    r.boss->hurtbox[0]=r.boss->hurtbox[1]=32;
    r.boss->spirit_accum=990;r.boss->spirit_threshold=1000;r.boss->hit_anim_timer=30;
    r.shot(0,80,12);const auto before=r.battle.sim.random.index();
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.score.score,1);CHECK(!r.battle.shots[0].alive);
    CHECK_EQ(r.boss->life,0.f);CHECK(!r.boss->dead);CHECK_EQ(r.boss->spirit_accum,1002.f);
    CHECK_EQ(r.battle.sim.random.index(),before+4);
    CHECK(r.battle.item_manager.spirits()[0].alive);
    CHECK_EQ(r.battle.item_manager.spirits()[0].kind,1);
}
TEST(ship_hull_boundary_and_query_damage_cap_match_original_query_contract) {
    for (float x : {81.f, 81.01f}) {
        Rig r; CHECK(r.init()); r.shot(x, 200, 120);
        r.battle.sim.enemy_hit(r.ship);
        CHECK_EQ(r.boss->pending_damage, x == 81.f ? 80 : 0);
        CHECK_EQ(r.score.score, x == 81.f ? 1 : 0);
    }
}
TEST(ship_callback_receives_base_damage_and_overwrites_boss_pending_value) {
    Rig r; CHECK(r.init()); r.ship.hurtbox[0] = r.ship.hurtbox[1] = 16;
    r.boss->pending_damage = 99;
    r.shot(0, 200, 13); r.shot(70, 200, 27);
    r.battle.sim.enemy_hit(r.ship);
    CHECK_EQ(r.boss->pending_damage, 40); //422cd0 returns incoming13 + hull27
    CHECK_EQ(r.ship.life, 199947.f); //419a90 adds that return to base13
    CHECK_EQ(r.score.score, 2); // two geometric queries, each scored once
}
TEST(ecl633_can_register_and_clear_ship_damage_callback) {
    StageSim sim; StageEnemy e; EclContext c; sim.commands.sim = &sim; sim.current = &e;
    for (u32 index : {1u, 0u}) {
        std::vector<u8> code; th13test::emit(code, 0, 633, 0, 255, 0, {index});
        c.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(c, 633), 0); CHECK_EQ(e.damage_function, i32(index));
    }
}
}
int main() { return th13test::run_all("Stage5 ship damage"); }
