// 音乐室独占字段；外层接入按注释地址与共享状态同步。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou {
struct MenuMusicContext {
    std::array<std::uint64_t,17> entry_x_bits{}; // A87F60..A87FE0，binary64存储位型。
    std::uint32_t selected=0,scroll=0;          // A87F34、A87F38。
    std::uint32_t phase=0,timer=0;             // A87F48、A87F4C。
    std::uint32_t outer_scene=0;               // A8CE58。
    std::array<std::uint32_t,4> bindings{};     // A733E4/E8/EC/F0，上/下/确认/取消。
    std::uint32_t cancel_sound=0,nav_sound=0;   // A73234、A73238。
};
}
