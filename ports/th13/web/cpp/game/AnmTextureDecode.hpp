// PNG 贴图装载（stb_image）：thtk 提取产物（artifacts/reference/textures/，
// 构建期打进 web 包）由本模块解码为紧密 RGBA8。
// include 与解码方式照抄 th10 web/cpp/sdl/FileHost.cpp:13/:75
// （STB_IMAGE_IMPLEMENTATION 集中在唯一 TU；stbi_load_from_memory 强制 4 通道）。
// stb_image.h v2.08 位于 portable/sdl/third_party/（8-bit RGBA 输出已实测，无 16bpc 需求）。
#pragma once
#include "Types.hpp"
#include <vector>
namespace th13 {
// 解码 PNG 字节流为 RGBA8；失败返回 false。输出行主序、紧密排布 w*h*4。
bool anm_decode_png(const u8* bytes, u32 size, u32& width, u32& height, std::vector<u8>& rgba);
}
