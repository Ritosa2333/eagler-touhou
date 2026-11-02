#include "FontDevice.hpp"
#include <SDL3/SDL.h>
namespace th17::sdl {
bool FontDevice::load(u32 slot){
 if(slot>=loaded.size()){error="Font slot outside range";return false;}if(loaded[slot])return true;
 const std::string name=slot<13?"font"+std::to_string(slot)+".bin":slot==13?"cp932.bin":"blend4444.bin",path="/fonts/"+name;size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(path.c_str(),&size));const bool valid=bytes&&size<=UINT32_MAX&&raster.glyphs.load(slot,bytes,u32(size));SDL_free(bytes);if(!valid){error="Invalid measured TH17 font: "+path;return false;}loaded[slot]=true;return true;
}
bool FontDevice::initialize(){return load(13)&&load(14);}
bool FontDevice::preload(i32 style){return initialize()&&load(TextRaster::font_index(style,fallback));}
bool FontDevice::text(AnmVm& vm,const TextureText& request){
 TextLayout layout;if(!TextLayout::prepare(vm,request,fallback,layout)){error="Missing text animation or style";return false;}if(!preload(request.font))return false;
 const u32 handle=graphics.texture(*layout.resource,layout.texture);auto* image=graphics.pixels(handle);if(!image){error="Text texture was not prepared";return false;}
 graphics.backend.flush();if(!raster.write(*image,layout.region,request.bytes,layout.style)){error="TH17 text raster bounds or format";return false;}graphics.changed(handle,{u32(layout.region.left),u32(layout.region.top),u32(layout.region.right),u32(layout.region.bottom)});vm.visual.flags|=1;++writes;return true;
}
bool FontDevice::dialogue(AnmVm& vm,u32 color,u32 outline,i32 style,i32 x,i32 spacing,const std::string& bytes){return text(vm,{color,outline,style,x,spacing,false,false,bytes});}
bool FontDevice::spell_title(AnmVm& vm,const std::string& bytes){return text(vm,{0xffffff,0,0,0,0,true,false,bytes});}
}
