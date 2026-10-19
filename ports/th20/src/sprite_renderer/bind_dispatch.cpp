#include "binding.hpp"
#include "anm_vm.hpp"
namespace th20::source::sprite {
void bind_animation_script(AnimationFile& file,Animation& animation,std::int32_t index,Animation* parent) {
    select_animation_template(file,animation,index);
    attach_animation_parent(animation,parent);
    execute_animation(animation);
}
}
