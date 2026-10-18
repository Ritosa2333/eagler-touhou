#include "controller.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
namespace th20::source::program_entry {
SpriteController* sprite_controller=nullptr; // actual BSS ownership 0x5c0028
namespace unrecovered {
SpriteController* make_sprite_controller(AllocationController*,const char*) {
    return sprite::create_controller(*function_controller,scheduler_environment,*graphics_state.device);
}
void free_sprite_controller(AllocationController*,SpriteController* controller){sprite::destroy_controller(controller);}
}
}
