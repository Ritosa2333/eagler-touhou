#include "effect.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/anm_vm.hpp"
namespace th20::source::effects::environment {
sprite::Controller& sprites(){return *program_entry::sprite_controller;}
game_session::Context& context(std::int32_t index){return game_session::context(index);}
sprite::AnimationFile* load(std::int32_t index,const char* filename){return sprite::load_animation_file(sprites(),index,filename,program_entry::log_buffer,program_entry::graphics_event_flags);}
void unload(std::int32_t index){sprite::unload_animation_file(sprites(),index);}
void load_error(){runtime::log_printf(program_entry::log_buffer,"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");} //56f610 rawCP932
}
namespace th20::source::sprite::anm_environment {
void spawn_effect(Animation& animation,std::int32_t type){effects::controller(0)->spawn(type,&animation,&animation);}
}
