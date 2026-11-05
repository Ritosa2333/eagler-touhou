#include "EclContext.hpp"
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
i32 EclContext::resolve_integer(i32 value,EclGlobals& globals){if(value>=0)return stack.local<i32>(value);if(value!=-1)return globals.integer(value);u32 bits=u32(value);EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Float)return truncate_int(float_bits(bits));return signed_bits(bits);}
double EclContext::resolve_float(float value,EclGlobals& globals){if(value>=0)return stack.local<float>(truncate_int(value));if(value!=-1)return globals.floating(truncate_int(value));u32 bits=float_bits(value);EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Integer)return float(signed_bits(bits));return float_bits(bits);}
i32 EclContext::integer_argument(u32 index,EclGlobals& globals){const i32 value=instruction->argument<i32>(index);if(!(instruction->references&(1u<<(index&31))))return value;if(value!=-1)return resolve_integer(value,globals);u32 bits=index;EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Float)return truncate_int(float_bits(bits));return signed_bits(bits);}
double EclContext::float_argument(u32 index,EclGlobals& globals){const float value=instruction->argument<float>(index);if(!(instruction->references&(1u<<(index&31))))return value;if(value!=-1)return resolve_float(value,globals);u32 bits=index;EclValueType type;if(stack.pop(bits,type)&&type==EclValueType::Integer)return float(signed_bits(bits));return float_bits(bits);}
i32* EclContext::integer_reference(u32 index,EclGlobals& globals){if(!(instruction->references&(1u<<(index&31))))return nullptr;const i32 value=instruction->argument<i32>(index);return value<0?globals.integer_reference(value):reinterpret_cast<i32*>(stack.data+stack.frame_base+value);}
float* EclContext::float_reference(u32 index,EclGlobals& globals){if(!(instruction->references&(1u<<(index&31))))return nullptr;const float value=instruction->argument<float>(index);const i32 offset=truncate_int(value);return value>=0?reinterpret_cast<float*>(stack.data+stack.frame_base+offset):globals.float_reference(offset);}
}
