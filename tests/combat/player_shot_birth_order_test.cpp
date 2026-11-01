#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include <algorithm>

using namespace th13;

namespace {
// H-BIRTH-ORDER: 00445de6 computes initial velocity and subtracts it from
// every launch position; 004464d0 updates even age-zero straight shots, and
// 00473780 quantizes the resulting position. Quarter-pixel launch coordinates
// expose an implementation that leaves straight shots unquantized at age zero.
class Provider final : public EclResourceProvider {
public:
    Provider() {
        std::vector<u8> main;
        th13test::emit(main, 0, 23, 0, 0xff, 0, {600});
        th13test::emit(main, 0, 10, 0, 0xff, 0, {});
        stage = th13test::build_scpt({{"main", main}}, {"enemy.anm", "st01enm.anm"});
    }
    bool read(const std::string& name, std::vector<u8>& out) override {
        if (name != "birth.ecl") return false;
        out = stage;
        return true;
    }
    bool animation(u32, const std::string& name) override {
        animations->push_back(name);
        return true;
    }
    std::vector<std::string>* animations = nullptr;
    std::vector<u8> stage;
};

struct Host final : StageBattleServices {
    std::vector<std::pair<i32, float>> sounds;
    std::vector<i32> stopped_sounds;
    std::vector<std::pair<char, i32>> audio_events;
    std::vector<std::pair<i32, float>> pans;
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
    bool hud_sound(i32 id, float x) override { sounds.emplace_back(id, x); audio_events.emplace_back('s', id); return true; }
    bool stop_sound(i32 id) override { stopped_sounds.push_back(id); audio_events.emplace_back('x', id); return true; }
    bool set_voice_pan(i32 id, float x) override { audio_events.emplace_back('p', id); pans.emplace_back(id, x); return true; }
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
        player.power = 0;
        provider.animations = &battle.sim.anim_list;
        if (!battle.load(provider, "birth.ecl", 2, host, progress, player, score)) return false;
        battle.sim.enemies.clear();
        return true;
    }

    void reach_first_launch() {
        for (int frame = 1; frame < 20; ++frame) CHECK(battle.step(idle));
        battle.set_player_pos(0.125f, 400.125f);
        GameInput fire;
        fire.update(1);
        CHECK(battle.step(fire));
        CHECK_EQ(battle.sim.frame, 20u);
    }
};
}  // namespace

TEST(age_zero_straight_shot_moves_then_quantizes) {
    Fixture fixture;
    CHECK(fixture.init());
    PlayerShot shot;
    shot.x = 0.125f;
    shot.y = 200.125f;
    shot.angle = 0;
    shot.speed = 6;
    shot.alive = true;
    fixture.battle.shots.push_back(shot);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    if (fixture.battle.shots.empty()) return;
    CHECK_EQ(fixture.battle.shots.front().age, 1);
    CHECK_EQ(fixture.battle.shots.front().x, 6.12f);
    CHECK_EQ(fixture.battle.shots.front().y, 200.12f);
}

TEST(sht_straight_launch_compensates_and_quantizes_on_birth_frame) {
    Fixture fixture;
    CHECK(fixture.init());
    ShtShotRecord record;
    record.interval = 255;
    record.angle = 0;
    record.speed = 6;
    fixture.battle.sht_shots_loaded = true;
    fixture.battle.sht_shots[0].push_back(record);
    fixture.reach_first_launch();
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    if (fixture.battle.shots.empty()) return;
    CHECK_EQ(fixture.battle.shots.front().age, 1);
    CHECK_EQ(fixture.battle.shots.front().x, 0.12f);
    CHECK_EQ(fixture.battle.shots.front().y, 400.12f);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots.front().age, 2);
    CHECK_EQ(fixture.battle.shots.front().x, 6.12f);
}

TEST(sht_shot_sound_uses_pre_update_birth_position) {
    Fixture fixture; CHECK(fixture.init());
    ShtShotRecord record; record.interval = 255; record.angle = 0;
    record.speed = 6; record.x = 10; record.sound = 7;
    fixture.battle.sht_shots_loaded = true;
    fixture.battle.sht_shots[0].push_back(record);
    fixture.reach_first_launch();
    CHECK_EQ(fixture.host.sounds.size(), size_t(1));
    CHECK_EQ(fixture.host.sounds[0].first, 7);
    CHECK_EQ(fixture.host.sounds[0].second, 4.125f); // offset - velocity + player x
}

TEST(marisa_beam_follows_option_and_grows_damage_rectangle) {
    Fixture fixture; CHECK(fixture.init());
    fixture.player.power = 100;
    // Exercise 446f20 with one active option and a no-cull type-2 beam.
    fixture.battle.option_count = 1;
    fixture.battle.options[0].active = true;
    fixture.battle.options[0].x = 10.f;
    fixture.battle.options[0].y = 200.f;
    PlayerShot beam; beam.alive = true; beam.state = 1; beam.type = 2;
    beam.behavior = 2; beam.anchor = 1; beam.anchor_power = 1;
    beam.angle = beam.target_angle = -1.5707964f; beam.speed = 0;
    beam.width = 0; beam.height = 16;
    fixture.battle.shots.push_back(beam);
    // Firing clock remains active after the startup gate.
    for (int i = 0; i < 20; ++i) CHECK(fixture.battle.step(fixture.idle));
    const auto& shot = fixture.battle.shots.front();
    CHECK_EQ(shot.width, 18.f);
    CHECK_EQ(shot.beam_visual_width, 18.f);
    CHECK_NEAR(shot.damage_y, shot.y - 9.f, 1e-5);
}

