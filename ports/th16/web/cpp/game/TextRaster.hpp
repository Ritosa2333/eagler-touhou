// th16 文字光栅——原版 0x459240 的 DIB 管线（GDI 字形之外全部在此逐步复刻）。
//
// 0x4585a0：CreateDIBSection 1024×129 自顶向下、格式 0x1a（A4R4G4B4，位域 R 0xf00 / G 0xf0 / B 0xf，
//           α 掩码 0xf000）；GDI 只写 RGB 位，α 位恒写 0。
// 0x459240：memset → 0x458c80 预翻 α（底色变不透明黑）→ TextOut（阴影 8 向 / 逐字 4 角 + 主色）
//           → 0x458c80 再翻（GDI 触及像素 α=f，其余 α=0）→ 0x458af0 边缘 α 柔化 → 0x458730 透明像素
//           取邻色 → D3DXLoadSurfaceFromMemory（FILTER_NONE 原尺寸拷贝，メイリオ在且字体非 2/6 时源行偏 6）。
//
// 字形本身（GDI TextOutA）由宿主 TextGlyphSource 提供；harness 用合成字形源对拍各遍。
#pragma once
#include <vector>

#include "TextPaint.hpp"
#include "Types.hpp"

namespace th16 {

// DAT_004a5d80 / 4a5d64 / 4a5d68 / 4a5d70
struct TextDib {
    static constexpr i32 kWidth = 0x400;      // DAT_004a5d64
    static constexpr i32 kRows = 0x80;        // DAT_004a5d68（0x458730 的行界）
    static constexpr i32 kAllocRows = 0x81;   // biHeight = -0x81
    std::vector<u16> pixels = std::vector<u16>(size_t(kWidth) * kAllocRows, 0);
};

class TextGlyphSource {
public:
    virtual ~TextGlyphSource() = default;
    // DAT_004a6f2c：0x458d60 枚举到 "メイリオ" 时置 1，0x458db0 改用メイリオ字号表
    virtual bool meiryo() const = 0;
    // GDI TextOutA(hdc, x, y, bytes, len)：TA_TOP、透明背景；按覆盖率把 colorref 混进 RGB 位并清 α 位
    virtual void text_out(TextDib& dib, i32 font, i32 x, i32 y, u32 colorref, const char* bytes, i32 len) = 0;
};

// GDI 写像素：按 4 位通道混合 colorref（0x00BBGGRR），α 位写 0
u16 text_dib_blend(u16 before, u32 colorref, u8 coverage) noexcept;

// 0x458c80 / 0x458af0 / 0x458730（格式 0x1a 分支）；rows = 0x459240 的 uVar1
void text_dib_flip_alpha(TextDib& dib, i32 rows);
void text_dib_soften_alpha(TextDib& dib, i32 rows);
void text_dib_bleed_color(TextDib& dib, i32 rows);

// 0x459240：画字并给出贴向精灵矩形的 A4R4G4B4 像素（out_w × out_h，行优先）
void text_raster_paint(const TextIntoTextureRequest& req, TextGlyphSource& glyphs, TextDib& dib,
                       std::vector<u16>& out, i32& out_w, i32& out_h);

}  // namespace th16
