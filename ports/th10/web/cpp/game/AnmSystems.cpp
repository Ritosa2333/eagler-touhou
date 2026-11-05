#include "AnmSystems.hpp"
namespace th10 {
// 0x445900. Original embedded constructors run before this full clear and
// allocate nothing. The final state and registrations are reproduced directly.
void AnmManager::initialize(AnmSystemEnvironment& env){
    std::memset(this,0,sizeof(*this));const Vec2 uv[]={{0,0},{1,0},{0,1},{1,1}};
    for(u32 i=0;i<4;++i){env.initial_quad[i].attribute=0x3f800000;env.initial_quad[i].uv=uv[i];env.render_quad[i].reciprocal_w=1;env.render_quad[i].uv=uv[i];}
    current_material_color=1;cached_draw_state[4]=255;const i32 invalid[]={-1,-1};std::memcpy(header,invalid,sizeof(invalid));for(auto& vm:pool)vm.initialize();
    constexpr i32 priorities[]={26,8,9,11,13,15,16,17,18,19,21,23,24,26,28,33,36,41,42,45};
    for(u32 i=0;i<20;++i)env.chain->add(env.frame_callbacks[i],this,priorities[i],i>=2,true,*env.callbacks);env.clear_pixel_shader();
}
// 0x446220. The global chain is cleared by the outer shutdown sequence. This
// destructor removes instances, then destroys layer, resource and pool geometry.
void AnmManager::release(AnmAllocationEnvironment& env){
    auto* node=registry.world_head;while(node){auto* vm=node->value;node=node->next;remove(*vm,env);}node=registry.ui_head;while(node){auto* vm=node->value;node=node->next;remove(*vm,env);}
    const auto destroy=[&](AnmVm& vm){if(vm.geometry)env.release_memory(vm.geometry);vm.geometry=nullptr;};for(i32 i=19;i>=0;--i)destroy(draw_layers[i]);destroy(resource_animation);for(i32 i=4095;i>=0;--i)destroy(pool[i]);
}
// 0x4462f0. Model vertices are uploaded as XYZ+UV (20 bytes), while the global
// 24-byte copy preserves its existing fourth attribute.
void AnmManager::initialize_model(AnmSystemEnvironment& env){
    const AnmModelVertex vertices[]={{{-128,-128,0},{0,0}},{{128,-128,0},{1,0}},{{-128,128,0},{0,1}},{{128,128,0},{1,1}}};std::memcpy(model_vertices,vertices,sizeof(vertices));
    for(u32 i=0;i<4;++i){env.model_quad[i].position=vertices[i].position;env.model_quad[i].uv=vertices[i].uv;}
    env.create_model_buffer(model_vertex_buffer);auto* bytes=env.lock_model_buffer(model_vertex_buffer);std::memcpy(bytes,model_vertices,sizeof(model_vertices));env.unlock_model_buffer(model_vertex_buffer);env.bind_model_buffer(model_vertex_buffer);
}
}
