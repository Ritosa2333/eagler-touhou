// Shared production path for 0046e8f0/0046e970/0046ea30 style0 ANM text.
#pragma once
#include "DialogueText.hpp"

namespace th13 {
using AnmTextRasterizer = std::function<bool(TextureImage&, i32 x, i32 y, u32 color,
                                            const std::string& cp932)>;
enum class AnmTextAlignment { Left, Center, Right };
// Nominal17/style0 is the proven default for the menu text VMs. Unsupported
// nominal heights fail instead of borrowing an unrelated font profile.
// Color is original COLORREF (0x00BBGGRR); output is the selected ARGB4444 sprite.
bool rasterize_anm_text(AnmVm&, DialogueTextureSink&, const AnmTextRasterizer&,
                        u32 color, const std::string& cp932,
                        AnmTextAlignment = AnmTextAlignment::Left,
                        i32 nominal_height = 17, std::string* error = nullptr);
} // namespace th13

