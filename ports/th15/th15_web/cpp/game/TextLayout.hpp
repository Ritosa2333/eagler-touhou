#pragma once
#include "TextRaster.hpp"
#include "Dialogue.hpp"
#include "AnmVm.hpp"
namespace th15 {
struct TextLayout {TextRect region;TextStyle style;u32 texture=0;AnmResource* resource=nullptr;static bool dialogue(AnmVm&,const DialogueText&,bool fallback,TextLayout&);};
}
