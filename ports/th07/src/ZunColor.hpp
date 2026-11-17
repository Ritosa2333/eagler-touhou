#pragma once

#include "inttypes.hpp"
#include <algorithm>

union ZunColor {
    static u8 Multiply(u8 src, u8 factor)
    {
        u32 tmp = (u32)src * factor >> 7;
        if (tmp >= 256)
        {
            tmp = 255;
        }
        return tmp;
    }

    static ZunColor Lerp(ZunColor a, ZunColor b, f32 t)
    {
        ZunColor res;
        res.bytes.r = (u8)std::clamp(a.bytes.r + (b.bytes.r - a.bytes.r) * t, 0.f, 255.f);
        res.bytes.g = (u8)std::clamp(a.bytes.g + (b.bytes.g - a.bytes.g) * t, 0.f, 255.f);
        res.bytes.b = (u8)std::clamp(a.bytes.b + (b.bytes.b - a.bytes.b) * t, 0.f, 255.f);
        res.bytes.a = (u8)std::clamp(a.bytes.a + (b.bytes.a - a.bytes.a) * t, 0.f, 255.f);
        return res;
    }

    u32 color;
    struct ColorBytes
    {
        u8 b;
        u8 g;
        u8 r;
        u8 a;
    } bytes;
    u8 raw[4];
};
