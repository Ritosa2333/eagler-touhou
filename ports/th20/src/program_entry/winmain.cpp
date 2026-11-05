#include "program_entry.hpp"
#ifndef TH_SDL3

// Normal source-level Windows entry for a future fully linked source build.
// The MSVC linker supplies fresh CRT startup (the role of original 0x5435e0 /
// 0x543461). This function invokes the recovered C++ body and has no PE loader,
// original-executable launch, original-code address, or machine-code fallback.
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show) {
    return th20::source::program_entry::recovered_win_main(instance, previous, command_line, show);
}
#endif
