#include "AnmText.hpp"

#include <algorithm>
#include <cmath>

namespace th13 {

bool rasterize_anm_text(AnmVm& target, DialogueTextureSink& sink,
                        const AnmTextRasterizer& rasterize, u32 color,
                        const std::string& cp932, AnmTextAlignment alignment,
                        i32 nominal_height, std::string* error) {
    const auto fail = [&](const std::string& message) {
        if (error && error->empty()) *error = message;
        return false;
    };
    auto* vm = &target;
    if (!vm->resource || !vm->sprite) return fail("ANM text sprite unavailable");
    if (!rasterize) return fail("ANM text font rasterizer unavailable");
    if (nominal_height != 17) return fail("ANM text font height is not implemented");
    const auto& sprite = *vm->sprite;
    const auto integer = [](float value) { return std::isfinite(value) &&
                                                  value == std::floor(value); };
    if (!integer(sprite.x) || !integer(sprite.y) || !integer(sprite.width) ||
        !integer(sprite.height) || sprite.x < 0 || sprite.y < 0 ||
        sprite.width <= 0 || sprite.width > 512 || sprite.height <= 0 || sprite.height > 512)
        return fail("ANM text sprite rectangle unsupported");
    const u32 x = u32(sprite.x), y = u32(sprite.y), width = u32(sprite.width),
              height = u32(sprite.height);
    const u32 handle = sink.texture(*vm->resource, sprite.texture);
    TextureImage* destination = handle ? sink.pixels(handle) : nullptr;
    if (!destination || destination->format != touhou::graphics::PixelFormat::Argb4444 ||
        destination->pitch < destination->width * 2 ||
        destination->pixels.size() < size_t(destination->pitch) * destination->height ||
        x > destination->width || width > destination->width - x ||
        y > destination->height || height > destination->height - y)
        return fail("ANM text ARGB4444 texture unavailable");

    // 0046e970 / 0045aef0: style0 font32, nominal line height17, main
    // TextOut at (0,2), with diagonal black outline at (+/-2,0/4).
    TextureImage glyph;
    glyph.width = 1024;
    glyph.height = 40; // nominal17*2+6, the original scratch clearing height
    glyph.pitch = glyph.width * 2;
    glyph.packed_format = 5;
    glyph.format = touhou::graphics::PixelFormat::Argb4444;
    glyph.pixels.assign(size_t(glyph.pitch) * glyph.height, 0);
    const size_t nul = cp932.find(char(0));
    const i32 length = i32(nul == std::string::npos ? cp932.size() : nul);
    if (length > 127) return fail("ANM text exceeds original text buffer");
    i32 offset = 0;
    if (alignment == AnmTextAlignment::Right)
        offset = i32(width) - ((nominal_height - 1) * length / 2);
    else if (alignment == AnmTextAlignment::Center)
        offset = i32(width) / 2 - ((nominal_height - 1) * length / 4);
    if (!rasterize(glyph, offset * 2, 2, 0xffffffffu, cp932))
        return fail("ANM text CP932 font rasterization failed");
    if (glyph.width != 1024 || glyph.height != 40 || glyph.pitch != 2048 ||
        glyph.format != touhou::graphics::PixelFormat::Argb4444 ||
        glyph.pixels.size() != size_t(glyph.pitch) * glyph.height)
        return fail("ANM text rasterizer changed target layout");

    const u32 source_width = std::min(width * 2 + 22, 1024u);
    const u32 source_height = 36; // nominal17*2+2, original D3DX source RECT
    std::vector<std::array<float, 4>> pixels(size_t(source_width) * source_height);
    const auto coverage = [&](i32 px, i32 py) -> float {
        if (px < 0 || py < 0 || px >= i32(glyph.width) || py >= i32(glyph.height)) return 0;
        u16 value = 0;
        std::memcpy(&value, glyph.pixels.data() + size_t(py) * glyph.pitch + size_t(px) * 2, 2);
        return float(value >> 12) / 15.0f;
    };
    for (u32 py = 0; py < source_height; ++py) {
        for (u32 px = 0; px < source_width; ++px) {
            const float foreground = coverage(i32(px), i32(py));
            float border = 0;
            for (i32 dy : {-2, 2}) for (i32 dx : {-2, 2})
                border = std::max(border, coverage(i32(px) - dx, i32(py) - dy));
            const float alpha = foreground + border * (1.0f - foreground);
            const float amount = alpha > 0 ? foreground / alpha : 0;
            pixels[size_t(py) * source_width + px] = {
                alpha, amount * float(color & 255u) / 255.0f,
                amount * float((color >> 8) & 255u) / 255.0f,
                amount * float((color >> 16) & 255u) / 255.0f};
        }
    }
    // D3DXLoadSurfaceFromMemory filter4 = TRIANGLE. Apply the corresponding
    // separable tent support in source pixel coordinates. Font rasterization,
    // GDI edge treatment and D3DX rounding still require original pixel truth.
    const float scale_x = float(source_width) / float(width);
    const float scale_y = float(source_height) / float(height);
    const float radius_x = std::max(scale_x, 1.0f), radius_y = std::max(scale_y, 1.0f);
    std::vector<u16> result(size_t(width) * height);
    for (u32 py = 0; py < height; ++py) {
        const float center_y = (float(py) + 0.5f) * scale_y - 0.5f;
        for (u32 px = 0; px < width; ++px) {
            const float center_x = (float(px) + 0.5f) * scale_x - 0.5f;
            std::array<float, 4> sum{};
            float total = 0;
            for (i32 sy = i32(std::ceil(center_y - radius_y));
                 sy <= i32(std::floor(center_y + radius_y)); ++sy) {
                const float wy = std::max(0.0f, 1.0f - std::fabs(float(sy) - center_y) / radius_y);
                for (i32 sx = i32(std::ceil(center_x - radius_x));
                     sx <= i32(std::floor(center_x + radius_x)); ++sx) {
                    const float weight = wy * std::max(0.0f, 1.0f -
                                                    std::fabs(float(sx) - center_x) / radius_x);
                    total += weight;
                    if (sx < 0 || sy < 0 || sx >= i32(source_width) || sy >= i32(source_height)) continue;
                    const auto& sample = pixels[size_t(sy) * source_width + size_t(sx)];
                    for (size_t c = 0; c < 4; ++c) sum[c] += sample[c] * weight;
                }
            }
            u16 packed = 0;
            for (size_t c = 0; c < 4; ++c) {
                const u16 value = u16(std::clamp(i32(std::floor(sum[c] / total * 15.0f + 0.5f)), 0, 15));
                packed = u16((packed << 4) | value);
            }
            result[size_t(py) * width + px] = packed;
        }
    }
    // Commit only after the complete glyph/outline/scaling pipeline succeeds.
    // This region is what the same ANM VM already samples in the real renderer.
    for (u32 py = 0; py < height; ++py)
        std::memcpy(destination->pixels.data() + size_t(y + py) * destination->pitch + size_t(x) * 2,
                    result.data() + size_t(py) * width, size_t(width) * 2);
    sink.changed(handle, {x, y, x + width, y + height});
    return true;
}

} // namespace th13

