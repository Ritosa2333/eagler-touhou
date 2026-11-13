#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace th13 { class AnmVm; }
namespace th13::multiplayer {
#if defined(__wasm__)
#define TH13_DIGEST_INLINE [[gnu::always_inline]]
#else
#define TH13_DIGEST_INLINE
#endif
// FNV-1a 64（与 tests/multiplayer/native/state_digest.hpp 的 DigestSink 同
// 族：域分隔、顺序敏感；此处为生产侧 hash——回滚一致性与锁步确认帧共用。
// 具名作用域（非匿名）：WorldState.hpp 的成员声明前向引用此类。
class DigestSink {
public:
    void vm(const AnmVm& value);
    void tag(const char* text) {
        mix(0x9e3779b97f4a7c15ull ^ static_cast<uint64_t>(std::strlen(text)));
        for (const char* p = text; *p; ++p) mix(static_cast<uint8_t>(*p));
        mix(0);
    }
    TH13_DIGEST_INLINE void u8(uint8_t value) { mix(value); }
    TH13_DIGEST_INLINE void u32(uint32_t value) {
        // Four zero bytes multiply by P^4 modulo 2^64. Keep the exact
        // serialized bytes while avoiding four dependent multiplies.
        if (value == 0) { zero_bytes_ += 4; return; }
        mix(value & 0xffu); mix((value >> 8) & 0xffu);
        mix((value >> 16) & 0xffu); mix((value >> 24) & 0xffu);
    }
    TH13_DIGEST_INLINE void i32(int32_t value) { u32(static_cast<uint32_t>(value)); }
    TH13_DIGEST_INLINE void u64(uint64_t value) {
        u32(static_cast<uint32_t>(value));
        u32(static_cast<uint32_t>(value >> 32));
    }
    TH13_DIGEST_INLINE void f32(float value) {
        static_assert(sizeof(float) == 4, "digest requires float32");
        uint32_t bits = 0;
        std::memcpy(&bits, &value, sizeof bits);
        u32(bits);
    }
    TH13_DIGEST_INLINE void boolean(bool value) { u8(value ? 1 : 0); }
    // Append known serialized zero bytes without visiting each scalar.
    void zero_bytes(uint64_t count) { zero_bytes_ += count; }
    // Apply an exactly precomputed fixed byte stream after pending zero bytes.
    void fixed_stream(uint64_t factor, const std::array<uint64_t, 256>& adjustment) {
        state_ = with_zeros(state_, zero_bytes_);
        zero_bytes_ = 0;
        state_ = state_ * factor + adjustment[uint8_t(state_)];
    }
    uint64_t value() const { return with_zeros(state_, zero_bytes_); }
private:
    static uint64_t with_zeros(uint64_t state, uint64_t count) {
        static const auto powers = [] {
            std::array<uint64_t, 1025> result{};
            result[0] = 1;
            for (size_t i = 1; i < result.size(); ++i)
                result[i] = result[i - 1] * 0x100000001b3ull;
            return result;
        }();
        while (count > 1024) { state *= powers[1024]; count -= 1024; }
        return state * powers[count];
    }
    TH13_DIGEST_INLINE void mix(uint8_t byte) {
        if (!byte) { ++zero_bytes_; return; }
        if (zero_bytes_) { state_ = with_zeros(state_, zero_bytes_); zero_bytes_ = 0; }
        state_ ^= byte; state_ *= 0x100000001b3ull;
    }
    uint64_t zero_bytes_ = 0;
    uint64_t state_ = 0xcbf29ce484222325ull;
};

#undef TH13_DIGEST_INLINE
}
