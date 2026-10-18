#include "EclContext.hpp"
namespace th10 {
namespace {
i32 integer_bits(u32 bits) { i32 value; std::memcpy(&value, &bits, 4); return value; }
float float_bits(u32 bits) { float value; std::memcpy(&value, &bits, 4); return value; }
}
// 0x44fdb0. Empty stack returns the argument index, as in the original.
i32 EclContext::integer_argument(u32 index, EclGlobals& globals) {
    const i32 value = integer_bits(instruction->argument(index));
    if (!instruction->is_reference(index)) return value;
    if (value >= 0) return stack.local<i32>(value);
    if (value != -1) return globals.integer(value);
    u32 bits = index; EclValueType type;
    if (stack.pop_value(bits, type) && type == EclValueType::Float) return number(float_bits(bits)).truncate_int();
    return integer_bits(bits);
}
// 0x44fe40. Integer stack values retain extended precision until the caller stores them.
Extended EclContext::float_argument(u32 index, EclGlobals& globals) {
    const float value = float_bits(instruction->argument(index));
    if (!instruction->is_reference(index)) return number(value);
    if (value >= 0) return number(stack.local<float>(Scalar::truncate(value)));
    if (value != -1.0f) return globals.floating(Scalar::truncate(value));
    u32 bits = index; EclValueType type;
    if (stack.pop_value(bits, type) && type == EclValueType::Integer) return Extended::from_int(integer_bits(bits));
    return number(float_bits(bits));
}
// 0x44ff00. Used when a variable-length string precedes the numeric arguments.
i32 EclContext::resolve_integer(u32 index, i32 value, EclGlobals& globals) {
    if (!instruction->is_reference(index)) return value;
    if (value >= 0) return stack.local<i32>(value);
    if (value != -1) return globals.integer(value);
    u32 bits = static_cast<u32>(value); EclValueType type;
    if (stack.pop_value(bits, type) && type == EclValueType::Float) return number(float_bits(bits)).truncate_int();
    return integer_bits(bits);
}
// 0x44ff80. This variant rounds the integer-to-float stack conversion before returning.
Extended EclContext::resolve_float(u32 index, float value, EclGlobals& globals) {
    if (!instruction->is_reference(index)) return number(value);
    if (value >= 0) return number(stack.local<float>(Scalar::truncate(value)));
    if (value != -1.0f) return globals.floating(Scalar::truncate(value));
    u32 bits; std::memcpy(&bits, &value, 4); EclValueType type;
    if (stack.pop_value(bits, type) && type == EclValueType::Integer) return number(Extended::from_int(integer_bits(bits)).to_float());
    return number(float_bits(bits));
}
// 0x450030 / 0x450070. A non-reference has no writable destination.
i32* EclContext::integer_reference(u32 index, EclGlobals& globals) {
    if(!instruction->is_reference(index))return nullptr;
    const i32 value=integer_bits(instruction->argument(index));
    if(value<0)return globals.integer_reference(value);
    return reinterpret_cast<i32*>(stack.data+stack.frame_base+value);
}
float* EclContext::float_reference(u32 index, EclGlobals& globals) {
    if(!instruction->is_reference(index))return nullptr;
    const float value=float_bits(instruction->argument(index));
    const i32 offset=Scalar::truncate(value);
    if(value<0 || value!=value)return globals.float_reference(offset);
    return reinterpret_cast<float*>(stack.data+stack.frame_base+offset);
}
}
