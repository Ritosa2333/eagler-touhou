#include "effect.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/platform_window.hpp"
#include <bit>
namespace th20::source::screen {
namespace pe=program_entry;namespace n=th20::recovered;
void draw_rectangle(sprite::Controller& c,Device& device,const float (&bounds)[4],std::uint32_t color){
    sprite::flush_textured_quads(c,device);sprite::Vertex20 vertices[4];
    const float offset_x=std::bit_cast<float>(c.fields_c8[2]),offset_y=std::bit_cast<float>(c.fields_c8[3]);
    for(unsigned i=0;i<4;++i)vertices[i]={n::add32(bounds[(i&1)?2:0],offset_x),n::add32(bounds[(i&2)?3:1],offset_y),0.f,1.f,color};
#ifdef TH_SDL3
    using namespace touhou::graphics;
    auto& p=device.pipeline();
    p.color.operation=p.alpha.operation=ColorOperation::First;
    p.color.first=p.alpha.first={ArgumentSource::Diffuse};
    p.destinationBlend=BlendFactor::InverseSourceAlpha;
    device.vertex_format(VertexLayout::ScreenColor);
    device.draw(Topology::Strip,2,vertices,sizeof(sprite::Vertex20));
    c.unknown_cached_e0e=0xff;c.field_e18=0;c.cached_texture=0xffffffff;c.unknown_cached_e0d=0xff;c.blend_mode=11;c.field_e0f=0xff;
    p.color.operation=p.alpha.operation=ColorOperation::Multiply;
    p.color.first=p.alpha.first={ArgumentSource::Texture};
#else
    device.SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);device.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
    device.SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);device.SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
    device.SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);device.SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    device.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,vertices,sizeof(sprite::Vertex20));
    c.unknown_cached_e0e=0xff;c.field_e18=0;c.cached_texture=0xffffffff;c.unknown_cached_e0d=0xff;c.blend_mode=11;c.field_e0f=0xff;
    device.SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);device.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
    device.SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);device.SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
#endif
}
int draw_display(Effect& value){
    const float bounds[]{0,0,n::int_float(pe::window_state.scaled_width),n::int_float(pe::window_state.scaled_height)};
    sprite::flush_textured_quads(*pe::sprite_controller,*pe::graphics_state.device);platform_window::select_viewport(pe::graphics_state,2);
    draw_rectangle(*pe::sprite_controller,*pe::graphics_state.device,bounds,(static_cast<std::uint32_t>(value.alpha)<<24)|value.argument_20);return 1;
}
int draw_playfield(Effect& value){
    const float bounds[]{128.f,16.f,512.f,464.f};draw_rectangle(*pe::sprite_controller,*pe::graphics_state.device,bounds,(static_cast<std::uint32_t>(value.alpha)<<24)|value.argument_20);return 1;
}
int draw_current_view(Effect& value){
    const float bounds[]{0,0,n::int_float(pe::window_state.scaled_width),n::int_float(pe::window_state.scaled_height)};
    draw_rectangle(*pe::sprite_controller,*pe::graphics_state.device,bounds,(static_cast<std::uint32_t>(value.alpha)<<24)|value.argument_20);return 1;
}
int draw_offset_playfield(Effect& value){
    auto& w=pe::window_state;const float bounds[]{n::int_float(w.offset_x),n::int_float(w.offset_y),n::add32(n::int_float(w.offset_x),n::int_float(w.playfield_width)),n::add32(n::int_float(w.offset_y),n::int_float(w.playfield_height))};
    draw_rectangle(*pe::sprite_controller,*pe::graphics_state.device,bounds,(static_cast<std::uint32_t>(value.alpha)<<24)|(value.argument_24&0xffffff));return 1;
}
}
