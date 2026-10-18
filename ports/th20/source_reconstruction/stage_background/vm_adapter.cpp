#include "vm.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include <bit>
namespace th20::source::background {
namespace vm_environment {
void set_clear_color(std::uint32_t color){program_entry::graphics_state.clear_color=color;}
void assign_animation(ScriptState& s,int index,int script){auto& animation=s.animations[index];sprite::destroy_animation_contents(animation);sprite::bind_animation_script(*s.owner->animation_file,animation,script,nullptr);}
void reset_meshes(ScriptState& s){
    for(int i=0;i<2;++i){
        sprite::destroy_render_mesh(reinterpret_cast<sprite::RenderMesh*>(s.fields_3294[9+i]));s.fields_3294[9+i]=0;
        s.fields_3294[11+i]=std::bit_cast<std::uint32_t>(112.f);s.fields_3294[13+i]=std::bit_cast<std::uint32_t>(192.f);s.fields_3294[15+i]=0xffffffff;
        s.mesh_phase_x[i]=s.mesh_phase_y[i]=0.f;recovered::timer_set(s.mesh_timers[i],0);program_entry::sprite_controller->field_6c4=i;
        s.fields_3294[9+i]=reinterpret_cast<std::uint32_t>(sprite::create_render_mesh(17,s.mesh_mode==1?7:17,0,i));
    }
}
void interrupt_animations(Background& background,std::uint32_t interrupt){
    auto run=[&](sprite::Animation& animation){animation.base.field_438=interrupt;sprite::execute_animation(animation);};
    if(background.primitive_animations)for(int i=0;i<background.file->animation_count;++i)run(background.primitive_animations[i]);
    for(auto& animation:background.state.animations)run(animation);
}
void set_primary_rotation_flag(std::uint32_t value){primary->state_flags=(primary->state_flags&~16u)|((value&1u)<<4);}
}
namespace unrecovered {
void destroy_render_mesh(void* mesh){sprite::destroy_render_mesh(static_cast<sprite::RenderMesh*>(mesh));}
}
}
