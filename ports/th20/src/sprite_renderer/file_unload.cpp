#include "animation_file.hpp"
namespace th20::source::sprite {
void unload_animation_file(Controller& controller,std::int32_t index) {
    if(index<0||index>=42||!controller.files[index])return;
    clear_animation_file(controller,*controller.files[index]);
    destroy_animation_file(controller,controller.files[index]);controller.files[index]=nullptr;
}
}
