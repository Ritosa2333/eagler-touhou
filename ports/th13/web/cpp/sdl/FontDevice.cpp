#include "FontDevice.hpp"
#include "../game/MsgPlayer.hpp"  // build_cp932_font_mapping（仓内 cp932 表）

#include <algorithm>

namespace th13::sdl {

// 原版对话日文栅格走 text.anm 动态纹理（0x4282b0 建 scripts0/1/2/3 正文/ruby
// VM；0x46e8f0/0x45aef0 逐行写入 texture0）——本设备是该链路的字形栅格源，
// 结构与 th15 FontDevice 同族（eagler::text::RuntimeFontRaster + SDL3_ttf）。
//
// cp932 映射：th15 经 /fonts/cp932.bin 启动资产；th13 在仓内由
// MsgPlayer 的 cp932 表推导（同 131072 字节 u16le 语义），不再要求 VFS 文件。
//
// 字体资产边界：RuntimeFontRaster 依次尝试 /msgothic.ttc 与 /fonts/msgothic.ttc。
// 启动资产在 web/sdl-runtime/assets/msgothic.ttc 就位后，由 th13.html 写入 /msgothic.ttc。
// 真实字体缺失时显式失败（ready_ = false，BLOCKED），开发测试可在 ignored 路径载入。
bool FontDevice::initialize() {
    if (ready_) return true;
    if (!build_cp932_font_mapping(mapping_) || mapping_.size() != 131072) {
        error = "TH13 dialogue cp932 mapping derivation failed (in-repo table)";
        return false;
    }
    if (!raster.initialize(mapping_.data(), mapping_.size())) {
        error =
            "TH13 dialogue font unavailable: original MS Gothic (/msgothic.ttc or "
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
// 0x45aef0 语义：TextOut 写入的是 GDI 覆盖率混合后的文字颜色；0045a3c0 的
// alpha 反转进出让最终 4444 面得到 A=字形覆盖率、RGB=原版 SetTextColor 颜色。
// 覆盖率只能从白色栅格提取（黑字时 max(RGB) 恒 0），所以先按白画再重着色。
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
            const u16 coverage = std::max(u16((value >> 8) & 0xF),
                                          std::max(u16((value >> 4) & 0xF), u16(value & 0xF)));
            if (!coverage) continue;
            value = u16((coverage << 12) | (r4 << 8) | (g4 << 4) | b4);
            std::memcpy(pixel, &value, 2);
        }
    }
}
}  // namespace

bool FontDevice::draw_text(TextureImage& target, i32 x, i32 y, i32 profile, u32 color,
                           const std::string& cp932_text) {
    if (!ready_ && !initialize()) return false;
    if (profile < 0) {
        error = "TH13 dialogue font profile out of range";
        return false;
    }
    // 白色栅格出覆盖率（0x45aef0 的 SetTextColor 在 GDI 混合中只决定 RGB，
    // A 由 0045a3c0 反转链从字形覆盖率恢复）。
    if (!raster.draw(target, x, y, u32(profile), 0xffffffu, cp932_text)) {
        error = "Failed to rasterize dialogue CP932 text";
        return false;
    }
    // text.anm texture0 是 ARGB4444：RGB = 请求的正文色（0046e8f0 param_1），
    // A = 覆盖率。黑色正文（0x428ae0 case 0x11：fg=EDI[0x62+side]=0）在此
    // 得到 RGB=0、A=覆盖，不再被强制白化。
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
    // 0045aef0 param_9==0 分支：描边色画 (x*2±2, 0/4) 四次，正文色画
    // (x*2, 2)。对话正文描边=fg=0（00428ae0 case 0x11 push $0），因此
    // 五次 pass 同色，覆盖率为五次 pass 的并集；y 默认 2 时偏移即 ±2。
    static constexpr i32 kStrokeOffsets[4][2] = {{-2, -2}, {2, -2}, {-2, 2}, {2, 2}};
    for (const auto& offset : kStrokeOffsets) {
        if (!raster.draw(target, x + offset[0], y + offset[1], u32(profile), 0xffffffu,
                         cp932_text)) {
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

bool FontDevice::render_dialogue_line(TextureImage& target, i32 x, i32 y, const MsgLine& line,
                                      u32 color) {
    if (!line.text_cp932.empty() && !draw_text(target, x, y, 1, color, line.text_cp932)) return false;
    if (line.has_ruby && !line.ruby_cp932.empty()) {
        // 调试入口：生产 ruby 排版由 DialogueText 按原版第二参数（逐字步进）
        // 处理；此处只画单串，不参与网页最终输出。
        if (!draw_text(target, x + line.ruby_x, y, 4, color, line.ruby_cp932)) return false;
    }
    return true;
}

}  // namespace th13::sdl
