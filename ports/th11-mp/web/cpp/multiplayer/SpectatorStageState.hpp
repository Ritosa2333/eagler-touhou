#pragma once
#ifndef TH11_MULTIPLAYER
#error Streaming Multiplayer Replay belongs to the MP runtime
#endif
#include "../game/GameSession.hpp"
#include "SpectatorStageSchema.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
namespace th11::multiplayer {
// The native Replay header plus each seat's resource and position entry.
struct SpectatorStageState {
    u32 frame=0,stage=0,seats=0,fragments=0,run_ticks=0;
    i32 rank=0,background_frame=0;
    std::array<ReplayStageState,3> players{};
    std::array<u32,3> ghost_random{},challenge_misses{};
};
template<class C> bool ReplayStageValues(C& c,SpectatorStageState& s){
    if(!c.Values(s.frame,s.stage,s.seats,s.fragments,s.rank,s.run_ticks,s.background_frame))return false;
    for(auto& p:s.players)if(!c.Values(p.seed,p.score,p.power,p.points,p.lives,p.fragments,p.rank,
        p.x,p.y,p.continues,p.extends,p.focused,p.graze,p.weapon)||!c.Array(p.spell_times))return false;
    return c.Array(s.ghost_random)&&c.Array(s.challenge_misses);
}
inline bool ValidReplayStage(const SpectatorStageState& s){
    if(s.stage<1||s.stage>7||s.seats<2||s.seats>3||s.fragments>=5||s.rank < -1024||s.rank>1024||s.background_frame < -1||s.background_frame>3000000)return false;
    for(unsigned i=0;i<s.seats;++i){const auto& p=s.players[i];
        if(p.seed>65535||p.lives < -1||p.lives>9||p.power<0||p.power>10000||
           p.fragments<0||p.fragments>=5||p.focused<0||p.focused>1)return false;
    }
    return true;
}
inline bool DecodeSpectatorStageState(const std::vector<u8>& bytes,SpectatorStageState& output){
    if(bytes.empty()||bytes.size()>Netplay::SpectatorStageMaxStateBytes)return false;
    SpectatorStageState s;Netplay::SpectatorScalars::Reader reader(bytes);
    if(!ReplayStageValues(reader,s)||!reader.Done()||!ValidReplayStage(s))return false;
    output=s;return true;
}
inline bool EncodeSpectatorStageState(const SpectatorStageState& state,std::vector<u8>& bytes){
    if(!ValidReplayStage(state))return false;auto copy=state;Netplay::SpectatorScalars::Writer writer;
    if(!ReplayStageValues(writer,copy)||!writer.Good())return false;bytes=writer.Bytes();return true;
}
inline bool CaptureSpectatorStageState(const GameSession& session,u32 frame,std::vector<u8>& bytes){
    if(!session.multiplayer_active||session.state.phase!=GameSessionPhase::stage||!session.battle||
       !session.battle->stage_active)return false;
    const auto& battle=*session.battle;SpectatorStageState s;
    s.frame=frame;s.stage=session.state.stage;s.seats=session.multiplayer_options.seat_count;
    s.fragments=battle.mp_fragments;s.rank=battle.mp_rank;s.run_ticks=battle.mp_tick;
    if(s.stage==6){if(!battle.stage)return false;s.background_frame=battle.stage->state.script_timer.current;}
    for(unsigned i=0;i<s.seats;++i){
        if(!battle.pilots[i]||!battle.pilots[i]->player)return false;
        const auto& pilot=*battle.pilots[i];const auto& e=pilot.economy;const auto& m=pilot.player->motion.state;
        auto& p=s.players[i];p.seed=session.replay_stage_seed();p.score=e.score_units;p.power=e.power;
        p.points=e.point_value;p.lives=e.lives;p.fragments=e.life_fragments;p.rank=e.rank;
        p.x=m.x;p.y=m.y;p.continues=battle.hud.score.continues;p.focused=m.focused;
        p.graze=e.graze;p.weapon=m.weapon_mode;p.spell_times=battle.spell_timing.records;
        s.ghost_random[i]=pilot.ghost_random;s.challenge_misses[i]=pilot.challenge_misses;
    }
    return EncodeSpectatorStageState(s,bytes);
}
inline bool ApplySpectatorStageState(GameSession& session,const SpectatorStageState& s){
    if(!ValidReplayStage(s)||!session.multiplayer_active||!session.battle||
       session.state.stage!=s.stage||session.multiplayer_options.seat_count!=s.seats)return false;
    auto& b=*session.battle;session.multiplayer_frame=s.frame;
    for(unsigned i=0;i<s.seats;++i){
        if(!b.pilots[i]||!b.pilots[i]->player)return false;
        auto& pilot=*b.pilots[i];pilot.ghost_random=s.ghost_random[i];pilot.challenge_misses=s.challenge_misses[i];
    }
    if(s.stage==6){if(!b.stage)return false;b.stage->multiplayer_replay_background_frame=s.background_frame;}
    b.mp_fragments=s.fragments;b.mp_rank=s.rank;b.mp_tick=s.run_ticks;
    b.spell_timing.records=s.players[0].spell_times;
    return true;
}
}
