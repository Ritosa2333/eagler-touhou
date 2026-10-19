#include "platform_window.hpp"
#include "data_constants.hpp"
#include "directx_math.hpp"
#include <cmath>
#include <emmintrin.h>

namespace th20::source::platform_window {
namespace {
float mul(float a,float b) {return _mm_cvtss_f32(_mm_mul_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b) {return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float plus(float a,float b) {return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float minus(float a,float b) {return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
int trunc(float a) {return _mm_cvtt_ss2si(_mm_set_ss(a));}
int add(int a,int b) {return static_cast<int>(static_cast<unsigned>(a)+static_cast<unsigned>(b));}
float from_signed(int a) {return _mm_cvtss_f32(_mm_cvtsi32_ss(_mm_setzero_ps(),a));}
float from_unsigned(std::uint32_t a) {
    __m128d value=_mm_cvtsi32_sd(_mm_setzero_pd(),static_cast<std::int32_t>(a));
    if(a&0x80000000) value=_mm_add_sd(value,_mm_set_sd(4294967296.0));
    return _mm_cvtss_f32(_mm_cvtsd_ss(_mm_setzero_ps(),value));
}
void bounds(program_entry::ViewportState& v) {
    const auto& p=v.adjusted_viewport;
    v.bounds[0]=from_unsigned(p.X);v.bounds[1]=from_unsigned(p.Y);
    v.bounds[2]=plus(from_unsigned(p.X),from_unsigned(p.Width));
    v.bounds[3]=plus(from_unsigned(p.Y),from_unsigned(p.Height));
}
}
void update_camera(program_entry::ViewportState& v,const D3DVIEWPORT9& rectangle) {
    const float tangent=static_cast<float>(std::tan(static_cast<double>(div(v.field_of_view,2.0f)))); // 439850->CRT tan
    directx::Vector3 eye{
        plus(from_unsigned(rectangle.X),div(from_unsigned(rectangle.Width),2.0f)),
        plus(from_unsigned(rectangle.Y),div(from_unsigned(rectangle.Height),2.0f)),
        div(from_unsigned(rectangle.Height>>1),tangent)};
    directx::Vector3 target{
        plus(from_unsigned(v.viewport.X),div(from_unsigned(v.viewport.Width),2.0f)),
        plus(from_unsigned(v.viewport.Y),div(from_unsigned(v.viewport.Height),2.0f)),0.0f};
    directx::Vector3 up{0.0f,data::value_0056e0f4,0.0f};
    directx::look_at(v.view,eye,target,up);
    directx::perspective(v.projection,v.field_of_view,
        div(from_unsigned(rectangle.Width),from_unsigned(rectangle.Height)),1.0f,data::value_00571ea0);
}
void set_render_offsets(GraphicsStatePrefix& g,int x,int y) {
    auto& w=program_entry::window_state;auto& full=g.viewports[2];auto& play=g.viewports[5];
    full.offset_x=x;full.offset_y=y;
    full.adjusted_viewport=full.viewport;full.adjusted_viewport.X=x;full.adjusted_viewport.Y=y;
    full.adjusted_viewport.Width=w.scaled_width;full.adjusted_viewport.Height=w.scaled_height;
    bounds(full);
    play.offset_x=add(add(trunc(mul(data::value_0056cd94,w.scale)),x),trunc(mul(data::value_0056cda4,w.scale))/2);
    play.offset_y=add(trunc(mul(data::value_0056cd90,w.scale)),y);
    play.viewport.X=add(trunc(mul(data::value_0056cd94,w.scale)),x);
    play.viewport.Y=add(trunc(mul(data::value_0056cd90,w.scale)),y);
    play.viewport.Width=trunc(mul(data::value_0056cda4,w.scale));
    play.viewport.Height=trunc(mul(data::value_0056d7c0,w.scale));
    play.adjusted_viewport=play.viewport;bounds(play);
    update_camera(play,play.viewport);
}
void center_render_viewports() {
    auto& w=program_entry::window_state;
    const auto dx=static_cast<int>(static_cast<unsigned>(w.viewport_width)-static_cast<unsigned>(w.scaled_width));
    const auto dy=static_cast<int>(static_cast<unsigned>(w.viewport_height)-static_cast<unsigned>(w.scaled_height));
    // The diagnostic 40c6b0 is a proved no-op and has no observable output.
    set_render_offsets(program_entry::graphics_state,trunc(div(from_signed(dx),2.0f)),trunc(div(from_signed(dy),2.0f)));
}
void initialize_render_viewports(GraphicsStatePrefix& graphics) { // 4daba0
    auto& window=program_entry::window_state;auto& full=graphics.viewports[2];
    for(unsigned index:{0u,1u,2u,5u,6u}) for(float& value:full.vectors[index]) value=0.0f;
    full.vectors[2][1]=1.0f;
    full.field_of_view=div(data::value_0056e0f0,data::value_0056f00c);
    full.viewport={0,0,static_cast<DWORD>(window.scaled_width),static_cast<DWORD>(window.scaled_height),0.0f,1.0f};
    full.field_00f8=2;full.field_0058=window.scaled_width;full.field_005c=window.scaled_height;
    full.offset_x=0;full.offset_y=0;full.adjusted_viewport=full.viewport;
    bounds(full);update_camera(full,full.viewport);
    const float width=plus(data::value_0056cda4,data::value_0056cd94);
    const float height=plus(data::value_0056d7c0,data::value_0056cd94);
    auto& unscaled=graphics.viewports[0];unscaled=full;
    unscaled.field_00f8=0;unscaled.offset_x=0;unscaled.offset_y=0;
    unscaled.viewport.X=add(window.scaled_width/2,-trunc(div(width,2.0f)));
    unscaled.viewport.Y=add(window.scaled_height/2,-trunc(div(height,2.0f)));
    unscaled.viewport.Width=trunc(width);unscaled.viewport.Height=trunc(height);
    unscaled.adjusted_viewport=unscaled.viewport;bounds(unscaled);update_camera(unscaled,unscaled.viewport);
    const int left=trunc(mul(div(minus(data::value_0056cdac,data::value_0056cda4),2.0f),window.scale));
    const int top=trunc(mul(data::value_0056cd90,window.scale));
    const int play_width=trunc(mul(data::value_0056cda4,window.scale));
    const int play_height=trunc(mul(data::value_0056d7c0,window.scale));
    auto& play=graphics.viewports[1];play=unscaled;play.field_00f8=1;play.offset_x=0;play.offset_y=0;
    play.viewport.X=left;play.viewport.Y=top;play.viewport.Width=play_width;play.viewport.Height=play_height;
    play.adjusted_viewport=play.viewport;bounds(play);update_camera(play,play.viewport);
    auto& aligned=graphics.viewports[4];aligned=unscaled;aligned.field_00f8=1;
    aligned.offset_x=full.offset_x;aligned.offset_y=full.offset_y;
    aligned.viewport.X=aligned.offset_x;aligned.viewport.Y=aligned.offset_y;
    aligned.viewport.Width=window.scaled_width;aligned.viewport.Height=window.scaled_height;
    aligned.adjusted_viewport=aligned.viewport;bounds(aligned);update_camera(aligned,aligned.viewport);
    auto& centered=graphics.viewports[5];centered=full;centered.field_00f8=1;
    centered.offset_x=add(add(full.offset_x,left),play_width/2);centered.offset_y=add(top,full.offset_y);
    centered.viewport.X=add(left,full.offset_x);centered.viewport.Y=add(top,full.offset_y);
    // These unusual width/height additions are in the specimen (4db948/4db980).
    centered.viewport.Width=add(play_width,full.offset_x);centered.viewport.Height=add(play_height,full.offset_y);
    centered.adjusted_viewport=centered.viewport;bounds(centered);update_camera(centered,centered.viewport);
    auto& final=graphics.viewports[3];final=unscaled;final.field_00f8=3;final.offset_x=0;final.offset_y=0;
    final.viewport.X=trunc(div(minus(from_signed(window.scaled_width),width),2.0f));
    final.viewport.Y=trunc(div(minus(from_signed(window.scaled_height),height),2.0f));
    final.viewport.Width=trunc(width);final.viewport.Height=trunc(height);
    final.adjusted_viewport=final.viewport;bounds(final);update_camera(final,final.viewport);
}
}
namespace th20::source::program_entry::unrecovered {
void fn_004dbce0(GraphicsStatePrefix&) {platform_window::center_render_viewports();}
}
