#include "enemy_opcode.hpp"
#include "enemy_shot.hpp"
#include "enemy_opcode_laser.hpp"
#include "enemy_opcode_misc.hpp"
#include <string>
namespace th20::source::gameplay {
int execute_enemy_opcode(EnemyState& state){ //48c010, normal epilogue4963df explicitly returns0
    EnemyOpcodeReader reader(state);const unsigned opcode=reader.opcode();EnemyOpcodeResult result;
    if(opcode>=300&&opcode<=344)result=execute_enemy_animation_opcode(reader);
    else if(opcode>=400&&opcode<=448)result=execute_enemy_movement_opcode(reader);
    else if(opcode>=500&&opcode<=575&&opcode!=569)result=execute_enemy_state_opcode(reader);
    else if(opcode>=600&&opcode<=633)result=execute_enemy_bullet_opcode(reader);
    else if(opcode>=700&&opcode<=714)result=execute_enemy_laser_opcode(reader);
    else if((opcode>=800&&opcode<=802)||(opcode>=1001&&opcode<=1003))result=execute_enemy_misc_opcode(reader);
    else return 0; //actual original jump-table default, including undefined opcode569
    if(!result)throw std::runtime_error("Unrecovered enemy ECL opcode "+std::to_string(opcode));
    return *result;
}
namespace unrecovered {int execute_enemy_opcode_0048c010(EnemyState& state){return execute_enemy_opcode(state);}}
}
