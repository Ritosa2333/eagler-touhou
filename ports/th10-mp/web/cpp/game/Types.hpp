#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace th10 {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
struct Vec2 { float x, y; };
struct Vec3 { float x, y, z; };
struct Matrix4 {
    float elements[4][4];
    void identity() noexcept {
        std::memset(elements, 0, sizeof(elements));
        for (unsigned axis = 0; axis < 4; ++axis) elements[axis][axis] = 1.0f;
    }
};
template<class T> struct ListNode {
    T* value;
    ListNode* next;
    ListNode* previous;
    void initialize(T* owner) noexcept { value = owner; next = previous = nullptr; }
    void insert_after(ListNode& head) noexcept {
        // The original insertion preserves an existing tail when head is empty.
        if(head.next){next=head.next;head.next->previous=this;}
        head.next=this;previous=&head;
    }
    void unlink() noexcept {
        if (previous) previous->next = next;
        if (next) next->previous = previous;
        next = previous = nullptr;
    }
};
inline i32 wrapping_add(i32 value, i32 delta) noexcept {
    const u32 bits = static_cast<u32>(value) + static_cast<u32>(delta);
    i32 result;
    std::memcpy(&result, &bits, sizeof(result));
    return result;
}
}
