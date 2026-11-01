#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

using namespace th13;
namespace {
struct Host : StageBattleServices {
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
};
struct Provider : EclResourceProvider {
    std::vector<u8> sht = std::vector<u8>(0xf5, 0);
    std::vector<u8> ecl;
    std::vector<std::string>* names = nullptr;
    Provider() {
        sht[2] = 1; sht[0x20] = 4; sht[0xf4] = 0xff;
        std::vector<u8> instructions;
        th13test::emit(instructions, 0, 23, 0, 255, 0, {1000});
        th13test::emit(instructions, 0, 10, 0, 255, 0, {});
        ecl = th13test::build_scpt({{"main", instructions}}, {"enemy.anm", "stage.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        out = name == "player.sht" ? sht : ecl; return true;
    }
    bool animation(u32, const std::string& name) override { names->push_back(name); return true; }
};

TEST(cold_entry_uses_stage_or_spell_power_before_replay_restore) {
    for (int scenario = 0; scenario < 4; ++scenario) {
        Provider provider; SessionState progress; HudPlayerState player; StageBattle battle;
        progress.stage = scenario == 1 ? 2 : 1;
        progress.run_mode_flags = scenario == 2 ? 0x20 : 0;
        if (scenario == 3) provider.sht[0x20] = 2;
        CHECK(battle.prepare_player(provider, progress, player, "player.sht", true));
        CHECK_EQ(player.power_step, 100);
        CHECK_EQ(player.max_power, scenario == 3 ? 200 : 400);
        const int expected = scenario == 1 || scenario == 2 ? 4 : 1;
        CHECK_EQ(player.power, expected * 100);
        CHECK_EQ(battle.option_count, expected);
    }
}

TEST(restored_power_rebuilds_at_player_activation_not_stage_ecl_start) {
    Provider provider; Host host; SessionState progress; HudPlayerState player;
    HudScoreState score; StageBattle battle;
    provider.names = &battle.sim.anim_list;
    progress.stage = 2;
    CHECK(battle.prepare_player(provider, progress, player, "player.sht", true));
    player.power = 293; // 447900 stage header restore
    const bool loaded = battle.load(provider, "stage.ecl", 2, host, progress, player, score);
    if (!loaded) std::printf("load error: %s\n", battle.error().c_str());
    CHECK(loaded);
    CHECK_EQ(player.power, 293);
    CHECK_EQ(battle.option_count, 4);
    battle.restore_replay_player(128, 51200, false); // 448ec0 / 424d0
    CHECK_EQ(battle.option_count, 2);
    CHECK_EQ(battle.options[0].x, 1.f);
}

TEST(continuing_stage_does_not_apply_cold_entry_power) {
    Provider provider; Host host; SessionState progress; HudPlayerState player;
    HudScoreState score; StagePlayerState persistent;
    progress.stage = 2; player.power = 293;
    StageBattle battle(nullptr, &persistent);
    provider.names = &battle.sim.anim_list;
    const bool loaded = battle.load(provider, "stage.ecl", 2, host, progress, player, score,
                                    nullptr, "player.sht");
    if (!loaded) std::printf("load error: %s\n", battle.error().c_str());
    CHECK(loaded);
    CHECK_EQ(player.power, 293);
    CHECK_EQ(battle.option_count, 2);
}

TEST(power_item_rebuilds_options_during_collection_without_extra_movement) {
    Provider provider; Host host; SessionState progress; HudPlayerState player;
    HudScoreState score; StageBattle battle;
    provider.names = &battle.sim.anim_list;
    progress.character = 3; player.power = 299;
    CHECK(battle.load(provider, "stage.ecl", 2, host, progress, player, score,
                      nullptr, "player.sht"));
    int callbacks = 0;
    battle.option_power_changed = [&](StageBattle& active) {
        ++callbacks;
        CHECK_EQ(player.power, 300);
        CHECK_EQ(active.option_count, 3);
        CHECK_EQ(active.options[0].x, active.player_x);
        return true;
    };
    battle.item_manager.spawn_item(0, 400, 0, 1, 0, 0);
    GameInput input;
    CHECK(battle.step(input));
    CHECK_EQ(callbacks, 1);
    CHECK_EQ(battle.option_count, 3);
}

TEST(power_rebuild_snaps_options_on_the_next_player_movement) {
    Provider provider; Host host; SessionState progress; HudPlayerState player;
    HudScoreState score; StageBattle battle; provider.names = &battle.sim.anim_list;
    const float diagonal_focus_speed = 2.f;
    std::memcpy(provider.sht.data() + 0x1c, &diagonal_focus_speed, sizeof diagonal_focus_speed);
    progress.character = 2; player.power = 299;
    CHECK(battle.load(provider, "stage.ecl", 2, host, progress, player, score,
                      nullptr, "player.sht"));
    battle.sim.enemies.clear(); battle.set_player_pos(0, 200);
    battle.item_manager.spawn_item(0, 200, 0, 1, 0, 0);
    CHECK(battle.step(GameInput{}));
    CHECK_EQ(player.power, 300);
    GameInput input; input.update(0x58); // focus + up + left
    CHECK(battle.step(input));
    CHECK_EQ(battle.player_x, -2.f);
    CHECK_EQ(battle.player_y, 198.f);
    CHECK_EQ(battle.options[0].x, battle.player_x);
    CHECK_EQ(battle.options[0].y, battle.player_y);
}

TEST(sanae_shot_and_marisa_live_beam_follow_snapped_option_after_power_up) {
    for (int character : {1, 2}) {
        Provider provider; Host host; SessionState progress; HudPlayerState player;
        HudScoreState score; StagePlayerState state; StageBattle battle(nullptr, &state);
        provider.names = &battle.sim.anim_list;
        const float diagonal_focus_speed = 2.f;
        std::memcpy(provider.sht.data() + 0x1c, &diagonal_focus_speed, sizeof diagonal_focus_speed);
        progress.character = character; player.power = 199;
        CHECK(battle.load(provider, "stage.ecl", 2, host, progress, player, score,
                          nullptr, "player.sht"));
        battle.sim.enemies.clear(); battle.set_player_pos(0, 200);
        state.startup_clock.set(25);
        if (character == 1) {
            PlayerShot beam; beam.alive = true; beam.state = 1; beam.type = 2;
            beam.behavior = 2; beam.anchor = 1; beam.anchor_power = 1;
            beam.angle = beam.target_angle = -1.5707964f; beam.speed = 0;
            battle.shots.push_back(beam);
        } else {
            ShtShotRecord shot; shot.interval = 1; shot.anchor = 1; shot.speed = 0;
            battle.sht_shots_loaded = true;
            battle.sht_shots[7] = {shot}; // focus, power level2
        }
        battle.item_manager.spawn_item(0, 200, 0, 1, 0, 0);
        CHECK(battle.step(GameInput{}));
        CHECK_EQ(player.power, 200);
        GameInput input; input.update(0x59); // fire, focus, up, left
        CHECK(battle.step(input));
        CHECK(!battle.shots.empty());
        if (!battle.shots.empty()) {
            CHECK_EQ(battle.shots[0].x, battle.options[0].x);
            CHECK_EQ(battle.shots[0].y, battle.options[0].y);
        }
        CHECK_EQ(battle.options[0].x, battle.player_x);
        CHECK_EQ(battle.options[0].y, battle.player_y);
    }
}

TEST(youmu_power_rebuild_preserves_existing_movement_history) {
    Provider provider; Host host; SessionState progress; HudPlayerState player;
    HudScoreState score; StageBattle battle; provider.names = &battle.sim.anim_list;
    const float speed = 1.f; std::memcpy(provider.sht.data() + 0x10, &speed, sizeof speed);
    progress.character = 3; player.power = 199;
    CHECK(battle.load(provider, "stage.ecl", 2, host, progress, player, score,
                      nullptr, "player.sht"));
    battle.sim.enemies.clear(); battle.set_player_pos(0, 200);
    GameInput input;
    for (int i = 0; i < 20; ++i) { input.update(0x80); CHECK(battle.step(input)); }
    CHECK_EQ(battle.player_x, 20.f);
    battle.item_manager.spawn_item(20, 200, 0, 1, 0, 0);
    input.update(0); CHECK(battle.step(input));
    CHECK_EQ(player.power, 200);
    input.update(0x80); CHECK(battle.step(input));
    CHECK_EQ(battle.player_x, 21.f);
    CHECK_EQ(battle.options[0].x, 14.f);
}

TEST(bullet_contact_keeps_fading_node_and_checks_graze_in_active_order) {
    Provider provider; Host host; SessionState progress; HudPlayerState player;
    HudScoreState score; StageBattle battle; provider.names = &battle.sim.anim_list;
    CHECK(battle.load(provider, "stage.ecl", 2, host, progress, player, score));
    battle.sim.enemies.clear(); battle.set_player_pos(0, 200); battle.invulnerable = 0;
    StageBullet graze; graze.x = 25; graze.y = 200;
    StageBullet hit; hit.x = 0; hit.y = 200;
    battle.sim.bullets = {graze, hit};
    int fading = 0;
    battle.sim.bullet_cancel_effect = [&](i32, const Vec3&, const Vec3&) { ++fading; return true; };
    CHECK(battle.step(GameInput{}));
    CHECK(battle.player_phase == StageBattle::PlayerPhase::Deathbomb);
    CHECK_EQ(progress.graze, 1);
    CHECK_EQ(fading, 1);
    CHECK_EQ(battle.sim.bullets.size(), size_t(2));
    CHECK(battle.sim.bullets[1].alive);
    CHECK_EQ(battle.sim.bullets[1].state, 3);
    CHECK_EQ(battle.sim.bullets[1].animation_interrupt, 1);
}

TEST(delayed_bullet_contact_uses_stationary_particle_and_protection_retains_node) {
    for (bool protected_bullet : {false, true}) {
        Provider provider; Host host; SessionState progress; HudPlayerState player;
        HudScoreState score; StageBattle battle; provider.names = &battle.sim.anim_list;
        CHECK(battle.load(provider, "stage.ecl", 2, host, progress, player, score));
        battle.sim.enemies.clear(); battle.set_player_pos(0, 200); battle.invulnerable = 0;
        StageBullet bullet; bullet.x = 0; bullet.y = 200; bullet.state = 2;
        bullet.animation_ready = false; bullet.state_age = 8; bullet.state_time = 8;
        bullet.cancel_protection = protected_bullet ? 5 : 0;
        battle.sim.bullets.push_back(bullet);
        int stationary = 0, moving = 0;
        battle.sim.stationary_cancel_effect = [&](i32, const Vec3&) { ++stationary; return true; };
        battle.sim.bullet_cancel_effect = [&](i32, const Vec3&, const Vec3&) { ++moving; return true; };
        CHECK(battle.step(GameInput{}));
        CHECK(battle.player_phase == StageBattle::PlayerPhase::Deathbomb);
        CHECK_EQ(moving, 0);
        CHECK_EQ(stationary, protected_bullet ? 0 : 1);
        CHECK(battle.sim.bullets[0].alive);
        CHECK_EQ(battle.sim.bullets[0].state, protected_bullet ? 2 : 3);
    }
}
}
int main() { return th13test::run_all("player-stage-entry"); }
