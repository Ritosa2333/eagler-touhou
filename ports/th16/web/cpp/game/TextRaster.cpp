#include "TextRaster.hpp"

#include <algorithm>
#include <cstring>

namespace th16 {

u16 text_dib_blend(u16 before, u32 colorref, u8 coverage) noexcept {
    auto channel = [coverage](u32 old, u32 target8) {
        const u32 target = target8 >> 4;
        return (old * (255u - coverage) + target * coverage + 127u) / 255u;
    };
    return u16(channel((before >> 8) & 15u, colorref & 0xffu) << 8 |
               channel((before >> 4) & 15u, (colorref >> 8) & 0xffu) << 4 |
               channel(before & 15u, (colorref >> 16) & 0xffu));
}

void text_dib_flip_alpha(TextDib& dib, i32 rows) {
    const i32 count = TextDib::kWidth * std::min(rows, TextDib::kAllocRows);
    for (i32 i = 0; i < count; ++i) {
        u16& p = dib.pixels[size_t(i)];
        p ^= 0xf000;
        const u16 a = p & 0xf000;
        if (a != 0 && a != 0xf000) p |= 0xf000;
    }
}

void text_dib_soften_alpha(TextDib& dib, i32 rows) {
    constexpr i32 w = TextDib::kWidth;
    rows = std::min(rows, TextDib::kAllocRows);
    if (rows < 3) return;
    // 原版副本只拷 rows 行（+1 字节）；末像素右下邻读到副本之外，这里取 DIB 下一行（memset 后为 0）
    std::vector<u16> copy(dib.pixels.begin(), dib.pixels.begin() + std::min<size_t>(size_t(w) * rows + 1, dib.pixels.size()));
    if (copy.size() < size_t(w) * rows + 1) copy.resize(size_t(w) * rows + 1, 0);
    auto a = [&copy](i32 i) { return u32(copy[size_t(i)] >> 12); };
    for (i32 k = 0; k < (rows - 2) * w; ++k) {
        const i32 i = w + k;
        if (copy[size_t(i)] >= 0x1000 || k % w == 0) continue;
        const u32 sum = (a(i - w) + a(i + w) + a(i - 1) + a(i + 1)) * 2 +
                        a(i + w - 1) + a(i + w + 1) + a(i - w - 1) + a(i - w + 1);
        u16& p = dib.pixels[size_t(i)];
        p = u16(((sum / 14u) << 12) | (p & 0xfffu));
    }
}

void text_dib_bleed_color(TextDib& dib, i32 rows) {
    constexpr i32 w = TextDib::kWidth;
    rows = std::min(rows, TextDib::kAllocRows);
    for (i32 y = 0; y < rows; ++y) {
        for (i32 x = 0; x < w; ++x) {
            u16& p = dib.pixels[size_t(y) * w + size_t(x)];
            if (p >= 0x1000) continue;
            u32 r = 0, g = 0, b = 0, n = 0;
            auto take = [&](u16 q) {
                if (q < 0x1000) return;
                r += (q >> 8) & 15u;
                g += (q >> 4) & 15u;
                b += q & 15u;
                ++n;
            };
            if (x != 0) take(dib.pixels[size_t(y) * w + size_t(x - 1)]);
            if (x < w - 1) take(dib.pixels[size_t(y) * w + size_t(x + 1)]);
            if (y != 0) take(dib.pixels[size_t(y - 1) * w + size_t(x)]);
            if (y < TextDib::kRows - 1) take(dib.pixels[size_t(y + 1) * w + size_t(x)]);
            if (n > 1) {
                r /= n;
                g /= n;
                b /= n;
            }
            p = u16((((r >> 1) & 15u) << 8) | (((g >> 1) & 15u) << 4) | ((b >> 1) & 15u) | (p & 0xf000u));
        }
    }
}

void text_raster_paint(const TextIntoTextureRequest& req, TextGlyphSource& glyphs, TextDib& dib,
                       std::vector<u16>& out, i32& out_w, i32& out_h) {
    std::fill(dib.pixels.begin(), dib.pixels.end(), u16(0));
    const i32 height = req.height > 0x10 ? req.height : 0x11;
    const i32 rows = height * 2 + 0xc;
    text_dib_flip_alpha(dib, rows);

    const char* text = req.text.c_str();
    const i32 len = i32(req.text.size());
    const i32 font = req.font;
    if (req.spacing == 0) {
        const i32 x0 = req.x + 2;
        if (req.shadow) {
            static constexpr i32 kOffsets[8][2] = {{1, 1}, {-1, 1}, {1, 3}, {-1, 3}, {2, 2}, {-2, 2}, {0, 0}, {0, 4}};
            for (const auto& o : kOffsets) glyphs.text_out(dib, font, x0 + o[0], o[1], req.outline, text, len);
        }
        glyphs.text_out(dib, font, x0, 2, req.color, text, len);
    } else if (len > 0) {
        // 逐双字节：奇数长度时末对带上结尾 NUL
        i32 x = req.x - 2;
        for (i32 i = 0; i < ((len - 1) >> 1) + 1; ++i) {
            const char pair[2] = {text[2 * i], text[2 * i + 1]};
            if (req.shadow) {
                glyphs.text_out(dib, font, x + 4, 4, req.outline, pair, 2);
                glyphs.text_out(dib, font, x, 4, req.outline, pair, 2);
                glyphs.text_out(dib, font, x + 4, 0, req.outline, pair, 2);
                glyphs.text_out(dib, font, x, 0, req.outline, pair, 2);
            }
            glyphs.text_out(dib, font, x + 2, 2, req.color, pair, 2);
            x += req.spacing;
        }
    }

    text_dib_flip_alpha(dib, rows);
    text_dib_soften_alpha(dib, rows);
    text_dib_bleed_color(dib, rows);

    out_w = std::min(req.right - req.left, TextDib::kWidth);
    out_h = req.bottom - req.top;
    if (out_w <= 0 || out_h <= 0) {
        out_w = out_h = 0;
        out.clear();
        return;
    }
    const i32 offset = glyphs.meiryo() && font != 6 && font != 2 ? 6 : 0;
    out.assign(size_t(out_w) * out_h, 0);
    for (i32 y = 0; y < out_h; ++y) {
        const i32 sy = y + offset;
        if (sy >= TextDib::kAllocRows) break;
        std::memcpy(out.data() + size_t(y) * out_w, dib.pixels.data() + size_t(sy) * TextDib::kWidth, size_t(out_w) * 2);
    }
}

}  // namespace th16
