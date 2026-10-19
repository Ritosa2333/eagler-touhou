#include "anm_vm.hpp"
#include "binding.hpp"
#include "pool.hpp"
#include "quad.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::sprite::anm_environment {
namespace pe=th20::source::program_entry;
namespace st=th20::source::state;
namespace unrecovered {
AnimationFile& text_animation_file(); //437950 reads actual TextRenderer5c0698, not a substitute resource
}
float& clock_scale(){return st::clock_scale;}
const float* timer_rate(){return st::timer_rate;}
AnmInstruction* script(Animation& animation){return script_start(*pe::sprite_controller,animation);}
std::uint32_t random_next(){return st::next(st::random_streams[1]);}
std::uint32_t random_bounded(std::uint32_t count){return st::bounded(st::random_streams[1],count);}
float random_unit(){return st::unit(st::random_streams[1]);}
float random_signed_unit(){return st::signed_unit(st::random_streams[1]);}
float camera_component(std::int32_t variable){
    auto& camera=pe::graphics_state.viewports[3];const auto index=variable-10016;
    if(index<0||index>5)throw std::out_of_range("ANM camera variable outside original switch");
    return index<3?th20::recovered::add32(camera.vectors[0][index],camera.vectors[5][index]):camera.vectors[3][index-3];
}
void add_camera_offset(Vec3& value){
    auto* offset=pe::graphics_state.viewports[3].final_vector;
    value.x=th20::recovered::add32(value.x,offset[0]);value.y=th20::recovered::add32(value.y,offset[1]);value.z=th20::recovered::add32(value.z,offset[2]);
}
void assign_sprite(Animation& animation,std::int32_t index){
    if(index<0)assign_animation_sprite(unrecovered::text_animation_file(),animation,0x120);
    else assign_animation_sprite(script_file(*pe::sprite_controller,animation),animation,index);
}
void calculate_corners(Animation& animation,Vec3 (&corners)[4]){calculate_animation_corners(animation,corners);}
void set_layer(Animation& animation,std::int32_t layer){set_animation_layer(animation,layer);}
float screen_scale(){return pe::window_state.scale;}
std::int32_t screen_offset(unsigned preset,unsigned axis){
    if(preset==0)return axis?pe::window_state.field_005c:pe::window_state.field_0058;
    return axis?pe::window_state.field_0064:pe::window_state.field_0060;
}
void* allocate_geometry(std::uint32_t bytes){return runtime::allocate_bytes(bytes);}
std::uint32_t spawn_child(Animation& animation,std::int32_t script,std::uint32_t flags){
    return spawn_child_animation(*pe::sprite_controller,script_file(*pe::sprite_controller,animation),animation,script,flags);
}
std::uint32_t spawn_detached(Animation& animation,std::int32_t script,std::uint32_t flags){
    return spawn_detached_animation(*pe::sprite_controller,script_file(*pe::sprite_controller,animation),animation,script,flags);
}
Animation& lookup_animation(std::uint32_t handle){return *resolve_animation_handle(*pe::sprite_controller,handle);}
}
