#include "draw.hpp"
namespace th20::source::pause {
void reset_pause_text(text::Renderer& r){
    r.color=0xffffffff;r.field_1a1c8=0xffffffff;r.shadow_color=0xff000000;
    r.fields_1a1d4[1]=0;r.font_width=9;r.scale_x=r.scale_y=1;
    r.fields_1a1d4[2]=0;r.fields_1a1d4[3]=0;r.fields_1a1d4[4]=2;r.fields_1a1d4[6]=0;r.fields_1a1d4[7]=0;
    r.fields_1a1d4[8]=r.fields_1a1d4[9]=1;r.rotation=0;r.fields_1a1d4[10]=0;
}
}
