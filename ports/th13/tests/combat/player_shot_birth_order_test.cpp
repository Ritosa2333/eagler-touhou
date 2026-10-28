#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"

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
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override { return true; }
    bool hud_sound(i32) override { return true; }
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
    // Actual4460d0, startup -1: emits0,1,..14 on updates0,2,..28;
    // rewind -14*.5 yields7, then emits7,8,... on updates29,31,...
    for(int frame=0;frame<36;++frame){
        fixture.battle.shots.clear();CHECK(fixture.battle.step(fire));
        const bool emitted=frame<=28?frame%2==0:frame%2==1;
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

int main() { return th13test::run_all("player shot birth order"); }
