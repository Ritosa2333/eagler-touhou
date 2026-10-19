#include "capture.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/directx_math.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
namespace th20::source::pause {
namespace pe=program_entry;
namespace {
std::uint32_t next_raw(state::Random& random){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(10));return random.last=recovered::lcg_next(random.state);} //449d20
void enqueue_copy(sprite::Controller& c,int file,int texture,int x,int y,int width,int height,int left,int top,int dest_width,int dest_height){ //4e6530
    for(auto& request:c.draw_state)if(static_cast<std::int32_t>(request[0])<0){const int values[]{file,texture,x,y,width,height,left,top,dest_width,dest_height};for(unsigned i=0;i<10;++i)request[i]=static_cast<std::uint32_t>(values[i]);return;}
}
}
void copy_background_texture(sprite::Controller& c,int file,int index,DeviceTexture* source,const RECT& destination_rect,const RECT& source_rect,int noise){
    auto* texture=c.files[file]->textures[index].texture;if(!texture)return;
    sprite::flush_textured_quads(c,*pe::graphics_state.device);texture=c.files[file]->textures[index].texture;
#ifdef TH_SDL3
    if(platform_window::directx::copy_surface(texture,destination_rect,source,source_rect)==0&&noise==1){
        auto& device=web::shared_device();
        const auto locked=device.lock(texture,reinterpret_cast<const std::int32_t*>(&destination_rect.left));
        if(!locked.pixels)return;
        auto* row=static_cast<std::uint8_t*>(locked.pixels);
#else
    IDirect3DSurface9* destination;
    if(texture->GetSurfaceLevel(0,&destination)!=D3D_OK)return;
    if(platform_window::directx::copy_surface(destination,destination_rect,source,source_rect)==D3D_OK&&noise==1){
        D3DLOCKED_RECT locked;if(destination->LockRect(&locked,&destination_rect,0)!=D3D_OK){destination->Release();return;}
        auto* row=static_cast<std::uint8_t*>(locked.pBits);
#endif
        // Original 44be40 uses width for the outer loop and height for the
        // inner loop. Preserve that traversal and the three raw RNG draws.
        for(int x=0;x<destination_rect.right-destination_rect.left;++x){
            auto* pixel=reinterpret_cast<std::uint32_t*>(row);
            for(int y=0;y<destination_rect.bottom-destination_rect.top;++y,++pixel){
                const auto color=*pixel;const auto green=(color>>8)&255u,blue=color&255u,red=(color>>16)&255u;
                auto* channels=reinterpret_cast<std::uint8_t*>(pixel);
                channels[1]=static_cast<std::uint8_t>(green-green*(next_raw(state::random_streams[1])&255u)/0x300u);
                channels[0]=static_cast<std::uint8_t>(blue-blue*(next_raw(state::random_streams[1])&255u)/0x300u);
                channels[2]=static_cast<std::uint8_t>(red-red*(next_raw(state::random_streams[1])&255u)/0x500u);channels[3]=255;
            }
            row+=static_cast<std::uint32_t>(locked.pitch)&~3u;
        }
#ifdef TH_SDL3
        device.unlock(texture);
#else
        destination->UnlockRect();
#endif
    }
#ifndef TH_SDL3
    destination->Release();
#endif
}
void capture_background(PauseInf& o){
    auto& c=*pe::sprite_controller;auto& g=pe::graphics_state;
    sprite::request_animation_deletion(c,o.background_handle);
    sprite::spawn_named_animation(c,*g.surface_animation,o.background_handle,"text",0x57,nullptr,0.f,-1,4);
    const auto& a=*sprite::resolve_animation_handle(c,o.background_handle);const auto& descriptor=sprite::current_sprite(c,a);
    const RECT destination{static_cast<LONG>(descriptor.left),static_cast<LONG>(descriptor.top),static_cast<LONG>((descriptor.left+descriptor.extent_4c)-1.f),static_cast<LONG>((descriptor.top+descriptor.extent_48)-1.f)};
    const auto& v=g.viewports[1].adjusted_viewport;const RECT source{static_cast<LONG>(v.X),static_cast<LONG>(v.Y),static_cast<LONG>(v.X+v.Width),static_cast<LONG>(v.Y+v.Height)};
#ifdef TH_SDL3
    copy_background_texture(c,static_cast<int>(g.surface_animation->id),static_cast<int>(descriptor.field_04),reinterpret_cast<DeviceTexture*>(g.resource_01a0),destination,source,1);
#else
    copy_background_texture(c,static_cast<int>(g.surface_animation->id),static_cast<int>(descriptor.field_04),static_cast<IDirect3DSurface9*>(g.resource_01a0),destination,source,1);
#endif
}
void capture_practice_background(PauseInf& o){
    auto& c=*pe::sprite_controller;auto& g=pe::graphics_state;
    sprite::spawn_named_animation(c,*g.surface_animation,o.background_handle,"text",0x57,nullptr,0.f,-1,4);
    const auto& a=*sprite::resolve_animation_handle(c,o.background_handle);const auto& descriptor=sprite::current_sprite(c,a);const float scale=pe::window_state.scale;
    enqueue_copy(c,static_cast<int>(a.base.fields_10_28[3]),static_cast<int>(descriptor.field_04),static_cast<int>(32.f*scale),static_cast<int>(16.f*scale),static_cast<int>(384.f*scale),static_cast<int>(448.f*scale),static_cast<int>(descriptor.left),static_cast<int>(descriptor.top),static_cast<int>(descriptor.extent_4c),static_cast<int>(descriptor.extent_48));
}
void show_animation_tree(sprite::Animation& a){
    a.base.flags[1]|=1u;auto* first=a.links[3].next;if(first){scheduler::Iterator iterator(reinterpret_cast<scheduler::Link*>(first));while(iterator.current){show_animation_tree(*reinterpret_cast<sprite::Animation*>(iterator.current->value));iterator.advance();}}
}
void show_animation(std::uint32_t handle,bool visible){if(auto* a=sprite::find_animation(*pe::sprite_controller,handle)){if(visible)show_animation_tree(*a);else sprite::hide_animation_tree(*a);}}
}
