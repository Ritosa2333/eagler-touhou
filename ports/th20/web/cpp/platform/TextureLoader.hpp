// Texture image loading for the TH_SDL3 build, replacing
// D3DXCreateTexture/D3DXLoadSurfaceFromFileInMemory. Decoding goes through
// stb_image; the triangle-filtered, mirror-wrapped resample replaces the D3DX
// filter mode 0x30004 (D3DX_FILTER_TRIANGLE|MIRROR_U|MIRROR_V) used by the
// game's hi-res downsample path. Kernel weights follow the tent definition;
// exact D3DX coefficient rounding may differ by an LSB (documented deviation).
#pragma once

#include "Graphics.hpp"
#include <cstdint>
#include <vector>

namespace th20::web {

struct DecodedImage {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> pixels; // BGRA, tightly packed
};

// Decodes a PNG/TGA/BMP/JPEG memory image into BGRA (the A8R8G8B8 byte
// order the game textures use). Returns false for undecodable input.
bool decode_image(const std::uint8_t* bytes, std::uint32_t size, DecodedImage& out);

// D3DXLoadSurfaceFromFileInMemory shape: decode the memory image, take
// source_rect within it, and write it into the texture at destination_rect.
// filter 1 (D3DX_FILTER_NONE): point copy (source and destination sizes must
// match). filter 0x30004: separable triangle resample with mirror wrap at the
// source rectangle boundary. The texture is locked/unlocked internally.
bool load_surface_from_memory(GraphicsDevice& device, Texture* texture,
    const std::int32_t* destination_rect, const std::uint8_t* bytes, std::uint32_t size,
    const std::int32_t* source_rect, std::uint32_t filter);
}
