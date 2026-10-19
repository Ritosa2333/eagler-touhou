#include "loading_interrupt.hpp"
#include "../core_scheduler/scheduler.hpp"
namespace th20::source::sprite {
void set_animation_interrupt(Animation& animation,std::int32_t event) {
    if(auto* callback=reinterpret_cast<AnimationCallback*>(animation.fields_550[6]))callback->interrupt(event);
    animation.base.field_438=static_cast<std::uint32_t>(event);
}
void interrupt_animation_children(Controller& controller,std::uint32_t handle,std::int32_t event) {
    auto* animation=find_animation(controller,handle);
    if(!animation)return;
    auto* first=animation->links[3].value?&animation->links[3]:nullptr;
    scheduler::Iterator iterator(reinterpret_cast<scheduler::Link*>(first));
    // 44b9f0 constructs a temporary4119b0 iterator, copies its two pointers to
    // the return object, then destroys the temporary411b00. That clears the
    // initial observer links. advance() reattaches later next nodes normally.
    // Do not replace this with the usual always-observed list traversal.
    if(iterator.current)iterator.current->iterator=nullptr;
    if(iterator.next)iterator.next->iterator=nullptr;
    for(;iterator.current;iterator.advance())
        set_animation_interrupt(*reinterpret_cast<Animation*>(iterator.current->value),event);
}
}
