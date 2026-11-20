#include "LanguagePack.hpp"
#include "../game/AnmTextureDecode.hpp"
#include <SDL3/SDL.h>
namespace th13::sdl {
namespace {
bool suffix(std::string_view name, std::string_view ext) {
    return name.size() >= ext.size() && name.substr(name.size() - ext.size()) == ext;
}
bool safe_relative(std::string_view name) {
    if (name.empty() || name.size() > 180 || name.front() == '/' || name.front() == '\\') return false;
    std::string_view rest = name;
    while (!rest.empty()) {
        const auto slash = rest.find_first_of("/\\");
        const auto part = rest.substr(0, slash);
        if (part.empty() || part == "." || part == "..") return false;
        if (slash == std::string_view::npos) break;
        rest.remove_prefix(slash + 1);
    }
    return true;
}
}
bool language_asset_allowed(std::string_view base) {
    // Dialogue and comments are CP932. thcrap Simplified Chinese msg text is
    // UTF-8, so applying those files would render as mojibake. Bitmap and
    // animation replacements are the verified surface.
    return safe_relative(base) && (suffix(base, ".anm") || suffix(base, ".png"));
}
bool language_texture_allowed(std::string_view name) {
    return language_asset_allowed(name) && suffix(name, ".png");
}
std::string language_asset_path(std::string_view base) {
    return std::string("/thcrap/th13/") + std::string(base);
}
std::string language_texture_path(std::string_view name) {
    return language_asset_path(name);
}
bool apply_language_png(th13::AnmTexture& texture, const u8* bytes, u32 size) {
    if (texture.kind != th13::AnmTexture::Kind::Embedded || !texture.width || !texture.height) return false;
    u32 width = 0, height = 0;
    std::vector<u8> rgba;
    if (!th13::anm_decode_png(bytes, size, width, height, rgba)) return false;
    if (width != texture.width || height != texture.height || rgba.size() != size_t(width) * height * 4) return false;
    texture.pixels.resize(rgba.size());
    for (u32 i = 0; i < width * height; ++i) {
        texture.pixels[i * 4] = rgba[i * 4 + 2];
        texture.pixels[i * 4 + 1] = rgba[i * 4 + 1];
        texture.pixels[i * 4 + 2] = rgba[i * 4];
        texture.pixels[i * 4 + 3] = rgba[i * 4 + 3];
    }
    texture.pixel_format = 1;
    texture.pixel_width = th13::u16(width);
    texture.pixel_height = th13::u16(height);
    return true;
}
bool language_pack_active() {
    size_t size = 0;
    auto* data = static_cast<char*>(SDL_LoadFile("/thcrap/th13/runtime-language.txt", &size));
    if (!data) return false;
    const bool active = std::string(data, size).find("lang_zh-hans") != std::string::npos;
    SDL_free(data);
    return active;
}
bool read_language_file(const std::string& path, std::vector<u8>& bytes) {
    size_t size = 0;
    auto* data = static_cast<u8*>(SDL_LoadFile(path.c_str(), &size));
    if (!data) return false;
    bytes.assign(data, data + size);
    SDL_free(data);
    return true;
}
}
