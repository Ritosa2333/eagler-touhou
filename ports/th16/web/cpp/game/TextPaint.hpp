// th16 文字贴图——原版 0x46d990（对话行）/ 0x46dab0（符卡名居中）把 GDI TextOut 结果
// 经 0x459240 写入 ANM 纹理，并置 VM flags_lo bit0（0x530 |= 1）。
//
// 逻辑侧：格式化字符串、取精灵像素矩形（AnmRuntimeSprite +0x0c..+0x18）、置标志、入队请求。
// 像素写入在逻辑帧末经 TextRaster（0x459240 的 DIB 管线）+ 宿主字形源执行。
#pragma once
#include <functional>
#include <string>
#include <vector>

#include "AnmVm.hpp"
#include "Types.hpp"

namespace th16 {

class AnmManager;
struct AnmFile;

// 0x459240 的实参（逻辑侧已算好），外加目标 VM 与精灵像素矩形
struct TextIntoTextureRequest {
    u32 vm_id = 0;
    std::string text;
    u32 color = 0xffffff;     // param_4 COLORREF：0x00BBGGRR
    u32 outline = 0;          // param_5 阴影色；flags_hi bit12 时强制 0
    i32 font = 0;             // param_8：0..8 → 0x458db0 字体句柄
    i32 height = 0x11;        // param_3：DAT_00491b68[font]
    i32 x = 0;                // param_2：起点像素（0x46d990 为 param_6×2；0x46dab0 为居中式）
    i32 spacing = 0;          // param_9：逐双字节步长（调用方参数 ×2）；0 = 整串 TextOut
    bool shadow = true;       // param_10：!(flags_hi & 0x1000)
    i32 left = 0, top = 0, right = 0, bottom = 0;  // param_1：精灵像素矩形（float 截断）
};

// DAT_00491b68
inline i32 text_font_height(i32 font) noexcept {
    static constexpr i32 kHeights[] = {17, 17, 17, 17, 21, 21, 21, 21, 14};
    if (font < 0 || font > 8) return 0x11;
    return kHeights[font];
}

// 0x46d990：把格式化后的文字画进 vm 纹理（左对齐起点）；返回 false 表示 VM / 精灵无效
bool text_paint_vm(AnmManager& animations, AnmVm& vm, u32 color, u32 outline, i32 font, i32 x, i32 y,
                   const char* text, std::vector<TextIntoTextureRequest>* out);

// 0x46dab0：x = (int)(sprite.width − (u32)((h×2−1)×strlen) >> 1)
bool text_paint_vm_centered(AnmManager& animations, AnmVm& vm, u32 color, u32 outline, i32 font, i32 spacing,
                            const char* text, std::vector<TextIntoTextureRequest>* out);

// 0x46dc20：居中——x = (int)sprite.width / 2 − ((u32)((h×2−1)×strlen) >> 2)
bool text_paint_vm_middle(AnmManager& animations, AnmVm& vm, u32 color, u32 outline, i32 font, i32 spacing,
                          const char* text, std::vector<TextIntoTextureRequest>* out);

}  // namespace th16
