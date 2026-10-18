#include "platform_window.hpp"
#include "directx_math.hpp"

namespace th20::source::platform_window {
namespace {
LONG coordinate(std::uint32_t value) {return static_cast<LONG>(value);}
}
void copy_render_surface(sprite::Controller& controller,const std::uint32_t (&request)[10]) { // 44bb10
    auto* texture=controller.files[request[0]]->textures[request[1]].texture;
    if(!texture) return;
    sprite::flush_textured_quads(controller,*program_entry::graphics_state.device);
    texture=controller.files[request[0]]->textures[request[1]].texture;
    RECT source_rect{coordinate(request[2]),coordinate(request[3]),coordinate(request[2]+request[4]),coordinate(request[3]+request[5])};
    RECT destination_rect{coordinate(request[6]),coordinate(request[7]),coordinate(request[6]+request[8]),coordinate(request[7]+request[9])};
#ifdef TH_SDL3
    auto* source=reinterpret_cast<DeviceTexture*>(program_entry::graphics_state.resource_01a4);
    if(directx::copy_surface(texture,destination_rect,source,source_rect)==0) {
        web::shared_device().add_dirty_rect(texture);
    }
#else
    IDirect3DSurface9* destination;
    if(texture->GetSurfaceLevel(0,&destination)==D3D_OK) {
        const auto source=static_cast<IDirect3DSurface9*>(program_entry::graphics_state.resource_01a4);
        if(directx::copy_surface(destination,destination_rect,source,source_rect)==D3D_OK) {
            controller.files[request[0]]->textures[request[1]].texture->AddDirtyRect(nullptr);
        }
        destination->Release();
    }
#endif
}
void process_surface_copies(sprite::Controller& controller) { // 41bfc0
    for(auto& request:controller.draw_state) {
        if(static_cast<std::int32_t>(request[0])>=0) {
            copy_render_surface(controller,request);request[0]=0xffffffff;
        }
    }
}
}
