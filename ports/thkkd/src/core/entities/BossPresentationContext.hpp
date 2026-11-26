// Boss阶段配置、入场及退场演出访问的独立全局字段。
#pragma once
#include <array>
#include <cstdint>

namespace kaikeidou {
struct BossPresentationContext {
    std::uint32_t field_a7eaa8 = 0, field_a7eab4 = 0;
    std::array<std::uint8_t, 32> boss_name{}; // A7EAF8。
    std::array<std::uint64_t, 4> spell_panel_reals{}; // A7EB18..A7EB30。
    std::uint32_t entry_state = 0, entry_timer = 0; // A7EB38、A7EB40。
    std::array<std::uint64_t, 5> entry_reals{}; // A7EB48..A7EB68。
    std::array<std::uint64_t, 4> spell_ring_reals{}; // A7EB70..A7EB88。
    std::uint32_t bonus_decrement = 0; // A7EB9C。
};
struct BossPresentationServices {
    virtual ~BossPresentationServices() = default;
    virtual void prepare_spell_music() = 0;
    virtual std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) = 0;
};
}
