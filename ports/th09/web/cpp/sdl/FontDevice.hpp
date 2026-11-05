#pragma once
#include "GraphicsDevice.hpp"
#include <memory>
namespace th09::sdl {
class FontDevice {
    struct Impl;std::unique_ptr<Impl> impl;GraphicsDevice& graphics;
public:
    std::string error;
    explicit FontDevice(GraphicsDevice&);~FontDevice();
    bool initialize();
    bool text(AnmVm&,const char* cp932,u32 color,u32 outline);
    void prewarm(const char* cp932,i32 height);
    void prewarm_game(ResourceReader&);
};
}
