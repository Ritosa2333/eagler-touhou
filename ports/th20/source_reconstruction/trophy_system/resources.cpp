#include "trophy.hpp"
#include "data_strings.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../archive/resource_manager.hpp"
#include <new>
namespace th20::source::trophy {
Message* messages=nullptr;
int initialize_resources(){
    auto& pe=program_entry::graphics_state;
    if(!sprite::load_animation_file(*program_entry::sprite_controller,20,"trophy.anm",program_entry::log_buffer,pe.event_flags)){
        runtime::log_error(program_entry::log_buffer,data::s_00571f5c);return -1;
    }
    //52d5c0 allocates128 records and425cc0 initializes only their firstword.
    messages=static_cast<Message*>(runtime::allocate_bytes(sizeof(Message)*128));if(!messages)throw std::bad_alloc();
    for(unsigned i=0;i<128;++i)messages[i].id=0;
    auto bytes=resources::read("trophy.txt",false);if(!bytes)throw std::runtime_error("Missing trophy.txt");
    parse_messages(std::span<Message,128>(messages,128),std::string_view(reinterpret_cast<const char*>(bytes->data()),bytes->size()));return 0;
}
void release_resources(){runtime::retire_callback_owner(controller);sprite::unload_animation_file(*program_entry::sprite_controller,20);runtime::release_bytes(messages);messages=nullptr;}
}
namespace th20::source::startup::unrecovered {
int initialize_resource_0052e210(){return trophy::initialize_resources();}
void release_resource_0052e730(){trophy::release_resources();}
}
