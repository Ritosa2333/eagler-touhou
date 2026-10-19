#include "enemy_opcode_misc.hpp"
#include "enemy_variables.hpp"
#include <bit>
namespace th20::source::gameplay {
EnemyOpcodeResult execute_enemy_misc_opcode(EnemyOpcodeReader& r,EnemyMiscOpcodeServices& env){
    switch(r.opcode()){
    case 800:{auto* target=env.find(r.controller(),unsigned(r.integer(0)));const auto* name=r.text(8,r.raw_word(4));if(target)env.select_script(*target,name);break;}
    case 801:{auto* target=env.find(r.controller(),unsigned(r.integer(2)));if(!target)throw std::logic_error("Original Enemy position lookup requires a valid target");r.float_destination(0)=std::bit_cast<unsigned>(enemy_position(target).x);r.float_destination(1)=std::bit_cast<unsigned>(enemy_position(target).y);break;}
    case 802:{const auto index=unsigned(r.integer(0));for(unsigned slot=0;slot<3;++slot){auto* target=env.selected(r.controller(),slot);if(slot!=r.state.fields_250[8]&&target){auto& entry=target->state.auxiliary.at(index);if(recovered::signed_bits(entry.values[0])>=0){const auto* name=reinterpret_cast<const char*>(entry.values.data()+2);env.select_script(*target,name);}}}break;}
    case 1001:{const int color=r.integer(0);env.attach_stone(r.state.identifier,color);r.state.fields_2c8[1]|=0x40000000u;const auto value=unsigned(r.integer(0));r.state.fields_2c8[2]=(r.state.fields_2c8[2]&~3u)|(value&3u);break;}
    case 1002:r.controller().field_e0=unsigned(r.integer(0));break;
    case 1003:r.state.fields_250[10]=unsigned(r.integer(0));break;
    default:return std::nullopt;
    }
    return 0;
}
}
