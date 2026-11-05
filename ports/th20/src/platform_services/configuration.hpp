#pragma once
#include <cstddef>
#include <cstdint>

namespace th20::source::platform {
#pragma pack(push, 1)
struct KeyBindings { std::uint16_t pad[8], alternate_pad[8], keyboard[8]; };
struct Configuration {                         // 0x005c4f08, 176 bytes
    std::uint32_t version;                       // +00
    std::uint32_t size;                          // +04
    KeyBindings bindings[2];                    // +08
    std::uint32_t value_68, value_6c;
    std::uint16_t value_70, value_72;
    std::uint8_t alternate_pixel_format;         // +74
    std::uint8_t value_75, value_76, padding_77;
    std::int32_t saved_display_mode;             // +78
    std::uint8_t frame_skip, value_7d, value_7e, value_7f;
    std::uint8_t value_80, presentation_mode, scale_choice, value_83;
    std::uint32_t flags;                         // +84
    std::int32_t saved_window_x, saved_window_y;  // +88,+8c
    std::uint32_t value_90;
    std::uint8_t values_94_a4[17];
    std::uint8_t padding_a5_a7[3];
    std::uint32_t value_a8, value_ac;
};
#pragma pack(pop)
static_assert(sizeof(KeyBindings) == 0x30);
static_assert(sizeof(Configuration) == 0xb0);
static_assert(offsetof(Configuration, alternate_pixel_format) == 0x74);
static_assert(offsetof(Configuration, frame_skip) == 0x7c);
static_assert(offsetof(Configuration, presentation_mode) == 0x81);
static_assert(offsetof(Configuration, flags) == 0x84);
static_assert(offsetof(Configuration, value_ac) == 0xac);
void initialize_bindings(KeyBindings&) noexcept;               // 0x41fb10
void initialize_configuration_flags(std::uint32_t&) noexcept;  // 0x4b9b80
// The caller supplies initialized storage. The original constructor preserves
// padding bytes and flag bits 9..31; those bytes are deliberately not cleared.
void initialize_configuration(Configuration&) noexcept;        // 0x4b9c10
bool valid_configuration(const Configuration&, std::size_t file_size) noexcept; // 0x4dc1c0 predicate
// The default factory defines formerly indeterminate stack padding as zero.
// This does not claim byte-identical serialization of uninitialized padding.
Configuration default_configuration() noexcept;
bool decode_configuration(Configuration&, const void*, std::size_t) noexcept;
}
