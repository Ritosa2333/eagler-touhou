#include "platform_window.hpp"
#include "data_constants.hpp"
#include <xmmintrin.h>
#include <emmintrin.h>

namespace th20::source::platform_window {
namespace {
// The specimen uses scalar SSE CVTSI2SS/DIVSS/MULSS/CVTTSS2SI. Explicit
// operations preserve each binary32 rounding point and integer-indefinite
// result, including the mode-8 dependency on the previous viewport height.
float cvt(std::int32_t a) { return _mm_cvtss_f32(_mm_cvtsi32_ss(_mm_setzero_ps(),a)); }
float mul(float a,float b) { return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(a),_mm_set_ss(b))); }
float div(float a,float b) { return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b))); }
std::int32_t trunc(float a) { return _mm_cvtt_ss2si(_mm_set_ss(a)); }
std::int32_t add(std::int32_t a,std::int32_t b) {
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(a)+static_cast<std::uint32_t>(b));
}
}
void calculate_layout(WindowStatePrefix& destination,int choose_scale) {
    auto& w=program_entry::window_state;
    auto& c=program_entry::graphics_state.configuration;
    if(w.display_mode==8 || w.display_mode==9) {
        w.client_width=w.display_width; w.client_height=w.display_height;
        if(choose_scale) {
            if(w.display_width>=2560 && w.display_height>=1920) {w.scale=2.0f;c.scale_choice=4;}
            else if(w.display_width>=1920 && w.display_height>=1440) {w.scale=2.0f;c.scale_choice=3;}
            else if(w.display_width>=1280 && w.display_height>=960) {w.scale=2.0f;c.scale_choice=2;}
            else if(w.display_width>=960 && w.display_height>=720) {w.scale=1.5f;c.scale_choice=1;}
            else {w.scale=1.0f;c.scale_choice=0;}
        }
        w.scaled_width=trunc(mul(data::value_0056cdac,w.scale));
        w.scaled_height=trunc(mul(data::value_0056cda8,w.scale));
        const bool narrow=div(cvt(w.client_width),cvt(w.client_height))<div(cvt(w.scaled_width),cvt(w.scaled_height));
        if(w.display_mode==9) {
            if(narrow) {
                w.viewport_width=w.scaled_width;
                w.viewport_height=trunc(mul(cvt(w.scaled_width),div(cvt(w.client_height),cvt(w.client_width))));
            } else {
                w.viewport_width=trunc(mul(cvt(w.scaled_height),div(cvt(w.client_width),cvt(w.client_height))));
                w.viewport_height=w.scaled_height;
            }
        } else if(narrow) {
            int extent=w.scaled_width;
            while(add(w.scaled_width/2,extent)<w.client_width) extent=add(w.scaled_width/2,extent);
            w.viewport_width=trunc(mul(cvt(w.scaled_height),div(cvt(w.client_width),cvt(extent))));
            // 0x0041e3a0 reads 0x005b8814, not the newly computed width.
            w.viewport_height=trunc(mul(cvt(w.viewport_height),div(cvt(w.client_height),cvt(w.client_width))));
        } else {
            int extent=w.scaled_height;
            while(add(w.scaled_height/2,extent)<w.client_height) extent=add(w.scaled_height/2,extent);
            w.viewport_height=trunc(mul(cvt(w.scaled_height),div(cvt(w.client_height),cvt(extent))));
            w.viewport_width=trunc(mul(cvt(w.viewport_height),div(cvt(w.client_width),cvt(w.client_height))));
        }
    } else {
        switch(w.display_mode) {
        case 7:w.scale=2.0f;w.client_width=2560;w.client_height=1920;break;
        case 6:w.scale=2.0f;w.client_width=1920;w.client_height=1440;break;
        case 2:case 5:w.scale=2.0f;w.client_width=1280;w.client_height=960;break;
        case 1:case 4:w.scale=1.5f;w.client_width=960;w.client_height=720;break;
        default:w.scale=1.0f;w.client_width=640;w.client_height=480;break;
        }
    }
    w.scaled_width=trunc(mul(data::value_0056cdac,w.scale));
    w.scaled_height=trunc(mul(data::value_0056cda8,w.scale));
    auto& d=destination;
    d.nominal_width=640;d.nominal_height=480;d.playfield_width=384;d.playfield_height=448;
    d.offset_x=add(w.scaled_width,-d.nominal_width)/2;
    d.offset_y=add(w.scaled_height,-d.nominal_height)/2;
    for(int i=0;i<2;++i) {
        d.field_0028[i]=add(d.offset_x,32);d.field_0030[i]=add(d.offset_x,16);
        d.field_0048[i]=trunc(mul(data::value_0056cd94,w.scale));
        d.field_0050[i]=trunc(mul(data::value_0056cd90,w.scale));
        d.field_0038[i]=add(d.offset_x,320);d.field_0040[i]=add(d.offset_y,16);
    }
    d.field_0058=w.scaled_width/2;d.field_005c=add(w.scaled_height,-448)/2;
    d.field_0060=w.scaled_width/2;d.field_0064=trunc(mul(data::value_0056cd90,w.scale));
}
}
namespace th20::source::program_entry::unrecovered {
void fn_0041e050(WindowStatePrefix& w,int mode) {platform_window::calculate_layout(w,mode);}
}
