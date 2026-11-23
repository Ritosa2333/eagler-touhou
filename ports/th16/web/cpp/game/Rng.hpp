// th16 随机数发生器——原版 th16.exe（v1.00a）0x402b70 / 0x402be0 的行为镜像。
//
// 反汇编依据（artifacts/th16/decomp/jp/functions/00402be0.asm）：
//   A    = u16((seed ^ 0x9630) - 0x6553)
//   mid  = u16((A >> 14) + A * 4)            ; 左旋 2 位
//   B    = u16((mid ^ 0x9630) - 0x6553)
//   seed = u16((B >> 14) + B * 4)
//   return (A << 16) | B ; 调用计数 [this+4] += 2
// 0x402b70 为单步变体：计数 +1，返回旋转后的新种子。
// 原版在 DAT_004c10b6 非零时用临界区包裹（装载线程并发保护），不影响取值序列。
//
// 浮点化：th16 以 SSE2 标量单精度编译（反汇编为 CVTDQ2PD/ADDSD/CVTPD2PS/MULSS/SUBSS/DIVSS，
// 不是 x87），每一步都是 IEEE float 单舍入，C++ float 运算在 -ffp-contract=off 下逐位一致：
//   r = next32()；d = (double)(i32)r + {0.0, 4294967296.0}[r>>31]（即无符号解释）；f = (float)d
//   0x402c70  unit()        : f * 2^-32f               （常量 0x4943d0 = 0x2f800000）→ [0,1)
//   0x402cb0  signed_unit() : f * 2^-31f - 1.0f        （0x4943d4 = 0x30000000, 0x4944d8 = 1.0f）→ [-1,1)
//   0x402cf0  angle()       : f / 683565248.0f - π_f   （0x494700 = 0x4e22f983, 0x494588 = 0x40490fdb）→ [-π,π)
// 以上三者已由 Unicorn 直接执行原版函数得到的 golden（tests/golden/rng.json）逐位验证。
#pragma once
#include "Types.hpp"

namespace th16 {

struct Rng;
// 调试钩子（对拍 game-oracle.py --trace-rng 用）：每次单步 / 双步前调用；正常运行为空
inline void (*g_rng_trace)(const Rng& rng, int step) = nullptr;

struct Rng {
    u16 seed = 0;
    u16 reserved = 0;
    u32 calls = 0;

    // 0x402b70
    u16 next16() noexcept {
        if (g_rng_trace != nullptr) g_rng_trace(*this, 1);
        const u16 mixed = u16((seed ^ 0x9630u) - 0x6553u);
        seed = u16((mixed >> 14) + mixed * 4);
        ++calls;
        return seed;
    }

    // 0x402be0
    u32 next32() noexcept {
        if (g_rng_trace != nullptr) g_rng_trace(*this, 2);
        const u16 high = u16((seed ^ 0x9630u) - 0x6553u);
        const u16 middle = u16((high >> 14) + high * 4);
        const u16 low = u16((middle ^ 0x9630u) - 0x6553u);
        seed = u16((low >> 14) + low * 4);
        calls += 2;
        return (u32(high) << 16) | low;
    }

    // CVTDQ2PD + ADDSD 表 + CVTPD2PS：32 位无符号值经 double 再单舍入到 float
    float next_unsigned_float() noexcept { return float(double(next32())); }

    // 0x402c70：×2^-32 → [0,1)
    float unit() noexcept { return next_unsigned_float() * 2.3283064365386963e-10f; }

    // 0x402cb0：×2^-31 − 1 → [-1,1)
    float signed_unit() noexcept { return next_unsigned_float() * 4.656612873077393e-10f - 1.0f; }

    // 0x402cf0：/ (2^32/2π) − π → [-π,π)
    float angle() noexcept { return next_unsigned_float() / 683565248.0f - 3.1415927410125732f; }
};
static_assert(sizeof(Rng) == 8, "原版 RNG 对象为 8 字节：u16 种子 + u16 填充 + u32 计数");

}  // namespace th16
