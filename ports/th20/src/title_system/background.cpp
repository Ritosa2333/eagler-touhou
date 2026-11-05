#include "background.hpp"
#include "data.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../ecl_vm/math.hpp"
namespace th20::source::title {
namespace pe=program_entry;
namespace {int __cdecl background_callback(sprite::Animation*){update_background(*controller());return 0;}} //51d170->51e0e0
void initialize_background(TitleInf& o){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(9));
    sprite::destroy_render_mesh(o.mesh);o.mesh=nullptr;o.mesh=nullptr;o.mesh=sprite::create_render_mesh(64,48,1,0);
    sprite::initialize_display_render_mesh(*o.mesh,0.f,0.f,static_cast<float>(pe::window_state.scaled_width),static_cast<float>(pe::window_state.scaled_height));
    o.angle=ecl::math::wrap_angle(0.f);auto* animation=sprite::resolve_animation_handle(*pe::sprite_controller,o.mesh->root_handle);if(!animation)animation=&pe::sprite_controller->animation_dc;animation->field_5dc=reinterpret_cast<unsigned>(&background_callback);
    o.color[0]=o.color[1]=o.color[2]=0xd0;o.color[3]=0xff;o.wave=data::f_005756a0;
}
void update_background(TitleInf& o){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(9));if(!o.mesh)return;
    sprite::initialize_display_render_mesh(*o.mesh,0.f,0.f,static_cast<float>(pe::window_state.scaled_width),static_cast<float>(pe::window_state.scaled_height));
    deform_background(o,state::random_streams[1]);sprite::update_render_mesh_strips(*o.mesh);
}
}
