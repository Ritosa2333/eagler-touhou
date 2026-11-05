#include "graphics_callbacks.hpp"
#include "frame_statistics.hpp"
#include "data_constants.hpp"
#include "../runtime_core/worker.hpp"
#include "../runtime_state/state.hpp"
#include "../archive/resource_manager.hpp"
#ifdef TH_SDL3
#include "platform/Time.hpp"
#endif
#include <cstring>
#include <new>
#include <emmintrin.h>

namespace th20::source::audio {void update_stream(SoundInf&);}
namespace th20::source::platform_window {
namespace pe=program_entry;
void (*surface_callback_first)()=nullptr;
void (*surface_callback_second)()=nullptr;
namespace {
auto& graphics() {return pe::graphics_state;}
auto& controller() {return *pe::sprite_controller;}
auto& device() {return *graphics().device;}
void flush() {sprite::flush_textured_quads(controller(),device());}
#ifdef TH_SDL3
using namespace touhou::graphics;
web::Texture* surface(IUnknown* pointer) {return reinterpret_cast<web::Texture*>(pointer);}
void set_target(IUnknown* pointer) {device().set_render_target(surface(pointer));}
#else
IDirect3DSurface9* surface(IUnknown* pointer) {return static_cast<IDirect3DSurface9*>(pointer);}
void set_target(IUnknown* pointer) {device().SetRenderTarget(0,surface(pointer));}
#endif
float from_unsigned(std::uint32_t value) {
    auto converted=_mm_cvtsi32_sd(_mm_setzero_pd(),static_cast<std::int32_t>(value));
    if(value&0x80000000) converted=_mm_add_sd(converted,_mm_set_sd(4294967296.0));
    return _mm_cvtss_f32(_mm_cvtsd_ss(_mm_setzero_ps(),converted));
}
float add(float a,float b) {return _mm_cvtss_f32(_mm_add_ss(_mm_set_ss(a),_mm_set_ss(b)));}
#ifdef TH_SDL3
void alpha_test(bool value) {device().set_alpha_test(value);}
#else
void alpha_test(bool value) {device().SetRenderState(D3DRS_ALPHATESTENABLE,value);}
#endif
void clear(DWORD flags,DWORD color) {
#ifdef TH_SDL3
    device().clear(flags,color,1.0f,0);
#else
    device().Clear(0,nullptr,flags,color,1.0f,0);
#endif
}
int __cdecl begin_draw(void* self) { // 4dc5f0
    auto& g=*static_cast<GraphicsStatePrefix*>(self);auto& c=controller();
    if(!graphics().resource_019c) clear(3,graphics().clear_color);
    else {
        clear(3,0xffffffff);set_target(graphics().resource_019c);
        clear(3,graphics().clear_color);
    }
    c.field_e18=0;c.cached_texture=0xffffffff;c.unknown_cached_e0d=0xff;c.blend_mode=0x0b;
    c.field_e0f=0xff;c.unknown_cached_e10=0xff;
    c.field_7d40e90=0;c.field_7d40e8c=0x80808080;c.field_e12=0xff;c.field_00=0xff;
    c.fields_c8[1]=0;c.fields_c8[0]=0;c.unknown_cached_e0e=0xff;
    select_viewport(g,2);return 1;
}
int __cdecl composite_mask(void*) { // 4dc8b0
    auto& g=graphics();
    if(g.resource_019c) {
#ifdef TH_SDL3
        set_depth_compare(g,Compare::Always);flush();
        auto& p=device().pipeline();
        p.alphaTest=false;p.separateAlphaBlend=true;
        p.sourceBlend=BlendFactor::Zero;p.destinationBlend=BlendFactor::One;p.blendEquation=BlendEquation::Add;
        p.sourceBlendAlpha=BlendFactor::One;p.destinationBlendAlpha=BlendFactor::Zero;p.blendEquationAlpha=BlendEquation::Add;
#else
        set_render_state(g,D3DRS_ZFUNC,8);flush();
        const struct {D3DRENDERSTATETYPE state;DWORD value;} states[]={
            {D3DRS_ALPHATESTENABLE,0},{D3DRS_SEPARATEALPHABLENDENABLE,1},
            {D3DRS_SRCBLEND,1},{D3DRS_DESTBLEND,2},{D3DRS_BLENDOP,1},
            {D3DRS_SRCBLENDALPHA,2},{D3DRS_DESTBLENDALPHA,1},{D3DRS_BLENDOPALPHA,1}};
        for(auto pair:states) device().SetRenderState(pair.state,pair.value);
#endif
        sprite::Vertex20 vertices[4];for(auto& vertex:vertices) sprite::initialize_colored_vertex(vertex);
        const auto& rectangle=g.viewports[3].viewport;
        const float left=from_unsigned(rectangle.X),top=from_unsigned(rectangle.Y);
        const float right=add(from_unsigned(rectangle.Width),left),bottom=add(from_unsigned(rectangle.Height),top);
        vertices[0]={left,top,0.0f,1.0f,0xff000000};vertices[1]={right,top,0.0f,1.0f,0xff000000};
        vertices[2]={left,bottom,0.0f,1.0f,0xff000000};vertices[3]={right,bottom,0.0f,1.0f,0xff000000};
#ifdef TH_SDL3
        p.color.operation=p.alpha.operation=ColorOperation::First;
        p.color.first=p.alpha.first={ArgumentSource::Diffuse};
        device().vertex_format(VertexLayout::ScreenColor);
        device().draw(Topology::Strip,2,vertices,sizeof(sprite::Vertex20));
        p.alphaTest=true;
        p.color.operation=p.alpha.operation=ColorOperation::Multiply;
        p.color.first=p.alpha.first={ArgumentSource::Texture};
        p.sourceBlendAlpha=BlendFactor::SourceAlpha;p.destinationBlendAlpha=BlendFactor::One;p.blendEquationAlpha=BlendEquation::Add;
#else
        device().SetTextureStageState(0,D3DTSS_ALPHAOP,2);device().SetTextureStageState(0,D3DTSS_COLOROP,2);
        device().SetTextureStageState(0,D3DTSS_ALPHAARG1,0);device().SetTextureStageState(0,D3DTSS_COLORARG1,0);
        device().SetFVF(0x44);device().DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,vertices,sizeof(sprite::Vertex20));
        device().SetRenderState(D3DRS_ALPHATESTENABLE,1);
        device().SetTextureStageState(0,D3DTSS_ALPHAOP,4);device().SetTextureStageState(0,D3DTSS_COLOROP,4);
        device().SetTextureStageState(0,D3DTSS_ALPHAARG1,2);device().SetTextureStageState(0,D3DTSS_COLORARG1,2);
        device().SetRenderState(D3DRS_SRCBLENDALPHA,5);device().SetRenderState(D3DRS_DESTBLENDALPHA,2);device().SetRenderState(D3DRS_BLENDOPALPHA,1);
#endif
        controller().blend_mode=0x0b;
        flush();set_target(g.resource_01a0);clear(3,g.clear_color);
        // The original computes an unused rectangle before this full clear.
    }
    return 1;
}
int __cdecl composite_first(void*) { // 4d9120
    auto& g=graphics();
    if(g.resource_019c) {
        if(surface_callback_first) surface_callback_first();
        else {
#ifdef TH_SDL3
            device().set_blend_equation(BlendEquation::Add);
#else
            device().SetRenderState(D3DRS_BLENDOP,1);
#endif
            alpha_test(false);
            disable_depth_write(g);select_viewport(g,3);
            unrecovered::draw_animation(*g.surface_sprites[0]);g.surface_sprites[0]->base.field_490=0xffffffff;
            unrecovered::draw_animation_layer(controller(),0x29);flush();alpha_test(true);
        }
    }
    return 1;
}
void target_and_clear_depth(IUnknown* target,const D3DVIEWPORT9& viewport) {
    auto& g=graphics();flush();set_target(target);
#ifdef TH_SDL3
    std::int32_t rectangle[]{static_cast<LONG>(viewport.X),static_cast<LONG>(viewport.Y),
        static_cast<LONG>(viewport.X+viewport.Width),static_cast<LONG>(viewport.Y+viewport.Height)};
    device().clear(2,g.clear_color,1.0f,0,rectangle,1);
#else
    D3DRECT rectangle{static_cast<LONG>(viewport.X),static_cast<LONG>(viewport.Y),
        static_cast<LONG>(viewport.X+viewport.Width),static_cast<LONG>(viewport.Y+viewport.Height)};
    device().Clear(1,&rectangle,D3DCLEAR_ZBUFFER,g.clear_color,1.0f,0);
#endif
}
int __cdecl clear_first_depth(void*) { // 4dd1c0
    auto& g=graphics();if(g.resource_019c) target_and_clear_depth(g.resource_019c,g.viewports[3].viewport);return 1;
}
int __cdecl composite_second(void*) { // 4d8e90
    auto& g=graphics();
    if(g.resource_019c) {
        if(surface_callback_second) surface_callback_second();
        else {
#ifdef TH_SDL3
            device().set_blend_equation(BlendEquation::Add);
#else
            device().SetRenderState(D3DRS_BLENDOP,1);
#endif
            alpha_test(false);
            disable_depth_write(g);select_viewport(g,3);
            unrecovered::draw_animation(*g.surface_sprites[1]);g.surface_sprites[1]->base.field_490=0xffffffff;
            flush();alpha_test(true);
        }
    }
    return 1;
}
int __cdecl clear_second_depth(void*) { // 4dd2e0
    auto& g=graphics();if(g.resource_019c) target_and_clear_depth(g.resource_01a0,g.viewports[1].viewport);return 1;
}
int __cdecl composite_third(void*) { // 4d8f80: flush precedes marker reset in this layer
    auto& g=graphics();
    if(g.resource_019c) {
#ifdef TH_SDL3
        device().set_blend_equation(BlendEquation::Add);
#else
        device().SetRenderState(D3DRS_BLENDOP,1);
#endif
        alpha_test(false);disable_depth_write(g);
        unrecovered::draw_animation(*g.surface_sprites[2]);flush();g.surface_sprites[2]->base.field_490=0xffffffff;
        alpha_test(true);
    }
    return 1;
}
int __cdecl restore_backbuffer(void*) { // 4dd400
    flush();set_target(graphics().resource_01a4);clear(3,0xff000000);return 1;
}
int __cdecl composite_final(void*) { // 4d9040
    auto& g=graphics();
    if(g.resource_019c) {
        unrecovered::select_sprite_layer(controller(),0x1f,0);alpha_test(false);
        auto& animation=*g.surface_sprites[g.field_0b0c==7 || g.field_0b0c==12?3:4];
        unrecovered::draw_animation(animation);animation.base.field_490=0xffffffff;
        flush();alpha_test(true);
    }
    return 1;
}
int __cdecl end_draw(void*) { // 4dc7c0
    flush();for(unsigned point=0;point<3;++point) for(unsigned i:{3u,2u,0u,1u}) {
        graphics().viewports[i].points[point][0]=0.0f;graphics().viewports[i].points[point][1]=0.0f;
    }
    return 1;
}
}
void open_game_data() {
#ifdef TH_SDL3
    if(!resources::open(L"/game/th20.dat")) {runtime::log_error(pe::log_buffer,data::archive_open_failed);return;}
#else
    if(!resources::open(L"th20.dat")) {runtime::log_error(pe::log_buffer,data::archive_open_failed);return;}
#endif
    auto data_bytes=resources::read("th20_0100a.ver");auto& g=graphics();
    g.version_data_size=data_bytes?static_cast<std::uint32_t>(data_bytes->size()):0;
    g.dynamic_buffer=data_bytes?runtime::allocate_bytes(data_bytes->size()):nullptr;
    if(g.dynamic_buffer) std::memcpy(g.dynamic_buffer,data_bytes->data(),data_bytes->size());
    else runtime::log_error(pe::log_buffer,data::version_data_missing);
}
int __cdecl initialize_graphics_callbacks(void*) {
    open_game_data();state::set_clock_scale(1.0f);graphics().clear_color=0xff000000;
    initialize_render_viewports(graphics());center_render_viewports();
#ifdef TH_SDL3
    graphics().startup_seed=web::time::monotonic_milliseconds();
#else
    graphics().startup_seed=timeGetTime();
#endif
    state::seed(state::random_streams[0],graphics().startup_seed);state::seed(state::random_streams[1],graphics().startup_seed);
    state::seed(state::random_streams[2],graphics().startup_seed>>1);state::seed(state::random_streams[3],graphics().startup_seed/3);
    create_frame_statistics();th20::source::input::create_game_controller();
    unrecovered::initialize_sprite_assets(controller());
    for(auto*& animation:graphics().surface_sprites) animation=unrecovered::create_animation_vm();
    surface_callback_first=nullptr;surface_callback_second=nullptr;return 0;
}
int __cdecl update_graphics(void* self) {
    auto& g=*static_cast<GraphicsStatePrefix*>(self);
    auto& worker=*std::launder(reinterpret_cast<runtime::Worker*>(g.worker_storage[0]));
    if(((g.event_flags>>5)&3)==1 && !worker.thread.joinable()) graphics().field_0b0c=3;
    unrecovered::poll_background_jobs(pe::thread_registry);audio::update_stream(pe::thread_registry);
    th20::source::input::sample_game_frame();
    if(unrecovered::update_sprite_tasks(controller())!=0) return 4;
    if(pe::window_state.reset_delay) --pe::window_state.reset_delay;
    if(!g.field_0db0) return switch_scene(g);
    return g.field_0db0==2?4:1;
}
int register_graphics_callbacks() {
    auto& g=graphics();g.field_0b08=-2;g.field_0b0c=0;g.field_0b14=0;
    auto* update=scheduler::create(update_graphics);scheduler::set_userdata(*update,&g);
    scheduler::set_before_insert(*update,initialize_graphics_callbacks);scheduler::enable(*update);
    const int result=scheduler::insert(*pe::function_controller,pe::scheduler_environment,*update,1,false);
    if(result!=0) return result;
    const struct {int priority;scheduler::Callback callback;} callbacks[]={
        {1,begin_draw},{14,composite_mask},{15,composite_first},{25,clear_first_depth},{26,composite_second},
        {47,clear_second_depth},{48,composite_third},{66,restore_backbuffer},{67,composite_final},{110,end_draw}};
    for(auto callback:callbacks) scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,callback.priority,callback.callback,&g,true,true);
#ifdef TH_SDL3
    g.resource_01a4=reinterpret_cast<IUnknown*>(device().back_buffer());
#else
    device().GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,reinterpret_cast<IDirect3DSurface9**>(&g.resource_01a4));
#endif
    return 0;
}
}
#ifdef TH_SDL3
namespace th20::source::platform_window::unrecovered {
void pump_background_jobs(sprite::Controller& controller) {
    poll_background_jobs(pe::thread_registry);
    audio::update_stream(pe::thread_registry);
    update_sprite_tasks(controller);
    process_surface_copies(controller);
}
}
#endif
namespace th20::source::program_entry::unrecovered {
int fn_004de1f0() {return platform_window::register_graphics_callbacks();}
}
