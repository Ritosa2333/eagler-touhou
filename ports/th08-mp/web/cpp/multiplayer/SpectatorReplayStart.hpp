#pragma once
#include "../platform/BrowserRuntime.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
namespace th08::multiplayer {
constexpr u32 SpectatorReplaySchema=3;
constexpr u32 LegacySpectatorReplaySchema=2;

// The retail stage header seeds resource construction. The published cursor
// is later, after the native stage load and any transition update/draw.
struct SpectatorStageBoundary {
    Rng random;
    u32 game_flags=0;
    i32 control_frames=0,enemy_frames=0,human_frames=0,active_frames=0;
    i32 stall_frames=0;
    std::array<i8,3> drift_x{},drift_y{};

    template<class C> bool Values(C& c){
        return c.Values(random.seed,random.backup,random.calls,game_flags,
            control_frames,enemy_frames,human_frames,active_frames,stall_frames)&&
            c.Array(drift_x)&&c.Array(drift_y);
    }
};
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
    const auto& app=runtime.app;const auto& g=app.game;const auto& enemy=g.enemies.state;
    SpectatorStageBoundary boundary{app.session.random,g.globals.game_flags,
        g.control.state.frames,enemy.frames,enemy.unfocused_frames,enemy.active_frames,app.session.stall_frames};
    for(u32 seat=0;seat<s.seats;++seat){
        boundary.drift_x[seat]=g.cooperation.seats[seat].drift_x;
        boundary.drift_y[seat]=g.cooperation.seats[seat].drift_y;
    }
    Netplay::SpectatorScalars::Writer w;
    if(!ReplayStartValues(w,s)||!boundary.Values(w)||!w.Good())return false;
    bytes=w.Bytes();return true;
}
inline bool RestoreSpectatorReplayStart(BrowserRuntime& runtime,const Netplay::SpectatorStageEvent& event,u32* failure=nullptr){
    if(failure)*failure=0;ReplayStageStart s;SpectatorStageBoundary boundary;
    Netplay::SpectatorScalars::Reader r(event.state);
    const bool exact=event.schema==SpectatorReplaySchema;
    if(event.kind!=Netplay::SpectatorEventKind::Checkpoint||event.game!='8'||
       (!exact&&event.schema!=LegacySpectatorReplaySchema)||
       !ReplayStartValues(r,s)||(exact&&!boundary.Values(r))||!r.Done()||!ValidReplayStart(s)||
       event.playerCount!=s.seats||event.stage!=(s.stage|(u32(s.route)<<8))){if(failure)*failure=1;return false;}
    auto& app=runtime.app;auto& session=app.session;s.present=true;
    session.player_count=s.seats;session.multiplayer_route_state=s.route;session.config=s.configuration;
    std::copy(s.characters.begin(),s.characters.end(),session.player_characters);
    auto& setup=session.multiplayer_session;setup.configured=true;setup.player_count=s.seats;
    setup.seed=s.seed;setup.difficulty=s.difficulty;setup.challenge_mode=s.pilots[0].challenge_mode;
    std::copy(s.characters.begin(),s.characters.end(),setup.characters);
    app.game.replay_stage_start_pending=s;
    GameplayLoad load;load.stage=i32(s.stage);load.difficulty=s.difficulty;load.character=s.characters[0];
    if(!app.restore_multiplayer_stage(load,event.frame==0,failure,s.loading,s.loading_frames))return false;
    if(exact){
        auto& g=app.game;auto& enemy=g.enemies.state;
        session.random=boundary.random;
        g.globals.game_flags=boundary.game_flags;
        g.control.state.frames=boundary.control_frames;
        enemy.frames=boundary.enemy_frames;
        enemy.unfocused_frames=boundary.human_frames;
        enemy.active_frames=boundary.active_frames;
        session.stall_frames=boundary.stall_frames;
        for(u32 seat=0;seat<s.seats;++seat){
            g.cooperation.seats[seat].drift_x=boundary.drift_x[seat];
            g.cooperation.seats[seat].drift_y=boundary.drift_y[seat];
        }
    }
    return true;
}
}
