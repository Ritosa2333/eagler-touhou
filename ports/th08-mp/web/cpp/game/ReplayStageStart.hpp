#pragma once
#include "ReplayFile.hpp"
#include "GameplaySession.hpp"
#include <array>
#include <algorithm>
namespace th08::multiplayer {
// Retail ReplayStage and the multiplayer resource extension at the same entry.
struct ReplayStageStart {
    u32 stage=0,score=0,seats=0,seed=0,loading_frames=0;
    bool loading=false;
    u8 route=0,difficulty=0;
    std::array<u8,3> characters{};
    GameConfiguration configuration{};
    ReplayStage native{};
    std::array<PilotResources,3> pilots{};
    bool present=false;
};
inline void CaptureReplayStageStart(ReplayStageStart& s,const GameplaySession& session,
    const EclGlobals& game,u8 counter){
    s={};s.stage=game.stage;s.score=session.numbers.score;s.seats=session.player_count;
    s.seed=session.multiplayer_session.seed;s.route=session.multiplayer_route_state;
    s.difficulty=u8(game.difficulty);s.configuration=session.config;
    std::copy(session.player_characters,session.player_characters+3,s.characters.begin());
    const auto& n=session.numbers;auto& h=s.native;
    h.graze=n.graze;h.bombs=u8(session.pilot_resources[0].bombs);h.lives=u8(session.pilot_resources[0].lives);
    h.power=u8(session.pilot_resources[0].power);h.rank=u8(session.rank.value);
    h.points=n.points;h.seed=session.random.seed;h.power_item_counter=counter;
    h.captured_spells=u8(n.captured_spells);h.extends=i32(n.point_extends);h.next_extend=n.next_point_extend;
    h.humanity=n.gauge;h.clock=u8(n.clock_time);h.point_value=n.point_value;
    std::copy(session.pilot_resources,session.pilot_resources+3,s.pilots.begin());s.present=true;
}
inline void ApplyReplayStageStart(const ReplayStageStart& s,GameplaySession& session,u8& counter){
    const auto& h=s.native;auto& n=session.numbers;
    n.score=n.display_score=s.score;n.points=h.points;session.rank.value=h.rank;
    n.graze=h.graze;counter=h.power_item_counter;n.point_value=h.point_value;
    session.config=s.configuration;session.random.seed=h.seed;session.random.calls=0;
    n.captured_spells=h.captured_spells;n.point_extends=u32(h.extends);n.next_point_extend=h.next_extend;
    n.gauge=h.humanity;n.clock_time=i8(h.clock);
    std::copy(s.pilots.begin(),s.pilots.end(),session.pilot_resources);
}
}
