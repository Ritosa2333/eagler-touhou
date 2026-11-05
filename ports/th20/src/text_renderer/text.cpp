#include "text.hpp"
#include "deferred_queue.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/platform_window.hpp"
#include "../platform_window/fonts.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/quad.hpp"
#include <algorithm>
#include <cstring>
#include <new>
namespace th20::source::text {
namespace pe=program_entry;namespace pw=platform_window;namespace s=sprite;namespace n=th20::recovered;
Renderer* renderer=nullptr;
Renderer::Renderer():line_count(0),color(0xffffffffu),shadow_color(0xff000000u),field_1a1c8(0xffffffffu),scale_x(1.f),scale_y(1.f),
    fields_1a1d4{0,0,0,0,2,0,0,0,1,1,0},texture_width(0),texture_height(0),rotation(0),font_width(9),frame_count(0),animation_file(nullptr),
    animation_handle(0),loading_handle(0),additional_draw_nodes{},text_buffer{} {renderer=this;}
Renderer::~Renderer() {
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    for(auto* node:additional_draw_nodes)scheduler::remove(*pe::function_controller,pe::scheduler_environment,node);
    runtime::join_worker(worker);s::unload_animation_file(*pe::sprite_controller,2);s::unload_animation_file(*pe::sprite_controller,0);renderer=nullptr;
    for(scheduler::Iterator it(jobs.sentinel.next);it.current;it.advance())destroy_job(reinterpret_cast<Job*>(it.current->value));
#ifndef TH_SDL3
    for(auto font:pw::fonts)DeleteObject(font);
#endif
    // TH_SDL3 font faces are owned by the FreeType font host.
    // Member destructors execute list -> Worker -> animations[2..0], exactly
    // matching the original. OwnedAnimation supplies the actual ANM cleanup.
}
void Renderer::enable_callbacks() {
    scheduler::enable(*update_node);scheduler::enable(*draw_node);for(auto* node:additional_draw_nodes)scheduler::enable(*node);
}
namespace {
int __cdecl update_callback(void* self){return static_cast<Renderer*>(self)->update();}
int __cdecl draw0(void* self){return static_cast<Renderer*>(self)->draw_layer(0);}
int __cdecl draw1(void* self){return static_cast<Renderer*>(self)->draw_primary_layer();}
int __cdecl draw2(void* self){s::dispatch_environment::select_layer_camera(2);const auto result=static_cast<Renderer*>(self)->draw_layer(2);pw::select_viewport(pe::graphics_state,2);return result;}
int __cdecl draw3(void* self){pw::select_viewport(pe::graphics_state,2);return static_cast<Renderer*>(self)->draw_layer(3);}
int __cdecl draw4(void* self){pw::select_viewport(pe::graphics_state,2);return static_cast<Renderer*>(self)->draw_layer(4);}
void set_rotation(s::Animation& a,float value){a.base.vector_38.z=value;a.base.flags[1]|=2;}
void draw_job_sprite(s::Animation& a,float angle){if(angle==0.f)s::draw_axis_aligned_sprite(*pe::sprite_controller,a,false);else s::draw_rotated_sprite(*pe::sprite_controller,a);}
}
int Renderer::initialize() {
#ifdef TH_SDL3
    const char* name="ascii1280.anm";
#else
    const auto scale=pe::window_state.scale;const char* name=scale>1.1f?(scale>1.6f?"ascii1280.anm":"ascii_960.anm"):"ascii.anm";
#endif
    animation_file=s::load_animation_file(*pe::sprite_controller,2,name,pe::log_buffer,pe::graphics_state.event_flags);
    if(!animation_file){runtime::log_printf(pe::log_buffer,"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");return -1;}
#ifdef TH_SDL3
    // Only the first atlas differs between the ASCII resolution variants.
    // Its texels and UVs stay native; its layout metrics use the game's logical
    // units. The remaining atlases already contain resolution-independent data.
    for(std::uint32_t i=0;i<animation_file->sprite_count;++i) {
        auto& glyph=animation_file->sprites[i];
        if(glyph.field_04!=0)continue;
        glyph.scale_50*=2.f;glyph.scale_54*=2.f;
        glyph.extent_4c*=.5f;glyph.extent_48*=.5f;
    }
#endif
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,8,update_callback,this,false,false);
    draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,0x66,draw0,this,true,false);
    constexpr int priorities[]{0x3d,0x54,0x4b,0x55};constexpr scheduler::Callback callbacks[]{draw1,draw2,draw3,draw4};
    for(unsigned i=0;i<4;++i)additional_draw_nodes[i]=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,priorities[i],callbacks[i],this,true,false);
    initialize_animation_sprite(*animation_file,animations[0],0);initialize_animation_sprite(*animation_file,animations[1],0x62);
    texture_width=texture_height=0x800;return 0;
}
Renderer* create_renderer() {
    auto* memory=::operator new(sizeof(Renderer),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(Renderer));Renderer* object;
    try{object=::new(memory)Renderer;}catch(...){::operator delete(memory);throw;}
    if(object->initialize()!=0){runtime::retire_callback_owner(object);return nullptr;}return object;
}
int Renderer::update() {
    compact_lines();while(!pending_tasks.empty()){std::function<void()> function=pending_tasks.front();pending_tasks.pop_front();function();}
    process_one_deferred_task();++frame_count;return 1; //explicitmov eax,1 at46b7c5
}
int Renderer::draw_primary_layer() {
    pw::select_viewport(pe::graphics_state,0);s::flush_textured_quads(*pe::sprite_controller,*pe::graphics_state.device);
    animations[0].base.flags[2]=(animations[0].base.flags[2]&~0x03000000u)|0x02000000u;
    draw_layer(1);animations[0].base.flags[2]&=~0x03000000u;
    s::flush_textured_quads(*pe::sprite_controller,*pe::graphics_state.device);pw::select_viewport(pe::graphics_state,2);return 1;
}
int Renderer::draw_layer(std::int32_t layer) {
    const auto count=std::min(line_count,320);for(int i=0;i<count;++i)if(lines[i].layer==layer)draw_line(lines[i]);
    draw_jobs(layer);s::flush_textured_quads(*pe::sprite_controller,*pe::graphics_state.device);pw::select_viewport(pe::graphics_state,2);return 1; //46bb75
}
void Renderer::draw_jobs(std::int32_t layer) {
    std::lock_guard lock(runtime::shared_locks().slot(18));
    for(scheduler::Iterator it(jobs.sentinel.next);it.current;it.advance()) {
        auto& job=*reinterpret_cast<Job*>(it.current->value);if(job.layer!=layer)continue;
        if(job.frames<1){if(!job.ready)job.canceled.store(true);else destroy_job(&job);continue;}
        if((job.ready && !job.external_ready)||(job.external_ready && *job.external_ready)) {
            auto& a=job.animation;const auto scale=pe::window_state.scale;
            a.base.vector_50={n::mul32(n::mul32(job.scale_x,scale),.5f),n::mul32(n::mul32(job.scale_y,scale),.5f)};a.base.flags[1]|=4;
            a.base.flags[4]=job.align_x;a.base.flags[5]=job.align_y;set_rotation(a,job.rotation);
            if(job.shadow){a.vector_5bc={n::add32(n::mul32(2.f,scale),job.position.x),n::add32(n::mul32(2.f,scale),job.position.y),job.position.z};s::set_animation_color(a,0xc0000000u);draw_job_sprite(a,job.rotation);}
            a.vector_5bc=job.position;s::set_animation_color(a,job.color);draw_job_sprite(a,job.rotation);
        }
        --job.frames;
    }
}
void Renderer::create_loading_text(float x,float y) {
#ifdef TH_SDL3
    // Browser resource mounts and the title loader complete before the next
    // presented scene; this ANM can survive the transition in the web host.
    (void)x; (void)y;
#else
    s::Vec3 position{n::mul32(x,2.f),n::mul32(y,2.f),0};
    if(loading_handle==0)s::spawn_named_animation(*pe::sprite_controller,*animation_file,loading_handle,nullptr,0x11,&position,0.f,-1,0,nullptr);
#endif
}
}
namespace th20::source::sprite::anm_environment::unrecovered {
AnimationFile& text_animation_file(){return *text::renderer->animation_file;}
}
namespace th20::source::gameplay::unrecovered {
void screen_transition(float x,float y){text::renderer->create_loading_text(x,y);}
}
