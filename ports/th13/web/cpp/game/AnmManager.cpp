#include "AnmManager.hpp"
#include "AnmOverlay.hpp"
namespace th13 {
void AnmManager::unload(i32 id){auto found=banks.find(id);if(found==banks.end())return;auto* resource=&found->second->data;registry.retire_resource(resource);if(environment.fallback_sprite_resource==resource)environment.fallback_sprite_resource=nullptr;retired_banks.push_back(std::move(found->second));banks.erase(found);resource_names.erase(id);collect_resources();}
void AnmManager::collect_resources(){
 for(auto next=retired_banks.begin();next!=retired_banks.end();){const auto* resource=&(*next)->data;bool referenced=registry.references_resource(resource);if(!referenced)for(const auto& bank:banks){for(const auto& vm:bank.second->templates)if(vm.resource==resource||vm.sprite_resource==resource){referenced=true;break;}if(referenced)break;}
  if(referenced){++next;continue;}if(resource_release)resource_release(*resource);next=retired_banks.erase(next);
 }
}
bool AnmManager::load(i32 id,const u8* bytes,u32 size){
    if(banks.count(id)){error="Animation resource already loaded";return false;}
    auto bank=std::make_unique<Bank>();if(!bank->data.open(bytes,size)){error="Invalid animation resource";return false;}
    return adopt_bank(id,std::move(bank));
}
bool AnmManager::adopt(i32 id,AnmResource&& data){
    if(banks.count(id)){error="Animation resource already loaded";return false;}
    auto bank=std::make_unique<Bank>();bank->data=std::move(data);
    return adopt_bank(id,std::move(bank));
}
bool AnmManager::adopt_bank(i32 id,std::unique_ptr<Bank> bank){
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
    const Vec3 translation=vm.visual.translation;vm=*source;vm.presentation_generation=++AnmVm::presentation_counter;vm.presentation_motion=false;vm.visual.translation=translation;vm.rotation_parent=vm.creation_parent=nullptr;vm.slowdown=0;vm.pending_interrupt=0;vm.reset_runtime_timers();vm.instruction_offset=0;return true;
}
AnmVm* AnmManager::instantiate(AnmResource& resource,i32 script){
    for(const auto& bank:banks)if(&bank.second->data==&resource){auto* vm=registry.allocate();if(bind_template(*vm,bank.first,script))return vm;registry.discard(*vm);return nullptr;}
    error="Animation source resource not owned by manager";return nullptr;
}
bool AnmManager::initial_tick(AnmVm& vm){if(vm.tick(random,rate)<0){error=vm.error;registry.destroy_tree(vm);return false;}return true;}
// 0x46f820：池分配→模板复制+时间重置→0x200 活动→layer∈[0,21)→0x40000→
// 位置（bit0 时 +320/+16）→anmArg→立即 tick→按 flags 入链（Registry::submit）。
u32 AnmManager::create(i32 id,i32 script,i32 layer,u32 ordering,const Vec3& translation,float anm_arg,bool game_coordinates){
    auto* source=resource(id);if(!source){error="Animation resource unavailable";return 0;}auto* vm=instantiate(*source,script);if(!vm)return 0;
    vm->visual.render_flags|=0x200;
    if(layer>=0){vm->visual.layer=layer;if(layer<21)vm->visual.render_flags|=0x40000;}
    vm->visual.translation=translation;
    if(game_coordinates){vm->visual.translation.x=float(vm->visual.translation.x+320.f);vm->visual.translation.y=float(vm->visual.translation.y+16.f);}
    vm->variables.rotation.z=anm_arg;  // anmArg 落 vm+0x50（变量 10025 槽）
    if(!initial_tick(*vm))return 0;vm->ordering=ordering;return registry.submit(*vm,ordering);
}
AnmVm* AnmManager::spawn_child(AnmVm& source,i32 script,u32 ordering){
    if(!source.resource){error="Child animation source has no resource";return nullptr;}auto* vm=instantiate(*source.resource,source.local_script_index(script));if(!vm)return nullptr;
    // 0x46f990：位置清零、layer 继承父、父指针直挂本源、0x200 活动位。
    vm->visual.translation={};vm->visual.layer=source.visual.layer;vm->creation_parent=&source;vm->rotation_parent=&source;vm->visual.render_flags|=0x200;
    if(!initial_tick(*vm))return nullptr;vm->ordering=ordering;if(!registry.submit(*vm,ordering)||!registry.attach(*vm,source)){error=registry.error;registry.destroy_tree(*vm);return nullptr;}return vm;
}
AnmVm* AnmManager::spawn_detached(AnmVm& source,i32 script){
    if(!source.resource){error="Detached animation source has no resource";return nullptr;}auto* vm=instantiate(*source.resource,source.local_script_index(script));if(!vm)return nullptr;
    // 0x46fab0：layer/平移/旋转继承父，锚点=父脚本位置，主链尾入链。
    vm->visual.render_flags|=0x200;vm->visual.layer=source.visual.layer;
    vm->visual.translation=source.visual.translation;vm->variables.rotation=source.variables.rotation;vm->visual.child_anchor=source.variables.position;
    if(!initial_tick(*vm))return nullptr;vm->ordering=0;if(!registry.submit(*vm)){error=registry.error;registry.destroy_tree(*vm);return nullptr;}return vm;
}
bool AnmManager::spawn_effect(AnmVm& vm,i32 script){if(effect)return effect(vm,script);if(script==0){
 // 402ff0 copies four effect templates and the astral template into its
 // embedded VMs. Resolve the astral bank by its declared resource name.
 AnmResource* astral=nullptr;
 for(const auto& item:resource_names)if(item.second=="astral.anm"){astral=resource(item.first);break;}
 if(!vm.resource||!astral){error="Scene effect 0 requires effect and astral animation banks";return false;}
 vm.geometry.clear();vm.geometry.overlay=std::make_unique<AnmOverlay>();
 vm.geometry.allocation_bytes=0x1c3c;vm.visual.layer=0;
 if(!vm.geometry.overlay->initialize(*vm.resource,*astral,vm,random)){
  error="Scene effect 0 overlay initialization failed";return false;
 }
 return true;
 }if(script==2){
  vm.geometry.clear();vm.geometry.circle=std::make_unique<AnmCircle>();
  const auto p=vm.visual.translation;
  vm.geometry.circle->initialize({float(p.x+384.f),float(p.y+16.f),p.z},128.f);
  vm.visual.translation={};vm.geometry.allocation_bytes=0x2c;vm.visual.layer=13;return true;
 }if(script==5){vm.geometry.clear();vm.geometry.ribbon=std::make_unique<AnmRibbon>();vm.geometry.ribbon->initialize(random);vm.geometry.allocation_bytes=0x418;vm.visual.layer=13;return true;}if(script==6){
 vm.geometry.clear();vm.geometry.wind=std::make_unique<AnmWindTrail>();
 vm.geometry.wind->initialize(random,environment);vm.geometry.allocation_bytes=0x97c;
 vm.visual.translation={};vm.visual.layer=2;
 vm.visual.flags=(vm.visual.flags&0xfffffe3fu)|0x20u;
 vm.interpolators.alpha.begin(160,0,{64},{0});return true;
 }if(script==1||script==3){
 // 4bba80[1]（401000/401190/4012c0）：8 段轨迹；位置=op334 传入的敌机 VM+0x44
 // （41b7ac→415650），经 translation 携带；415650 先清 VM 绘制位再调 init，故读后清零。
 vm.geometry.clear();vm.geometry.trail=std::make_unique<AnmTrail>();
 const Vec3 effect_position=vm.visual.translation;
 vm.geometry.trail->initialize_small(effect_position,random);
 if(script==3){
  // 401310 shares 401190/4012c0 with kind1, but starts in the
  // supplied coordinate system and uses the blue/green palette.
  vm.geometry.trail->points[0]={effect_position.x,effect_position.y};
  for(u32 i=0;i<8;++i){
   u32 color=0xff40ff00u | u32(u8(255-i*32));
   if(i>=4)color=(color&0xffffffu)|(u32(u8(255-(i-4)*16))<<24);
   vm.geometry.trail->colors[i]=color;
  }
 }
 vm.geometry.allocation_bytes=0x78;
 vm.visual.translation={};vm.visual.layer=13;
 if(script==1)vm.visual.flags=(vm.visual.flags&0xfffffe3fu)|0x20u;
 return true;
 }error="Scene effect manager not bound";return false;}
i32 AnmManager::update_effect(AnmVm& vm,float rate){if(effect_update)return effect_update(vm,rate);if(vm.geometry.overlay)return vm.geometry.overlay->tick(random,rate);if(vm.geometry.circle){
 auto& circle=*vm.geometry.circle;if(circle.age.current>=50)return 1;
 if(circle.age.current!=circle.age.previous){
  const i32 bank=resource_id(vm.resource);
  for(int i=0;i<2;++i){const u32 h=create(bank,0,-1,0,circle.center);auto* child=registry.find(h);if(!child||!spawn_effect(*child,1)){vm.error=error;return -1;}}
 }
 circle.rim_color=(circle.rim_color&0x00ffffffu)|(((circle.rim_color>>24)+3u)&255u)<<24;
 circle.radius=float(circle.radius-1.5f);circle.age.tick(&rate);circle.rebuild();return 0;
 }vm.error="Scene effect update not bound";return -1;}
bool AnmManager::interrupt_effect(AnmVm& vm,i32 label,float rate){if(effect_interrupt&&!effect_interrupt(vm,label,rate))return false;if(vm.geometry.overlay){if(!vm.geometry.overlay->interrupt(label,random,rate))return false;if(label>=7&&label<=9)vm.visual.layer=label==7?26:label==8?20:6;}return true;}
bool AnmManager::rebind(u32& handle,i32 script){
    auto* old=registry.find(handle);if(!old)return true;old->visual.flags&=~1u;old->visible=false;old->instruction_offset=-1;
    auto* vm=instantiate(*old->resource,old->local_script_index(script));if(!vm)return false;vm->visual.render_flags|=0x200;vm->visual.translation={};vm->variables.rotation.z=0;
    if(!initial_tick(*vm))return false;vm->ordering=0;handle=registry.submit(*vm);return handle!=0;
}
bool AnmManager::retire(u32& handle){auto* vm=registry.find(handle);if(vm){vm->visual.render_flags|=32;if(!registry.retire_children(*vm))return false;}handle=0;return true;}
bool AnmManager::pause(u32 handle,bool paused){auto* vm=registry.find(handle);return !vm||registry.pause_tree(*vm,paused);}
bool AnmManager::interrupt(u32 handle,i32 label,bool immediate){auto* vm=registry.find(handle);if(!vm||registry.interrupt(*vm,label,random,rate,immediate))return true;error=registry.error;return false;}
bool AnmManager::update(){if(registry.update(random,rate)){collect_resources();return true;}error=registry.error;return false;}
bool AnmManager::update(bool alternate){if(registry.update_chain(alternate,random,rate)){collect_resources();return true;}error=registry.error;return false;}
int AnmManager::tick_instance(AnmVm& vm){const int result=vm.tick(random,rate);if(result<0)error=vm.error;return result;}
}
