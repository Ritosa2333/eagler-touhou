#include "platform_window.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"

namespace th20::source::platform_window {
void release_render_surfaces() {
    // This routine uses the global object, not the incoming ECX.
    auto& g=program_entry::graphics_state;
#ifdef TH_SDL3
    // The wasm host owns the backbuffer and the sprite module owns the render
    // target textures; dropping the references is the whole release.
    g.resource_019c=nullptr;g.resource_01a0=nullptr;g.resource_01a4=nullptr;
#else
    if(g.resource_019c) {g.resource_019c->Release();g.resource_019c=nullptr;}
    if(g.resource_01a0) {g.resource_01a0->Release();g.resource_01a0=nullptr;}
    if(g.resource_01a4) {g.resource_01a4->Release();g.resource_01a4=nullptr;}
    g.resource_019c=nullptr;
#endif
}
void acquire_render_surfaces(GraphicsStatePrefix& g) {
    if(g.resource_019c) {
        // 0x4dbfd? REP MOVSD, exact 0x16c viewport record copy.
        g.viewports[3]=g.viewports[0];return;
    }
#ifdef TH_SDL3
    g.resource_01a4=reinterpret_cast<IUnknown*>(g.device->back_buffer());
    if(!g.resource_01a4) return;
    g.resource_019c=reinterpret_cast<IUnknown*>(g.surface_animation->textures[0].texture);
    g.resource_01a0=reinterpret_cast<IUnknown*>(g.surface_animation->textures[1].texture);
#else
    if(!g.resource_01a4 && g.device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,
        reinterpret_cast<IDirect3DSurface9**>(&g.resource_01a4))!=D3D_OK) return;
    g.surface_animation->textures[0].texture->GetSurfaceLevel(0,reinterpret_cast<IDirect3DSurface9**>(&g.resource_019c));
    g.surface_animation->textures[1].texture->GetSurfaceLevel(0,reinterpret_cast<IDirect3DSurface9**>(&g.resource_01a0));
#endif
    // 445ab0 reads VM+49a bit0; 4dda30 clears VM+4a0 bits2..3.
    if(!(g.surface_sprites[0]->base.flags[0]&(1u<<16))) {
        const auto width=program_entry::window_state.scaled_width;
        int variant=-1;
        if(width==640) variant=0;else if(width==960) variant=1;else if(width==1280) variant=2;
        if(variant>=0) {
            const int scripts[5]={variant,variant+7,variant+4,variant+10,variant+13};
            for(int i=0;i<5;++i) unrecovered::bind_animation_script(*g.surface_animation,g.surface_sprites[i],scripts[i],0);
        }
    }
    if(program_entry::window_state.scale==1.5f) g.surface_sprites[2]->base.flags[2]&=~0xcu;
}
}
namespace th20::source::program_entry::unrecovered {
void fn_004dd840(GraphicsStatePrefix&) {platform_window::release_render_surfaces();}
void fn_004dbd70(GraphicsStatePrefix& g) {platform_window::acquire_render_surfaces(g);}
}
