#include "AnmRegistry.hpp"
#include "AnmFrame.hpp"
namespace th10 {
namespace {
template<class F> void with_children(AnmVm* vm,F apply){
    if(!vm)return;apply(*vm);
    if(!vm->child_node.previous)for(auto* node=vm->child_node.next;node;node=node->next)apply(*node->value);
}
}
// 0x4491c0: world entries take priority when an id appears in both lists.
AnmVm* AnmRegistry::find(u32 id) const noexcept {
    if(!id)return nullptr;
    for(auto* node=world_head;node;node=node->next)if(node->value->id==id)return node->value;
    for(auto* node=ui_head;node;node=node->next)if(node->value->id==id)return node->value;
    return nullptr;
}
AnmVm* AnmRegistry::find_and_clear(u32& id) const noexcept {auto* vm=find(id);if(!vm)id=0;return vm;}
// 0x4497d0 includes the parent's own node before searching its children.
u32 AnmRegistry::find_child(u32& id,i32 script) const noexcept {auto* vm=find_and_clear(id);if(!vm)return 0;for(auto* node=&vm->child_node;node;node=node->next)if(node->value->script_index==script)return node->value->id;return 0;}
// 0x449210/0x449470, 0x449590/0x4495e0, 0x4492a0.
void AnmRegistry::interrupt(u32 id,std::int16_t label) const noexcept {with_children(find(id),[&](AnmVm& vm){vm.pending_interrupt=label;});}
// 0x449250. Updating may change the parent's child list or a child's successor,
// so these links are read after their respective updates.
i32 AnmRegistry::interrupt_and_update(u32 id,std::int16_t label,AnmFrameEnvironment& env) const {
    auto* vm=find(id);if(!vm)return 0;vm->pending_interrupt=label;env.update(*vm);i32 result=reinterpret_cast<uintptr_t>(vm->child_node.previous);if(!result)for(auto* node=vm->child_node.next;node;node=node->next){node->value->pending_interrupt=label;result=env.update(*node->value);}return result;
}
void AnmRegistry::set_visibility(u32 id,bool visible) const noexcept {
    with_children(find(id),[&](AnmVm& vm){if(visible)vm.flags|=2;else vm.flags&=~2u;});
}
void AnmRegistry::request_delete(u32 id) const noexcept {with_children(find(id),[](AnmVm& vm){vm.flags|=0x4000000;});}
// 0x449630. The handle is cleared even when the animation has already expired.
void AnmRegistry::delete_and_clear(u32& id) const noexcept {request_delete(id);id=0;}
// 0x449350/0x4492f0. Only the root propagates to the flat child list.
void AnmRegistry::set_position(u32 id,const Vec3& position,bool playfield_coordinates) const noexcept {
    with_children(find(id),[&](AnmVm& vm){
        vm.position.x=playfield_coordinates?Scalar::add(position.x,224.0f):position.x;
        vm.position.y=playfield_coordinates?Scalar::add(position.y,16.0f):position.y;
        vm.position.z=position.z;
    });
}
}
