#pragma once
#include "AnimationEngine.hpp"
#include "TextureResample.hpp"
#include "../game/CapturePixels.hpp"
namespace th10::browser {
// Surface locks make pending GPU pixels available through the graphics host.
// D3DX's fallback conversions execute in C++, including pause-menu captures.
struct Captures final:CapturePixelEnvironment {
    AnimationEngine& engine;GraphicsDevice& graphics;Textures textures;
    void* native_device;u32 surface_format=22,texture_flags=0;
    explicit Captures(AnimationEngine&);
    ~Captures();
    CapturePixels pixels();void process();void release_slot(i32);
    void flush(AnmManager&) override;
    i32 back_buffer(void*,void**) override;
    i32 texture_surface(void*,void**) override;
    i32 render_target(void*,i32,i32,u32,void**) override;
    i32 offscreen_surface(void*,i32,i32,u32,void**) override;
    i32 copy_surface(void*,const TextureRect*,void*,const TextureRect*,u32) override;
    void update_surface(void*,void*,const TextureRect&,void*,const CapturePoint&) override;
    void release_surface(void*) override;void free_pixels(void*) override;
};
}
