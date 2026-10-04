#include "PlayerResources.hpp"
#include "ResourceTrace.hpp"

namespace th08 {

PlayerResourceView::PlayerResourceView(GameGlobals& shared, PilotResources& pilot) noexcept
    :shared(shared),pilot(pilot),
     display_score(shared.display_score),graze_stage(shared.graze_stage),score(shared.score),
     graze(shared.graze),score_increment(shared.score_increment),high_score(shared.high_score),
     high_score_retries(shared.high_score_retries),captured_spells(shared.captured_spells),
     gauge_copy(pilot.gauge_copy),gauge(pilot.gauge),point_value(shared.point_value),
     clock_time(shared.clock_time),retries(shared.retries),points_stage(shared.points_stage),
     points(shared.points),point_extends(shared.point_extends),
     next_point_extend(shared.next_point_extend),time_orbs(shared.time_orbs),
     last_spell_requirement(shared.last_spell_requirement),total_time_orbs(shared.total_time_orbs),
     deaths(pilot.deaths),deaths_stage(pilot.deaths_stage),lives(pilot.lives),bombs(pilot.bombs),
     bombs_used(pilot.bombs_used),bombs_used_stage(pilot.bombs_used_stage),power(pilot.power){}

void PlayerValues::set_lives(i32 value){resources.lives=Extended::from_int(value).to_float();}
void PlayerValues::set_bombs(i32 value){resources.bombs=resources.pilot.challenge_mode?0:Extended::from_int(value).to_float();}
void PlayerValues::set_power(i32 value){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Scope trace("power.set",multiplayer::diagnostic::Seat(&resources.pilot),value);
#endif
    resources.power=Extended::from_int(value).to_float();
}
void PlayerValues::set_deaths_stage(i32 value){resources.deaths_stage=Extended::from_int(value).to_float();}
void PlayerValues::set_bombs_stage(i32 value){resources.bombs_used_stage=Extended::from_int(value).to_float();}

bool PlayerValues::add_lives(i32 value){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Scope trace("lives.add",multiplayer::diagnostic::Seat(&resources.pilot),value);
#endif
    resources.lives=(number(resources.lives)+Extended::from_int(value)).to_float();
    return true;
}
bool PlayerValues::add_bombs(i32 value){
    if(resources.pilot.challenge_mode){resources.bombs=0;return true;}
    resources.bombs=(number(resources.bombs)+Extended::from_int(value)).to_float();
    return true;
}
bool PlayerValues::add_power(i32 value){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Scope trace("power.add",multiplayer::diagnostic::Seat(&resources.pilot),value);
#endif
    resources.power=(number(resources.power)+Extended::from_int(value)).to_float();
    return true;
}
bool PlayerValues::add_deaths(i32 value){
    resources.deaths=(Extended::from_int(value)+number(resources.deaths)).to_float();
    resources.deaths_stage=(Extended::from_int(value)+number(resources.deaths_stage)).to_float();
    high_score.deaths=wrapping_add(high_score.deaths,1);
    return true;
}
bool PlayerValues::count_bombs(i32 value){
    resources.bombs_used=(Extended::from_int(value)+number(resources.bombs_used)).to_float();
    resources.bombs_used_stage=(Extended::from_int(value)+number(resources.bombs_used_stage)).to_float();
    return true;
}
void PlayerValues::add_time_orbs(i32 value){
#if defined(TH_MULTIPLAYER_FIXTURES) || defined(TH_MULTIPLAYER_RESOURCE_TRACE)
    multiplayer::diagnostic::Scope trace("time.add",multiplayer::diagnostic::Seat(&resources.pilot),value);
#endif
    if(value<0&&resources.time_orbs<wrapping_sub(0,value)){
        resources.time_orbs=0;
        return;
    }
    resources.time_orbs=wrapping_add(resources.time_orbs,value);
    resources.total_time_orbs=wrapping_add(resources.total_time_orbs,value);
    high_score.time_orbs=wrapping_add(high_score.time_orbs,value);
    if(value>0){
        const i32 half=wrapping_add(value,resources.total_time_orbs&1)/2;
        resources.point_value=wrapping_add(resources.point_value,signed_bits(u32(half)*10));
    }
}

} // namespace th08
