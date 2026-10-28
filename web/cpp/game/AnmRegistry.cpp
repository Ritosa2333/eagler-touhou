#include "AnmRegistry.hpp"
#include "AnmOverlay.hpp"
#include <algorithm>
namespace th13 {
// 卸载资源时把绑定它的 VM 标待回收（0x46fea0 语义：render_flags |= 0x20）。
void AnmRegistry::retire_resource(const AnmResource* resource)noexcept{if(!resource)return;for(auto& group:groups)for(auto* entry:group)if(entry->vm.resource==resource)entry->vm.visual.render_flags|=32u;}
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
    entry->active=true;entry->registered=false;entry->pending_collection=false;entry->handle=0;entry->parent=nullptr;entry->children.clear();entry->vm.rotation_parent=entry->vm.creation_parent=nullptr;active_count++;return &entry->vm;
}
u32 AnmRegistry::submit(AnmVm& vm,u32 ordering){
    auto* entry=node(vm);if(!entry||entry->borrowed||!entry->active||entry->registered){error="Animation instance cannot be registered";return 0;}
    // 句柄 = generation<<13 | slot（0x46f3f0：低 13 位槽、高 19 位代，代跳 0）。
    generation=(generation+1)&0x7ffff;if(!generation)generation=1;entry->handle=(generation<<13)|(entry->slot&0x1fff);entry->alternate=ordering&4;
    auto& list=groups[entry->alternate];if(ordering&2)entry->position=list.insert(list.begin(),entry);else entry->position=list.insert(list.end(),entry);entry->registered=true;
    // 副链插入清 0xc000（0046f820.c:65,71：bit2/bit1|bit2 两路插入后均清）。
    if(entry->alternate)vm.visual.render_flags&=~0xc000u;return entry->handle;
}
AnmVm* AnmRegistry::find(u32 handle)const noexcept{
    if(!handle)return nullptr;const u32 slot=handle&0x1fff;
    if(slot<pool.size()){const auto* entry=pool[slot].get();return entry&&entry->active&&entry->handle==handle?const_cast<AnmVm*>(&entry->vm):nullptr;}
    for(const auto& list:groups)for(auto* entry:list)if(entry->handle==handle)return &entry->vm;return nullptr;
}
bool AnmRegistry::attach(AnmVm& child,AnmVm& parent){
    // 子 VM 保留资源脚本身份供游戏回调（原版子链 vm+0x560/0x564 + 父指针）。
    auto* entry=node(child);auto* owner=node(parent);if(!entry||entry->borrowed||!entry->active||entry==owner||entry->parent){error="Animation child relationship invalid";return false;}
    // 玩家/弹幕对象内嵌根 VM：只跟踪子链，不接管生命周期。
    if(!owner){auto borrowed=std::make_unique<Node>();borrowed->borrowed=&parent;borrowed->active=true;owner=borrowed.get();external.emplace(&parent,std::move(borrowed));}
    if(!owner->active){error="Animation parent inactive";return false;}
    for(auto* ancestor=owner;ancestor;ancestor=ancestor->parent)if(ancestor==entry){error="Animation parent cycle";return false;}
    entry->parent=owner;owner->children.push_front(entry);return true;
}
void AnmRegistry::collect(Node& entry){
    // ANM render_flags bit 6 belongs to ins424, not pool bookkeeping.
    if(entry.pending_collection)return;
    entry.pending_collection=true;
    for(auto* child:entry.children)collect(*child);
    pending_deletion.push_back(&entry);
    entry.vm.rotation_parent=entry.vm.creation_parent=nullptr;entry.vm.visual.render_flags|=32;
}
void AnmRegistry::requeue_to_back(AnmVm& vm){
    // [f389 标题批序] 把已注册 VM 挪到其链表尾（不改变任何运行态；仅
    // 绘制提交序受影响）。oracle 侧重挂语义（0x46f3f0 尾插）的同构物。
    auto* entry=node(vm);
    if(!entry||!entry->registered||entry->alternate)return;
    groups[0].erase(entry->position);
    entry->position=groups[0].insert(groups[0].end(),entry);
}
void AnmRegistry::erase(Node& entry){
    // 幂等守卫：原版 0x46f690 对池内节点内联回收恰一次（子链只打 0x20 下帧
    // 链扫自回收，无二次路径）；移植侧直毁（destroy_tree/discard/spawn 失败
    // 清理）与延迟队列双路径并存，已回收节点必须静默跳过（collect 幂等的
    // defence in depth：直毁子后 update 队列残留项不再二次释放槽位）。
    if(!entry.active&&!entry.borrowed)return;
    if(entry.borrowed){external.erase(entry.borrowed);return;}
    if(entry.vm.geometry.gather)for(auto& h:entry.vm.geometry.gather->handles){auto* child=find(h);if(!child){h=0;continue;}child->visual.flags&=~1u;child->visible=false;child->instruction_offset=-1;}
    if(entry.registered)groups[entry.alternate].erase(entry.position);
    if(entry.parent)entry.parent->children.remove(&entry);
    // 0x46f690：子链全体摘父指针并标待回收（0x20），下帧由链扫描回收。
    for(auto* child:entry.children){child->parent=nullptr;child->vm.rotation_parent=child->vm.creation_parent=nullptr;child->vm.visual.render_flags|=32u;}entry.children.clear();
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
void AnmRegistry::retire(Node& entry){entry.vm.visual.render_flags|=32;for(auto* child:entry.children)retire(*child);}
bool AnmRegistry::retire_children(AnmVm& vm){auto* entry=node(vm);if(!entry||!entry->active)return true;for(auto* child:entry->children)retire(*child);return true;}
// 单条链一遍（0x46f170 主链 / 0x46f240 副链同构）：待回收(0x20)不 tick 直接
// 回收；tick 非零回收；零则归桶。副链层 ∉{31,32} 钳到 31（0x46f240:31-33）。
bool AnmRegistry::tick_chain(u32 alternate,GameRandom& random,float rate){
    auto next=groups[alternate].begin();while(next!=groups[alternate].end()){
        auto* entry=*next++;
        if(entry->pending_collection)continue;
        if(entry->vm.visual.render_flags&0x20){collect(*entry);continue;}
        const int result=entry->vm.tick(random,rate);
        if(result<0){error=entry->vm.error;return false;}
        if(result){collect(*entry);continue;}
        i32 layer=entry->vm.visual.layer;
        if(alternate){if(layer!=31&&layer!=32)layer=31;}
        else if(layer<0||layer>30){error="Animation draw layer outside range";return false;}
        entry->vm.visual.layer=layer;layers[u32(layer)].push_back(&entry->vm);
    }
    return true;
}
bool AnmRegistry::update(GameRandom& random,float rate){
    for(auto& bucket:layers)bucket.clear();pending_deletion.clear();
    if(!tick_chain(0,random,rate))return false;
    if(!tick_chain(1,random,rate))return false;
    // 原版先队列子节点再前插，释放顺序为父先；空出的槽放回空闲链头部。
    for(auto it=pending_deletion.rbegin();it!=pending_deletion.rend();++it)erase(**it);pending_deletion.clear();for(auto it=external.begin();it!=external.end();){if(it->second->frozen_root&&it->second->children.empty())it=external.erase(it);else ++it;}return true;
}
bool AnmRegistry::update_chain(bool alternate,GameRandom& random,float rate){
    for(u32 i=alternate?31:0;i<(alternate?33u:31u);++i)layers[i].clear();pending_deletion.clear();
    if(!tick_chain(alternate?1u:0u,random,rate))return false;
    for(auto it=pending_deletion.rbegin();it!=pending_deletion.rend();++it)erase(**it);pending_deletion.clear();for(auto it=external.begin();it!=external.end();){if(it->second->frozen_root&&it->second->children.empty())it=external.erase(it);else ++it;}return true;
}
bool AnmRegistry::pause_tree(AnmVm& vm,bool paused){auto* entry=node(vm);if(!entry||!entry->active)return true;if(paused)vm.visual.flags&=~2u;else vm.visual.flags|=2;for(auto* child:entry->children)if(!pause_tree(child->vm,paused))return false;return true;}
bool AnmRegistry::interrupt(AnmVm& vm,i32 label,GameRandom& random,float rate,bool immediate){
    // 0x46fca0：子中断回调(vm+0x59c) → pending=label → 立即 tick，逐子传播。
    auto* entry=node(vm);if(!entry||!entry->active)return true;if(vm.object_host&&!vm.object_host->interrupt_effect(vm,label,rate))return false;vm.pending_interrupt=label;if(immediate&&vm.tick(random,rate)<0){error=vm.error;return false;}
    for(auto* child:entry->children){if(child->vm.object_host&&!child->vm.object_host->interrupt_effect(child->vm,label,rate))return false;child->vm.pending_interrupt=label;if(immediate&&child->vm.tick(random,rate)<0){error=child->vm.error;return false;}}return true;
}
u32 AnmRegistry::handle(const AnmVm& vm)const noexcept{auto* entry=node(const_cast<AnmVm&>(vm));return entry?entry->handle:0;}
std::vector<AnmVm*> AnmRegistry::children(const AnmVm& vm)const{std::vector<AnmVm*> result;const auto* entry=node(const_cast<AnmVm&>(vm));if(entry)for(auto* child:entry->children)result.push_back(&child->vm);return result;}
AnmVm* AnmRegistry::find_child_script(AnmVm& vm,i32 script,i32 occurrence)const noexcept{auto* entry=node(vm);if(!entry||occurrence<0)return nullptr;for(auto* child:entry->children){if(child->vm.source_script==vm.local_script_index(script)||script==-1){if(occurrence==0)return &child->vm;occurrence--;}if(!child->children.empty())if(auto* found=find_child_script(child->vm,script,occurrence))return found;}return nullptr;}
u32 AnmRegistry::slot(const AnmVm& vm)const noexcept{auto* entry=node(const_cast<AnmVm&>(vm));return entry?entry->slot:0x1fff;}
u32 AnmRegistry::ordered_handle(bool alternate,u32 index)const noexcept{for(auto* entry:groups[alternate])if(index--==0)return entry->handle;return 0;}
}
