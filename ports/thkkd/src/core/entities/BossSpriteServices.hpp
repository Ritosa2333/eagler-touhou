// Boss 绘制的图形调用边界，参数中的实数保留原 binary64 位型。
#pragma once
#include <cstdint>

namespace kaikeidou {
struct BossSpriteServices {
    virtual ~BossSpriteServices() = default;
    virtual void blend(std::uint32_t mode, std::uint32_t opacity) = 0;
    virtual void sprite(std::uint64_t x, std::uint64_t y, std::uint64_t scale,
                        std::uint64_t angle, std::uint32_t handle,
                        std::uint32_t transparent, std::uint32_t flip) = 0;
    virtual std::uint32_t sprite_handle(std::uint32_t original_address) const = 0;
};
}
