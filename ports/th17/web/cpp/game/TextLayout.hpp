#pragma once
#include "TextRaster.hpp"
#include "AnmVm.hpp"
namespace th17 {
struct TextureText {u32 color=0xffffff,outline=0;i32 font=0,x=0,spacing=0;bool right_aligned=false,centered=false;std::string bytes;};
struct TextLayout {TextRect region;TextStyle style;u32 texture=0;AnmResource* resource=nullptr;static bool prepare(AnmVm&,const TextureText&,bool fallback,TextLayout&);};
}
