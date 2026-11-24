#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
using namespace th13;
namespace {
struct Provider : EclResourceProvider {
    std::vector<u8> data;
    std::vector<std::string>* names = nullptr;
    Provider() {
        std::vector<u8> code;
        th13test::emit(code, 0, 502, 0, 255, 0, {0x20});
        th13test::emit(code, 0, 23, 0, 255, 0, {1000});
        data = th13test::build_scpt({{"main", code}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string&, std::vector<u8>& out) override {
        out = data;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        names->push_back(name);
        return true;
    }
};
struct Host : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Rig {
    Provider provider;
    Host host;
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    StageBattle battle;
    StageEnemy ship;
    StageEnemy* boss = nullptr;
    bool init() {
        provider.names = &battle.sim.anim_list;
        if (!battle.load(provider, "ship.ecl", 2, host, progress, player, score)) return false;
        auto b = std::make_unique<StageEnemy>();
        boss = b.get();
        boss->script_id = 99;
        boss->life = 1000;
        boss->position[1] = 80;
        boss->hurtbox[0] = boss->hurtbox[1] = 0;
        boss->spirit_threshold = 10000;
        battle.sim.enemies.push_back(std::move(b));
        battle.sim.boss_slot[0] = 99;
        ship.life = 200000;
        ship.position[1] = 200;
        ship.spirit_threshold = 10000;
        ship.hurtbox[0] = ship.hurtbox[1] = 0;
        ship.damage_function = 1;
        return true;
    }
    void shot(float x, float y, float damage) {
        PlayerShot s;
        s.alive = true;
        s.x = x;
        s.y = y;
        s.width = s.height = 2;
        s.angle = 0;
        s.damage = damage;
        battle.shots.push_back(s);
    }
};
TEST(ship_callback_stores_519c_on_boss_and_boss_consumes_it) {
    Rig r;
    CHECK(r.init());
    r.shot(70, 200, 27);
    r.battle.sim.enemy_hit(r.ship);
    CHECK_EQ(r.ship.life, 199973.f);
    CHECK_EQ(r.boss->life, 1000.f);
    CHECK_EQ(r.boss->pending_damage, 27);
    CHECK_EQ(r.ship.pending_damage, 0);
    CHECK_EQ(r.score.score, 1);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->life, 973.f);
    CHECK_EQ(r.boss->pending_damage, 0);
    CHECK_EQ(r.score.score, 1);
}
TEST(boss_519c_pending_damage_adds_to_single_15_hit) {
    Rig r;
    CHECK(r.init());
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 48;
    r.boss->life = 1885.f;
    r.boss->spell_active = true;
    r.boss->spell_life = 13198;
    r.boss->phase_life_threshold = 0;
    r.boss->spirit_accum = 12510.f;
    r.boss->spirit_threshold = 13000;
    r.boss->pending_damage = 15;
    r.shot(0, 80, 15);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->life, 1881.f);
    CHECK_EQ(r.boss->spirit_accum, 12540.f);
    CHECK_EQ(r.boss->pending_damage, 0);
    CHECK_EQ(r.boss->spirit_threshold, 13000);
}
TEST(boss_519c_pending_damage_is_not_reused_on_extra_15_hit) {
    Rig r;
    CHECK(r.init());
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 48;
    r.boss->life = 1885.f;
    r.boss->spell_active = true;
    r.boss->spell_life = 13198;
    r.boss->phase_life_threshold = 0;
    r.boss->spirit_accum = 12510.f;
    r.boss->spirit_threshold = 13000;
    r.boss->pending_damage = 15;
    r.shot(0, 80, 15);
    r.battle.sim.enemy_hit(*r.boss);
    r.shot(0, 80, 15);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->life, 1879.f);
    CHECK_EQ(r.boss->spirit_accum, 12555.f);
    CHECK_EQ(r.boss->pending_damage, 0);
}
TEST(ship_callback_doubles_consumed_base_damage) {
    Rig r;
    CHECK(r.init());
    r.ship.hurtbox[0] = r.ship.hurtbox[1] = 16;
    r.shot(0, 200, 13);
    r.battle.sim.enemy_hit(r.ship);
    CHECK_EQ(r.boss->pending_damage, 13);
    CHECK_EQ(r.ship.life, 199974.f);
    CHECK_EQ(r.score.score, 1);
}
TEST(ship_callback_combines_base_and_extra_query_hits) {
    Rig r;
    CHECK(r.init());
    r.ship.hurtbox[0] = r.ship.hurtbox[1] = 16;
    r.shot(0, 200, 13);
    r.shot(70, 200, 27);
    r.battle.sim.enemy_hit(r.ship);
    CHECK_EQ(r.boss->pending_damage, 40);
    CHECK_EQ(r.ship.life, 199947.f);
    CHECK_EQ(r.score.score, 2);
    CHECK_EQ(r.battle.shots[0].state, 2);
    CHECK_EQ(r.battle.shots[1].state, 2);
}
TEST(ship_without_callback_applies_only_geometric_damage) {
    Rig r;
    CHECK(r.init());
    r.ship.damage_function = 0;
    r.ship.hurtbox[0] = r.ship.hurtbox[1] = 16;
    r.shot(0, 200, 13);
    r.shot(70, 200, 27);
    r.battle.sim.enemy_hit(r.ship);
    CHECK_EQ(r.ship.life, 199987.f);
    CHECK_EQ(r.boss->pending_damage, 0);
    CHECK_EQ(r.score.score, 1);
    CHECK_EQ(r.battle.shots[0].state, 2);
    CHECK_EQ(r.battle.shots[1].state, 1);
}
TEST(zero_life_boss_death_animation_still_queries_hits_without_losing_health) {
    Rig r;
    CHECK(r.init());
    r.boss->life = 0;
    r.boss->flags = 0x100009cu;
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 32;
    r.boss->spirit_accum = 990;
    r.boss->spirit_threshold = 1000;
    r.boss->hit_anim_timer = 30;
    r.shot(0, 80, 12);
    const auto before = r.battle.sim.random.index();
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.score.score, 1);
    CHECK_EQ(r.battle.shots[0].state, 2);
    CHECK_EQ(r.boss->life, 0.f);
    CHECK(!r.boss->dead);
    CHECK_EQ(r.boss->spirit_accum, 1002.f);
    CHECK_EQ(r.battle.sim.random.index(), before + 4);
    CHECK(r.battle.item_manager.spirits()[0].alive);
    CHECK_EQ(r.battle.item_manager.spirits()[0].kind, 1);
}
TEST(ship_160x32_boundary_and_query_damage_cap_match_446870) {
    for (float x : {81.f, 81.01f}) {
        Rig r;
        CHECK(r.init());
        r.shot(x, 200, 120);
        r.battle.sim.enemy_hit(r.ship);
        CHECK_EQ(r.boss->pending_damage, x == 81.f ? 80 : 0);
        CHECK_EQ(r.ship.life, x == 81.f ? 199920.f : 200000.f);
        CHECK_EQ(r.score.score, x == 81.f ? 1 : 0);
    }
}
TEST(ecl633_can_register_and_clear_ship_damage_callback) {
    StageSim sim;
    StageEnemy e;
    EclContext c;
    sim.commands.sim = &sim;
    sim.current = &e;
    for (u32 index : {1u, 0u}) {
        std::vector<u8> code;
        th13test::emit(code, 0, 633, 0, 255, 0, {index});
        c.instruction = reinterpret_cast<const EclInstruction*>(code.data());
        CHECK_EQ(sim.commands.execute(c, 633), 0);
        CHECK_EQ(e.damage_function, i32(index));
    }
}
TEST(spell_bomb_protection_divides_damage_by_30) {
    Rig r;
    CHECK(r.init());
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 48;
    r.boss->spirit_accum = 0;
    r.boss->spirit_threshold = 100000;
    r.battle.sim.spell.begin(1, "bomb-protection", 600, 4, 2, true);
    CHECK(r.battle.sim.spell.state().flags & kSpellBombProtection);
    r.shot(0, 80, 60);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->spirit_accum, 2.f);
    CHECK_EQ(r.boss->life, 998.f);
}
TEST(spell_without_bomb_protection_keeps_full_damage) {
    Rig r;
    CHECK(r.init());
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 48;
    r.boss->spirit_accum = 0;
    r.boss->spirit_threshold = 100000;
    r.battle.sim.spell.begin(1, "plain-spell", 600, 4, 2, false);
    CHECK(!(r.battle.sim.spell.state().flags & kSpellBombProtection));
    r.shot(0, 80, 60);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->spirit_accum, 60.f);
    CHECK_EQ(r.boss->life, 940.f);
}
TEST(ex_spell_109_bomb_on_boss_bit_enemy_deals_no_damage) {
    Rig r;
    CHECK(r.init());
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 48;
    r.boss->spirit_accum = 0;
    r.boss->spirit_threshold = 100000;
    r.boss->flags |= 0x1000000u;
    r.battle.sim.spell.begin(109, "ex-immune", 600, 4, 2, false);
    r.battle.sim.bomb_active = true;
    r.shot(0, 80, 60);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->spirit_accum, 0.f);
    CHECK_EQ(r.boss->life, 1000.f);
}
TEST(overdrive_spell_126_bomb_damage_still_applies) {
    Rig r;
    CHECK(r.init());
    r.boss->hurtbox[0] = r.boss->hurtbox[1] = 48;
    r.boss->spirit_accum = 0;
    r.boss->spirit_threshold = 100000;
    r.boss->flags |= 0x1000000u;
    r.battle.sim.spell.begin(126, "overdrive", 600, 4, 2, false);
    r.battle.sim.bomb_active = true;
    r.shot(0, 80, 60);
    r.battle.sim.enemy_hit(*r.boss);
    CHECK_EQ(r.boss->spirit_accum, 60.f);
    CHECK_EQ(r.boss->life, 940.f);
}
}
int main() { return th13test::run_all("Stage5 ship damage"); }