// H-REGISTRATION: 4457e0 allocates a record at the cursor, 443240 ages
// existing records before firing, and 446870 scans the registration rather
// than the presentation shot.  447230 shrinks the beam at every overlap but
// returns damage only on every fourth shot-age tick.
TEST(marisa_registration_callback_shrinks_beam_and_uses_shot_age) {
    Fixture fixture; CHECK(fixture.init());
    CHECK(fixture.battle.step(fixture.idle)); // advance the player clock gate
    i32 effects = 0;
    Vec3 effect_position{}, effect_displacement{};
    fixture.battle.beam_hit_effect = [&](const Vec3& pos, const Vec3& move, float) {
        ++effects; effect_position = pos; effect_displacement = move; return true;
    };
    PlayerShot beam;
    beam.alive = true; beam.state = 1; beam.generation = 7;
    beam.type = 2; beam.behavior = 2; beam.hit_behavior = 1;
    beam.x = 0; beam.y = 200; beam.width = 200; beam.height = 16;
    beam.beam_visual_width = 200;
    beam.registration_slot = 0;
    beam.age_clock.set(5); // changed age, not a damage tick
    fixture.battle.shots.push_back(beam);
    auto& reg = fixture.battle.damage_registrations[0];
    reg.active = true; reg.flags = 1; reg.generation = 7;
    reg.center_x = 0; reg.center_y = 100; reg.width = 200; reg.height = 16;
    reg.angle = -1.5707964f; reg.damage = 15; reg.clock.set(9999976);
    StageEnemy enemy;
    enemy.life = 100; enemy.position[0] = 0; enemy.position[1] = 100;
    enemy.hurtbox[0] = enemy.hurtbox[1] = 16;
    fixture.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 100.f);
    CHECK_EQ(reg.width, 76.f); // sqrt((0-0)^2 + (100-200)^2) - 24
    CHECK_EQ(fixture.battle.shots[0].width, 76.f);
    CHECK_EQ(fixture.battle.shots[0].beam_visual_width, 200.f);
    CHECK_EQ(reg.accumulated_damage, 15); // nominal damage even on zero return
    CHECK(reg.active);
    CHECK_EQ(effects, 0);
    CHECK_EQ(fixture.battle.shots[0].beam_interrupt, 2);
    reg.width = 200;
    fixture.battle.shots[0].age_clock.set(8);
    fixture.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 85.f);
    CHECK_EQ(reg.width, 76.f);
    CHECK_EQ(reg.accumulated_damage, 30);
    CHECK(reg.active);
    CHECK_EQ(effects, 1);
    // Beam angle defaults to -pi/2; the impact begins at its shortened tip.
    CHECK_NEAR(effect_position.x, 0.f, 1e-4f);
    CHECK_NEAR(effect_position.y, 124.f, 1e-4f);
    CHECK_NEAR(effect_displacement.y, -64.f, 1e-4f);
    reg.width = 200; reg.interval = 2; reg.clock.set(9999975);
    fixture.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 85.f);
    CHECK_EQ(reg.width, 200.f); // odd registration timer misses interval gate
    reg.clock.set(9999974); reg.threshold = 45;
    fixture.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 70.f);
    CHECK(!reg.active);
    CHECK_EQ(reg.damage, 0.f);
}

TEST(fired_shots_allocate_independent_registration_slots) {
    Fixture fixture; CHECK(fixture.init());
    ShtShotRecord record;
    record.interval = 255; record.angle = 0; record.speed = 0;
    record.width = 10; record.height = 12; record.damage = 15;
    fixture.battle.sht_shots_loaded = true;
    fixture.battle.sht_shots[0] = {record, record};
    fixture.reach_first_launch();
    CHECK_EQ(fixture.battle.shots.size(), size_t(2));
    CHECK_EQ(fixture.battle.shots[0].registration_slot, 0);
    CHECK_EQ(fixture.battle.shots[1].registration_slot, 1);
    const auto& first = fixture.battle.damage_registrations[0];
    CHECK(first.active);
    CHECK_EQ(first.clock.current, 9999999);
    CHECK_EQ(first.clock.previous, 9999998);
    CHECK_EQ(first.damage, 15.f);
    CHECK_EQ(first.width, 10.f);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(first.clock.current, 9999998);
    CHECK_EQ(first.clock.previous, 9999999);
}

