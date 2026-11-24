#pragma once
#include "../game/AnmResource.hpp"
#include <string>
#include <string_view>
#include <vector>
namespace th13::sdl {
// Pack files may replace named textures and animation banks. Dialogue stays
// on the CP932 original until a Unicode font path exists. ECL, STD and shot
// binaries stay on the retail path.
bool language_asset_allowed(std::string_view base);
bool language_texture_allowed(std::string_view name);
std::string language_asset_path(std::string_view base);
std::string language_texture_path(std::string_view name);
// Replaces embedded pixels when the PNG has the same dimensions. A size
// mismatch leaves the retail texture unchanged.
bool apply_language_png(th13::AnmTexture& texture, const u8* bytes, u32 size);
bool language_pack_active();
bool read_language_file(const std::string& path, std::vector<u8>& bytes);
}  // namespace th13::sdl
