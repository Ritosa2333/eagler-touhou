#include "AnmManager.hpp"
namespace th10 {
// 0x449950. Probe two slots, then allocate on the heap; this is not a pool scan.
AnmVm* AnmManager::allocate(AnmAllocationEnvironment& env){
    AnmVm* result;
    if(occupied[cursor])cursor=wrapping_add(cursor,1)%4096;
    if(occupied[cursor]){result=env.allocate_animation();if(result)result->clear();result->initialize();}
    else{result=&pool[cursor];occupied[cursor]=1;}
    cursor=wrapping_add(cursor,1)%4096;return result;
}
// 0x449a00.
bool AnmManager::is_pooled(const AnmVm* vm) const noexcept {
    const auto at=reinterpret_cast<uintptr_t>(vm);return at>=reinterpret_cast<uintptr_t>(pool)&&at<reinterpret_cast<uintptr_t>(pool+4096);
}
// 0x4489d0 / 0x448a50 / 0x448ac0 / 0x448b40.
u32 AnmManager::insert(AnmVm& vm,AnimationPlacement placement) noexcept {
    const bool ui=placement==AnimationPlacement::UiBack||placement==AnimationPlacement::UiFront;
    const bool front=placement==AnimationPlacement::WorldFront||placement==AnimationPlacement::UiFront;
    auto*& head=ui?registry.ui_head:registry.world_head;auto*& tail=ui?registry.ui_tail:registry.world_tail;
    auto& node=vm.registry_node;node.initialize(&vm);
    if(front){if(!head)tail=&node;else{node.next=head;head->previous=&node;}head=&node;}
    else{if(!head)head=&node;else node.insert_after(*tail);tail=&node;}
    ++last_id;if(!last_id)++last_id;vm.id=last_id;return last_id;
}
// 0x448bb0. Removing a parent detaches its child-list link without recursively
// freeing the children. Pooled VMs are reset; heap VMs are released.
i32 AnmManager::remove(AnmVm& vm,AnmAllocationEnvironment& env){
    auto& node=vm.registry_node;
    if(registry.world_tail==&node)registry.world_tail=node.previous;if(registry.world_head==&node)registry.world_head=node.next;
    if(registry.ui_tail==&node)registry.ui_tail=node.previous;if(registry.ui_head==&node)registry.ui_head=node.next;
    node.unlink();vm.child_node.unlink();
    const bool pooled=is_pooled(&vm);
    if(pooled)occupied[(&vm-pool)]=0;
    if(vm.geometry)env.release_memory(vm.geometry);vm.geometry=nullptr;
    if(pooled)vm.initialize();else env.release_memory(&vm);
    return 0;
}
u32 AnmManager::create(AnmFile& file,i32 script,u32 tag,AnimationPlacement placement,AnmEnvironment& animations,AnmAllocationEnvironment& allocation){
    auto& vm=*allocate(allocation);vm.owner_tag=tag;vm.flags|=0x40000000;
    file.prepare_script(vm,script,animations,started_scripts);return insert(vm,placement);
}
u32 AnmManager::create_at(AnmFile& file,i32 script,const Vec3& position,bool playfield,AnimationPlacement placement,AnmEnvironment& animations,AnmAllocationEnvironment& allocation){
    auto& vm=*allocate(allocation);vm.owner_tag=0;vm.flags|=0x40000000;
    vm.position={playfield?Scalar::add(position.x,224.0f):position.x,
                 playfield?Scalar::add(position.y,16.0f):position.y,position.z};
    file.initialize_script(vm,script,animations,started_scripts);return insert(vm,placement);
}
}
