#pragma once
#include "../program_entry/program_entry.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
namespace th20::source::platform {
using Bytes = std::vector<std::uint8_t>;
std::optional<Bytes> read_loose_file(const char*); // 0x410aa0, loose-only branch
int write_loose_file(const char*, const void*, std::uint32_t); // 0x410e70: -1 open, -2 short write, 0 success
int load_configuration(program_entry::GraphicsStatePrefix&, const program_entry::WindowStatePrefix&,
                       runtime::Log&, const char* filename); // 0x4dc1c0
void save_configuration(const Configuration&, const program_entry::WindowStatePrefix&); // 0x41aa70
// Extracted path portion of 0x41b4f0, also usable for isolated directory tests.
void initialize_directories(program_entry::WindowStatePrefix&, runtime::Log&,
                            const wchar_t* appdata, const wchar_t* module_filename);
int initialize_platform(program_entry::WindowStatePrefix&, runtime::Log&); // 0x41b4f0, real OS side effects
double read_clock(program_entry::WindowStatePrefix&); // 0x41cb10
// Direct-touch port extension with no original counterpart. The browser adapter
// marks a run once a non-zero unlimited drag movement has been consumed by
// gameplay, and clears the mark when a new run starts; the recovered Result and
// Replay writers read it to force a 100% processing-drop rate (see the
// eagler-touhou touch protocol). A native build never sets it, so its computed
// rates and the CPU comparisons are unchanged.
bool unlimited_touch_used() noexcept;
void set_unlimited_touch_used(bool) noexcept;
}
