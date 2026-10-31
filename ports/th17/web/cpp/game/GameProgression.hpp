#pragma once
#include "Hud.hpp"
#include "TitleRecords.hpp"
#include "StageConfiguration.hpp"
namespace th17 {
struct GameRun {
    HudScoreState scoring;i32 stage=1,character=0,animal=0,difficulty=1,spell_id=-1;
    u32 scene_flags=0,application_flags=0;i32 screen=0,ending_age=0,demo_slot=-1;bool practice_record=false;
    i32 practice_lives=0,initial_point_value=0,ranking_default=0;
    i32 initial_stage=1,replay_reserved_40=0,replay_reserved_60=0;float rate=1;
    // Only for identified recordings made by the early web release. This is
    // application state, never part of the original serialized run layout.
    bool historical_web_replay=false;
};
struct GameRecords {
    TitleRecords title;
    std::array<std::array<i32,5>,9> clears{};
    std::array<std::array<std::array<u8,8>,5>,9> stage_cleared{};
    std::array<std::array<i32,101>,9> spell_best{};
    struct HighScore {i32 score=0,extra=0;};
    std::array<std::array<HighScore,5>,9> high_scores{};
    std::array<std::array<std::array<i32,8>,5>,9> practice_scores{};
    std::array<i32,9> starts{};
};
struct GameProgressionHost {
    virtual ~GameProgressionHost()=default;
    virtual bool finish_player_options()=0;virtual bool stop_bomb()=0;
    virtual bool record_play_time()=0;virtual bool trophy(i32)=0;
    virtual bool finish_practice()=0;virtual bool return_to_title()=0;
};
// Run transitions operate on named game state. File layout, platform timing
// and the application scene dispatcher remain separate owners.
class GameProgression {
    GameRun& run;GameRecords& records;ItemScoreState& score;PlayerLifeSession& player;Hud& hud;GameProgressionHost& host;
    bool check(bool,const char*);void add_bonus(i32)noexcept;void count_clear(i32)noexcept;
public:
    std::string error;
    GameProgression(GameRun& r,GameRecords& data,ItemScoreState& s,PlayerLifeSession& p,Hud& h,GameProgressionHost& services):run(r),records(data),score(s),player(p),hud(h),host(services){}
    bool next_stage();
};
}
