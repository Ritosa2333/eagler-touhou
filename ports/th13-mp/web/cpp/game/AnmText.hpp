#pragma once
#include "DialogueText.hpp"

namespace th13 {
using AnmTextRasterizer = std::function<bool(TextureImage&, i32 x, i32 y, u32 color, const std::string& cp932)>;
enum class AnmTextAlignment { Left, Center, Right };
bool rasterize_anm_text(AnmVm&, DialogueTextureSink&, const AnmTextRasterizer&, u32 color, const std::string& cp932,
                        AnmTextAlignment = AnmTextAlignment::Left, i32 nominal_height = 17,
                        std::string* error = nullptr);
}
