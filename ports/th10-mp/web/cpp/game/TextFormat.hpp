#pragma once
#include "Types.hpp"
namespace th10 {
// Game text uses the 32-bit Windows argument widths. Pointers refer to native
// Wasm memory; this formatter does not call the old CRT or execute code tokens.
// Returns the full output length, or -1 for an unsupported/invalid format.
i32 format_text(char* output,u32 capacity,const char* format,const u32* words,u32 word_count=0xffffffffu) noexcept;
}