TEST(marisa_beam_hit_distance_stores_square_sum_once) {
    // Retail 44728e..4472a2, replay frame188: separate f32 products
    // produce 0x430aae56, while the x87 sum followed by one f32 store
    // produces the observed 0x430aae55.
    Fixture fixture; CHECK(fixture.init());
    CHECK(fixture.battle.step(fixture.idle));
    PlayerShot beam; beam.alive = true; beam.state = 1; beam.generation = 7;
    beam.type = beam.behavior = 2; beam.hit_behavior = 1; beam.registration_slot = 0;
    beam.x = 41.540000915527344f; beam.y = 268.8800048828125f;
    beam.age_clock.set(124);
    fixture.battle.shots.push_back(beam);
    auto& reg = fixture.battle.damage_registrations[0];
    reg.active = true; reg.flags = 1; reg.generation = 7;
    reg.center_x = 56.189998626708984f; reg.center_y = 106.86000061035156f;
    reg.width = 200; reg.height = 16; reg.damage = 15;
    reg.clock.set(9999974);
    StageEnemy enemy; enemy.life = 100;
    enemy.position[0] = reg.center_x; enemy.position[1] = reg.center_y;
    enemy.hurtbox[0] = enemy.hurtbox[1] = 16;
    fixture.battle.sim.enemy_hit(enemy);
    CHECK_EQ(float_to_bits(reg.width), 1124773461u);
    CHECK_EQ(float_to_bits(fixture.battle.shots[0].width), 1124773461u);
}

TEST(marisa_contact_end_uses_label_three_without_retiring_beam) {
    Fixture fixture; CHECK(fixture.init());
    fixture.player.power = 100;
    ShtShotRecord record; record.interval = 255; record.anchor = 1;
    record.type = record.behavior = record.initializer = 2; record.hit_behavior = 1;
    record.angle = -1.5707964f; record.height = 16; record.damage = 15;
    fixture.battle.sht_shots_loaded = true;
    fixture.battle.sht_shots[1].push_back(record);
    fixture.reach_first_launch();
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    auto& beam = fixture.battle.shots[0];
    StageEnemy enemy; enemy.life = 100;
    enemy.position[0] = beam.damage_x; enemy.position[1] = beam.damage_y;
    enemy.hurtbox[0] = enemy.hurtbox[1] = 16;
    fixture.battle.sim.enemy_hit(enemy);
    CHECK_EQ(beam.beam_interrupt, 2);
    beam.beam_interrupt = 0; // presentation consumed label2
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots[0].beam_interrupt, 0);
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.shots[0].beam_interrupt, 3);
    CHECK_EQ(fixture.battle.shots[0].state, 1);
    CHECK(fixture.battle.damage_registrations[0].active);
}

TEST(reimu_orbs_share_registration_slots_and_age_after_birth) {
    Fixture fixture; CHECK(fixture.init());
    fixture.player.bombs = 2;
    // 4456e0 leaves the previous rectangle's dimensions and angle untouched.
    fixture.battle.damage_registrations[0].width = 123;
    fixture.battle.damage_registrations[0].height = 45;
    fixture.battle.damage_registrations[0].angle = .5f;
    GameInput bomb; bomb.update(2);
    CHECK(fixture.battle.step(bomb));
    for (int i = 0; i < 8; ++i) {
        const auto& orb = fixture.battle.bomb_orbs[size_t(i)];
        CHECK_EQ(orb.registration_slot, i);
        const auto& reg = fixture.battle.damage_registrations[size_t(i)];
        CHECK(reg.active);
        CHECK_EQ(reg.flags, 7u);
        CHECK_EQ(reg.radius, 56.f);
        CHECK_EQ(reg.clock.current, 9999);
        CHECK_EQ(reg.clock.previous, 9998);
        CHECK_EQ(reg.interval, 3);
        CHECK_EQ(reg.threshold, 300);
        if (i == 0) {
            CHECK_EQ(reg.width, 123.f); CHECK_EQ(reg.height, 45.f);
            CHECK_EQ(reg.angle, .5f);
        }
    }
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.damage_registrations[0].clock.current, 9998);
}

TEST(bomb_rectangles_share_pool_and_expire_on_next_player_update) {
    for (int character : {1, 2}) {
        Fixture fixture; fixture.progress.character = character;
        CHECK(fixture.init()); fixture.player.bombs = 2;
        // 4457e0 leaves the previous circle's radius/growth words untouched.
        fixture.battle.damage_registrations[0].radius = 56;
        fixture.battle.damage_registrations[0].radius_growth = 8;
        GameInput bomb; bomb.update(2); CHECK(fixture.battle.step(bomb));
        const int expected = character == 1 ? 3 : 5;
        for (int i = 0; i < expected; ++i) {
            const auto& reg = fixture.battle.damage_registrations[size_t(i)];
            CHECK(reg.active); CHECK_EQ(reg.flags, 5u);
            CHECK_EQ(reg.clock.current, 0); CHECK_EQ(reg.clock.previous, -1);
            if (i == 0) {
                CHECK_EQ(reg.radius, 56.f); CHECK_EQ(reg.radius_growth, 8.f);
            }
            CHECK_EQ(reg.damage, character == 1 && i == 0 ? 60.f :
                                 character == 1 ? 20.f : 10.f);
        }
        CHECK(fixture.battle.step(fixture.idle));
        // Marisa creates no new rectangles at age1. Sanae begins its next
        // scan at the previous cursor (slot4), then wraps through 5..8.
        const auto& first = fixture.battle.damage_registrations[0];
        CHECK(!first.active);
        if (character == 2) {
            CHECK(fixture.battle.damage_registrations[4].active);
            CHECK_EQ(fixture.battle.damage_registrations[4].clock.current, 0);
            CHECK(fixture.battle.damage_registrations[8].active);
        }
    }
}

