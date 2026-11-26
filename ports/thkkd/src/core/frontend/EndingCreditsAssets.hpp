// 结局图片表及字体/片尾底图由冷启动资源初始化，运行期按原地址共享。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou {
struct EndingCreditsAssets {
    std::array<std::uint32_t,15> images{}; // A7E61C..A7E657，三角色每组五项。
    std::uint32_t font=0; // A74BD8。
    std::uint32_t credits_background=0; // A75874。
};
template<class Memory>void bind_ending_credits_assets(Memory& memory,EndingCreditsAssets& assets){
    memory.bind_bytes(0xa7e61c,assets.images.data(),sizeof(assets.images));
    memory.bind(0xa74bd8,assets.font);memory.bind(0xa75874,assets.credits_background);
}
}
