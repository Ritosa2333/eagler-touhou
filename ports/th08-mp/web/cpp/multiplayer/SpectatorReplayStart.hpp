#pragma once
#include "../platform/BrowserRuntime.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
namespace th08::multiplayer {
constexpr u32 SpectatorReplaySchema=2;
template<class C> bool ReplayStartValues(C& c,ReplayStageStart& s){
    auto& h=s.native;auto& cfg=s.configuration;
    if(!c.Values(s.stage,s.score,s.seats,s.seed,s.route,s.difficulty,s.loading,s.loading_frames)||!c.Array(s.characters)||
       !c.Array(cfg.controller)||!c.Values(cfg.version,cfg.pad_x,cfg.pad_y,cfg.lives,cfg.bombs,cfg.color16,
       cfg.music,cfg.sounds,cfg.difficulty,cfg.windowed,cfg.frameskip,cfg.effects,cfg.slow_mode,
       cfg.shot_slow,cfg.music_volume,cfg.sound_volume,cfg.options)||
       !c.Values(h.points,h.graze,h.extends,h.next_extend,h.point_value,h.humanity,h.seed,h.power,
       h.lives,h.bombs,h.rank,h.power_item_counter,h.captured_spells,h.clock))return false;
    for(auto& p:s.pilots)if(!c.Values(p.lives,p.bombs,p.power,p.challenge_mode,p.gauge,p.gauge_copy,
        p.deaths,p.bombs_used))return false;
    return true;
}
inline bool ValidReplayStart(const ReplayStageStart& s){
    if(s.stage>=9||s.seats<2||s.seats>3||s.seed>65535||s.route>2||s.difficulty>4||s.loading_frames>80)return false;
    for(unsigned i=0;i<s.seats;++i)if(s.characters[i]>=12||s.pilots[i].lives< -1||s.pilots[i].lives>8||
        s.pilots[i].bombs<0||s.pilots[i].bombs>8||s.pilots[i].power<0||s.pilots[i].power>128)return false;
    return true;
}
inline bool EncodeSpectatorReplayStart(BrowserRuntime& runtime,std::vector<u8>& bytes,u32* failure=nullptr){
    if(failure)*failure=0;auto s=runtime.app.game.replay_stage_start;
    if(!runtime.app.in_game()||!s.present||s.stage!=runtime.app.game.globals.stage){if(failure)*failure=31;return false;}
    s.route=runtime.app.session.multiplayer_route_state;
    s.loading=runtime.app.loading_game();s.loading_frames=u32(runtime.app.game.control.state.load_frames);
    if(!ValidReplayStart(s)){if(failure)*failure=11;return false;}
    Netplay::SpectatorScalars::Writer w;if(!ReplayStartValues(w,s)||!w.Good())return false;
    bytes=w.Bytes();return true;
}
inline bool RestoreSpectatorReplayStart(BrowserRuntime& runtime,const Netplay::SpectatorStageEvent& event,u32* failure=nullptr){
    if(failure)*failure=0;ReplayStageStart s;Netplay::SpectatorScalars::Reader r(event.state);
    if(event.kind!=Netplay::SpectatorEventKind::Checkpoint||event.game!='8'||event.schema!=SpectatorReplaySchema||
       !ReplayStartValues(r,s)||!r.Done()||!ValidReplayStart(s)||event.playerCount!=s.seats||event.stage!=(s.stage|(u32(s.route)<<8))){if(failure)*failure=1;return false;}
    auto& app=runtime.app;auto& session=app.session;s.present=true;
    session.player_count=s.seats;session.multiplayer_route_state=s.route;session.config=s.configuration;
    std::copy(s.characters.begin(),s.characters.end(),session.player_characters);
    auto& setup=session.multiplayer_session;setup.configured=true;setup.player_count=s.seats;
    setup.seed=s.seed;setup.difficulty=s.difficulty;
    std::copy(s.characters.begin(),s.characters.end(),setup.characters);
    app.game.replay_stage_start_pending=s;
    GameplayLoad load;load.stage=i32(s.stage);load.difficulty=s.difficulty;load.character=s.characters[0];
    return app.restore_multiplayer_stage(load,event.frame==0,failure,s.loading,s.loading_frames);
}
}
