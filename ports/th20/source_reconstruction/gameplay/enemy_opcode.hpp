#pragma once
#include "enemy_entity.hpp"
#include <cstring>
#include <optional>
#include <stdexcept>
namespace th20::source::gameplay {
// 48c010 receives EnemyState (+88), not Enemy. Arguments are resolved against
// Enemy.current. Ordinary entity reads peek tagged stack operands, never pop.
class EnemyOpcodeReader {
public:
    EnemyState& state;
    Enemy& entity;
    EnemyRuntime& runtime;
    ecl::Instruction instruction;
    explicit EnemyOpcodeReader(EnemyState& value):state(value),entity(*static_cast<Enemy*>(value.entity)),runtime(*entity.current),instruction(runtime.current()){}
    unsigned opcode() const{return instruction.opcode();}
    std::int32_t integer(int index){return runtime.integer_argument(instruction,index);} //4ab080/53ed50
    float real(int index){return runtime.float_argument(instruction,index);} //4aafb0/53e970
    std::int32_t integer_value(int mask_index,std::uint32_t raw){return runtime.integer_value(instruction,mask_index,raw);} //4ab0b0
    float real_value(int mask_index,float raw){return runtime.float_value(instruction,mask_index,raw);} //4aafe0
    std::uint32_t& integer_destination(int index){return runtime.integer_destination(instruction,index);} //4aa9e0
    std::uint32_t& float_destination(int index){return runtime.float_destination(instruction,index);} //4aa980
    // Raw payload offsets count from instruction+10; length-prefixed strings
    // begin at payload+4. Callers preserve the original logical mask index.
    std::uint32_t raw_word(unsigned byte_offset) const{
        if(std::size_t(byte_offset)+20>instruction.size())throw std::out_of_range("Truncated entity ECL argument");
        std::uint32_t value;std::memcpy(&value,instruction.bytes+16+byte_offset,4);return value;
    }
    const char* text(unsigned byte_offset,unsigned size) const{
        if(std::size_t(byte_offset)+size+16>instruction.size())throw std::out_of_range("Truncated entity ECL string");
        const auto* p=reinterpret_cast<const char*>(instruction.bytes+16+byte_offset);
        if(!std::memchr(p,0,size))throw std::runtime_error("Unterminated entity ECL string");return p;
    }
    template<class T> T get(unsigned offset) const{T value;std::memcpy(&value,reinterpret_cast<const std::uint8_t*>(&state)+offset,sizeof(T));return value;}
    template<class T> void put(unsigned offset,T value){std::memcpy(reinterpret_cast<std::uint8_t*>(&state)+offset,&value,sizeof(T));}
    game_session::Context& context() const{return *get<game_session::Context*>(0x2ec);}
    EnemyController& controller() const{return *static_cast<EnemyController*>(context().objects_04[1]);}
};
// nullopt means this handler does not own the opcode, not successful execution.
// Original default returns0; recognized but unfinished cases must stay explicit.
using EnemyOpcodeResult=std::optional<int>;
EnemyOpcodeResult execute_enemy_animation_opcode(EnemyOpcodeReader&); //300..344
EnemyOpcodeResult execute_enemy_movement_opcode(EnemyOpcodeReader&); //400..448
EnemyOpcodeResult execute_enemy_state_opcode(EnemyOpcodeReader&); //500..575 (569 absent)
EnemyOpcodeResult execute_enemy_bullet_opcode(EnemyOpcodeReader&); //600..633
EnemyOpcodeResult execute_enemy_laser_opcode(EnemyOpcodeReader&); //700..714
EnemyOpcodeResult execute_enemy_misc_opcode(EnemyOpcodeReader&); //800..802,1001..1003
int execute_enemy_opcode(EnemyState&); //48c010
}
