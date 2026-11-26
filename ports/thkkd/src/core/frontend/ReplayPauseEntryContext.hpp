// 5E3630场景0分支及Replay EOF钩子的原字段；低字节写保留其余24位。
#pragma once
#include <cstdint>
namespace kaikeidou {
struct ReplayPauseEntryContext {
    std::uint32_t pause_binding=0; // A733F8。
    std::uint32_t scene=0;         // A78E2C，入口前置为0。
    std::uint32_t phase=0;         // A87F48。
    std::uint32_t selection=0;     // A87F50。
    std::uint32_t replay_cursor=0; // B43110。
    std::uint32_t replay_count=0;  // B43A08。
    std::uint32_t replay_mode=0;   // B4310C。
    std::uint32_t eof_flags=0;     // B43C00。
};
}
