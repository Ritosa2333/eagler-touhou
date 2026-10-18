#include "EnemySpawnEnvironment.hpp"
namespace th10 {
// Enemy creation cases of 0x40e770. The string occupies an aligned variable
// number of argument words; reference bits still use the logical argument index.
bool EnemyState::spawn_command(EclContext& context,EclGlobals& globals,EnemySpawnEnvironment& env){
    auto opcode=context.instruction->opcode;
    if(opcode>=0x109&&opcode<=0x10c){if(*env.boss)return true;opcode-=opcode<0x10b?9:7;}
    if(opcode==0x10f){if(*env.boss)return true;opcode=0x10e;}
    if(opcode!=0x100&&opcode!=0x101&&opcode!=0x104&&opcode!=0x105&&opcode!=0x10e)return false;
    const auto* instruction=context.instruction;
    const auto offset=static_cast<u32>(wrapping_add(static_cast<i32>(instruction->argument(0)),4)/4)*4;
    const auto raw=[&](u32 index){u32 bits;__builtin_memcpy(&bits,reinterpret_cast<const u8*>(instruction)+16+offset+(index-1)*4,4);return bits;};
    const auto floating=[&](u32 index){const auto bits=raw(index);float value;__builtin_memcpy(&value,&bits,4);return context.resolve_float(index,value,globals);};
    const auto integer=[&](u32 index){return context.resolve_integer(index,static_cast<i32>(raw(index)),globals);};
    EnemySpawnParameters parameters{};
    if(opcode==0x10e){
        parameters.position.x=(floating(1)+number(env.camera_origin->x)).to_float();parameters.position.y=(floating(2)+number(env.camera_origin->y)).to_float();parameters.position.z=floating(3).to_float();
        parameters.health=integer(4);parameters.score=integer(5);parameters.drop_kind=integer(6);parameters.flags=1;
    }else{
        const bool relative=opcode==0x100||opcode==0x104;
        if(relative){const float origin_x=current.position.x;parameters.position.x=(floating(1)+number(origin_x)).to_float();const float origin_y=current.position.y;parameters.position.y=(floating(2)+number(origin_y)).to_float();}
        else{parameters.position.x=floating(1).to_float();parameters.position.y=floating(2).to_float();}
        if(opcode==0x100&&(flags&0x40000)){
            parameters.position.z=current.position.z;parameters.position=env.project_world(parameters.position);
            parameters.position.x=Scalar::sub(parameters.position.x,224.f);parameters.position.y=Scalar::sub(parameters.position.y,16.f);parameters.position.z=0;
        }
        parameters.health=integer(3);parameters.score=integer(4);parameters.drop_kind=integer(5);parameters.mirror=opcode==0x104||opcode==0x105?1:0;
    }
    __builtin_memcpy(parameters.integer_variables,integer_variables,sizeof(integer_variables));__builtin_memcpy(parameters.float_variables,float_variables,sizeof(float_variables));
    env.spawn(reinterpret_cast<const char*>(instruction)+20,parameters);return true;
}
}
