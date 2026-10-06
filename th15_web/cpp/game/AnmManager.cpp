#include "AnmManager.hpp"
namespace th15 {
void AnmManager::unload(i32 id){auto found=banks.find(id);if(found==banks.end())return;auto* resource=&found->second->data;registry.retire_resource(resource);if(environment.fallback_sprite_resource==resource)environment.fallback_sprite_resource=nullptr;retired_banks.push_back(std::move(found->second));banks.erase(found);resource_names.erase(id);collect_resources();}
void AnmManager::collect_resources(){
 for(auto next=retired_banks.begin();next!=retired_banks.end();){const auto* resource=&(*next)->data;bool referenced=registry.references_resource(resource);if(!referenced)for(const auto& bank:banks){for(const auto& vm:bank.second->templates)if(vm.resource==resource||vm.sprite_resource==resource){referenced=true;break;}if(referenced)break;}
  if(referenced){++next;continue;}if(resource_release)resource_release(*resource);next=retired_banks.erase(next);
 }
}
bool AnmManager::load(i32 id,const u8* bytes,u32 size){
    if(banks.count(id)){error="Animation resource already loaded";return false;}
    auto bank=std::make_unique<Bank>();if(!bank->data.open(bytes,size)){error="Invalid animation resource";return false;}
    bank->templates.resize(bank->data.scripts.size());
    for(u32 i=0;i<bank->templates.size();i++){auto& vm=bank->templates[i];vm.environment=&environment;vm.object_host=this;
        if(!vm.bind(bank->data,i)){error="Animation template binding failed";return false;}
        vm.prepare_template();if(vm.tick(random,rate)<0){error="Animation template "+std::to_string(i)+": "+vm.error;return false;}
    }
    banks.emplace(id,std::move(bank));return true;
}
AnmResource* AnmManager::resource(i32 id)const noexcept{auto it=banks.find(id);return it==banks.end()?nullptr:&it->second->data;}
i32 AnmManager::resource_id(const AnmResource* value)const noexcept{for(const auto& bank:banks)if(&bank.second->data==value)return bank.first;return -1;}
AnmVm* AnmManager::script_template(i32 id,u32 script)const noexcept{auto it=banks.find(id);return it==banks.end()||script>=it->second->templates.size()?nullptr:&it->second->templates[script];}
bool AnmManager::bind_template(AnmVm& vm,i32 id,i32 script){
    auto* source=script>=0?script_template(id,u32(script)):nullptr;if(!source){error="Animation template unavailable";return false;}
    const Vec3 translation=vm.visual.translation;vm=*source;vm.presentation_generation=++AnmVm::presentation_counter;vm.presentation_motion=false;vm.visual.translation=translation;vm.rotation_parent=vm.creation_parent=nullptr;vm.slowdown=0;vm.pending_interrupt=0;vm.reset_runtime_timers();return true;
}
AnmVm* AnmManager::instantiate(AnmResource& resource,i32 script){
    for(const auto& bank:banks)if(&bank.second->data==&resource){auto* vm=registry.allocate();if(bind_template(*vm,bank.first,script))return vm;registry.discard(*vm);return nullptr;}
    error="Animation source resource not owned by manager";return nullptr;
}
bool AnmManager::initial_tick(AnmVm& vm){if(vm.tick(random,rate)<0){error=vm.error;registry.destroy_tree(vm);return false;}return true;}
u32 AnmManager::create(i32 id,i32 script,i32 layer,u32 ordering,const Vec3& translation,float rotation_z){
    auto* source=resource(id);if(!source){error="Animation resource unavailable";return 0;}auto* vm=instantiate(*source,script);if(!vm)return 0;
    vm->visual.render_flags|=0x400;if(layer>=0){vm->visual.layer=layer;if(layer<24)vm->visual.render_flags=(vm->visual.render_flags&~0x80000u)|0x40400;}
    vm->visual.translation=translation;vm->variables.rotation.z=rotation_z;if(!initial_tick(*vm))return 0;vm->ordering=ordering;return registry.submit(*vm,ordering);
}
u32 AnmManager::create_overlay(i32 resource,i32 script){
    const u32 handle=create(resource,script,-1,4);if(auto* vm=registry.find(handle))vm->visual.render_flags&=~0xc000u;return handle;
}
AnmVm* AnmManager::spawn_child(AnmVm& source,i32 script,u32 ordering){
    if(!source.resource){error="Child animation source has no resource";return nullptr;}auto* vm=instantiate(*source.resource,script);if(!vm)return nullptr;
    vm->visual.translation={};vm->creation_parent=&source;vm->rotation_parent=source.rotation_parent?source.rotation_parent:&source;
    vm->visual.render_flags^=(source.visual.render_flags^vm->visual.render_flags)&0x2000000;
    if(!initial_tick(*vm))return nullptr;vm->ordering=ordering;if(!registry.submit(*vm,ordering)||!registry.attach(*vm,source)){error=registry.error;registry.destroy_tree(*vm);return nullptr;}return vm;
}
AnmVm* AnmManager::spawn_detached(AnmVm& source,i32 script){
    if(!source.resource){error="Detached animation source has no resource";return nullptr;}auto* vm=instantiate(*source.resource,script);if(!vm)return nullptr;
    vm->visual.render_flags|=0x400;vm->visual.layer=source.visual.layer;vm->visual.render_flags^=(source.visual.render_flags^vm->visual.render_flags)&0x2000000;
    vm->visual.translation=source.visual.translation;vm->variables.rotation=source.variables.rotation;vm->visual.child_anchor=source.variables.position;
    if(!initial_tick(*vm))return nullptr;vm->ordering=0;if(!registry.submit(*vm)){error=registry.error;registry.destroy_tree(*vm);return nullptr;}return vm;
}
bool AnmManager::spawn_effect(AnmVm& vm,i32 script){if(effect)return effect(vm,script);error="Scene effect manager not bound";return false;}
i32 AnmManager::update_effect(AnmVm& vm,float rate){if(effect_update)return effect_update(vm,rate);vm.error="Scene effect update not bound";return -1;}
bool AnmManager::interrupt_effect(AnmVm& vm,i32 label,float rate){return !effect_interrupt||effect_interrupt(vm,label,rate);}
bool AnmManager::rebind(u32& handle,i32 script){
    auto* old=registry.find(handle);if(!old)return true;old->visual.flags&=~1u;old->visible=false;old->instruction_offset=-1;
    auto* vm=instantiate(*old->resource,script);if(!vm)return false;vm->visual.render_flags|=0x400;vm->visual.translation={};vm->variables.rotation.z=0;
    if(!initial_tick(*vm))return false;vm->ordering=0;handle=registry.submit(*vm);return handle!=0;
}
bool AnmManager::retire(u32& handle){auto* vm=registry.find(handle);if(vm&&!(vm->visual.render_flags&0x4000000)){vm->visual.render_flags=(vm->visual.render_flags&~64u)|32;if(!registry.retire_children(*vm))return false;}handle=0;return true;}
bool AnmManager::pause(u32 handle,bool paused){auto* vm=registry.find(handle);return !vm||registry.pause_tree(*vm,paused);}
bool AnmManager::interrupt(u32 handle,i32 label,bool immediate){auto* vm=registry.find(handle);if(!vm||registry.interrupt(*vm,label,random,rate,immediate))return true;error=registry.error;return false;}
bool AnmManager::update(bool alternate){if(registry.update(alternate,random,rate)){collect_resources();return true;}error=registry.error;return false;}
int AnmManager::tick_instance(AnmVm& vm){const int result=vm.tick(random,rate);if(result<0)error=vm.error;return result;}
}
