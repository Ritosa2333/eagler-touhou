#include "ecl_test_fixtures.hpp"
#include "game/StageSim.hpp"
using namespace th13;
TEST(enemy_count_matches_original_flag_and_invulnerability_predicate) {
    StageSim sim; sim.gvars.sim = &sim;
    sim.enemies.push_back(std::make_unique<StageEnemy>());
    auto& enemy = *sim.enemies[0];
    for (u32 flags = 0; flags < 64; ++flags) {
        for (i32 invulnerable : {-1, 0, 1, 120}) {
            enemy.flags = flags; enemy.invulnerable = invulnerable;
            i32 integer = -1; float floating = -1;
            CHECK(sim.gvars.integer(-9908, integer));
            CHECK(sim.gvars.floating(-9908, floating));
            // 256 native00417e90 cases; bits0/4/5 exclude, other bits do not.
            const i32 expected = (flags & 0x31u) == 0 && invulnerable < 1;
            CHECK_EQ(integer, expected); CHECK_EQ(floating, float(expected));
        }
    }
}
TEST(enemy_count_uses_link_membership_until_retirement_not_port_dead_marker) {
    StageSim sim; sim.gvars.sim = &sim;
    i32 value = -1; CHECK(sim.gvars.integer(-9908, value)); CHECK_EQ(value, 0);
    auto enemy = std::make_unique<StageEnemy>();
    enemy->flags = 0; enemy->life = 0; enemy->dead = true;
    sim.enemies.push_back(std::move(enemy));
    CHECK(sim.gvars.integer(-9908, value)); CHECK_EQ(value, 1);
    sim.enemies.clear();
    CHECK(sim.gvars.integer(-9908, value)); CHECK_EQ(value, 0);
}
int main() { return th13test::run_all("enemy count global"); }