TEST(youmu_slash_is_resolved_before_same_frame_damage_registration) {
    Fixture fixture; fixture.progress.character = 3;
    CHECK(fixture.init()); fixture.player.bombs = 2;
    int callbacks = 0;
    fixture.battle.youmu_bomb_update = [&](StageBattle& battle) {
        ++callbacks;
        CHECK_EQ(battle.youmu_slash_spawns_.size(), size_t(1));
        const auto spawn = battle.youmu_slash_spawns_[0];
        battle.youmu_bomb_.set_vm(spawn.slot, true, 0);
        return true;
    };
    GameInput bomb; bomb.update(2); CHECK(fixture.battle.step(bomb));
    CHECK_EQ(callbacks, 1);
    const auto& reg = fixture.battle.damage_registrations[0];
    CHECK(reg.active); CHECK_EQ(reg.flags, 5u);
    CHECK_EQ(reg.width, 96.f); CHECK_EQ(reg.height, 0.f);
    CHECK_EQ(reg.clock.current, 0); CHECK_EQ(reg.damage, 30.f);
}

TEST(youmu_new_option_uses_history_saturated_at_previous_option_count) {
    Fixture fixture; fixture.progress.character = 3;
    CHECK(fixture.init()); fixture.player.power = 200;
    fixture.battle.set_player_pos(0, 400);
    GameInput move; move.update(0x80);
    for (int i = 0; i < 24; ++i) CHECK(fixture.battle.step(move));
    CHECK_EQ(fixture.battle.player_x, 108.f);
    // count2 retains the history[16] tail. At step24 it is the position from
    // step7 (31.5), not the stale history[23] from a full 32-entry shift.
    fixture.player.power = 300;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.option_count, 3);
    CHECK_EQ(fixture.battle.options[2].x, 31.5f);
    CHECK_EQ(fixture.battle.options[2].y, 400.f);
}

TEST(youmu_history_advances_while_holding_direction_at_boundary) {
    Fixture fixture; fixture.progress.character = 3;
    CHECK(fixture.init()); fixture.player.power = 200;
    fixture.battle.set_player_pos(100, 400);
    GameInput move; move.update(0x80);
    for (int i = 0; i < 40; ++i) CHECK(fixture.battle.step(move));
    CHECK_EQ(fixture.battle.player_x, 184.f);
    fixture.player.power = 300;
    CHECK(fixture.battle.step(fixture.idle));
    CHECK_EQ(fixture.battle.options[2].x, 184.f);
}

TEST(youmu_focus_charge_releases_forty_frames_from_saved_origin) {
    Fixture fixture; fixture.progress.character = 3; CHECK(fixture.init());
    fixture.battle.sht_shots_loaded = true;
    ShtShotRecord record; record.interval = 1; record.speed = 0;
    record.angle = 0; record.damage = 10; record.width = record.height = 4;
    fixture.battle.sht_shots[5].push_back(record);
    for (int i = 0; i < 19; ++i) CHECK(fixture.battle.step(fixture.idle));
    fixture.host.sounds.clear(); fixture.host.stopped_sounds.clear(); fixture.host.audio_events.clear();
    fixture.battle.set_player_pos(48.f, 400.f);
    std::vector<int> effects;
    fixture.battle.youmu_charge_effect = [&](i32 action, const Vec3&) {
        effects.push_back(action); return true;
    };
    GameInput charging; charging.update(9); // held focus + fire
    for (int i = 0; i < 60; ++i) CHECK(fixture.battle.step(charging));
    CHECK(fixture.battle.shots.empty());
    CHECK_EQ(fixture.host.sounds.size(), size_t(2));
    CHECK_EQ(fixture.host.sounds[0].first, 30);
    CHECK_EQ(fixture.host.sounds[1].first, 55);
    CHECK_EQ(fixture.host.stopped_sounds.size(), size_t(1));
    CHECK_EQ(fixture.host.stopped_sounds[0], 30);
    // 446174/4461aa run in this order each charge frame; the ready
    // transition follows the final pair of direct buffer writes.
    const auto& events = fixture.host.audio_events;
    CHECK_EQ(events.size(), size_t(123));
    CHECK_EQ(fixture.host.pans.size(), size_t(120));
    CHECK(events[0] == std::make_pair('s', 30));
    for (size_t i = 0; i < 60; ++i) {
        CHECK(events[1 + 2 * i] == std::make_pair('p', 30));
        CHECK(events[2 + 2 * i] == std::make_pair('p', 55));
        CHECK_EQ(fixture.host.pans[2 * i].second, 48.f);
        CHECK_EQ(fixture.host.pans[2 * i + 1].second, 48.f);
    }
    CHECK(events[121] == std::make_pair('x', 30));
    CHECK(events[122] == std::make_pair('s', 55));
    CHECK_EQ(std::count(effects.begin(), effects.end(), 0), 1);
    CHECK_EQ(std::count(effects.begin(), effects.end(), 1), 1);

    GameInput release; release.update(8); CHECK(fixture.battle.step(release));
    CHECK(fixture.battle.shots.empty()); // 446208 sets -40; no shot yet
    const float origin = fixture.battle.player_x;
    CHECK_EQ(fixture.host.stopped_sounds.size(), size_t(3));
    CHECK_EQ(fixture.host.stopped_sounds[1], 30);
    CHECK_EQ(fixture.host.stopped_sounds[2], 55);
    GameInput move; move.update(0x88);
    for (int i = 0; i < 40; ++i) CHECK(fixture.battle.step(move));
    CHECK_EQ(fixture.battle.shots.size(), size_t(40));
    CHECK_EQ(fixture.battle.shots[0].x, origin);
    CHECK(fixture.battle.player_x > origin);
    CHECK(fixture.battle.step(move));
    CHECK_EQ(fixture.battle.shots.size(), size_t(40));
}

