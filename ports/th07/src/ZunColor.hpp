#pragma once

#include "inttypes.hpp"

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
        res.bytes.r = (u8)(a.bytes.r + (b.bytes.r - a.bytes.r) * t);
        res.bytes.g = (u8)(a.bytes.g + (b.bytes.g - a.bytes.g) * t);
        res.bytes.b = (u8)(a.bytes.b + (b.bytes.b - a.bytes.b) * t);
        res.bytes.a = (u8)(a.bytes.a + (b.bytes.a - a.bytes.a) * t);
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
