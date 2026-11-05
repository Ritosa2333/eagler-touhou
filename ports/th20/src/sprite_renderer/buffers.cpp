#include "sprite.hpp"
#include <stdexcept>
namespace th20::source::sprite {
void initialize_corner(TexturedCorner20& v) noexcept { v={0,0,0,0,0}; }
void initialize_colored_vertex(Vertex20& v) noexcept { v={0,0,0,0,0xffffffffu}; }
void initialize_textured_vertex(Vertex28& v) noexcept { v={0,0,0,0,0xffffffffu,0,0}; }
void prepare_buffers(Controller& c) noexcept {
    c.quad_count=0;
    c.textured_write=c.textured_batch_start=c.textured_vertices;
    c.colored_primitive_count=0;
    c.colored_write=c.colored_batch_start=c.colored_vertices;
}
void flush_textured_quads(Controller& c,Device& device) {
    if(c.quad_count) {
#ifdef TH_SDL3
        device.vertex_format(touhou::graphics::VertexLayout::ScreenColorUv);
        device.draw_triangle_batch(c.quad_count*2u,c.textured_batch_start,sizeof(Vertex28));
#else
        device.SetFVF(0x144);
        device.DrawPrimitiveUP(D3DPT_TRIANGLELIST,c.quad_count*2u,c.textured_batch_start,sizeof(Vertex28));
#endif
        c.textured_batch_start=c.textured_write;
        c.quad_count=0;
        ++c.draw_calls;
    }
}
std::int32_t create_render_target(TextureRecord& record,Device& device,UINT width,UINT height,D3DFORMAT backbuffer) {
    record.flags=(record.flags&~3u)|1u;
#ifdef TH_SDL3
    record.texture=device.create_texture(width,height,touhou::graphics::PixelFormat::Bgra8,true);
#else
    device.CreateTexture(width,height,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&record.texture,nullptr);
#endif
    // Original diagnostics on nonzero HRESULT resolve to verified no-op 0x40c6b0.
    // It still sets this field and returns zero even when CreateTexture fails.
    record.bytes_per_pixel=backbuffer==D3DFMT_A8R8G8B8?4u:2u;
    return 0;
}
std::uint32_t create_dynamic_texture(TextureRecord& record,Device& device,UINT width,UINT height,UINT format) {
    // Original table 0x56e208: UNKNOWN for 0, A8R8G8B8 for 1..8;
    // 0x56e22c has nine 4-byte entries. No pixel conversion is inferred here.
    if(format>8) throw std::out_of_range("Texture format exceeds original table");
    record.flags=(record.flags&~3u)|2u;
#ifdef TH_SDL3
    // D3DFMT_UNKNOWN resolves to the backbuffer format (A8R8G8B8) here.
    record.texture=device.create_texture(width,height,touhou::graphics::PixelFormat::Bgra8,false);
#else
    device.CreateTexture(width,height,1,D3DUSAGE_DYNAMIC,format?D3DFMT_A8R8G8B8:D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,&record.texture,nullptr);
#endif
    record.bytes_per_pixel=4;
    return width*height*4u; // x86 IMUL wraps modulo 2^32.
}
void release_device_textures(Controller& c) {
    for(auto* file:c.files) if(file) for(std::int32_t i=0;i<file->texture_count;++i) {
        auto& r=file->textures[i];
#ifdef TH_SDL3
        if((r.flags&3u)&&r.texture) { web::shared_device().release_texture(r.texture); }
#else
        if((r.flags&3u)&&r.texture) { r.texture->Release();r.texture=nullptr; }
#endif
    }
}
void recreate_device_textures(Controller& c,Device& device,D3DFORMAT backbuffer) {
    for(auto* file:c.files) if(file) for(std::int32_t i=0;i<file->texture_count;++i) {
        auto& r=file->textures[i];
        if((r.flags&3u)==1) create_render_target(r,device,r.header->width,r.header->height,backbuffer);
        else if((r.flags&3u)==2) create_dynamic_texture(r,device,r.header->width,r.header->height,r.header->format);
    }
}
}
