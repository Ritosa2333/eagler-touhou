// Development renderer integration fixture. This is not the TH11 game entry.
#include "../../cpp/sdl/GraphicsDevice.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "../../cpp/game/AnmRenderer.hpp"
#include <memory>
#include <emscripten.h>
namespace {
using namespace th11;using namespace touhou::graphics;
struct Viewer {sdl::GraphicsDevice graphics;AnmRenderer renderer{graphics};AnmResource resource;std::unique_ptr<AnmManager> animations;std::string error;u32 frame=0;
 bool initialize(){if(!graphics.initialize()){error=graphics.error;return false;}auto& p=graphics.pipeline();p.blend=true;p.alphaTest=true;p.alphaCompare=Compare::Greater;p.alphaReference=0;p.depthTest=false;p.color.operation=p.alpha.operation=ColorOperation::Multiply;return true;}
 bool open(const u8* data,u32 size,i32 script){error.clear();graphics.error.clear();renderer.invalidate();animations.reset();graphics.unload(resource);if(!resource.open(data,size)||!graphics.preload(resource)){error="Resource load failed: "+graphics.error;return false;}if(script<0||u32(script)>=resource.scripts.size()){error="Script out of range";return false;}bool overlay=false;const auto& bytes=resource.scripts[script].bytes;for(u32 off=0;off+8<=bytes.size();){const auto* op=reinterpret_cast<const AnmInstruction*>(bytes.data()+off);if(op->opcode==-1)break;if(op->opcode==68&&op->argument<u32>(0)>=29)overlay=true;off+=op->length;}animations=std::make_unique<AnmManager>();animations->script_rng.seed=animations->visual_rng.seed=12345;auto* vm=animations->create(resource,script,0,0,overlay);if(!vm){error="Animation creation failed";return false;}frame=0;return true;}
 bool step(){if(!animations)return false;if(!animations->update(false)||!animations->update(true)){error="Animation update failed at opcode "+std::to_string(animations->last_error);return false;}graphics.clear(0xff202020);for(u32 layer=0;layer<31;++layer)if(renderer.draw_layer(animations->layer_first(layer))<0){error="Animation rendering failed";return false;}renderer.flush();graphics.present();++frame;if(!graphics.error.empty()){error=graphics.error;return false;}if(*graphics.backend.error()){error=graphics.backend.error();return false;}return true;}
};std::unique_ptr<Viewer> viewer;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE void* th11_viewer_allocate(unsigned n){return std::malloc(n);}
EMSCRIPTEN_KEEPALIVE void th11_viewer_release(void* p){std::free(p);}
EMSCRIPTEN_KEEPALIVE int th11_viewer_initialize(){viewer=std::make_unique<Viewer>();return viewer->initialize();}
EMSCRIPTEN_KEEPALIVE int th11_viewer_open(const unsigned char* data,unsigned size,int script){return viewer&&viewer->open(data,size,script);}
EMSCRIPTEN_KEEPALIVE int th11_viewer_step(){return viewer&&viewer->step();}
EMSCRIPTEN_KEEPALIVE const char* th11_viewer_error(){return viewer?viewer->error.c_str():"Viewer not initialized";}
EMSCRIPTEN_KEEPALIVE unsigned th11_viewer_frames(){return viewer?viewer->frame:0;}
EMSCRIPTEN_KEEPALIVE unsigned th11_viewer_active(){return viewer&&viewer->animations?viewer->animations->active_count():0;}
}
