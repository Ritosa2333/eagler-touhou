#include "EclContext.hpp"
#include "PortTitle.hpp"
namespace th11 {
i32 EclStack::push(EclValueType type,u32 bits)noexcept{
    // Original checks only the four-byte value before its optional type slot.
    // Reject malformed negative tops instead of writing outside stack storage.
    if(top<0||(top&3)||top>4096-8)return -1;
    if(type!=EclValueType::Untyped){data[top]=u8(type);top+=4;}
    std::memcpy(data+top,&bits,4);top+=4;return 0;
}
bool EclStack::pop_raw(u32& bits)noexcept{if(top<4)return false;top-=4;std::memcpy(&bits,data+top,4);return true;}
bool EclStack::pop(u32& bits,EclValueType& type)noexcept{if(top<8)return false;top-=4;std::memcpy(&bits,data+top,4);top-=4;type=EclValueType(data[top]);return true;}
i32 EclStack::enter_frame(i32 local_bytes)noexcept{const i32 previous=top,next=wrapping_add(top,local_bytes);if(next<0||next>=4096)return -1;top=next;if(next+4<4096){std::memcpy(data+top,&frame_base,4);top+=4;}frame_base=previous;return 0;}
void EclStack::leave_frame()noexcept{const i32 previous=frame_base;u32 bits;if(pop_raw(bits))frame_base=signed_bits(bits);top=previous;}
i32 ecl_stack_clamp_count=0,ecl_stack_clamp_first=0;
const EclInstruction* ecl_last_integer_access=nullptr;
i32 ecl_last_integer_argument=0,ecl_clamp_opcode=0,ecl_clamp_argument=0;
i32 ecl_stack_clamp(i32 position,u32 size)noexcept{
    if(position>=0&&u32(position)+size<=sizeof(EclStack::data))return position;
    ++ecl_stack_clamp_count;
    if(position>ecl_stack_clamp_first)ecl_stack_clamp_first=position;
    if(ecl_last_integer_access){
        ecl_clamp_opcode=ecl_last_integer_access->opcode;
        ecl_clamp_argument=ecl_last_integer_argument;
    }
    if(position<0)return 0;
    const u32 limit=u32(sizeof(EclStack::data))-size;
    return position>i32(limit)?i32(limit):position;
}
i32 EclContext::resolve_integer(i32 value,EclGlobals& globals){
    ecl_last_integer_access=instruction;if(value>=0)return stack.local<i32>(value);
    if constexpr((kPortTitle==14||kPortTitle==16)){
        if(value>=-100&&value<=-1){const i32 at=ecl_stack_clamp(stack.top+value*8,8);u32 bits;std::memcpy(&bits,stack.data+at+4,4);return stack.data[at]=='f'?truncate_int(float_bits(bits)):signed_bits(bits);}
        return globals.integer(value);
    }
    if(value!=-1)return globals.integer(value);u32 bits=u32(value);EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Float)return truncate_int(float_bits(bits));return signed_bits(bits);
}
double EclContext::resolve_float(float value,EclGlobals& globals){
    ecl_last_integer_access=instruction;if(value>=0)return stack.local<float>(truncate_int(value));
    if constexpr((kPortTitle==14||kPortTitle==16)){
        if(value>=-100&&value<=-1){const i32 at=ecl_stack_clamp(stack.top+truncate_int(value*8.f),8);u32 bits;std::memcpy(&bits,stack.data+at+4,4);return stack.data[at]=='i'?float(signed_bits(bits)):float_bits(bits);}
        return float(globals.floating(truncate_int(value)));
    }
    if(value!=-1)return globals.floating(truncate_int(value));u32 bits=float_bits(value);EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Integer)return float(signed_bits(bits));return float_bits(bits);
}
i32 EclContext::integer_argument(u32 index,EclGlobals& globals){ecl_last_integer_access=instruction;ecl_last_integer_argument=i32(index);const i32 value=instruction->argument<i32>(index);if(!(instruction->references&(1u<<(index&31))))return value;if((kPortTitle==14||kPortTitle==16)||value!=-1)return resolve_integer(value,globals);u32 bits=index;EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Float)return truncate_int(float_bits(bits));return signed_bits(bits);}
double EclContext::float_argument(u32 index,EclGlobals& globals){ecl_last_integer_access=instruction;ecl_last_integer_argument=i32(index);const float value=instruction->argument<float>(index);if(!(instruction->references&(1u<<(index&31))))return value;if((kPortTitle==14||kPortTitle==16)||value!=-1)return resolve_float(value,globals);u32 bits=index;EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Integer)return float(signed_bits(bits));return float_bits(bits);}
i32* EclContext::integer_reference(u32 index,EclGlobals& globals){ecl_last_integer_access=instruction;ecl_last_integer_argument=i32(index);if(!(instruction->references&(1u<<(index&31))))return nullptr;const i32 value=instruction->argument<i32>(index);return value<0?globals.integer_reference(value):reinterpret_cast<i32*>(stack.data+ecl_stack_clamp(stack.frame_base+value,sizeof(i32)));}
float* EclContext::float_reference(u32 index,EclGlobals& globals){ecl_last_integer_access=instruction;ecl_last_integer_argument=i32(index);if(!(instruction->references&(1u<<(index&31))))return nullptr;const float value=instruction->argument<float>(index);const i32 offset=truncate_int(value);return value>=0?reinterpret_cast<float*>(stack.data+ecl_stack_clamp(stack.frame_base+offset,sizeof(float))):globals.float_reference(offset);}
}
