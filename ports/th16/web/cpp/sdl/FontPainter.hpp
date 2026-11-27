// th16 宿主文字刷入——原版 0x459240 中只有 GDI TextOutA 的字形光栅由这里提供（SDL3_ttf）；
// DIB 预翻 α / 柔化 / 取邻色 / 源行偏移在 game/TextRaster 逐步复刻。
//
// 字体按 0x458db0 的 CreateFontA 表建（正高度 = GDI 单元高）：宿主装有 メイリオ（/meiryo.ttc）时走
// DAT_004a6f2c=1 的字号表，否则走 ＭＳ ゴシック / ＭＳ 明朝 表（明朝缺失时 GDI 式回落到ゴシック）。
// CP932 → Unicode 映射取 /fonts/cp932.bin（网页壳按 TextDecoder('shift_jis') 生成）；字体文件由宿主提供，不入库。
// 未定义 TH16_HAS_TTF 或缺字体时只跑 DIB 管线（矩形被覆盖为透明），不留旧字。
#pragma once
#include "../game/TextPaint.hpp"
#include "../game/TextRaster.hpp"

#include <memory>
#include <string>
#include <vector>

namespace th16 {
class AnmManager;
}

namespace th16::sdl {

class GraphicsDevice;

class FontPainter {
    struct Faces;
    std::unique_ptr<Faces> faces_;
    TextDib dib_;
    std::vector<u16> rect_;
    bool tried_ = false;

public:
    FontPainter();
    ~FontPainter();
    std::string error;
    bool initialize();
    // 把队列刷进 GraphicsDevice 纹理；刷完清空 queue
    bool flush(GraphicsDevice& graphics, AnmManager& animations, std::vector<TextIntoTextureRequest>& queue);
};

}  // namespace th16::sdl
