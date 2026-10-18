#include "AnmEnvironment.hpp"
namespace th10 {
// 0x43ec70. Unknown variable IDs remain literal values.
i32 AnmVm::integer_variable(i32 value) const noexcept {
    const u32 index=static_cast<u32>(value)-10000;
    if(index<4)return integer_variables[index];
    if(index<8)return Scalar::truncate(float_variables[index-4]);
    if(index<10)return extra_integer_variables[index-8];
    return value;
}
// 0x43eac0. Integer storage is returned at extended precision.
Extended AnmVm::float_variable(float value,AnmEnvironment& env) const noexcept {
    const u32 index=static_cast<u32>(Scalar::truncate(value))-10000;
    if(index<4)return Extended::from_int(integer_variables[index]);
    if(index<8)return number(float_variables[index-4]);
    if(index<10)return Extended::from_int(extra_integer_variables[index-8]);
    if(index==10)return env.random(*this).signed_unit()*number(3.1415927410125732421875f);
    if(index==11)return env.random(*this).unit();
    if(index==12)return env.random(*this).signed_unit();
    if(index<16){const float coordinates[]={script_position.x,script_position.y,script_position.z};return number(coordinates[index-13]);}
    if(index<22){const u32 component=index-16;float coordinate;std::memcpy(&coordinate,reinterpret_cast<const u8*>(env.reference_positions[component/3])+(component%3)*4,4);return number(coordinate);}
    return number(value);
}
// 0x43eda0 / 0x43ed00. Literal destinations modify the instruction itself.
i32* AnmVm::integer_reference(u32 index,u16 references,i32* argument) noexcept {
    if(references&(1u<<(index&31))){
        const u32 variable=static_cast<u32>(*argument)-10000;
        if(variable<4)return &integer_variables[variable];
        if(variable==8||variable==9)return &extra_integer_variables[variable-8];
    }
    return argument;
}
float* AnmVm::float_reference(u32 index,u16 references,float* argument) noexcept {
    if(references&(1u<<(index&31))){
        const u32 variable=static_cast<u32>(Scalar::truncate(*argument))-10000;
        if(variable>=4&&variable<8)return &float_variables[variable-4];
        if(variable>=13&&variable<16)return reinterpret_cast<float*>(reinterpret_cast<u8*>(&script_position)+(variable-13)*4);
    }
    return argument;
}
}
