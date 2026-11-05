#include "platform_window.hpp"
#include "data_constants.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
#include <cstring>

namespace th20::source::platform_window {
namespace pe=program_entry;
int create_direct3d() {
    auto& g=pe::graphics_state;auto& w=pe::window_state;
#ifdef TH_SDL3
    // The wasm host owns the single GL context and a 640x480 canvas; the
    // semantic device is a process singleton bound to the graphics state.
    w.display_width=640;w.display_height=480;
    w.client_width=w.display_width;w.client_height=w.display_height;
    g.device=&web::shared_device();
    return 0;
#else
    g.direct3d=Direct3DCreate9(D3D_SDK_VERSION);
    if(!g.direct3d) {pe::unrecovered::log_error(pe::log_buffer,data::direct3d_failed);return 1;}
    DEVMODEW mode{};mode.dmPelsWidth=0;
    EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&mode);
    w.display_width=static_cast<int>(mode.dmPelsWidth);w.display_height=static_cast<int>(mode.dmPelsHeight);
    w.client_width=w.display_width;w.client_height=w.display_height;
    return 0; // 0x0041c3c4
#endif
}
#ifndef TH_SDL3
bool try_create_device(D3DPRESENT_PARAMETERS& pp) {
    auto& g=pe::graphics_state;
    pe::graphics_event_flags&=~1u;
    // 0x0041d0a0 reads graphics+0x24c = configuration.flags, bit1.
    if(!(g.configuration.flags&2)) {
        if(g.direct3d->CreateDevice(0,D3DDEVTYPE_HAL,pe::window_state.window,
            D3DCREATE_HARDWARE_VERTEXPROCESSING,&pp,&g.device)==D3D_OK) {
            pe::graphics_event_flags|=1;return false;
        }
        if(g.direct3d->CreateDevice(0,D3DDEVTYPE_HAL,pe::window_state.window,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING,&pp,&g.device)==D3D_OK) return false;
    }
    return g.direct3d->CreateDevice(0,D3DDEVTYPE_REF,pe::window_state.window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,&pp,&g.device)!=D3D_OK;
}
#endif
int create_or_reset_backbuffer(int reset) {
    auto& g=pe::graphics_state;auto& w=pe::window_state;
    auto pp=g.presentation;
    if(g.configuration.presentation_mode==3) {w.flags&=~4u;pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;}
    else pp.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
    for(int attempt=0;attempt<2;++attempt) {
        unsigned candidate=0;
        for(;candidate<16;++candidate) {
            if(candidate==0 && (w.display_mode==8 || w.display_mode==9)) {
                pp.BackBufferWidth=w.viewport_width;pp.BackBufferHeight=w.viewport_height;
            } else {
                const int width=data::resolution_candidates[candidate*2];
                const int height=data::resolution_candidates[candidate*2+1];
                if(width<w.scaled_width || height<w.scaled_height) continue;
                pp.BackBufferWidth=width;pp.BackBufferHeight=height;
                w.viewport_width=width;w.viewport_height=height;
            }
#ifdef TH_SDL3
            // The semantic device accepts the negotiated size directly; canvas
            // refresh is 60 Hz, so the original refresh-driven flag updates
            // (60 Hz branch) leave w.flags untouched.
            const bool created=!reset
                ?g.device->create(pp.BackBufferWidth,pp.BackBufferHeight,touhou::graphics::PixelFormat::Bgra8,pp.PresentationInterval)
                :g.device->reset_backbuffer(pp.BackBufferWidth,pp.BackBufferHeight,touhou::graphics::PixelFormat::Bgra8,pp.PresentationInterval);
            if(!created) continue;
#else
            if(!reset) {
                if(try_create_device(pp)) {pe::release_device(g);continue;}
            } else if(g.device->Reset(&pp)!=D3D_OK) continue;
            // Original observes refresh twice, including this ignored first read.
            HDC dc=GetDC(w.window);GetDeviceCaps(dc,VREFRESH);ReleaseDC(w.window,dc);
#endif
            break;
        }
        if(candidate==16) continue;
#ifndef TH_SDL3
        HDC dc=GetDC(w.window);const int refresh=GetDeviceCaps(dc,VREFRESH);ReleaseDC(w.window,dc);
        if(refresh!=60) {w.flags&=~4u;pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;}
#endif
        g.presentation=pp;
#ifdef TH_SDL3
        pe::unrecovered::log_append(pe::log_buffer,data::backbuffer_size,pp.BackBufferWidth,pp.BackBufferHeight);
#else
        // Original returns success even if this second Reset fails (0x41c9b9).
        if(g.device->Reset(&pp)==D3D_OK)
            pe::unrecovered::log_append(pe::log_buffer,data::backbuffer_size,pp.BackBufferWidth,pp.BackBufferHeight);
#endif
        return 0;
    }
    if(!reset) {
        pe::unrecovered::log_error(pe::log_buffer,data::device_failed);
#ifndef TH_SDL3
        pe::release_direct3d(g); // 0x41c91b calls 0x41a200, NOT release_device.
#endif
    }
    return -1;
}

