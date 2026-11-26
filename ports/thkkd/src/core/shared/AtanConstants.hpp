// 高精度级数生成的 atan 范围缩减常量；表项对应 atan(k/16)。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou::atan_data {
inline constexpr std::array<std::array<std::uint64_t, 2>, 17> table{{
    {{0x0000000000000000ull, 0x0000000000000000ull}},
    {{0x3faff55bb72cfdeaull, 0xbc3c934d86d23f1dull}},
    {{0x3fbfd5ba9aac2f6eull, 0xbc4cd37686760c17ull}},
    {{0x3fc7b97b4bce5b02ull, 0x3c5347b0b4f881caull}},
    {{0x3fcf5b75f92c80ddull, 0x3c68ab6e3cf7afbdull}},
    {{0x3fd362773707ebccull, 0xbc6963a544b672d8ull}},
    {{0x3fd6f61941e4def1ull, 0xbc7c63aae6f6e918ull}},
    {{0x3fda64eec3cc23fdull, 0xbc724dec1b50b7ffull}},
    {{0x3fddac670561bb4full, 0x3c7a2b7f222f65e2ull}},
    {{0x3fe0657e94db30d0ull, 0xbc7d5b495f6349e6ull}},
    {{0x3fe1e00babdefeb4ull, 0xbc5928df287a668full}},
    {{0x3fe345f01cce37bbull, 0x3c81021137c71102ull}},
    {{0x3fe4978fa3269ee1ull, 0x3c72419a87f2a458ull}},
    {{0x3fe5d58987169b18ull, 0x3c60028e4bc5e7caull}},
    {{0x3fe700a7c5784634ull, 0xbc78c34d25aadef6ull}},
    {{0x3fe819d0b7158a4dull, 0xbc7bf76229d3b917ull}},
    {{0x3fe921fb54442d18ull, 0x3c81a62633145c07ull}},
}};
inline constexpr std::array<std::uint64_t, 2> half_pi{{0x3ff921fb54442d18ull, 0x3c91a62633145c07ull}};
}