TEST(youmu_early_charge_release_produces_no_charge_shots) {
    Fixture fixture; fixture.progress.character = 3; CHECK(fixture.init());
    fixture.battle.sht_shots_loaded = true;
    ShtShotRecord record; record.interval = 1; record.speed = 0;
    fixture.battle.sht_shots[5].push_back(record);
    for (int i = 0; i < 19; ++i) CHECK(fixture.battle.step(fixture.idle));
    GameInput charging; charging.update(9);
    for (int i = 0; i < 20; ++i) CHECK(fixture.battle.step(charging));
    GameInput release; release.update(8);
    for (int i = 0; i < 45; ++i) CHECK(fixture.battle.step(release));
    CHECK(fixture.battle.shots.empty());
}

TEST(fallback_launch_uses_the_same_birth_frame_update) {
    Fixture fixture;
    CHECK(fixture.init());
    fixture.reach_first_launch();
    CHECK_EQ(fixture.battle.shots.size(), size_t(3));
    for (const auto& shot : fixture.battle.shots) {
        CHECK_EQ(shot.age, 1);
        CHECK_EQ(shot.y, 388.12f);
    }
}

TEST(deathbomb_resumes_the_frozen_shot_clock_instead_of_starting_a_new_burst) {
    Fixture fixture; CHECK(fixture.init());
    ShtShotRecord record;
    record.interval = 4; record.phase = 0; record.speed = 0;
    fixture.battle.sht_shots_loaded = true;
    fixture.battle.sht_shots[0].push_back(record);
    fixture.reach_first_launch(); // counter0 fires; next counter1
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    fixture.battle.player_phase = StageBattle::PlayerPhase::Deathbomb;
    fixture.battle.player_phase_frame = 0;
    GameInput held; held.update(1);
    for (int i = 0; i < 3; ++i) CHECK(fixture.battle.step(held));
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    held.update(3); CHECK(fixture.battle.step(held)); // Bomb resumes counter1
    CHECK(fixture.battle.player_phase == StageBattle::PlayerPhase::Active);
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    for (int i = 0; i < 2; ++i) { held.update(1); CHECK(fixture.battle.step(held)); }
    CHECK_EQ(fixture.battle.shots.size(), size_t(1));
    CHECK(fixture.battle.step(held)); // counter4, the next scheduled shot
    CHECK_EQ(fixture.battle.shots.size(), size_t(2));
}
TEST(slow_firing_uses_integer_clock_edges_and_rate_scaled_rewind) {
    Fixture fixture;CHECK(fixture.init());
    fixture.battle.sht_shots_loaded=true;
    ShtShotRecord record;record.interval=1;record.speed=0;record.script=-1;
    fixture.battle.sht_shots[0].push_back(record);
    for(int i=0;i<19;++i)CHECK(fixture.battle.step(fixture.idle));
    fixture.progress.rate=.5f;
    GameInput fire;fire.update(1);
    // 00443240:317-331 ticks the startup timer +0x690 through the same rate
    // pointer (+0x698 = &DAT_004c0a28) as the firing rewind, so at rate .5 the
    // 20-frame gate is reached on the second held update: startup 19 -> 19.5
    // (trunc 19) fails, 20.5 (trunc 20) passes. Counter0 then fires there and
    // every 2 updates afterwards (0,2,..14 -> updates 1,3,..29); rewind -14*.5
    // leaves 7, emitted on update 30 and then every 2 updates.
    for(int frame=0;frame<36;++frame){
        fixture.battle.shots.clear();CHECK(fixture.battle.step(fire));
        const bool emitted=frame<=29?frame%2==1:frame%2==0;
        CHECK_EQ(fixture.battle.shots.size(),emitted?size_t(1):size_t(0));
    }
}
TEST(homing_angle_matches_native_wrap_and_float_store_sequence) {
 Fixture f;CHECK(f.init());
 auto target=std::make_unique<StageEnemy>();target->serial=272;target->script_id=272;
 target->life=1000;target->flags=0x80;target->hurtbox[0]=target->hurtbox[1]=0;
 target->position[0]=target->vel.pos[0]=-31.6f;target->position[1]=target->vel.pos[1]=154;
 std::vector<u8> code;th13test::emit(code,10000,0,0,255,0,{});
 target->threads=std::make_unique<EclThreads>(f.battle.sim.program);
 CHECK(target->threads->main.bind_code(code.data(),u32(code.size()),0));
 f.battle.sim.enemies.push_back(std::move(target));
 PlayerShot s;s.alive=true;s.script=1;s.behavior=1;s.target_serial=272;s.age=17;
 s.x=-39.45f;s.y=316.41f;s.angle=-1.64242554f;s.speed=6.59999752f;f.battle.shots.push_back(s);
 for(int i=0;i<24;++i)CHECK(f.battle.step(f.idle));
 const auto& shot=f.battle.shots[0];
 // Original446cb0+4736a0+473780, explicit037f component fixture.
 CHECK_EQ(float_to_bits(shot.x),3234856960u);CHECK_EQ(float_to_bits(shot.y),1122795520u);
 CHECK_EQ(float_to_bits(shot.angle),3202060183u);CHECK_EQ(float_to_bits(shot.speed),1091777326u);
}