int prepare_presentation() {
    auto& g=pe::graphics_state;auto& w=pe::window_state;auto& c=g.configuration;
    D3DPRESENT_PARAMETERS pp{};
#ifdef TH_SDL3
    // Host display mode: the SDL canvas presents 640x480@60 in A8R8G8B8.
    D3DDISPLAYMODE mode{640,480,60,D3DFMT_A8R8G8B8};
#else
    D3DDISPLAYMODE mode;
    g.direct3d->GetAdapterDisplayMode(0,&mode);
#endif
    if(mode.Format==D3DFMT_X8R8G8B8) mode.Format=D3DFMT_A8R8G8B8;
    g.adapter_width=mode.Width;g.adapter_height=mode.Height;
    g.adapter_refresh_rate=mode.RefreshRate;g.previous_backbuffer_format=mode.Format;
    if(g.presentation.Windowed && mode.RefreshRate!=60) w.flags&=~4u;
    if(w.unknown_0088) g.disable_vsync=1;
    if(!g.presentation.Windowed) {
        // 0x00412540 has a proven constant-zero body, so its ==1 branch is unreachable.
        if(c.alternate_pixel_format==255) {
            pp.BackBufferFormat=D3DFMT_A8R8G8B8;c.alternate_pixel_format=0;
            pe::unrecovered::log_append(pe::log_buffer,data::format_default);
        } else pp.BackBufferFormat=c.alternate_pixel_format==0?D3DFMT_A8R8G8B8:D3DFMT_R5G6B5;
        if(!g.disable_vsync)
            pp.PresentationInterval=!(w.flags&4) && c.presentation_mode==3?D3DPRESENT_INTERVAL_IMMEDIATE:D3DPRESENT_INTERVAL_ONE;
        else {pp.FullScreen_RefreshRateInHz=0;pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;}
    } else {
        pp.BackBufferFormat=mode.Format;
        if(mode.Format==D3DFMT_X8R8G8B8) pp.BackBufferFormat=D3DFMT_A8R8G8B8;
        if(c.presentation_mode==3 || mode.RefreshRate!=60) {pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;w.flags&=~4u;}
        else if(w.flags&4) pp.PresentationInterval=D3DPRESENT_INTERVAL_ONE;
        pp.Windowed=TRUE;
    }
    pp.SwapEffect=D3DSWAPEFFECT_DISCARD;pp.EnableAutoDepthStencil=TRUE;
    pp.AutoDepthStencilFormat=D3DFMT_D16;pp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    pe::graphics_event_flags|=2;g.graphics_ready=1;g.presentation=pp;
    if(create_or_reset_backbuffer(0)!=0) return 1; // 0x41c613
#ifdef TH_SDL3
    g.device_caps.TextureOpCaps|=0x40; // D3DTEXOPCAPS_MULTIPLYADD
    g.device_caps.MaxTextureWidth=g.device->max_texture_width();
    const bool texture_format_ok=g.device->supports_texture_format(touhou::graphics::PixelFormat::Bgra8);
#else
    g.device->GetDeviceCaps(&g.device_caps);
    const bool texture_format_ok=
        g.direct3d->CheckDeviceFormat(0,D3DDEVTYPE_HAL,pp.BackBufferFormat,0,D3DRTYPE_TEXTURE,D3DFMT_A8R8G8B8)==D3D_OK;
#endif
    static_assert(offsetof(D3DCAPS9,TextureOpCaps)==0x90);
    static_assert(offsetof(D3DCAPS9,MaxTextureWidth)==0x58);
    if(!(g.device_caps.TextureOpCaps&0x40)) pe::unrecovered::log_append(pe::log_buffer,data::missing_texture_cap);
    if(g.device_caps.MaxTextureWidth<257) pe::unrecovered::log_append(pe::log_buffer,data::small_texture);
    if(texture_format_ok)
        pe::graphics_event_flags|=4;
    else {
        pe::graphics_event_flags&=~4u;c.flags|=1;
        pe::unrecovered::log_append(pe::log_buffer,data::format_unsupported);
    }
    w.quit_requested=0;initialize_render_state();g.render_counter=0;
    return 0;
}
}
namespace th20::source::program_entry::unrecovered {
int fn_0041c320() {return platform_window::create_direct3d();}
int fn_0041c3e0(WindowStatePrefix&) {return platform_window::prepare_presentation();}
int fn_0041c730(int reset) {return platform_window::create_or_reset_backbuffer(reset);}
}
