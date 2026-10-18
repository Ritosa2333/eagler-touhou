#pragma once
#ifndef TH11_MULTIPLAYER
#error TH11 spectator state must remain outside the ordinary source set
#endif

#include "../game/GameSession.hpp"
#include "SpectatorShotState.hpp"
#include "SpectatorStageSchema.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <algorithm>

namespace th11::multiplayer {


struct SpectatorStagePilotState {
    std::uint32_t ghost=0,life_latched=0,life_hold=0,power_taps=0,power_gap=0;
    std::uint32_t ghost_random=0,challenge_misses=0;
    std::int32_t ghost_dx=0,ghost_dy=0,x=0,y=0,weapon_mode=0;
    std::uint32_t motion_flags=0;
    std::int32_t life_state=0;
    std::int32_t state_timer_previous=0,state_timer_current=0;
    float state_timer_fractional=0;
    std::int32_t invincibility_previous=0,invincibility_current=0;
    float invincibility_fractional=0;
    std::int32_t lives=0,power=0,communication=0,graze=0,point_value=0;
    std::uint32_t keys_held=0,keys_previous=0,keys_pressed=0,keys_released=0;
    std::uint32_t keys_long_repeat=0,keys_short_repeat=0,auto_focus_frames=0;
    std::array<std::uint32_t,32> key_counts{};
    std::int32_t bomb_active=0,bomb_elapsed_previous=0,bomb_elapsed_current=0;
    float bomb_elapsed_fractional=0;
};

struct SpectatorStageState {
    std::uint32_t schema=SpectatorStageSchema,frame=0,stage=0,session_frame=0,stage_frame=0;
    std::uint32_t seat_count=0,local_seat=0,seed=0,difficulty=0,challenge=0;
    std::array<std::int32_t,3> selections{};
    std::uint32_t phase=0,mp_tick=0,mp_fragments=0,mp_wipe_frames=0;
    std::int32_t mp_rank=0,score_units=0;
    std::uint16_t script_seed=0,script_reserved=0;
    std::uint32_t script_calls=0;
    std::uint16_t visual_seed=0,visual_reserved=0;
    std::uint32_t visual_calls=0;
    std::uint32_t spell_flags=0;
    std::int32_t spell_id=0,spell_bonus=0,spell_elapsed=0;
    std::array<std::int32_t,3> shared_integers{};
    float rate=1;
    std::uint32_t transitioning=0,transition_started=0,stage_active=0;
    std::int32_t transition_previous=0,transition_current=0;
    float transition_fractional=0;
    std::uint32_t stage_instruction_offset=0;
    std::int32_t stage_script_previous=0,stage_script_current=0;
    float stage_script_fractional=0;
    std::int32_t stage_fade_previous=0,stage_fade_current=0;
    std::uint32_t stage_frame_count=0;
    float stage_fade_fractional=0;
    std::uint32_t stage_draw_flags=0,stage_effects_enabled=0;
    std::array<SpectatorStagePilotState,3> pilots{};
    std::array<std::vector<u8>,3> shots;
    std::vector<u32> enemy_flags;
    std::uint32_t canonical_hash=0;
};

inline bool WriteSpectatorStageState(Netplay::SpectatorScalars::Writer& w,const SpectatorStageState& s){
    if(!w.Values(s.schema,s.frame,s.stage,s.session_frame,s.stage_frame,s.seat_count,s.local_seat,s.seed,
        s.difficulty,s.challenge)||!w.Array(s.selections)||!w.Values(s.phase,s.mp_tick,s.mp_fragments,
        s.mp_wipe_frames,s.mp_rank,s.score_units,s.script_seed,s.script_reserved,s.script_calls,
        s.visual_seed,s.visual_reserved,s.visual_calls,s.spell_flags,s.spell_id,s.spell_bonus,s.spell_elapsed)||
        !w.Array(s.shared_integers)||!w.Values(s.rate,s.transitioning,s.transition_started,s.stage_active,
        s.transition_previous,s.transition_current,s.transition_fractional,s.stage_instruction_offset,
        s.stage_script_previous,s.stage_script_current,s.stage_script_fractional,s.stage_fade_previous,
        s.stage_fade_current,s.stage_fade_fractional,s.stage_frame_count,s.stage_draw_flags,s.stage_effects_enabled))return false;
    for(const auto& p:s.pilots)if(!w.Values(p.ghost,p.life_latched,p.life_hold,p.power_taps,p.power_gap,p.ghost_random,
        p.challenge_misses,p.ghost_dx,p.ghost_dy,p.x,p.y,p.weapon_mode,p.motion_flags,p.life_state,
        p.state_timer_previous,p.state_timer_current,p.state_timer_fractional,p.invincibility_previous,
        p.invincibility_current,p.invincibility_fractional,p.lives,p.power,p.communication,p.graze,p.point_value,
        p.keys_held,p.keys_previous,p.keys_pressed,p.keys_released,p.keys_long_repeat,p.keys_short_repeat,
        p.auto_focus_frames)||!w.Array(p.key_counts)||!w.Values(p.bomb_active,p.bomb_elapsed_previous,
        p.bomb_elapsed_current,p.bomb_elapsed_fractional))return false;
    if(s.enemy_flags.size()>4096||!w.Value(u32(s.enemy_flags.size()))||!w.Array(s.enemy_flags))return false;
    for(const auto& bytes:s.shots)if(!w.Value(u32(bytes.size()))||!w.Array(bytes))return false;
    return w.Value(s.canonical_hash)&&w.Good();
}

inline bool ReadSpectatorStageState(Netplay::SpectatorScalars::Reader& r,SpectatorStageState& s){
    if(!r.Values(s.schema,s.frame,s.stage,s.session_frame,s.stage_frame,s.seat_count,s.local_seat,s.seed,
        s.difficulty,s.challenge)||!r.Array(s.selections)||!r.Values(s.phase,s.mp_tick,s.mp_fragments,
        s.mp_wipe_frames,s.mp_rank,s.score_units,s.script_seed,s.script_reserved,s.script_calls,
        s.visual_seed,s.visual_reserved,s.visual_calls,s.spell_flags,s.spell_id,s.spell_bonus,s.spell_elapsed)||
        !r.Array(s.shared_integers)||!r.Values(s.rate,s.transitioning,s.transition_started,s.stage_active,
        s.transition_previous,s.transition_current,s.transition_fractional,s.stage_instruction_offset,
        s.stage_script_previous,s.stage_script_current,s.stage_script_fractional,s.stage_fade_previous,
        s.stage_fade_current,s.stage_fade_fractional,s.stage_frame_count,s.stage_draw_flags,s.stage_effects_enabled))return false;
    for(auto& p:s.pilots)if(!r.Values(p.ghost,p.life_latched,p.life_hold,p.power_taps,p.power_gap,p.ghost_random,
        p.challenge_misses,p.ghost_dx,p.ghost_dy,p.x,p.y,p.weapon_mode,p.motion_flags,p.life_state,
        p.state_timer_previous,p.state_timer_current,p.state_timer_fractional,p.invincibility_previous,
        p.invincibility_current,p.invincibility_fractional,p.lives,p.power,p.communication,p.graze,p.point_value,
        p.keys_held,p.keys_previous,p.keys_pressed,p.keys_released,p.keys_long_repeat,p.keys_short_repeat,
        p.auto_focus_frames)||!r.Array(p.key_counts)||!r.Values(p.bomb_active,p.bomb_elapsed_previous,
        p.bomb_elapsed_current,p.bomb_elapsed_fractional))return false;
    u32 count=0;if(!r.Value(count)||count>4096)return false;s.enemy_flags.resize(count);
    if(!r.Array(s.enemy_flags))return false;
    for(auto& bytes:s.shots){if(!r.Value(count)||count>Netplay::SpectatorStageMaxStateBytes)return false;
        bytes.resize(count);if(!r.Array(bytes))return false;}
    return r.Value(s.canonical_hash)&&r.Done()&&s.schema==SpectatorStageSchema&&s.stage>=1&&s.stage<=7&&
        s.seat_count>=2&&s.seat_count<=3&&s.local_seat<s.seat_count&&s.phase==std::uint32_t(GameSessionPhase::stage)&&
        s.transitioning==0&&s.stage_active==1;
}

inline bool CaptureSpectatorStageState(const GameSession& session,std::uint32_t frame,std::vector<std::uint8_t>& bytes){
    if(!session.multiplayer_active||session.state.phase!=GameSessionPhase::stage||!session.battle||
       !session.battle->stage_active||session.battle->transitioning||session.battle->outgoing_stage)return false;
    SpectatorStageState s;s.frame=frame;s.stage=session.state.stage;s.session_frame=session.multiplayer_frame;
    s.stage_frame=session.state.frame;s.seat_count=session.multiplayer_options.seat_count;
    s.local_seat=session.multiplayer_options.local_seat;s.seed=session.multiplayer_options.seed;
    s.difficulty=std::uint32_t(session.multiplayer_options.difficulty);s.challenge=session.multiplayer_options.challenge;
    s.selections=session.multiplayer_options.selections;s.phase=std::uint32_t(session.state.phase);
    const auto& b=*session.battle;s.stage_frame=b.frame;s.mp_tick=b.mp_tick;s.mp_fragments=b.mp_fragments;
    s.mp_wipe_frames=b.mp_wipe_frames;s.mp_rank=b.mp_rank;s.score_units=session.economy.score_units;
    s.script_seed=session.animations.script_rng.seed;s.script_reserved=session.animations.script_rng.reserved;
    s.script_calls=session.animations.script_rng.calls;s.visual_seed=session.animations.visual_rng.seed;
    s.visual_reserved=session.animations.visual_rng.reserved;s.visual_calls=session.animations.visual_rng.calls;
    s.spell_flags=b.spell_flags;s.spell_id=b.spell_id;s.spell_bonus=b.spell_bonus;s.spell_elapsed=b.spell_elapsed;
    std::copy(std::begin(b.enemy_environment.shared_integers),std::end(b.enemy_environment.shared_integers),s.shared_integers.begin());
    s.rate=session.animations.rate;
    s.transitioning=b.transitioning;s.transition_started=b.transition_started;s.stage_active=b.stage_active;
    s.transition_previous=b.transition_timer.previous;s.transition_current=b.transition_timer.current;
    s.transition_fractional=b.transition_timer.fractional;
    if(!b.stage)return false;
    s.stage_instruction_offset=b.stage->state.instruction_offset;
    s.stage_script_previous=b.stage->state.script_timer.previous;s.stage_script_current=b.stage->state.script_timer.current;
    s.stage_script_fractional=b.stage->state.script_timer.fractional;
    s.stage_fade_previous=b.stage->state.fade_timer.previous;s.stage_fade_current=b.stage->state.fade_timer.current;
    s.stage_fade_fractional=b.stage->state.fade_timer.fractional;s.stage_frame_count=b.stage->state.frame_count;
    s.stage_draw_flags=b.stage->state.draw_flags;s.stage_effects_enabled=b.stage->state.effects_enabled;
    for(unsigned i=0;i<s.seat_count;++i){
        if(!b.pilots[i]||!b.pilots[i]->player)return false;const auto& p=*b.pilots[i];const auto& player=*p.player;
        auto& d=s.pilots[i];const auto& m=player.motion.state;
        d.ghost=p.ghost;d.life_latched=p.life_latched;d.life_hold=p.life_hold;d.power_taps=p.power_taps;
        d.power_gap=p.power_gap;d.ghost_random=p.ghost_random;d.challenge_misses=p.challenge_misses;
        d.ghost_dx=p.ghost_dx;d.ghost_dy=p.ghost_dy;d.x=m.x;d.y=m.y;d.weapon_mode=m.weapon_mode;
        d.motion_flags=m.flags;d.life_state=player.state.life_state;
        d.state_timer_previous=player.state.state_timer.previous;d.state_timer_current=player.state.state_timer.current;
        d.state_timer_fractional=player.state.state_timer.fractional;
        d.invincibility_previous=player.state.invincibility.previous;d.invincibility_current=player.state.invincibility.current;
        d.invincibility_fractional=player.state.invincibility.fractional;
        d.lives=p.economy.lives;d.power=p.economy.power;d.communication=p.economy.communication;
        d.graze=p.economy.graze;d.point_value=p.economy.point_value;
        d.keys_held=p.keys.held;d.keys_previous=p.keys.previous;d.keys_pressed=p.keys.pressed;d.keys_released=p.keys.released;
        d.keys_long_repeat=p.keys.long_repeat;d.keys_short_repeat=p.keys.short_repeat;d.key_counts=p.keys.counts;
        d.auto_focus_frames=p.keys.auto_focus_frames;
        if(p.bomb){d.bomb_active=p.bomb->state.active;d.bomb_elapsed_previous=p.bomb->state.elapsed.previous;
            d.bomb_elapsed_current=p.bomb->state.elapsed.current;d.bomb_elapsed_fractional=p.bomb->state.elapsed.fractional;}
        if(!SpectatorShotAccess::Capture(player,s.shots[i]))return false;
    }
    for(auto* n=b.enemies.first;n;n=n->next)s.enemy_flags.push_back(n->value->state.flags);
    s.canonical_hash=session.multiplayer_hash();Netplay::SpectatorScalars::Writer writer;
    if(!WriteSpectatorStageState(writer,s))return false;bytes=writer.Bytes();
    return !bytes.empty()&&bytes.size()<=Netplay::SpectatorStageMaxStateBytes;
}

inline bool ApplySpectatorStageState(GameSession& session,const SpectatorStageState& s){
    if(!session.multiplayer_active||!session.battle||session.state.stage!=s.stage||
       session.multiplayer_options.seat_count!=s.seat_count)return false;
    auto& b=*session.battle;session.multiplayer_frame=s.session_frame;session.state.frame=s.stage_frame;
    session.economy.score_units=s.score_units;session.animations.script_rng.seed=s.script_seed;
    session.animations.script_rng.reserved=s.script_reserved;session.animations.script_rng.calls=s.script_calls;
    session.animations.visual_rng.seed=s.visual_seed;session.animations.visual_rng.reserved=s.visual_reserved;
    session.animations.visual_rng.calls=s.visual_calls;session.animations.rate=s.rate;
    b.frame=s.stage_frame;b.mp_tick=s.mp_tick;b.mp_fragments=s.mp_fragments;b.mp_wipe_frames=s.mp_wipe_frames;
    b.mp_rank=s.mp_rank;b.spell_flags=s.spell_flags;b.spell_id=s.spell_id;b.spell_bonus=s.spell_bonus;
    b.spell_elapsed=s.spell_elapsed;std::copy(s.shared_integers.begin(),s.shared_integers.end(),std::begin(b.enemy_environment.shared_integers));
    b.transitioning=s.transitioning!=0;b.transition_started=s.transition_started!=0;b.stage_active=s.stage_active!=0;
    b.transition_timer.previous=s.transition_previous;b.transition_timer.current=s.transition_current;
    b.transition_timer.fractional=s.transition_fractional;
    if(!b.stage)return false;b.stage->state.instruction_offset=s.stage_instruction_offset;
    b.stage->state.script_timer.previous=s.stage_script_previous;b.stage->state.script_timer.current=s.stage_script_current;
    b.stage->state.script_timer.fractional=s.stage_script_fractional;
    b.stage->state.fade_timer.previous=s.stage_fade_previous;b.stage->state.fade_timer.current=s.stage_fade_current;
    b.stage->state.fade_timer.fractional=s.stage_fade_fractional;b.stage->state.frame_count=s.stage_frame_count;
    b.stage->state.draw_flags=s.stage_draw_flags;b.stage->state.effects_enabled=s.stage_effects_enabled;
    if(b.enemies.count!=s.enemy_flags.size())return false;
    u32 enemy_index=0;for(auto* n=b.enemies.first;n;n=n->next)n->value->state.flags=s.enemy_flags[enemy_index++];
    for(unsigned i=0;i<s.seat_count;++i){
        if(!b.pilots[i]||!b.pilots[i]->player)return false;auto& p=*b.pilots[i];auto& player=*p.player;const auto& d=s.pilots[i];
        p.ghost=d.ghost!=0;p.life_latched=d.life_latched!=0;p.life_hold=d.life_hold;p.power_taps=d.power_taps;
        p.power_gap=d.power_gap;p.ghost_random=d.ghost_random;p.challenge_misses=d.challenge_misses;
        p.ghost_dx=d.ghost_dx;p.ghost_dy=d.ghost_dy;auto& m=player.motion.state;m.x=d.x;m.y=d.y;
        m.position={float(d.x)/128.f,float(d.y)/128.f,0};m.weapon_mode=d.weapon_mode;m.flags=d.motion_flags;
        player.state.life_state=d.life_state;player.state.state_timer.previous=d.state_timer_previous;
        player.state.state_timer.current=d.state_timer_current;player.state.state_timer.fractional=d.state_timer_fractional;
        player.state.invincibility.previous=d.invincibility_previous;player.state.invincibility.current=d.invincibility_current;
        player.state.invincibility.fractional=d.invincibility_fractional;
        p.economy.lives=d.lives;p.economy.power=d.power;p.economy.communication=d.communication;
        p.economy.graze=d.graze;p.economy.point_value=d.point_value;p.keys.held=d.keys_held;p.keys.previous=d.keys_previous;
        p.keys.pressed=d.keys_pressed;p.keys.released=d.keys_released;p.keys.long_repeat=d.keys_long_repeat;
        p.keys.short_repeat=d.keys_short_repeat;p.keys.counts=d.key_counts;p.keys.auto_focus_frames=d.auto_focus_frames;
        if(p.bomb){p.bomb->state.active=d.bomb_active;p.bomb->state.elapsed.previous=d.bomb_elapsed_previous;
            p.bomb->state.elapsed.current=d.bomb_elapsed_current;p.bomb->state.elapsed.fractional=d.bomb_elapsed_fractional;}
        if(!SpectatorShotAccess::Apply(player,s.shots[i]))return false;
    }
    // Restoring resource-local ANMs executes their constructor instruction.
    // Preserve the source RNG frontier after all constructors have completed.
    session.animations.script_rng={s.script_seed,s.script_reserved,s.script_calls};
    session.animations.visual_rng={s.visual_seed,s.visual_reserved,s.visual_calls};
    return session.multiplayer_hash()==s.canonical_hash;
}

inline bool DecodeSpectatorStageState(const std::vector<std::uint8_t>& bytes,SpectatorStageState& s){
    if(bytes.empty()||bytes.size()>Netplay::SpectatorStageMaxStateBytes)return false;
    Netplay::SpectatorScalars::Reader reader(bytes);return ReadSpectatorStageState(reader,s);
}
inline bool EncodeSpectatorStageState(const SpectatorStageState& s,std::vector<std::uint8_t>& bytes){
    Netplay::SpectatorScalars::Writer writer;if(!WriteSpectatorStageState(writer,s))return false;
    bytes=writer.Bytes();return !bytes.empty()&&bytes.size()<=Netplay::SpectatorStageMaxStateBytes;
}
} // namespace th11::multiplayer
