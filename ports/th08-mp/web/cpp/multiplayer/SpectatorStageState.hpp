#pragma once

#include "../platform/BrowserRuntime.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <array>
#include <algorithm>
#include <cstddef>

namespace th08 {
extern "C" const u32* multiplayer_canonical_state(BrowserRuntime*);
namespace multiplayer {
static_assert(offsetof(EnemySimulationState,drops)==28);

// The checkpoint is a portable visit of named scalar state. Native structs,
// padding, resource pointers and object ownership are reconstructed by the
// title and never copied into the wire initializer.
struct SpectatorStageScalars {
    u32 version=0,stage=0,player_count=0,seed=0;
    i32 difficulty=0,character=0,spell=-1;
    u32 flags=0;
    GameplayLoad load_request{};
    u8 route=0;
    std::array<u8,3> characters{};
    std::array<u32,3> player_hud_flags{};
    std::array<bool,3> player_youkai_bonus{};
    Rng random{};
    GameConfiguration config{};
    GameGlobals values{};
    PilotResources pilots[3]{};
    GameRank rank{};
    CooperativeState cooperation{};
    u16 committed[3]{},previous[3]{},shared_previous=0;
    GameplayControlState control{};
    bool cheat_movement_used=false;
    bool time_stopped=false,paused=false,retrying=false;
    i32 enemy_frames=0,enemy_unfocused=0,enemy_active=0;
    std::array<u16,2> enemy_drops{};
    InputFrame supervisor_input{};
    bool loading_gate=false,loading_hidden=true;
    std::array<u32,13> canonical{};
};

inline u32 SpectatorStageRestoreRequestFailure(const SpectatorStageScalars& s) {
    const auto& load=s.load_request;
    if(load.stage!=i32(s.stage))return 31;
    if(load.difficulty!=s.difficulty)return 32;
    if(load.character!=s.character)return 33;
    if(load.spell!=s.spell)return 34;
    if(load.supervisor_state<i32(Scene::Init)||load.supervisor_state>i32(Scene::NextStage))return 35;
    return 0;
}

inline u32 SpectatorStageRestoreRequestFailure(const BrowserRuntime& runtime) {
    SpectatorStageScalars s;
    const auto& game=runtime.app.game;
    s.stage=game.globals.stage;s.difficulty=i32(game.globals.difficulty);
    s.character=game.globals.shot;s.spell=i32(game.globals.current_spell);
    s.load_request=game.load_request();
    return SpectatorStageRestoreRequestFailure(s);
}

inline bool WriteSpectatorGameGlobals(Netplay::SpectatorScalars::Writer& w,const GameGlobals& g) {
    return w.Values(g.display_score,g.graze_stage,g.score,g.graze,g.score_increment,g.high_score,
        g.high_score_retries)&&w.Array(g.padding19)&&w.Values(g.captured_spells,g.gauge_copy,g.gauge,
        g.point_value,g.clock_time,g.retries)&&w.Array(g.padding2a)&&
        w.Values(g.points_stage,g.points,g.point_extends,g.next_point_extend,g.time_orbs,
            g.last_spell_requirement,g.total_time_orbs)&&w.Array(g.rng1)&&
        w.Values(g.deaths,g.deaths_stage)&&w.Array(g.rng2)&&w.Values(g.lives)&&w.Array(g.rng3)&&
        w.Values(g.bombs,g.bombs_used,g.bombs_used_stage)&&w.Array(g.rng4)&&w.Values(g.power)&&
        w.Array(g.rng5)&&w.Values(g.rng6)&&w.Array(g.rng7)&&w.Values(g.integrity_value,
            g.integrity_checksum)&&w.Array(g.rng8);
}
inline bool ReadSpectatorGameGlobals(Netplay::SpectatorScalars::Reader& r,GameGlobals& g) {
    return r.Values(g.display_score,g.graze_stage,g.score,g.graze,g.score_increment,g.high_score,
        g.high_score_retries)&&r.Array(g.padding19)&&r.Values(g.captured_spells,g.gauge_copy,g.gauge,
        g.point_value,g.clock_time,g.retries)&&r.Array(g.padding2a)&&
        r.Values(g.points_stage,g.points,g.point_extends,g.next_point_extend,g.time_orbs,
            g.last_spell_requirement,g.total_time_orbs)&&r.Array(g.rng1)&&
        r.Values(g.deaths,g.deaths_stage)&&r.Array(g.rng2)&&r.Values(g.lives)&&r.Array(g.rng3)&&
        r.Values(g.bombs,g.bombs_used,g.bombs_used_stage)&&r.Array(g.rng4)&&r.Values(g.power)&&
        r.Array(g.rng5)&&r.Values(g.rng6)&&r.Array(g.rng7)&&r.Values(g.integrity_value,
            g.integrity_checksum)&&r.Array(g.rng8);
}

inline bool WriteSpectatorStageScalars(Netplay::SpectatorScalars::Writer& w,
                                       BrowserRuntime& runtime,
                                       const std::array<u32,13>& canonical) {
    auto& app=runtime.app;auto& session=app.session;auto& game=app.game;
    const auto& g=game.globals;const auto& c=session.config;
    bool loading_gate=false,loading_hidden=true;app.spectator_loading_state(loading_gate,loading_hidden);
    if(!app.in_game()||session.player_count<2||session.player_count>3||session.multiplayer_route_state>2)return false;
    if(!w.Values(u32(4),g.stage,g.difficulty,g.shot,i32(g.current_spell),g.game_flags,
        session.multiplayer_route_state,session.player_count,session.multiplayer_session.seed)||
       !w.Values(game.load_request().stage,game.load_request().difficulty,game.load_request().character,
        game.load_request().spell,game.load_request().flags,game.load_request().initial,
        game.load_request().keep_resources,game.load_request().release_resources,
        game.load_request().supervisor_state)||
       !w.Array(session.player_characters)||
       !w.Values(session.random.seed,session.random.backup,session.random.calls))return false;
    if(!w.Array(c.controller)||!w.Values(c.version,c.pad_x,c.pad_y,c.lives,c.bombs,c.color16,c.music,c.sounds,
        c.difficulty,c.windowed,c.frameskip,c.effects,c.slow_mode,c.shot_slow,c.music_volume,c.sound_volume,c.options))return false;
    if(!WriteSpectatorGameGlobals(w,session.numbers))return false;
    for(const auto& p:session.pilot_resources)
        if(!w.Values(p.lives,p.bombs,p.power,p.challenge_mode,p.gauge,p.gauge_copy,p.deaths,p.deaths_stage,p.bombs_used,p.bombs_used_stage))return false;
    std::array<u32,3> player_hud_flags{};
    std::array<bool,3> player_youkai_bonus{};
    for(u32 seat=0;seat<session.player_count;++seat){
        player_hud_flags[seat]=game.pilot(seat).status().context.hud_flags;
        const auto bonus=game.pilot(seat).status().shots.youkai_bonus;
        if(bonus>1)return false;
        player_youkai_bonus[seat]=bonus!=0;
    }
    if(!w.Array(player_hud_flags)||!w.Array(player_youkai_bonus))return false;
    if(!w.Values(session.rank.value,session.rank.maximum,session.rank.minimum,session.rank.fraction,
        game.cooperation.count,game.cooperation.wipe_progress,game.cooperation.retry_pending))return false;
    for(const auto& s:game.cooperation.seats)
        if(!w.Values(s.spirit,s.waiting_for_focus_release,s.progress,s.power_taps,s.power_window,s.target,s.drift_x,s.drift_y))return false;
    if(!w.Array(game.committed_buttons)||!w.Array(game.previous_buttons)||
       !w.Values(game.spectator_previous_input(),game.control.state.frames,game.control.state.restore_viewport,
        game.control.state.next_scene,game.control.state.load_state,game.control.state.load_frames,
        game.control.state.start_music,game.control.state.slow_frames,game.control.state.play_frames,
        game.control.state.demo_frames,game.control.state.demo_index,game.control.state.replay_mode,
        game.control.state.stage_mask,game.control.state.sticky_input,game.control.state.loading_images_setup,
        game.time_stopped,game.paused,game.retrying,game.enemies.state.frames,
        game.enemies.state.unfocused_frames,game.enemies.state.active_frames,
        game.enemies.state.drops.enemies,game.enemies.state.drops.item,
        app.supervisor.input.current,app.supervisor.input.previous,app.supervisor.input.scrolling,
        app.supervisor.input.held,session.multiplayer_cheat_movement_used,loading_gate,loading_hidden))return false;
    return w.Array(canonical)&&w.Good();
}

inline bool CaptureSpectatorStageScalars(BrowserRuntime& runtime,std::vector<u8>& bytes,u32* failure=nullptr) {
    if(failure)*failure=0;
    if(!runtime.app.in_game()||runtime.app.session.player_count<2||runtime.app.session.player_count>3){if(failure)*failure=11;return false;}
    const auto request_failure=SpectatorStageRestoreRequestFailure(runtime);
    if(request_failure){if(failure)*failure=request_failure;return false;}
    std::array<u32,13> canonical{};const auto* hash=multiplayer_canonical_state(&runtime);
    if(!hash||hash[0]!=1){if(failure)*failure=12;return false;}std::copy(hash,hash+canonical.size(),canonical.begin());
    Netplay::SpectatorScalars::Writer writer;
    if(!WriteSpectatorStageScalars(writer,runtime,canonical)){if(failure)*failure=13;return false;}
    bytes=writer.Bytes();if(bytes.empty()||bytes.size()>Netplay::SpectatorStageMaxStateBytes){if(failure)*failure=14;bytes.clear();return false;}return true;
}

inline bool ReadSpectatorStageScalars(Netplay::SpectatorScalars::Reader& r,SpectatorStageScalars& s,u32* failure=nullptr) {
    if(!r.Values(s.version,s.stage,s.difficulty,s.character,s.spell,s.flags,s.route,s.player_count,s.seed)||
       !r.Values(s.load_request.stage,s.load_request.difficulty,s.load_request.character,
        s.load_request.spell,s.load_request.flags,s.load_request.initial,s.load_request.keep_resources,
        s.load_request.release_resources,s.load_request.supervisor_state)){if(failure)*failure=1;return false;}
    if(!r.Array(s.characters)||!r.Values(s.random.seed,s.random.backup,s.random.calls)){if(failure)*failure=2;return false;}
    auto& c=s.config;
    if(!r.Array(c.controller)||!r.Values(c.version,c.pad_x,c.pad_y,c.lives,c.bombs,c.color16,c.music,c.sounds,
        c.difficulty,c.windowed,c.frameskip,c.effects,c.slow_mode,c.shot_slow,c.music_volume,c.sound_volume,c.options)){if(failure)*failure=3;return false;}
    auto& g=s.values;if(!ReadSpectatorGameGlobals(r,g)){if(failure)*failure=4;return false;}
    for(auto& p:s.pilots)if(!r.Values(p.lives,p.bombs,p.power,p.challenge_mode,p.gauge,p.gauge_copy,p.deaths,p.deaths_stage,p.bombs_used,p.bombs_used_stage)){if(failure)*failure=5;return false;}
    if(!r.Array(s.player_hud_flags)||!r.Array(s.player_youkai_bonus)){if(failure)*failure=5;return false;}
    if(!r.Values(s.rank.value,s.rank.maximum,s.rank.minimum,s.rank.fraction,
        s.cooperation.count,s.cooperation.wipe_progress,s.cooperation.retry_pending)){if(failure)*failure=6;return false;}
    for(auto& seat:s.cooperation.seats)
        if(!r.Values(seat.spirit,seat.waiting_for_focus_release,seat.progress,seat.power_taps,seat.power_window,seat.target,seat.drift_x,seat.drift_y)){if(failure)*failure=7;return false;}
    if(!r.Array(s.committed)||!r.Array(s.previous)||
       !r.Values(s.shared_previous,s.control.frames,s.control.restore_viewport,s.control.next_scene,s.control.load_state,
        s.control.load_frames,s.control.start_music,s.control.slow_frames,s.control.play_frames,s.control.demo_frames,
        s.control.demo_index,s.control.replay_mode,s.control.stage_mask,s.control.sticky_input,s.control.loading_images_setup,
        s.time_stopped,s.paused,s.retrying,s.enemy_frames,s.enemy_unfocused,s.enemy_active,
        s.enemy_drops[0],s.enemy_drops[1],
        s.supervisor_input.current,s.supervisor_input.previous,s.supervisor_input.scrolling,s.supervisor_input.held,
        s.cheat_movement_used,s.loading_gate,s.loading_hidden)){if(failure)*failure=8;return false;}
    if(!r.Array(s.canonical)){if(failure)*failure=9;return false;}
    if(!r.Done()||s.version!=4||s.stage>=9||s.difficulty<0||s.difficulty>4||
       s.character<0||s.character>=12||s.player_count<2||s.player_count>3||s.route>2){if(failure)*failure=10;return false;}
    return true;
}

inline bool RestoreSpectatorStageScalars(BrowserRuntime& runtime,const Netplay::SpectatorStageEvent& event,u32* failure=nullptr) {
    if(failure)*failure=0;
    if(event.kind!=Netplay::SpectatorEventKind::Checkpoint||event.game!='8'||event.schema!=1){if(failure)*failure=1;return false;}
    Netplay::SpectatorScalars::Reader reader(event.state);SpectatorStageScalars s;
    u32 parse_failure=0;
    if(!ReadSpectatorStageScalars(reader,s,&parse_failure)){if(failure)*failure=10+parse_failure;return false;}
    if(event.stage!=(s.stage|(u32(s.route)<<8))){if(failure)*failure=30;return false;}
    auto& app=runtime.app;auto& session=app.session;
    session.player_count=s.player_count;for(u32 i=0;i<3;++i)session.player_characters[i]=s.characters[i];
    session.multiplayer_route_state=s.route;session.config=s.config;session.random=s.random;
    session.multiplayer_session.configured=true;session.multiplayer_session.player_count=s.player_count;
    session.multiplayer_session.difficulty=u32(s.difficulty);
    session.multiplayer_session.seed=s.seed;
    for(u32 i=0;i<3;++i)session.multiplayer_session.characters[i]=s.characters[i];
    const auto load=s.load_request;
    if(const auto request_failure=SpectatorStageRestoreRequestFailure(s)){if(failure)*failure=request_failure;return false;}
    u32 native_failure=0;
    const bool initial_ready_boundary=event.frame==0&&load.initial;
    if(!app.restore_multiplayer_stage(load,initial_ready_boundary,&native_failure)){if(failure)*failure=40+native_failure;return false;}
    auto& game=app.game;auto& g=game.globals;session.config=s.config;session.random=s.random;
    session.numbers.display_score=s.values.display_score;session.numbers.graze_stage=s.values.graze_stage;
    session.numbers.score=s.values.score;session.numbers.graze=s.values.graze;session.numbers.score_increment=s.values.score_increment;
    session.numbers.high_score=s.values.high_score;session.numbers.high_score_retries=s.values.high_score_retries;
    session.numbers.captured_spells=s.values.captured_spells;session.numbers.gauge_copy=s.values.gauge_copy;
    session.numbers.gauge=s.values.gauge;session.numbers.point_value=s.values.point_value;session.numbers.clock_time=s.values.clock_time;
    session.numbers.retries=s.values.retries;session.numbers.points_stage=s.values.points_stage;session.numbers.points=s.values.points;
    session.numbers.point_extends=s.values.point_extends;session.numbers.next_point_extend=s.values.next_point_extend;
    session.numbers.time_orbs=s.values.time_orbs;session.numbers.last_spell_requirement=s.values.last_spell_requirement;
    session.numbers.total_time_orbs=s.values.total_time_orbs;session.numbers.deaths=s.values.deaths;
    session.numbers.deaths_stage=s.values.deaths_stage;session.numbers.lives=s.values.lives;session.numbers.bombs=s.values.bombs;
    session.numbers.bombs_used=s.values.bombs_used;session.numbers.bombs_used_stage=s.values.bombs_used_stage;
    session.numbers.power=s.values.power;session.numbers.integrity_value=s.values.integrity_value;
    session.numbers.integrity_checksum=s.values.integrity_checksum;
    for(u32 seat=0;seat<3;++seat){auto& d=session.pilot_resources[seat];const auto& p=s.pilots[seat];
        d.lives=p.lives;d.bombs=p.bombs;d.power=p.power;d.challenge_mode=p.challenge_mode;d.gauge=p.gauge;d.gauge_copy=p.gauge_copy;
        d.deaths=p.deaths;d.deaths_stage=p.deaths_stage;d.bombs_used=p.bombs_used;d.bombs_used_stage=p.bombs_used_stage;
        if(seat<session.player_count)game.pilot_services(seat).sync_values();}
    session.rank=s.rank;game.cooperation=s.cooperation;
    game.restore_spectator_input_edges(s.shared_previous,s.committed,s.previous);game.control.state=s.control;
    game.time_stopped=s.time_stopped;game.paused=s.paused;game.retrying=s.retrying;
    game.enemies.state.frames=s.enemy_frames;game.enemies.state.unfocused_frames=s.enemy_unfocused;
    game.enemies.state.active_frames=s.enemy_active;
    game.enemies.state.drops.enemies=s.enemy_drops[0];game.enemies.state.drops.item=s.enemy_drops[1];
    app.supervisor.input=s.supervisor_input;
    session.multiplayer_cheat_movement_used=s.cheat_movement_used;
    app.restore_spectator_loading_state(s.loading_gate,s.loading_hidden);
    g.stage=s.stage;g.difficulty=u32(s.difficulty);g.shot=s.character;g.current_spell=i16(s.spell);g.game_flags=s.flags;
    g.spectator_boss_slots=0;app.set_spectator_playback_input(s.supervisor_input.current);
    for(u32 seat=0;seat<s.player_count;++seat){
        game.pilot(seat).status().context.hud_flags=s.player_hud_flags[seat];
        game.pilot(seat).status().shots.youkai_bonus=u8(s.player_youkai_bonus[seat]);
    }
    const auto* actual=multiplayer_canonical_state(&runtime);
    const bool matched=actual&&actual[0]==1&&std::equal(s.canonical.begin(),s.canonical.end(),actual);
    if(!matched&&failure)*failure=5;return matched;
}
}
}