TEST(retail447590_accelerates_active_shots_before_birth_movement) {
    Fixture f; CHECK(f.init());
    PlayerShot shot; shot.alive = true; shot.script = 1; shot.behavior = 3;
    shot.x = 0; shot.y = 200; shot.angle = 0; shot.speed = 0;
    f.battle.shots.push_back(shot);
    CHECK(f.battle.step(f.idle));
    CHECK_EQ(f.battle.shots[0].speed, 1.f); CHECK_EQ(f.battle.shots[0].x, 1.f);
    CHECK(f.battle.step(f.idle));
    CHECK_EQ(f.battle.shots[0].speed, 2.f); CHECK_EQ(f.battle.shots[0].x, 3.f);
    f.battle.shots[0].state = 2;
    CHECK(f.battle.step(f.idle));
    CHECK_EQ(f.battle.shots[0].speed, 2.f);
}

TEST(retail447510_scales_active_speed_with_original_f64_constant) {
    Fixture f; CHECK(f.init());
    PlayerShot shot; shot.alive = true; shot.script = 1; shot.behavior = 5;
    shot.x = 0; shot.y = 200; shot.angle = 0; shot.speed = 10;
    f.battle.shots.push_back(shot);
    CHECK(f.battle.step(f.idle)); CHECK_EQ(f.battle.shots[0].speed, 8.f);
    CHECK(f.battle.step(f.idle)); CHECK_EQ(float_to_bits(f.battle.shots[0].speed), 0x40cccccdu);
    f.battle.shots[0].state = 2;
    CHECK(f.battle.step(f.idle)); CHECK_EQ(float_to_bits(f.battle.shots[0].speed), 0x40cccccdu);
}

// H-HIT-CALLBACKS: the actual table at 004bb8e8 maps SHT hit2 to
// 4475b0 and hit4 to447530. Verify return values, slot-scan order and the
// copied missile mover, rather than approximating both as generic retirement.
TEST(retail447530_returns_decremented_damage_and_keeps_projectile_live) {
    Fixture f; CHECK(f.init()); CHECK(f.battle.step(f.idle));
    PlayerShot shot; shot.alive = true; shot.generation = 17;
    shot.hit_behavior = 4; shot.damage = 3; shot.x = 0; shot.y = 200;
    shot.registration_slot = 0;
    f.battle.shots.push_back(shot);
    auto& reg = f.battle.damage_registrations[0];
    reg.active = true; reg.flags = 1; reg.generation = 17;
    reg.center_y = 200; reg.width = reg.height = 16; reg.damage = 3;
    reg.clock.set(9999976);
    StageEnemy enemy; enemy.life = 100; enemy.position[1] = 200;
    enemy.hurtbox[0] = enemy.hurtbox[1] = 16;
    f.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 98.f); CHECK_EQ(f.battle.shots[0].damage, 2.f);
    CHECK_EQ(reg.accumulated_damage, 3); // nominal registration is unchanged
    f.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 97.f); CHECK_EQ(f.battle.shots[0].damage, 1.f);
    f.battle.sim.enemy_hit(enemy);
    CHECK_EQ(enemy.life, 96.f); CHECK_EQ(f.battle.shots[0].damage, 1.f);
    CHECK_EQ(reg.accumulated_damage, 9);
    CHECK(reg.active); CHECK_EQ(f.battle.shots[0].state, 1);
    CHECK_EQ(f.battle.shots[0].registration_slot, 0);
}

TEST(retail4475b0_allocates_before_retirement_and_obeys_slot_scan_order) {
    for (int old_slot : {0, 255}) {
        Fixture f; CHECK(f.init()); CHECK(f.battle.step(f.idle));
        PlayerShot shot; shot.alive = true; shot.generation = 19;
        shot.hit_behavior = 2; shot.damage = 10; shot.x = 0; shot.y = 200;
        shot.angle = 0; shot.speed = 8; shot.registration_slot = old_slot;
        f.battle.shots.push_back(shot);
        auto& reg = f.battle.damage_registrations[size_t(old_slot)];
        reg.active = true; reg.flags = 1; reg.generation = 19;
        reg.center_y = 200; reg.width = reg.height = 16; reg.damage = 10;
        reg.clock.set(9999976);
        f.battle.damage_registration_cursor = old_slot;
        StageEnemy enemy; enemy.life = 100; enemy.position[1] = 200;
        enemy.hurtbox[0] = enemy.hurtbox[1] = 16;
        f.battle.sim.enemy_hit(enemy);
        const int new_slot = (old_slot + 1) & 255;
        auto& blast = f.battle.damage_registrations[size_t(new_slot)];
        CHECK(!reg.active); CHECK(blast.active);
        CHECK_EQ(enemy.life, old_slot == 0 ? 80.f : 90.f);
        CHECK_EQ(blast.accumulated_damage, old_slot == 0 ? 10 : 0);
        CHECK_EQ(blast.flags, 3u); CHECK_EQ(blast.interval, 4);
        CHECK_EQ(blast.radius, 24.f); CHECK_EQ(blast.radius_growth, 1.f);
        CHECK_EQ(blast.clock.current, 20); CHECK_EQ(blast.clock.previous, 19);
        CHECK_EQ(blast.center_y, 200.f); CHECK_EQ(blast.travel_speed, 2.f);
        CHECK_EQ(f.battle.shots[0].state, 2); CHECK_EQ(f.battle.shots[0].speed, 2.f);
        CHECK_EQ(f.battle.shots[0].registration_slot, -1);
        CHECK_EQ(f.host.sounds.size(), size_t(1));
        CHECK_EQ(f.host.sounds[0].first, 65); CHECK_EQ(f.host.sounds[0].second, 0.f);
        CHECK(f.battle.step(f.idle));
        CHECK_EQ(blast.center_x, 2.f); CHECK_EQ(blast.center_y, 200.f);
        CHECK_EQ(blast.radius, 25.f); CHECK_EQ(blast.clock.current, 19);
        for (int i = 1; i < 20; ++i) CHECK(f.battle.step(f.idle));
        CHECK(!blast.active); CHECK_EQ(blast.clock.current, 0);
        CHECK_EQ(blast.center_x, 40.f); CHECK_EQ(blast.radius, 44.f);
    }
}

