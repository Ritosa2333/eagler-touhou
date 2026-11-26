// 61D690直接读取的场景、着色、低速图像和玩家位置；位置保存binary64位型。
#pragma once
#include <cstdint>
namespace kaikeidou {
struct GameplayPresentationDispatchContext {
    std::uint8_t tint=255;             // A7E5EC，原MOVZX读取单字节。
    std::uint32_t scene=0;             // A78E2C。
    std::uint32_t practice=0;          // A7E618。
    std::uint32_t slow_binding=0;      // A733F4。
    std::uint32_t slow_graph=0;        // A74F48。
    std::uint64_t player_x_bits=0;     // A8CDF8。
    std::uint64_t player_y_bits=0;     // A8CE00。
};
}
