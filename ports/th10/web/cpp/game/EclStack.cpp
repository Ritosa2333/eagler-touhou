#include "EclStack.hpp"
namespace th10 {
// 0x4505b0. The header is one byte in a four-byte slot: its padding is retained.
i32 EclStack::push(EclValueType type, const void* source, u32 length) noexcept {
    if (wrapping_add(top, static_cast<i32>(length)) >= 4096) return -1;
    if (type != EclValueType::Untyped) {
        data[top] = static_cast<u8>(type);
        top += 4;
    }
    std::memcpy(data + top, source, length);
    top += length;
    return 0;
}
// 0x450610. The caller supplies valid typed records; underflow leaves output untouched.
i32 EclStack::pop(EclValueType requested, void* destination, u32 length) noexcept {
    const i32 next_top = wrapping_add(top, -static_cast<i32>(length));
    if (next_top < 0) return -1;
    top = next_top;
    std::memcpy(destination, data + top, length);
    if (requested != EclValueType::Untyped) {
        top -= 4;
        const auto stored = static_cast<EclValueType>(data[top]);
        if (stored == EclValueType::Integer && requested == EclValueType::Float) {
            i32 value; std::memcpy(&value, destination, 4);
            const float converted = Extended::from_int(value).to_float();
            std::memcpy(destination, &converted, 4);
        } else if (stored == EclValueType::Float && requested == EclValueType::Integer) {
            float value; std::memcpy(&value, destination, 4);
            const i32 converted = Scalar::truncate(value);
            std::memcpy(destination, &converted, 4);
        }
    }
    return 0;
}
// Inlined by the original parameter readers. Keep its empty-stack convention.
bool EclStack::pop_value(u32& bits, EclValueType& type) noexcept {
    if (top - 4 < 0) return false;
    top -= 4;
    std::memcpy(&bits, data + top, 4);
    top -= 4;
    type = static_cast<EclValueType>(data[top]);
    return true;
}
// 0x450690. Preserve the original strict bound on the saved frame word.
i32 EclStack::enter_frame(i32 local_bytes) noexcept {
    const i32 previous_top = top;
    const i32 next_top = wrapping_add(top, local_bytes);
    if (next_top >= 4096) return -1;
    top = next_top;
    if (next_top + 4 < 4096) {
        std::memcpy(data + top, &frame_base, 4);
        top += 4;
    }
    frame_base = previous_top;
    return 0;
}
// 0x4506d0.
void EclStack::leave_frame() noexcept {
    const i32 previous_base = frame_base;
    if (top - 4 >= 0) {
        top -= 4;
        std::memcpy(&frame_base, data + top, 4);
    }
    top = previous_base;
}
}