// H-HIT-RETIRE: 00446c8d/004474eb clear shot+0x98 after disabling the
// record, so a later real .sht allocation may reuse it without the old
// shot's retirement (00446542, no owner/generation check) clearing it.
TEST(retail446c8d_generic_hit_clears_slot_before_reallocation) {
    for (i32 hit_behavior : {0, 3}) {
        for (i32 old_slot : {0, 255}) {
            Fixture f; CHECK(f.init());
            for (int i = 0; i < 19; ++i) CHECK(f.battle.step(f.idle));
            PlayerShot shot;
            shot.alive = true; shot.state = 1; shot.generation = 19;
            shot.hit_behavior = hit_behavior;
            shot.damage = 10; shot.speed = 8; shot.x = 0; shot.y = 200;
            shot.angle = 0; shot.registration_slot = old_slot;
            f.battle.shots.push_back(shot);
            auto& old_reg = f.battle.damage_registrations[size_t(old_slot)];
            old_reg = DamageRegistration{};
            old_reg.active = true; old_reg.flags = 1; old_reg.generation = 19;
            old_reg.center_x = 0; old_reg.center_y = 200;
            old_reg.width = old_reg.height = 16; old_reg.angle = 0;
            old_reg.damage = 10; old_reg.clock.set(9999976);
            StageEnemy enemy;
            enemy.life = 100; enemy.position[0] = 0; enemy.position[1] = 200;
            enemy.hurtbox[0] = enemy.hurtbox[1] = 16;
            f.battle.sim.enemy_hit(enemy);
            CHECK_EQ(enemy.life, 90.f);
            CHECK_EQ(f.battle.shots[0].state, 2);
            CHECK_EQ(f.battle.shots[0].registration_slot, -1);
            CHECK_EQ(f.battle.shots[0].damage, 10.f);
            CHECK_EQ(f.battle.shots[0].speed, hit_behavior == 3 ? 3.f : 1.f);
            CHECK(!old_reg.active); CHECK_EQ(old_reg.flags & 1u, 0u);
            CHECK_EQ(old_reg.damage, 0.f);
            // Boundary: every other record is held active so the next real
            // allocation must land on the freed slot regardless of whether
            // the pool cursor names the next or the last used record.
            for (i32 i = 0; i < 256; ++i) {
                if (i == old_slot) continue;
                auto& area = f.battle.damage_registrations[size_t(i)];
                area = DamageRegistration{};
                area.active = true; area.flags = 1;
                area.owner = DamageRegistration::Owner::Area;
                area.center_x = area.center_y = -100000.f;
                area.width = area.height = 1;
            }
            f.battle.damage_registration_cursor = (old_slot + 255) & 255;
            ShtShotRecord record;
            record.interval = 255; record.phase = 0; record.angle = 0;
            record.speed = 0; record.damage = 10;
            record.width = record.height = 16;
            f.battle.sht_shots_loaded = true;
            f.battle.sht_shots[0].push_back(record);
            GameInput fire; fire.update(1);
            CHECK(f.battle.step(fire));
            CHECK_EQ(f.battle.shots.size(), size_t(2));
            if (f.battle.shots.size() != size_t(2)) continue;
            const u32 new_generation = f.battle.shots[1].generation;
            CHECK_EQ(f.battle.shots[1].state, 1);
            CHECK_EQ(f.battle.shots[1].registration_slot, old_slot);
            auto& new_reg = f.battle.damage_registrations[size_t(old_slot)];
            CHECK(new_reg.active); CHECK_EQ(new_reg.flags & 1u, 1u);
            CHECK_EQ(new_reg.generation, new_generation);
            CHECK(new_generation != 19u);
            f.battle.sht_shots[0].clear();
            f.battle.shots[0].animation_finished = true;
            CHECK(f.battle.step(f.idle));
            CHECK(new_reg.active);
            CHECK_EQ(new_reg.flags & 1u, 1u);
        }
    }
}

