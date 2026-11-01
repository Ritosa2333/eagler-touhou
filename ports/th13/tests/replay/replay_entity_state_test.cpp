#include "../harness/test_framework.hpp"
#include "game/ReplayEntityState.hpp"

#include <memory>

using namespace th13;

TEST(entity_sample_keeps_storage_identity_and_raw_float_bits) {
    StageBattle battle;
    battle.sim.frame = 17;
    battle.set_player_pos(-1.25f, 200.5f);
    battle.shots.push_back(PlayerShot{});
    battle.shots[0].alive = true; battle.shots[0].generation = 9;
    battle.shots[0].x = 1.0f; battle.shots[0].y = -2.0f; battle.shots[0].damage = 12.5f;
    StageBullet bullet; bullet.alive = true; bullet.generation = 12; bullet.source_enemy = 4;
    bullet.x = 3.0f; bullet.y = 4.0f; battle.sim.bullets.push_back(bullet);
    const auto sample = sample_replay_entities(battle);
    CHECK_EQ(sample.frame, 17u);
    CHECK_EQ(sample.shots.size(), 1u); CHECK_EQ(sample.shots[0].index, 0u);
    CHECK_EQ(sample.shots[0].generation, 9u);
    CHECK_EQ(sample.bullets.size(), 1u); CHECK_EQ(sample.bullets[0].index, 0u);
    CHECK_EQ(sample.bullets[0].source_enemy, 4u);
    CHECK(sample.json().find("\"schema\":\"th13-entity-state-v1\"") != std::string::npos);
    CHECK(sample.json().find("\"damage_bits\":1095237632") != std::string::npos);
}

TEST(entity_sample_exports_spirits_separately_from_items) {
    StageBattle battle;
    auto& spirit = battle.item_manager.spirits()[3];
    spirit.alive = true; spirit.serial = 77; spirit.state = 2; spirit.kind = 4;
    spirit.x = 6.0f; spirit.y = 7.0f;
    const auto sample = sample_replay_entities(battle);
    CHECK(sample.items.empty()); CHECK_EQ(sample.spirits.size(), 1u);
    CHECK_EQ(sample.spirits[0].slot, 3u); CHECK_EQ(sample.spirits[0].serial, 77u);
    CHECK(sample.json().find("\"spirits\":[") != std::string::npos);
}

TEST(entity_sample_exports_live_circle_registration_geometry) {
    StageBattle battle;
    auto& reg = battle.damage_registrations[7];
    reg.active = true; reg.flags = 3;
    reg.owner = DamageRegistration::Owner::Area;
    reg.radius = 48; reg.radius_growth = 16;
    const auto sample = sample_replay_entities(battle);
    CHECK_EQ(sample.damage_registrations.size(), 1u);
    CHECK_EQ(sample.damage_registrations[0].slot, 7u);
    CHECK_EQ(sample.damage_registrations[0].owner, 1u);
    CHECK_EQ(sample.damage_registrations[0].radius_bits, 1111490560u);
    CHECK_EQ(sample.damage_registrations[0].radius_growth_bits, 1098907648u);
    CHECK(sample.json().find("\"radius_growth_bits\":1098907648") != std::string::npos);
}

int main() { return th13test::run_all("replay entity state"); }
