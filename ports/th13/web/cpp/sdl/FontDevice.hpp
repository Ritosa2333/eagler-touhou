// 字体设备（对话/日文文本栅格源）：结构与 th15 同族——SDL3_ttf +
// eagler::text::RuntimeFontRaster（common/include/eagler/text）。
// cp932 映射在仓内推导（game/MsgPlayer 的 build_cp932_font_mapping），
// 不要求 /fonts/cp932.bin 启动资产；真实字体文件（MS Gothic）缺席时
// initialize 显式失败（BLOCKED，不伪造、不换字体），见 FontDevice.cpp 头注。
// 字号/字重槽集待 0x4282b0/0x46e8f0 校准后扩展。
#pragma once

#include "GraphicsDevice.hpp"
#include "../game/MsgPlayer.hpp"
#include <eagler/text/RuntimeFontRaster.hpp>

#include <string>
#include <vector>

namespace th13::sdl {

class FontDevice {
public:
    std::string error;
    explicit FontDevice(GraphicsDevice& g) : graphics_(g) {}
    bool initialize();  // 字体文件缺席 = false + error（显式 BLOCKED 面）
    bool ready() const noexcept { return ready_; }
    // 真实字体度量（接入 RuntimeFontRaster 实际字形 metric）
    i32 extent(const std::string& cp932_text, i32 profile = 0);
    // 生产栅格原语：把 CP932 文本画进调用方给的动态 TextureImage
    //（DialogueText 用 text.anm 的 1024x48 暂存面再缩入 texture0 条带）。
    bool draw_text(TextureImage& target, i32 x, i32 y, i32 profile, u32 color,
                   const std::string& cp932_text);
    // 0x45aef0 对话行栅格：4 次 (±2,±2) 偏移描边 pass + 中心正文 pass
    //（描边 TextOut 在 (x±2, y∓2)/(x±2, y±2)，正文在 (x,y)，均先以白色
    // 栅格出覆盖率，再按原版 SetTextColor 颜色重着色写入 ARGB4444）。
    bool draw_dialogue(TextureImage& target, i32 x, i32 y, i32 profile, u32 color,
                       const std::string& cp932_text);
    // 旧单行调试入口保留（未接生产绘制链；生产走 DialogueText）。
    bool render_dialogue_line(TextureImage& target, i32 x, i32 y, const MsgLine& line,
                              u32 color = 0xffffffff);

private:
    GraphicsDevice& graphics_;
    // 字号/字重槽集按原版 0x45ac90 CreateFontA 参数族对齐（0x45aef0 样式→槽）：
    // 样式0→槽0 (0x20,400)；样式1→槽2 (0x20,600)；样式2/3→槽4 (0xf,700)；
    // 样式4/5→槽1/3 (0x28,400/600)——0x428ae0 case 0xf/0x10 宣言大字专用，
    // case 0x11 对话正文用样式 0/1（32px）。
    eagler::text::RuntimeFontRaster raster{
        {32.0f, 400}, // 槽 0: 样式 0 对话正文/菜单 32px/400 (0x20,400)
        {40.0f, 400}, // 槽 1: 样式 4 宣言大字 (0x28,400)
        {32.0f, 600}, // 槽 2: 样式 1 对话正文加重 32px/600 (0x20,600)
        {40.0f, 600}, // 槽 3: 样式 5 宣言大字加重 (0x28,600)
        {15.0f, 700}, // 槽 4: 样式 2 注音 ruby (0xf,700)
    };
    std::vector<u8> mapping_;  // 131072 字节 cp932→Unicode（仓内推导）
    bool ready_ = false;
};

}  // namespace th13::sdl