// Stage6 21976: an owner that already left state1 must not keep a stale
// shot+0x98. 004464d0's later animation retirement has no owner/generation
// check, so sync must clear the owner pointer when it disables the record;
// otherwise it can erase a respawn area that reused the slot (native slot15
// lived 21968..21996 and supplied 17 further hit-score frames).
TEST(inactive_owner_registration_sync_clears_stale_shot_pointer) {
    Fixture f; CHECK(f.init()); CHECK(f.battle.step(f.idle));
    PlayerShot shot;
    shot.alive = true; shot.state = 2; shot.generation = 19;
    shot.x = 0; shot.y = 200; shot.registration_slot = 0;
    f.battle.shots.push_back(shot);
    auto& old_reg = f.battle.damage_registrations[0];
    old_reg = DamageRegistration{};
    old_reg.active = true; old_reg.flags = 1; old_reg.generation = 19;
    old_reg.owner = DamageRegistration::Owner::Shot;
    old_reg.center_x = 0; old_reg.center_y = 200;
    old_reg.width = old_reg.height = 16; old_reg.damage = 15;
    old_reg.clock.set(30); old_reg.interval = 1; old_reg.threshold = 9999999;
    CHECK(f.battle.step(f.idle));
    CHECK(f.battle.shots[0].alive);
    CHECK_EQ(f.battle.shots[0].registration_slot, -1);
    CHECK(!old_reg.active);

    old_reg = DamageRegistration{};
    old_reg.active = true; old_reg.flags = 1;
    old_reg.owner = DamageRegistration::Owner::Area;
    old_reg.center_x = 0; old_reg.center_y = 200;
    old_reg.radius = 56; old_reg.clock.set(30);
    old_reg.interval = 1; old_reg.damage = 150;
    old_reg.threshold = 9999999;

    f.battle.shots[0].animation_finished = true;
    CHECK(f.battle.step(f.idle));
    CHECK(old_reg.active);
    CHECK_EQ(old_reg.flags & 1u, 1u);
}

// H-BEAM-RETIRE: 004471b7 frees the beam record but intentionally leaves
// shot+0x98 stale; 00446542 later clears the enable bit of a reallocated
// record with no owner/generation guard.  Preserve that original exception.
TEST(retail4471b7_beam_termination_keeps_stale_slot_for_retirement) {
    Fixture f; CHECK(f.init());
    f.battle.option_count = 0;
    f.battle.options[0].active = true;
    f.battle.options[0].x = 10.f; f.battle.options[0].y = 200.f;
    PlayerShot beam;
    beam.alive = true; beam.state = 1; beam.generation = 5;
    beam.type = 2; beam.behavior = 2; beam.hit_behavior = 1;
    beam.anchor = 1; beam.anchor_power = 0;
    beam.angle = beam.target_angle = -1.5707964f; beam.speed = 0;
    beam.width = 0; beam.height = 16; beam.damage = 15;
    beam.registration_slot = 0;
    f.battle.shots.push_back(beam);
    auto& old_reg = f.battle.damage_registrations[0];
    old_reg = DamageRegistration{};
    old_reg.active = true; old_reg.flags = 1; old_reg.generation = 5;
    old_reg.center_x = 10; old_reg.center_y = 200;
    old_reg.width = 0; old_reg.height = 16; old_reg.angle = -1.5707964f;
    old_reg.damage = 15; old_reg.clock.set(9999976);
    for (int i = 0; i < 19; ++i) CHECK(f.battle.step(f.idle));
    CHECK_EQ(f.battle.shots[0].state, 2);
    CHECK_EQ(f.battle.shots[0].registration_slot, 0);
    CHECK(!old_reg.active);
    for (i32 i = 1; i < 256; ++i) {
        auto& area = f.battle.damage_registrations[size_t(i)];
        area = DamageRegistration{};
        area.active = true; area.flags = 1;
        area.owner = DamageRegistration::Owner::Area;
        area.center_x = area.center_y = -100000.f;
        area.width = area.height = 1;
    }
    f.battle.damage_registration_cursor = 255;
    ShtShotRecord record;
    record.interval = 255; record.phase = 0; record.angle = 0;
    record.speed = 0; record.damage = 10;
    record.width = record.height = 16;
    f.battle.sht_shots_loaded = true;
    f.battle.sht_shots[0].push_back(record);
    GameInput fire; fire.update(1);
    CHECK(f.battle.step(fire));
    CHECK_EQ(f.battle.shots.size(), size_t(2));
    if (f.battle.shots.size() != size_t(2)) return;
    const u32 new_generation = f.battle.shots[1].generation;
    CHECK_EQ(f.battle.shots[1].state, 1);
    CHECK_EQ(f.battle.shots[1].registration_slot, 0);
    auto& new_reg = f.battle.damage_registrations[0];
    CHECK(new_reg.active); CHECK_EQ(new_reg.flags & 1u, 1u);
    CHECK_EQ(new_reg.generation, new_generation);
    f.battle.sht_shots[0].clear();
    f.battle.shots[0].animation_finished = true;
    CHECK(f.battle.step(f.idle));
    CHECK(!new_reg.active);
    CHECK_EQ(new_reg.flags & 1u, 0u);
}

int main() { return th13test::run_all("player shot birth order"); }
