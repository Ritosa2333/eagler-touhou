#include "AnmManager.hpp"
#include <cstdlib>
namespace th11 {
AnmManager::~AnmManager(){for(auto& [vm,size]:geometry_sizes){std::free(vm->geometry);vm->geometry=nullptr;}}
AnmVm* AnmManager::find(u32 id)noexcept{auto it=active.find(id);return it==active.end()?nullptr:it->second;}
AnmVm* AnmManager::create(AnmResource& file,i32 script,u16 file_id,u32 layer,bool overlay,bool prepend,const Vec3* initial_position){
    if(script<0||u32(script)>=file.scripts.size()||spawn_depth>=128)return nullptr;
    AnmVm* vm;if(available.empty()){storage.push_back(std::make_unique<AnmVm>());vm=storage.back().get();vm->initialize();}else{vm=available.back();available.pop_back();}
    vm->layer=layer;
    if(initial_position){vm->position=*initial_position;vm->bind_script(file,script,file_id,&rate);}
    else{vm->position=vm->script_position=vm->child_position={};vm->flags|=0x40000000;
    vm->rectangle_columns=vm->rectangle_rows=16;vm->script_index=i16(script);vm->file_index=file_id;vm->resource=&file;
    vm->script_begin=vm->instruction=reinterpret_cast<AnmInstruction*>(file.scripts[script].bytes.data());vm->flags&=~0x601u;vm->timer.set(0,&rate);}
    ++spawn_depth;const int status=vm->update(*this);--spawn_depth;
    if(status<0){last_error=u32(u16(vm->instruction?vm->instruction->opcode:-1));destroy(*vm);return nullptr;}
    auto& node=vm->registry;node.value=vm;node.next=node.previous=nullptr;const u32 list=overlay?1:0;
    if(prepend){node.next=heads[list];if(heads[list])heads[list]->previous=&node;else tails[list]=&node;heads[list]=&node;}
    else{node.previous=tails[list];if(tails[list])tails[list]->next=&node;else heads[list]=&node;tails[list]=&node;}
    if(++next_id==0)++next_id;vm->id=next_id;active.emplace(next_id,vm);return vm;
}
AnmVm* AnmManager::spawn(AnmVm& parent,i32 script,u32 op){if(!parent.resource)return nullptr;return create(*parent.resource,script,parent.file_index,parent.layer,op==90||op==92,op==91||op==92);}
void AnmManager::destroy(AnmVm& vm){
    // A destroyed VM may still be referenced by the previous draw traversal.
    // Remove it there before initialize() resets its id and next pointer.
    for(auto& head:layers){auto** link=&head;while(*link){if(*link==&vm){*link=vm.draw_next;break;}link=&(*link)->draw_next;}}
    for(u32 i=0;i<2;++i){if(heads[i]==&vm.registry)heads[i]=vm.registry.next;if(tails[i]==&vm.registry)tails[i]=vm.registry.previous;}
    auto unlink=[](AnmVm::Node& node){if(node.next)node.next->previous=node.previous;if(node.previous)node.previous->next=node.next;node.next=node.previous=nullptr;};
    unlink(vm.registry);unlink(vm.child);active.erase(vm.id);if(auto it=geometry_sizes.find(&vm);it!=geometry_sizes.end()){std::free(vm.geometry);vm.geometry=nullptr;geometry_sizes.erase(it);}
    vm.initialize();available.push_back(&vm);
}
void AnmManager::clear(){
    // Walk ownership, not draw layers: newly created VMs are not on a draw
    // layer until update() has run, and deleted layer heads are not owners.
    layers.fill(nullptr);
    while(!active.empty())destroy(*active.begin()->second);
    last_error=0;
}
void AnmManager::retire_resource(const AnmResource& resource){
    // Deferred deletion matches 4564c0: an overlay callback and its children
    // may still occur later in the current tick. Keep the resource alive until
    // both ANM registries have consumed the deletion markers.
    for(auto& [id,vm]:active)if(vm->resource==&resource)vm->flags|=0x4000000;
}
bool AnmManager::references(const AnmResource& resource)const noexcept{
    for(const auto& entry:active)if(entry.second->resource==&resource)return true;
    return false;
}
bool AnmManager::update(bool overlay){
    const u32 first=overlay?29:0,last=overlay?31:29;std::array<AnmVm*,31> end{};for(u32 i=first;i<last;++i)layers[i]=nullptr;
    auto* cursor=heads[overlay?1:0];while(cursor){auto& vm=*cursor->value;cursor=cursor->next;
        if(vm.flags&0x4000000){destroy(vm);continue;}
        if(vm.before_update)vm.before_update(vm);const int status=vm.update(*this);
        if(status<0){last_error=u32(u16(vm.instruction?vm.instruction->opcode:-1));return false;}if(status){destroy(vm);continue;}
        if(overlay&&vm.layer!=29&&vm.layer!=30)vm.layer=29;
        if(vm.layer>=first&&vm.layer<last){if(end[vm.layer])end[vm.layer]->draw_next=&vm;else layers[vm.layer]=&vm;end[vm.layer]=&vm;vm.draw_next=nullptr;}
    }return true;
}
bool AnmManager::allocate_geometry(AnmVm& vm,u32 bytes){
    if(!bytes||bytes>16*1024*1024)return false;void* p=std::calloc(1,bytes);if(!p)return false;
    if(geometry_sizes.find(&vm)!=geometry_sizes.end())std::free(vm.geometry);vm.geometry=p;geometry_sizes[&vm]=bytes;return true;
}
void AnmManager::release_geometry(AnmVm& vm){auto it=geometry_sizes.find(&vm);if(it!=geometry_sizes.end()){std::free(vm.geometry);vm.geometry=nullptr;geometry_sizes.erase(it);}}
bool AnmManager::update_geometry(AnmVm& vm){auto it=geometry_sizes.find(&vm);return it!=geometry_sizes.end()&&anm_ring_vertices(vm,static_cast<AnmVertex*>(vm.geometry),it->second/sizeof(AnmVertex));}
bool AnmManager::change_draw_mode(AnmVm& vm){if(!allocate_geometry(vm,sizeof(AnmRipple)))return false;anm_ripple_initialize(vm,*static_cast<AnmRipple*>(vm.geometry),script_rng);return true;}
}
