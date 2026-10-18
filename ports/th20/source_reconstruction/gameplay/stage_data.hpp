#pragma once
#include "player_state.hpp"
namespace th20::source::gameplay {
struct StageDefinition {
    std::int32_t id;
    const char* std_file;
    const char* ecl_file;
    const char* tracks[2];
    const char* messages[16];
    const char* logo;
    std::int32_t fields_58[55];
};
static_assert(sizeof(StageDefinition)==0x134 && offsetof(StageDefinition,logo)==0x54);
extern const StageDefinition stages[8];               // initialized data5b0038..5b09d8
extern const StageDefinition* selected_stage;          // BSS5c6110
extern const std::int32_t initial_stage_parameter[6];  //5afd08
extern const std::int32_t meter_minimum[6];            //5afd20, units before *100
extern const std::int32_t meter_maximum[6];            //5afd38
void select_stage(game_session::PlayerTable&,std::int32_t); //4be390, writes+1f4 and global
void restore_stage(game_session::PlayerTable&);              //4dd8d0, reads+1f8 without clamp
const char* stage_background(const StageDefinition&,const game_session::Player&) noexcept; //4bb8c2..4bb954
}
