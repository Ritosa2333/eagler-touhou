// Development-only rendering acceptance fixture, not a playable release.
#include "GraphicsDevice.hpp"
#include <emscripten.h>
#include <memory>
namespace {
std::unique_ptr<th075::sdl::GraphicsDevice> device;
std::unique_ptr<th075::ZunGraphics> graphics;
th075::TexturePack pack;std::vector<th075::u32> handles;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th075_probe_initialize(unsigned scale){device=std::make_unique<th075::sdl::GraphicsDevice>();if(!device->initialize(scale))return 0;graphics=std::make_unique<th075::ZunGraphics>(*device);return 1;}
EMSCRIPTEN_KEEPALIVE const char* th075_probe_error(){return device?device->error.c_str():"Rendering probe not initialized";}
EMSCRIPTEN_KEEPALIVE int th075_probe_texture(const void* bytes,unsigned count,unsigned palette,unsigned index){
    if(!device||!pack.load(bytes,count,palette)||index>=pack.images.size())return 0;device->unload(handles);handles.clear();const auto handle=device->upload(pack.images[index]);if(!handle)return 0;handles.push_back(handle);return 1;
}
EMSCRIPTEN_KEEPALIVE int th075_probe_draw(float x,float y,float scale){
    if(!device||handles.empty())return 0;
    // Draw the allocated texture including its original power-of-two padding.
    const auto size=device->texture_size(handles[0]);
    const std::array<std::array<float,4>,4> points{{{x,y,0,1},{x+size.width*scale,y,0,1},{x+size.width*scale,y+size.height*scale,0,1},{x,y+size.height*scale,0,1}}};
    device->clear(0xff000000);graphics->textured_quad(points,{0,0,th075::i32(size.width),th075::i32(size.height)},0xffffffff,handles[0]);device->present();return 1;
}
EMSCRIPTEN_KEEPALIVE const th075::u8* th075_probe_pixels(){return device?device->capture_pixels():nullptr;}
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* th075_probe_stats(){return device?&device->renderer.stats:nullptr;}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE unsigned th075_probe_gl_error(){return glGetError();}
EMSCRIPTEN_KEEPALIVE unsigned th075_probe_framebuffer(){return glCheckFramebufferStatus(GL_FRAMEBUFFER);}
}

extern "C" EMSCRIPTEN_KEEPALIVE int th075_probe_monochrome(unsigned color){
    if(!device||handles.empty())return 0;const auto size=device->texture_size(handles[0]);
    const std::array<std::array<float,4>,4> points{{{0,0,0,1},{float(size.width),0,0,1},{float(size.width),float(size.height),0,1},{0,float(size.height),0,1}}};
    device->clear(0xff000000);graphics->monochrome(true);graphics->textured_quad(points,{0,0,th075::i32(size.width),th075::i32(size.height)},color,handles[0]);graphics->monochrome(false);device->present();return 1;
}
