#include "stone.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../archive/resource_manager.hpp"
#include <stdexcept>
namespace th20::source::stone_menu {
namespace pe=program_entry;
namespace {
struct GameEnvironment final:Environment {
    void select_view(int view) override{pe::sprite_controller->field_6c4=view;} //4776a0
    sprite::AnimationFile* load_animation(int index,const char* name) override{return sprite::load_animation_file(*pe::sprite_controller,index,name,pe::log_buffer,pe::graphics_state.event_flags);}
    std::string text_resource(const char* name) override{const auto bytes=resources::read(name);if(!bytes)throw std::runtime_error("Missing required StoneMenu text resource");return std::string(reinterpret_cast<const char*>(bytes->data()),bytes->size());}
    void load_error() override{runtime::log_printf(pe::log_buffer,"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");}
    void interrupt(std::uint32_t handle,int event) override{sprite::interrupt_animation_children(*pe::sprite_controller,handle,event);}
    void request_delete(std::uint32_t& handle) override{sprite::request_animation_deletion(*pe::sprite_controller,handle);}
    void unload_animation(int index) override{sprite::unload_animation_file(*pe::sprite_controller,index);}
};
}
Environment& environment(){static GameEnvironment value;return value;}
}
