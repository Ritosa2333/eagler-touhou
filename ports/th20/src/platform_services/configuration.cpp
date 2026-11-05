#include "configuration.hpp"
#include <algorithm>
#include <cstring>
#include <limits>

namespace th20::source::platform {
void initialize_bindings(KeyBindings& value) noexcept {
    constexpr KeyBindings defaults{{0,1,2,3,0xffff,0xffff,0xffff,0xffff},
        {0,1,5,10,0xffff,0xffff,0xffff,0xffff},
        {0x5a,0x58,0x10,0x1b,0x26,0x28,0x25,0x27}};
    value = defaults;
}
void initialize_configuration_flags(std::uint32_t& value) noexcept {
    value = (value & ~0x1ffu) | 0x80u;
}
void initialize_configuration(Configuration& c) noexcept {
    c.version = 0x200002; c.size = sizeof(c);
    for (auto& keys : c.bindings) initialize_bindings(keys);
    c.value_68 = c.value_6c = 0;
    c.value_70 = c.value_72 = 600;
    c.alternate_pixel_format = 0; c.value_75 = c.value_76 = 1;
    c.saved_display_mode = 8; c.frame_skip = 0;
    c.value_7d = 2; c.value_7e = 100; c.value_7f = 0x50;
    c.value_80 = 0; c.presentation_mode = 2; c.scale_choice = c.value_83 = 0;
    initialize_configuration_flags(c.flags);
    c.saved_window_x = c.saved_window_y = std::numeric_limits<std::int32_t>::min();
    c.value_90 = 1;
    std::fill(std::begin(c.values_94_a4), std::end(c.values_94_a4), std::uint8_t{0});
    c.values_94_a4[6] = 1;
    c.value_a8 = c.value_ac = 0;
}
Configuration default_configuration() noexcept {
    Configuration c{}; initialize_configuration(c); return c;
}
bool valid_configuration(const Configuration& c, std::size_t file_size) noexcept {
    // 0x4dc317 CMP dword / JGE is a signed comparison, with no lower bound.
    return c.size == 0xb0 && c.alternate_pixel_format < 2 && c.value_75 < 3 &&
        c.value_76 < 2 && c.saved_display_mode < 10 &&
        c.frame_skip < 3 && c.value_7d < 3 && c.version == 0x200002 && file_size == 0xb0;
}
bool decode_configuration(Configuration& output, const void* bytes, std::size_t size) noexcept {
    if (size == sizeof(Configuration) && bytes) {
        Configuration candidate; std::memcpy(&candidate, bytes, sizeof(candidate));
        if (valid_configuration(candidate, size)) { output = candidate; return true; }
    }
    // Original invalid/truncated files may overread their allocation. This
    // newly defined rejection path preserves defaults, without that overread.
    output = default_configuration(); return false;
}
}
