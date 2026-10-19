#pragma once
#include "Arithmetic.hpp"
namespace th10 {
enum class EclValueType : u8 { Untyped = 0, Integer = 'i', Float = 'f' };
struct EclStack {
    u8 data[4096];
    i32 top;
    i32 frame_base;
    i32 push(EclValueType type, const void* source, u32 length) noexcept;
    i32 pop(EclValueType type, void* destination, u32 length) noexcept;
    i32 enter_frame(i32 local_bytes) noexcept;
    void leave_frame() noexcept;
    bool pop_value(u32& bits, EclValueType& type) noexcept;
    template<class T> T local(i32 offset) const noexcept {
        T result; std::memcpy(&result, data + frame_base + offset, sizeof(result)); return result;
    }
};
static_assert(sizeof(EclStack) == 0x1008);
}
