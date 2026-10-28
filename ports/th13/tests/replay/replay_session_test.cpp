#include "../harness/test_framework.hpp"
#include "game/ReplaySession.hpp"
#include "game/StageBattle.hpp"
#include "../ecl/ecl_test_fixtures.hpp"

namespace {
using namespace th13;

void put32(std::vector<u8>& bytes, u32 offset, i32 value) {
    for (u32 byte = 0; byte < 4; ++byte) bytes[offset + byte] = u8(u32(value) >> (8 * byte));
}

ReplayStage stage(u16 number, u16 seed, i32 score, i32 power) {
    ReplayStage result;
    result.number = number;
    result.stamp = seed;
    result.header.resize(0xC4);
    result.header[0] = u8(number);
    result.header[2] = u8(seed);
    result.header[3] = u8(seed >> 8);
    put32(result.header, 0x14, 2);
    put32(result.header, 0x1C, score);
    put32(result.header, 0x20, 1);
    put32(result.header, 0x38, 1000000);
    put32(result.header, 0x40, 25000000);
    put32(result.header, 0x44, power);
    put32(result.header, 0x48, 400);
    put32(result.header, 0x4C, 100);
    put32(result.header, 0x50, 2);
    put32(result.header, 0x5C, 2);
    return result;
}

TEST(replay_stage_lookup_uses_recorded_stage_number) {
    ReplayFile file;
    file.stages.push_back(stage(3, 123, 30, 100));
    file.stages.push_back(stage(4, 456, 40, 200));
    CHECK_EQ(ReplaySession::stage_index(file, 3), 0);
    CHECK_EQ(ReplaySession::stage_index(file, 4), 1);
    CHECK_EQ(ReplaySession::stage_index(file, 1), -1);
    CHECK_EQ(ReplaySession::stage_index(file, 5), -1);
}

TEST(replay_next_stage_restores_its_own_seed_and_resources) {
    ReplayFile file;
    file.stages.push_back(stage(1, 60189, 0, 100));
    file.stages.push_back(stage(2, 347, 87654, 387));
    SessionState progress;
    HudPlayerState player;
    HudScoreState score;
    GameRandom random, visual;
    const ReplayRestoreTarget target{&progress, &player, &score, &random, &visual};
    ReplayRestoreLog log;
    std::string error;
    CHECK(ReplaySession::restore_stage(file, 0, target, log, error));
    CHECK_EQ(log.seed, u16(60189));
    CHECK_EQ(score.score, 0);
    CHECK_EQ(player.power, 100);
    score.score = 999;
    player.power = 20;
    const i32 index = ReplaySession::stage_index(file, 2);
    CHECK(index >= 0);
    CHECK(ReplaySession::restore_stage(file, u32(index), target, log, error));
    CHECK_EQ(log.stage_number, 2u);
    CHECK_EQ(log.seed, u16(347));
    CHECK_EQ(score.score, 87654);
    CHECK_EQ(player.power, 387);
    CHECK_EQ(progress.character, 2);
    CHECK_EQ(progress.difficulty, 1);
    CHECK_EQ(progress.max_point_cap, 25000000);
}
TEST(replay_start_restores_signed_fixed_position_and_focus_without_clamping) {
    StageBattle battle;std::string error;auto saved=stage(5,123,0,100);
    put32(saved.header,0x0c,-6163);put32(saved.header,0x10,47332);put32(saved.header,0x6c,1);
    CHECK(ReplaySession::restore_player_start(saved,battle,error));
    CHECK_EQ(battle.pos_fx,-6163);CHECK_EQ(battle.pos_fy,47332);
    CHECK_EQ(battle.player_x,-48.1484375f);CHECK_EQ(battle.player_y,369.78125f);
    CHECK_EQ(battle.sim.player_x,battle.player_x);CHECK(battle.focus);
    put32(saved.header,0x0c,0);put32(saved.header,0x10,61440);put32(saved.header,0x6c,0);
    CHECK(ReplaySession::restore_player_start(saved,battle,error));
    CHECK_EQ(battle.player_y,480.f);CHECK(!battle.focus);
    saved.header.resize(0x6f);CHECK(!ReplaySession::restore_player_start(saved,battle,error));
    CHECK_EQ(battle.pos_fy,61440);
}
struct StartProvider final:EclResourceProvider {
    StageSim& sim;explicit StartProvider(StageSim& s):sim(s){}
    bool read(const std::string&,std::vector<u8>& out)override {
        std::vector<u8> root;th13test::emit(root,0,23,0,255,0,{600});th13test::emit(root,0,10,0,255,0,{});
        out=th13test::build_scpt({{"main",root}},{"enemy.anm","st01enm.anm"});return true;
    }
    bool animation(u32,const std::string& name)override{sim.anim_list.push_back(name);return true;}
};
struct StartHost final:StageBattleServices {
    bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}
    bool request_game_over()override{return true;}bool hud_sound(i32)override{return true;}
};
TEST(replay_start_initializes_youmu_follow_history_before_first_movement) {
    StageBattle battle;StartProvider provider{battle.sim};StartHost host;
    SessionState progress;progress.character=3;HudPlayerState player;player.power=400;
    HudScoreState score;CHECK(battle.load(provider,"start.ecl",2,host,progress,player,score));
    auto saved=stage(3,123,0,400);put32(saved.header,0x0c,12800);put32(saved.header,0x10,25600);
    std::string error;CHECK(ReplaySession::restore_player_start(saved,battle,error));
    CHECK_EQ(battle.option_count,4);
    for(int i=0;i<4;++i){CHECK_EQ(battle.options[i].x,100.f);CHECK_EQ(battle.options[i].y,200.f);}
    GameInput input;input.held=0x80;CHECK(battle.step(input));
    CHECK(battle.player_x>100.f);
    for(int i=0;i<4;++i){CHECK_EQ(battle.options[i].x,100.f);CHECK_EQ(battle.options[i].y,200.f);}
}
}  // namespace

int main() { return th13test::run_all("Replay stage restoration"); }
