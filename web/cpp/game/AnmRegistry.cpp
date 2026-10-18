#include "AnmRegistry.hpp"
#include "AnmOverlay.hpp"
#include <algorithm>
namespace th15 {
void AnmRegistry::retire_resource(const AnmResource* resource)noexcept{if(!resource)return;for(auto& group:groups)for(auto* entry:group)if(entry->vm.resource==resource)entry->vm.visual.render_flags=(entry->vm.visual.render_flags&~64u)|32u;}
bool AnmRegistry::references_resource(const AnmResource* resource)const noexcept{
 if(!resource)return false;const auto uses=[&](auto&& self,const AnmVm& vm)->bool{if(vm.resource==resource||vm.sprite_resource==resource)return true;if(vm.geometry.overlay)for(const auto& panel:vm.geometry.overlay->panels)if(panel&&self(self,*panel))return true;return false;};
 for(const auto& entry:nodes)if(entry.second->active&&uses(uses,entry.second->vm))return true;
 for(const auto& entry:external)if(entry.second->active&&uses(uses,*entry.second->borrowed))return true;return false;
}
AnmRegistry::AnmRegistry(){free_slots.reserve(pool.size());for(u32 i=pool.size();i>0;i--)free_slots.push_back(i-1);}
AnmRegistry::Node* AnmRegistry::node(AnmVm& vm)const noexcept{const auto it=nodes.find(&vm);if(it!=nodes.end())return it->second;const auto borrowed=external.find(&vm);return borrowed==external.end()?nullptr:borrowed->second.get();}
AnmVm* AnmRegistry::allocate(){
    Node* entry=nullptr;if(!free_slots.empty()){const u32 slot=free_slots.back();free_slots.pop_back();if(!pool[slot]){pool[slot]=std::make_unique<Node>();pool[slot]->slot=slot;nodes.emplace(&pool[slot]->vm,pool[slot].get());}entry=pool[slot].get();}
    else{auto extra=std::make_unique<Node>();entry=extra.get();nodes.emplace(&entry->vm,entry);overflow.emplace(entry,std::move(extra));}
    entry->active=true;entry->registered=false;entry->handle=0;entry->parent=nullptr;entry->children.clear();entry->vm.rotation_parent=entry->vm.creation_parent=nullptr;active_count++;return &entry->vm;
}
u32 AnmRegistry::submit(AnmVm& vm,u32 ordering){
    auto* entry=node(vm);if(!entry||entry->borrowed||!entry->active||entry->registered){error="Animation instance cannot be registered";return 0;}
    generation=(generation+1)&0x7ffff;if(!generation)generation=1;entry->handle=(generation<<13)|(entry->slot&0x1fff);entry->alternate=ordering&4;
    auto& list=groups[entry->alternate];if(ordering&2)entry->position=list.insert(list.begin(),entry);else entry->position=list.insert(list.end(),entry);entry->registered=true;return entry->handle;
}
AnmVm* AnmRegistry::find(u32 handle)const noexcept{
    if(!handle)return nullptr;const u32 slot=handle&0x1fff;
    if(slot<pool.size()){const auto* entry=pool[slot].get();return entry&&entry->active&&entry->handle==handle?const_cast<AnmVm*>(&entry->vm):nullptr;}
    for(const auto& list:groups)for(auto* entry:list)if(entry->handle==handle)return &entry->vm;return nullptr;
}
bool AnmRegistry::attach(AnmVm& child,AnmVm& parent){
    // Children retain their resource script identity for game callbacks.
    auto* entry=node(child);auto* owner=node(parent);if(!entry||entry->borrowed||!entry->active||entry==owner||entry->parent){error="Animation child relationship invalid";return false;}
    // Player and bullet objects embed their root VM instead of allocating it
    // in the global pool. Track only its child list; never own its lifetime.
    if(!owner){auto borrowed=std::make_unique<Node>();borrowed->borrowed=&parent;borrowed->active=true;owner=borrowed.get();external.emplace(&parent,std::move(borrowed));}
    if(!owner->active){error="Animation parent inactive";return false;}
    for(auto* ancestor=owner;ancestor;ancestor=ancestor->parent)if(ancestor==entry){error="Animation parent cycle";return false;}
    entry->parent=owner;owner->children.push_front(entry);return true;
}
void AnmRegistry::collect(Node& entry){
    for(auto* child:entry.children)collect(*child);
    if((entry.vm.visual.render_flags&0x60)!=0x40)pending_deletion.push_back(&entry);
    entry.vm.rotation_parent=entry.vm.creation_parent=nullptr;entry.vm.visual.render_flags=(entry.vm.visual.render_flags&~32u)|64;
}
void AnmRegistry::erase(Node& entry){
    if(entry.borrowed){external.erase(entry.borrowed);return;}
    if(entry.vm.geometry.gather)for(auto& h:entry.vm.geometry.gather->handles){auto* child=find(h);if(!child){h=0;continue;}child->visual.flags&=~1u;child->visible=false;child->instruction_offset=-1;}
    if(entry.registered)groups[entry.alternate].erase(entry.position);
    if(entry.parent)entry.parent->children.remove(&entry);
    for(auto* child:entry.children){child->parent=nullptr;child->vm.rotation_parent=child->vm.creation_parent=nullptr;}entry.children.clear();
    entry.vm.rotation_parent=entry.vm.creation_parent=nullptr;entry.vm.instruction_offset=-1;entry.active=entry.registered=false;entry.handle=0;active_count--;
    if(entry.slot<pool.size())free_slots.push_back(entry.slot);else{nodes.erase(&entry.vm);overflow.erase(&entry);}
}
bool AnmRegistry::discard(AnmVm& vm){auto* entry=node(vm);if(!entry||!entry->active)return true;if(!entry->children.empty()){error="Animation with children needs recursive deletion";return false;}pending_deletion.erase(std::remove(pending_deletion.begin(),pending_deletion.end(),entry),pending_deletion.end());erase(*entry);return true;}
bool AnmRegistry::destroy_tree(AnmVm& vm){auto* entry=node(vm);if(!entry||!entry->active)return true;while(!entry->children.empty())if(!destroy_tree(entry->children.front()->vm))return false;pending_deletion.erase(std::remove(pending_deletion.begin(),pending_deletion.end(),entry),pending_deletion.end());erase(*entry);return true;}
bool AnmRegistry::preserve_embedded_children(AnmVm& vm){
    auto it=external.find(&vm);if(it==external.end())return true;auto owner=std::move(it->second);external.erase(it);
    if(owner->children.empty())return true;owner->vm=vm;owner->vm.sprite_source=nullptr;owner->borrowed=&owner->vm;owner->frozen_root=true;
    const auto remap=[&](auto&& self,Node& entry)->void{if(entry.vm.creation_parent==&vm)entry.vm.creation_parent=&owner->vm;if(entry.vm.rotation_parent==&vm)entry.vm.rotation_parent=&owner->vm;for(auto* child:entry.children)self(self,*child);};
    for(auto* child:owner->children)remap(remap,*child);const auto* key=owner->borrowed;external.emplace(const_cast<AnmVm*>(key),std::move(owner));return true;
}
void AnmRegistry::retire(Node& entry){if(entry.vm.visual.render_flags&0x4000000)return;entry.vm.visual.render_flags=(entry.vm.visual.render_flags&~64u)|32;for(auto* child:entry.children)retire(*child);}
bool AnmRegistry::retire_children(AnmVm& vm){auto* entry=node(vm);if(!entry||!entry->active)return true;for(auto* child:entry->children)retire(*child);return true;}
bool AnmRegistry::update(bool alternate,Rng& random,float rate){
    if(alternate)for(u32 i=35;i<42;i++)layers[i].clear();else for(u32 i=0;i<35;i++)layers[i].clear();pending_deletion.clear();
    auto next=groups[alternate].begin();while(next!=groups[alternate].end()){
        auto* entry=*next++;const u32 mode=entry->vm.visual.render_flags>>5&3;
        if(mode==1)collect(*entry);
        else if(!mode){const int result=entry->vm.tick(random,rate);if(result<0){error=entry->vm.error;return false;}if(result)collect(*entry);else{
            i32 layer=entry->vm.visual.layer;if(alternate){if(layer>=24&&layer<=30)layer+=11;else if(layer<35||layer>41)layer=37;}else if(layer>=35&&layer<=41)layer-=11;
            if(layer<0||layer>=42){error="Animation draw layer outside range";return false;}entry->vm.visual.layer=layer;layers[layer].push_back(&entry->vm);
        }}
    }
    // The source queues children first and prepends each entry, so release is
    // parent first. Freed pool slots are placed at the front of the free list.
    for(auto it=pending_deletion.rbegin();it!=pending_deletion.rend();++it)erase(**it);pending_deletion.clear();for(auto it=external.begin();it!=external.end();){if(it->second->frozen_root&&it->second->children.empty())it=external.erase(it);else ++it;}return true;
}
bool AnmRegistry::pause_tree(AnmVm& vm,bool paused){auto* entry=node(vm);if(!entry||!entry->active)return true;if(paused)vm.visual.flags&=~2u;else vm.visual.flags|=2;for(auto* child:entry->children)if(!pause_tree(child->vm,paused))return false;return true;}
bool AnmRegistry::interrupt(AnmVm& vm,i32 label,Rng& random,float rate,bool immediate){
    auto* entry=node(vm);if(!entry||!entry->active)return true;if(vm.object_host&&!vm.object_host->interrupt_effect(vm,label,rate))return false;vm.pending_interrupt=label;if(immediate&&vm.tick(random,rate)<0){error=vm.error;return false;}
    for(auto* child:entry->children){if(child->vm.object_host&&!child->vm.object_host->interrupt_effect(child->vm,label,rate))return false;child->vm.pending_interrupt=label;if(immediate&&child->vm.tick(random,rate)<0){error=child->vm.error;return false;}}return true;
}
u32 AnmRegistry::handle(const AnmVm& vm)const noexcept{auto* entry=node(const_cast<AnmVm&>(vm));return entry?entry->handle:0;}
std::vector<AnmVm*> AnmRegistry::children(const AnmVm& vm)const{std::vector<AnmVm*> result;const auto* entry=node(const_cast<AnmVm&>(vm));if(entry)for(auto* child:entry->children)result.push_back(&child->vm);return result;}
AnmVm* AnmRegistry::find_child_script(AnmVm& vm,i32 script,i32 occurrence)const noexcept{auto* entry=node(vm);if(!entry||occurrence<0)return nullptr;for(auto* child:entry->children){if(child->vm.source_script==script||script==-1){if(occurrence==0)return &child->vm;occurrence--;}if(!child->children.empty())if(auto* found=find_child_script(child->vm,script,occurrence))return found;}return nullptr;}
u32 AnmRegistry::slot(const AnmVm& vm)const noexcept{auto* entry=node(const_cast<AnmVm&>(vm));return entry?entry->slot:0x1fff;}
u32 AnmRegistry::ordered_handle(bool alternate,u32 index)const noexcept{for(auto* entry:groups[alternate])if(index--==0)return entry->handle;return 0;}
}
