#include "stage_data.hpp"
#include <stdexcept>
namespace th20::source::gameplay {
// Initialized data extracted from verified original .data; all code is C++.
const StageDefinition stages[8]={
    {0,"st01.std","st00.ecl",
        {"th20_02","th20_02"},
        {"st01a.msg","st01b.msg","st01c.msg","st01d.msg",nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr},
        "st01logo.anm",
        {1,2,0,3,13,0,0,0,0,3,21,3,17,3,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {1,"st01.std","st01.ecl",
        {"th20_02","th20_03"},
        {"st01r0.msg","st01r0.msg","st01r1.msg","st01r1.msg","st01r2.msg","st01r2.msg","st01r3.msg","st01r3.msg","st01m0.msg","st01m0.msg","st01m1.msg","st01m1.msg","st01m2.msg","st01m2.msg","st01m3.msg","st01m3.msg"},
        "st01logo.anm",
        {1,2,0,3,13,0,0,0,0,3,21,3,17,3,12,0,3,13,0,0,0,0,3,21,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {2,"st02.std","st02.ecl",
        {"th20_04","th20_05"},
        {"st02r0.msg","st02r0.msg","st02r1.msg","st02r1.msg","st02r2.msg","st02r2.msg","st02r3.msg","st02r3.msg","st02m0.msg","st02m0.msg","st02m1.msg","st02m1.msg","st02m2.msg","st02m2.msg","st02m3.msg","st02m3.msg"},
        "st02logo.anm",
        {3,4,0,3,13,0,0,0,0,3,21,3,17,3,12,1,3,13,0,0,0,0,3,21,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {3,"st03.std","st03.ecl",
        {"th20_06","th20_07"},
        {"st03r0.msg","st03r0.msg","st03r1.msg","st03r1.msg","st03r2.msg","st03r2.msg","st03r3.msg","st03r3.msg","st03m0.msg","st03m0.msg","st03m1.msg","st03m1.msg","st03m2.msg","st03m2.msg","st03m3.msg","st03m3.msg"},
        "st03logo.anm",
        {5,6,0,3,10,0,0,0,0,3,18,3,14,3,9,2,3,10,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {4,"st04.std","st04.ecl",
        {"th20_08","th20_09"},
        {"st04r0.msg","st04r0.msg","st04r1.msg","st04r1.msg","st04r2.msg","st04r2.msg","st04r3.msg","st04r3.msg","st04m0.msg","st04m0.msg","st04m1.msg","st04m1.msg","st04m2.msg","st04m2.msg","st04m3.msg","st04m3.msg"},
        "st04logo.anm",
        {7,8,0,3,10,0,0,0,0,3,18,3,14,3,9,3,3,10,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {5,"st05.std","st05.ecl",
        {"th20_10","th20_11"},
        {"st05r0.msg","st05r0.msg","st05r1.msg","st05r1.msg","st05r2.msg","st05r2.msg","st05r3.msg","st05r3.msg","st05m0.msg","st05m0.msg","st05m1.msg","st05m1.msg","st05m2.msg","st05m2.msg","st05m3.msg","st05m3.msg"},
        "st05logo.anm",
        {9,10,0,3,10,0,0,0,0,3,19,3,15,3,9,4,-1,-1,0,0,0,0,-1,-1,4,9,4,8,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {6,"st06.std","st06.ecl",
        {"th20_13","th20_12"},
        {"st06r0.msg","st06r0.msg","st06r1.msg","st06r1.msg","st06r2.msg","st06r2.msg","st06r3.msg","st06r3.msg","st06m0.msg","st06m0.msg","st06m1.msg","st06m1.msg","st06m2.msg","st06m2.msg","st06m3.msg","st06m3.msg"},
        "st06logo.anm",
        {11,12,0,3,10,0,3,11,0,3,24,3,20,3,9,5,-1,-1,0,0,0,0,-1,-1,4,9,4,8,-1,-1,0,1,0,0,0,3,24,3,20,3,9,-1,0,0,0,0,0,0,0,0,0,0,0,0,0}},
    {7,"st07.std","st07.ecl",
        {"th20_17","th20_18"},
        {"st07r0.msg","st07r1.msg","st07r2.msg","st07r3.msg","st07r4.msg","st07r5.msg","st07r6.msg","st07r7.msg","st07m0.msg","st07m1.msg","st07m2.msg","st07m3.msg","st07m4.msg","st07m5.msg","st07m6.msg","st07m7.msg"},
        "st07logo.anm",
        {13,14,0,3,18,0,0,0,0,3,29,3,25,3,17,6,4,10,1,0,0,0,4,18,4,14,4,9,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}
};
const StageDefinition* selected_stage=nullptr;
const std::int32_t initial_stage_parameter[6]={5,5,5,5,0,0};
const std::int32_t meter_minimum[6]={10000,10000,10000,10000,10000,100000};
const std::int32_t meter_maximum[6]={200000,500000,700000,1000000,500000,1000000};
void select_stage(game_session::PlayerTable& table,std::int32_t id) {
    // Invalid indices originally read past the table; fail explicitly instead.
    if(id<0||id>=8) throw std::out_of_range("Original stage table index");
    selected_stage=&stages[id];player_state::write(table,0x1f4,id);
}
const char* stage_background(const StageDefinition& stage,const game_session::Player& player) noexcept {
    if(stage.id==4) switch(player_state::read<std::uint32_t>(player,0xc)) {
        case 0:case 1:return "st04b.std";
        case 2:case 3:return "st04d.std";
        case 4:case 5:return "st04c.std";
        case 6:case 7:return "st04a.std";
    }
    return stage.std_file;
}
void restore_stage(game_session::PlayerTable& table) {select_stage(table,player_state::read<std::int32_t>(table,0x1f8));}
}
