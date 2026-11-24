#include "FontDevice.hpp"
#include "../game/MsgPlayer.hpp"

#include <algorithm>

namespace th13::sdl {

bool FontDevice::initialize() {
    if (ready_) return true;
    if (!build_cp932_font_mapping(mapping_) || mapping_.size() != 131072) {
        error = "TH13 dialogue cp932 mapping derivation failed (in-repo table)";
        return false;
    }
    if (!raster.initialize(mapping_.data(), mapping_.size())) {
        error = "TH13 dialogue font unavailable: original MS Gothic (/msgothic.ttc or "
                "/fonts/msgothic.ttc) is absent from the VFS; Japanese dialogue text "
                "rendering stays BLOCKED (no substitute font per asset red line; see "
                "artifacts/verification/r4/dialogue/HANDOFF.md)";
        return false;
    }
    ready_ = true;
    return true;
}

i32 FontDevice::extent(const std::string& cp932_text, i32 profile) {
    if (!ready_ && !initialize()) return -1;
    return raster.measure(cp932_text, u32(profile >= 0 ? profile : 0));
}

namespace {
void recolor_argb4444(TextureImage& target, u32 color) {
    if (target.format != touhou::graphics::PixelFormat::Argb4444) return;
    const u16 r4 = u16(((color & 255u) * 15u + 127u) / 255u);
    const u16 g4 = u16((((color >> 8) & 255u) * 15u + 127u) / 255u);
    const u16 b4 = u16((((color >> 16) & 255u) * 15u + 127u) / 255u);
    for (u32 row = 0; row < target.height; row++) {
        auto* pixel = target.pixels.data() + size_t(row) * target.pitch;
        for (u32 column = 0; column < target.width; column++, pixel += 2) {
            u16 value = 0;
            std::memcpy(&value, pixel, 2);
            const u16 coverage = std::max(u16((value >> 8) & 0xF), std::max(u16((value >> 4) & 0xF), u16(value & 0xF)));
            if (!coverage) continue;
            value = u16((coverage << 12) | (r4 << 8) | (g4 << 4) | b4);
            std::memcpy(pixel, &value, 2);
        }
    }
}
}

bool FontDevice::draw_text(TextureImage& target, i32 x, i32 y, i32 profile, u32 color, const std::string& cp932_text) {
    if (!ready_ && !initialize()) return false;
    if (profile < 0) {
        error = "TH13 dialogue font profile out of range";
        return false;
    }
    if (!raster.draw(target, x, y, u32(profile), 0xffffffu, cp932_text)) {
        error = "Failed to rasterize dialogue CP932 text";
        return false;
    }
    recolor_argb4444(target, color);
    return true;
}

bool FontDevice::draw_dialogue(TextureImage& target, i32 x, i32 y, i32 profile, u32 color,
                               const std::string& cp932_text) {
    if (!ready_ && !initialize()) return false;
    if (profile < 0) {
        error = "TH13 dialogue font profile out of range";
        return false;
    }
    static constexpr i32 kStrokeOffsets[4][2] = {{-2, -2}, {2, -2}, {-2, 2}, {2, 2}};
    for (const auto& offset : kStrokeOffsets) {
        if (!raster.draw(target, x + offset[0], y + offset[1], u32(profile), 0xffffffu, cp932_text)) {
            error = "Failed to rasterize dialogue CP932 outline";
            return false;
        }
    }
    if (!raster.draw(target, x, y, u32(profile), 0xffffffu, cp932_text)) {
        error = "Failed to rasterize dialogue CP932 text";
        return false;
    }
    recolor_argb4444(target, color);
    return true;
}

bool FontDevice::render_dialogue_line(TextureImage& target, i32 x, i32 y, const MsgLine& line, u32 color) {
    if (!line.text_cp932.empty() && !draw_text(target, x, y, 1, color, line.text_cp932)) return false;
    if (line.has_ruby && !line.ruby_cp932.empty()) {
        if (!draw_text(target, x + line.ruby_x, y, 4, color, line.ruby_cp932)) return false;
    }
    return true;
}

}
