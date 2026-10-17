#include "controller.hpp"
#include "pool.hpp"
#include "dispatch.hpp"
#include "quad.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../runtime_core/worker.hpp"
#include <cstring>
#include <new>
namespace th20::source::sprite {
UncoloredVertex24 uncolored_quad[4]{};
namespace {
int __cdecl primary_update(void* self){ //450880
    if(controller_environment::suppress_primary_update())return 1;
    return update_animations(*static_cast<Controller*>(self),false);
}
int __cdecl secondary_update(void* self){return update_animations(*static_cast<Controller*>(self),true);} //4508c0
template<int Layer,bool Reset>int __cdecl draw_callback(void* self){
    if constexpr(Reset)dispatch_environment::controller().field_6c0=0xffffffffu; //44ee70
    return draw_animation_layer(*static_cast<Controller*>(self),Layer);
}
struct Registration {std::int32_t priority;scheduler::Callback callback;};
const Registration draw_callbacks[]={
#include "controller_callbacks.inc"
};
static_assert(std::size(draw_callbacks)==50);
}
void construct_controller(Controller& c,scheduler::State& callbacks,scheduler::Environment& environment,Device& device) {
    c.field_00=0;new(c.worker_storage) runtime::Worker;
    c.field_14=0;std::memset(c.draw_state,0,sizeof(c.draw_state));
    c.field_b8=c.field_bc=c.field_c0=c.draw_calls=0;std::memset(c.fields_c8,0,sizeof(c.fields_c8));
    construct_animation(c.animation_dc);c.field_6c0=c.field_6c4=0;
    for(auto& list:c.lists)initialize_animation_list(list);
    for(auto& pooled:c.pool)construct_pooled_animation(pooled);
    c.field_6000710=c.field_6000714=0;initialize_animation_link(c.free_sentinel,nullptr);c.field_600072c=0;
    std::memset(c.files,0,sizeof(c.files));std::memset(&c.matrix_60007d8,0,sizeof(c.matrix_60007d8));
    construct_animation(c.animation_6000818);
    c.field_e00=c.field_e04=c.cached_texture=0;
    c.blend_mode=c.unknown_cached_e0d=c.unknown_cached_e0e=c.field_e0f=c.unknown_cached_e10=0;
    c.field_e11=c.field_e12=c.field_e13=c.field_e14=0;c.field_e18=0;c.corner_buffer=nullptr;
    for(auto& vertex:c.corners)initialize_corner(vertex);
    c.quad_count=0;for(auto& vertex:c.textured_vertices)initialize_textured_vertex(vertex);
    c.textured_write=c.textured_batch_start=nullptr;c.colored_primitive_count=0;
    for(auto& vertex:c.colored_vertices)initialize_colored_vertex(vertex);
    c.colored_write=c.colored_batch_start=nullptr;c.field_7d40e88=c.field_7d40e8c=c.field_7d40e90=0;
    for(unsigned i=0;i<4;++i){
        uncolored_quad[i].rhw=animation_quad[i].rhw=1.f;
        uncolored_quad[i].u=animation_quad[i].u=(i&1)?1.f:0.f;
        uncolored_quad[i].v=animation_quad[i].v=(i&2)?1.f:0.f;
    }
    c.cached_texture=0xffffffffu;c.field_e04=1;c.unknown_cached_e10=0xff;
    for(auto& state:c.draw_state)state[0]=0xffffffffu;
    initialize_animation_link(c.free_sentinel,nullptr);
    for(std::uint32_t i=0;i<0xffff;++i){
        auto& pooled=c.pool[i];reset_animation_state(pooled.animation);clear_animation_suffix(pooled.animation);
        pooled.animation.index=i;pooled.active=0;pooled.index=i;
        initialize_animation_link(pooled.free_link,&pooled.animation);
        scheduler::insert_after(reinterpret_cast<scheduler::Link&>(c.free_sentinel),reinterpret_cast<scheduler::Link&>(pooled.free_link));
    }
    c.field_6000710=0;
    scheduler::register_callback(callbacks,environment,44,&primary_update,&c,false,true);
    scheduler::register_callback(callbacks,environment,14,&secondary_update,&c,false,true);
    for(const auto& entry:draw_callbacks)scheduler::register_callback(callbacks,environment,entry.priority,entry.callback,&c,true,true);
    #ifdef TH_SDL3
    c.field_6000714=0; // no programmable shader state exists on the semantic device
#else
    device.SetVertexShader(nullptr);c.field_6000714=0; // device vtable +0x170
#endif
}
Controller* create_controller(scheduler::State& callbacks,scheduler::Environment& environment,Device& device) {
    auto* storage=::operator new(sizeof(Controller));std::memset(storage,0,sizeof(Controller));
    auto* controller=new(storage)Controller;
    construct_controller(*controller,callbacks,environment,device);return controller;
}
void destroy_controller_contents(Controller& controller) {
    retire_animation_group(controller,controller.lists);
    destroy_animation_contents(controller.animation_6000818);
    for(std::size_t i=std::size(controller.pool);i>0;--i)destroy_animation_contents(controller.pool[i-1].animation);
    destroy_animation_contents(controller.animation_dc);
    reinterpret_cast<runtime::Worker*>(controller.worker_storage)->~Worker();
}
void destroy_controller(Controller* controller) {
    if(!controller)return;
    destroy_controller_contents(*controller);
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));
    ::operator delete(controller);
}
}
