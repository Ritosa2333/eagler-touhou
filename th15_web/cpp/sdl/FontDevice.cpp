#include "FontDevice.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <utility>
namespace th15::sdl {
#include "FontLocalization.inc"
bool FontDevice::load(u32 slot){if(slot>=loaded.size()){error="Font slot outside range";return false;}if(loaded[slot])return true;const std::string name=slot<13?"font"+std::to_string(slot)+".bin":slot==13?"cp932.bin":"blend4444.bin",path="/fonts/"+name;size_t size=0;auto* bytes=static_cast<u8*>(SDL_LoadFile(path.c_str(),&size));const bool valid=bytes&&size<=UINT32_MAX&&raster.glyphs.load(slot,bytes,u32(size));SDL_free(bytes);if(!valid){error="Invalid measured original font: "+path;return false;}loaded[slot]=true;return true;}
bool FontDevice::initialize(){
#if defined(TH_ENABLE_THCRAP)
 if(!initialize_localized())return false;
#endif
 return load(13)&&load(14);
}
bool FontDevice::preload(i32 style){return initialize()&&load(TextRaster::font_index(style,fallback));}
i32 FontDevice::extent(const std::string& value,i32 font){
#if defined(TH_ENABLE_THCRAP)
 if(font>=0&&font<9&&value.size()<=4096&&initialize()&&localized[font]&&valid_utf8(value)){
  const auto saved=layout_tabs;const auto width=layout(value,font).width;layout_tabs=saved;return width;
 }
#endif
 return -1;
}
bool FontDevice::text(AnmVm& vm,const DialogueText& request){
    TextLayout paint;if(!TextLayout::dialogue(vm,request,fallback,paint)){error="Missing text animation or style";return false;}if(!initialize())return false;const u32 handle=graphics.texture(*paint.resource,paint.texture);auto* image=graphics.pixels(handle);if(!image){error="Text texture was not prepared";return false;}
    bool written=false;
#if defined(TH_ENABLE_THCRAP)
    if(localized[0]&&valid_utf8(request.bytes)){
      // Only Music Room prose: preserve authored line breaks and the note/title.
      // Restore both the font size and tab state before other text uses this slot.
      auto* font=localized[request.font];const float original_size=TTF_GetFontSize(font);
      const auto original_tabs=layout_tabs;
      if(request.music_detail){
        if(!english_details){
          if(!TTF_SetFontSize(font,30.f)){error="Music detail font resize failed";return false;}
        }else{
        const int available=std::min(1024,paint.region.right-paint.region.left)-paint.style.offset-4;
        if(available<=0){error="Music detail text region unavailable";return false;}
        for(int size=24;size>=1;--size){
          if(!TTF_SetFontSize(font,float(size))){TTF_SetFontSize(font,original_size);error="Music detail font resize failed";return false;}
          layout_tabs=original_tabs;const auto line=layout(request.bytes,request.font);int end=0;
          for(const auto& run:line.runs){int width=0,height=0;font_style(request.font,run.commands);TTF_GetStringSize(font,run.text.c_str(),run.text.size(),&width,&height);end=std::max(end,run.x+width);}font_style(request.font,"");
          if(end<=available)break;
        }
        layout_tabs=original_tabs;
        }
      }
      if(request.right_aligned)paint.style.offset=paint.region.right-paint.region.left-layout(request.bytes,request.font).width;
      written=write_unicode(*image,paint.region,request.bytes,paint.style);
      if(request.music_detail){layout_tabs=original_tabs;TTF_SetFontSize(font,original_size);}
    }else
#endif
    {if(!preload(request.font))return false;written=raster.write(*image,paint.region,request.bytes,paint.style);}
    if(!written){error="Text raster bounds or format";return false;}graphics.changed(handle,{u32(paint.region.left),u32(paint.region.top),u32(paint.region.right),u32(paint.region.bottom)});vm.visual.flags|=1;++writes;return true;
}
}
