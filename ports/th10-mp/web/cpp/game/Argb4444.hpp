#pragma once
#include "Arithmetic.hpp"
namespace th10 {
// Pixel conversion used by the original font atlas. These operations accept
// ordinary image rows and have no dependency on a CPU or DLL object layout.
struct Argb4444 {
    static void unpack(const u8* source,float* rgba,u32 width) noexcept;
    static bool pack(const float* rgba,u8* destination,u32 width) noexcept;
};
}
