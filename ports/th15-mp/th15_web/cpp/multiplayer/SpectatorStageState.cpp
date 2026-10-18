#include "SpectatorStageState.hpp"
#include <tuple>

namespace th15::multiplayer {
namespace {
template<class C,class T> bool scalar(C& c,T& v){return c.Value(v);}
template<class C,class T,std::size_t N> bool scalar(C& c,std::array<T,N>& v){for(auto& x:v)if(!scalar(c,x))return false;return true;}
template<class C,class... T> bool fields(C& c,T&... v){return (scalar(c,v)&&...);}
template<class C> bool vec(C& c,Vec3& v){return c.Values(v.x,v.y,v.z);}
template<class C> bool point(C& c,PlayerFixedPosition& v){return c.Values(v.x,v.y);}
template<class C> bool timer(C& c,Timer& v){return c.Values(v.previous,v.current,v.fractional,v.rate_index,v.flags);}
template<class C> bool input(C& c,GameInput& v){return fields(c,v.held,v.previous,v.repeated,v.pressed,v.released,v.long_held,v.repeat_age,v.duration);}
template<class C> bool resources(C& c,PlayerLifeSession& v){return c.Values(v.extra_lives,v.life_pieces,v.deaths,v.enemy_deaths,v.enemy_chain,v.bomb_state,v.mode_flags,v.power,v.power_step,v.bombs,v.bomb_pieces,v.replay_state);}
template<class C> bool motion(C& c,PlayerMotion& v){
    if(!c.Values(v.touch.mode,v.touch.x,v.touch.y,v.flip_vertical_step)||
       !point(c,v.position_fixed)||!point(c,v.last_step)||!point(c,v.step)||
       !vec(c,v.position)||!vec(c,v.velocity)||!vec(c,v.last_direction)||
       !c.Values(v.normal_speed,v.focus_speed,v.normal_diagonal_speed,v.focus_diagonal_speed,
          v.direction,v.focus,v.input_frame,v.option_follow_percentage,v.collapse_frame,v.option_count,
          v.behavior_flags,v.enlargement,v.movement_scale)||!vec(c,v.external_velocity)||!timer(c,v.barrier_timer))return false;
    for(auto& o:v.options)if(!c.Values(o.active,o.snap,o.index)||!point(c,o.target)||!point(c,o.position)||!point(c,o.normal_offset)||!point(c,o.focus_offset))return false;
    return true;
}
template<class C> bool pilot(C& c,SpectatorPilotState& v){
    return resources(c,v.resources)&&motion(c,v.motion)&&input(c,v.keys)&&
        c.Values(v.input.held,v.input.touch_mode,v.input.touch_x,v.input.touch_y,v.input.pause)&&
        timer(c,v.life_age)&&timer(c,v.invulnerability)&&timer(c,v.input_age)&&timer(c,v.ready_age)&&
        timer(c,v.shot_age)&&timer(c,v.continuous_age)&&vec(c,v.last_death_position)&&
        c.Values(v.life_state,v.base_bombs,v.power_level,v.shoot_auxiliary,v.life_piece_tier,
          v.shoot_auxiliary_active,v.ghost,v.spell_startup_bomb,v.life_latched,v.ghost_clock,
          v.ghost_random,v.life_hold,v.power_taps,v.power_gap,v.ghost_dx,v.ghost_dy)&&
        fields(c,v.option_angles,v.laser_power,v.reward_counters);
}
template<class C> bool progress(C& c,SessionState& v){
    return c.Values(v.stage,v.chapter,v.stage_frame,v.run_clock,v.character,v.subcharacter,
        v.difficulty,v.continues,v.spell_id,v.initial_point_value,v.high_score,v.high_score_extra,
        v.configured_lives)&&c.Array(v.stage_deaths)&&c.Values(v.chapter_deaths,v.rate,v.scene_flags,
        v.transition,v.startup_frames,v.new_run,v.replay,v.checkpoint_power,v.restart_frames,
        v.alternating_pieces,v.continue_budget,v.starting_stage);
}
template<class C> bool score(C& c,ItemScoreState& v){
    return c.Values(v.score,v.point_value,v.max_point_value,v.point_items,v.chapter_value,
        v.chapter_count,v.collection_timer)&&vec(c,v.collection_position)&&c.Values(v.life_piece_tier,
        v.difficulty,v.graze_total,v.graze_chapter,v.max_power);
}
template<class C> bool enemy(C& c,EnemyWorldState& v){
    return vec(c,v.player_position)&&c.Array(v.boss_ids)&&c.Values(v.manager_flags,v.rank,v.difficulty,
        v.mode,v.submode,v.enemy_count)&&c.Array(v.counters)&&c.Values(v.total,v.state,v.spell_state,v.player_state)&&
        fields(c,v.integer_registers,v.float_registers)&&c.Values(v.counter1,v.counter3,v.current_chapter,
        v.chapter_total,v.chapter_defeated,v.requested_chapter,v.scene_flags,v.local_spell.flags,
        v.local_spell.frame,v.local_spell.bonus,v.bomb_action,v.enemy_control,v.mode_flags,v.boss_seconds,
        v.boss_hundredths,v.game_state,v.boss_damage_gate,v.shot_damage,v.other_damage,
        v.player_damage_state,v.damage_disabled);
}
template<class C> bool body(C& c,SpectatorStageState& v){
    auto& s=v.setup;
    if(!c.Values(s.session_id,s.player_count,s.local_player,s.difficulty,s.seed,s.requested_delay,
         s.input_delay,s.prediction_reserve,s.automatic,s.challenge,s.chapter_threshold)||
       !c.Array(s.selections)||!c.Array(s.build)||
       !progress(c,v.progress)||!score(c,v.score)||!enemy(c,v.enemy)||
       !c.Values(v.game_random.seed,v.game_random.reserved,v.game_random.calls,
         v.visual_random.seed,v.visual_random.reserved,v.visual_random.calls)||!input(c,v.menu)||
       !c.Values(v.frame,v.application_frames,v.fragments))return false;
    for(auto& p:v.pilots)if(!pilot(c,p))return false;
    return true;
}

}
bool ValidSpectatorStageState(const SpectatorStageState& v){
    if(!v.setup.valid()||v.frame==Netplay::INVALID_FRAME||v.progress.stage<1||v.progress.stage>7||
       v.progress.difficulty!=i32(v.setup.difficulty)||v.progress.character!=i32(v.setup.selections[0])||
       (v.progress.stage==7)!=(v.progress.difficulty==4)||v.progress.replay||
       v.score.max_power!=400||v.enemy.enemy_count<0||v.enemy.enemy_count>4096||v.fragments<0||
       v.progress.rate<=0||v.progress.rate>1||v.progress.starting_stage<1||v.progress.starting_stage>7)return false;
    for(unsigned n=0;n<v.setup.player_count;++n){const auto& p=v.pilots[n];const auto& r=p.resources;
        if(r.extra_lives< -1||r.extra_lives>9||r.bombs<0||r.bombs>9||r.power<0||r.power>400||
           r.power_step!=100||p.motion.option_count<0||p.motion.option_count>8||
           p.life_state<0||p.life_state>4||p.input.touch_mode<0||p.input.touch_mode>3||
           p.motion.touch.mode<0||p.motion.touch.mode>3)return false;
    }
    return true;
}
bool RestoreSpectatorEnemyScalars(const EnemyWorldState& saved,EnemyWorldState& live){
    auto copy=saved;Netplay::SpectatorScalars::Writer writer;
    if(!enemy(writer,copy))return false;
    Netplay::SpectatorScalars::Reader reader(writer.Bytes());
    return enemy(reader,live)&&reader.Done();
}
bool EncodeSpectatorStageState(const SpectatorStageState& value,std::vector<u8>& out){
    if(!ValidSpectatorStageState(value))return false;
    auto copy=value;Netplay::SpectatorScalars::Writer writer;
    if(!writer.Value(SpectatorStageState::Schema)||!body(writer,copy)||!writer.Good())return false;
    out=writer.Bytes();return true;
}
bool DecodeSpectatorStageState(const std::vector<u8>& bytes,SpectatorStageState& out){
    if(bytes.empty()||bytes.size()>Netplay::SpectatorStageMaxStateBytes)return false;
    SpectatorStageState value;Netplay::SpectatorScalars::Reader reader(bytes);u32 schema=0;
    if(!reader.Value(schema)||schema!=SpectatorStageState::Schema||!body(reader,value)||
       !reader.Done()||!ValidSpectatorStageState(value))return false;
    out=std::move(value);return true;
}
}
