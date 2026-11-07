#include "FontDevice.hpp"
#include <SDL3/SDL.h>
namespace th17::sdl {
bool FontDevice::initialize(){
 if(!original_ready){
  size_t size=0;auto* mapping=static_cast<u8*>(SDL_LoadFile("/fonts/cp932.bin",&size));
  const bool valid=original_font.initialize(mapping,size);SDL_free(mapping);
  if(!valid){error="Shared MS Gothic font or CP932 mapping unavailable";return false;}
  raster.glyphs.renderer(&original_font,[](void* owner,TextureImage& image,i32 x,i32 y,u32 font,u32 color,const std::string& text){
      return static_cast<eagler::text::RuntimeFontRaster*>(owner)->draw(image,x,y,font,color,text);
  });
  original_ready=true;
 }
 return load_localized_font();
}
bool FontDevice::text(AnmVm& vm,const TextureText& request){
 TextLayout layout;if(!TextLayout::prepare(vm,request,fallback,layout)){error="Missing text animation or style";return false;}
 if(!write_text(layout,request))return false;
 vm.visual.flags|=1;
 return true;
}
bool FontDevice::write_text(const TextLayout& layout,const TextureText& request){
 if(!initialize())return false;
 const u32 handle=graphics.texture(*layout.resource,layout.texture);auto* image=graphics.pixels(handle);if(!image){error="Text texture was not prepared";return false;}
 graphics.backend.flush();if(!(request.utf8?write_localized(*image,layout,request):raster.write(*image,layout.region,request.bytes,layout.style))){error="TH17 text raster bounds or format";return false;}graphics.changed(handle,{u32(layout.region.left),u32(layout.region.top),u32(layout.region.right),u32(layout.region.bottom)});++writes;return true;
}
bool FontDevice::dialogue(AnmVm& vm,u32 color,u32 outline,i32 style,i32 x,i32 spacing,const std::string& bytes){return text(vm,{color,outline,style,x,spacing,false,false,bytes,translated_message(bytes)});}
TextureText FontDevice::spell_title_request(const std::string& bytes,bool utf8){return {0xffffff,0,utf8?8:0,0,0,true,false,bytes,utf8};}
bool FontDevice::spell_title(AnmVm& vm,const std::string& bytes,bool utf8){return text(vm,spell_title_request(bytes,utf8));}
}
