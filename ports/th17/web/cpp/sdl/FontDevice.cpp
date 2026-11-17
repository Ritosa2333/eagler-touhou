#include "FontDevice.hpp"
#include "../game/Dialogue.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
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
 TextLayout layout;if(!TextLayout::prepare(vm,request,fallback,layout)){error="Missing text animation or style";return false;}if(!initialize())return false;
 const u32 handle=graphics.texture(*layout.resource,layout.texture);auto* image=graphics.pixels(handle);if(!image){error="Text texture was not prepared";return false;}
 graphics.backend.flush();if(!(request.utf8?write_localized(*image,layout,request):raster.write(*image,layout.region,request.bytes,layout.style))){error="TH17 text raster bounds or format";return false;}graphics.changed(handle,{u32(layout.region.left),u32(layout.region.top),u32(layout.region.right),u32(layout.region.bottom)});vm.visual.flags|=1;++writes;return true;
}
bool FontDevice::dialogue(AnmVm& vm,u32 color,u32 outline,i32 style,i32 x,i32 spacing,const std::string& bytes){
 TextureText request{color,outline,style,x,spacing,false,false,bytes,translated_message(bytes)};
 if(style==0||style==1)request.width_limit=Dialogue::line_width_limit;
 return text(vm,request);
}
i32 FontDevice::dialogue_width(AnmVm& vm,const std::string& bytes,i32 style){
 if(!initialize())return -1;
 TextureText request{0xffffff,0,style,0,0,false,false,bytes,translated_message(bytes)};
 request.width_limit=Dialogue::line_width_limit;
 TextLayout layout;
 if(!TextLayout::prepare(vm,request,fallback,layout))return -1;
 if(!request.utf8)return std::min(request.width_limit,original_font.measure(bytes,TextRaster::font_index(style,fallback)));
 std::string visible;
 bool centered=false;
 i32 width=0;
 return fit_localized(layout,request,visible,centered,width)?width:-1;
}
bool FontDevice::spell_title(AnmVm& vm,const std::string& bytes,bool utf8){return text(vm,{0xffffff,0,utf8?8:0,0,0,true,false,bytes,utf8});}
}
