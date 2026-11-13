#include "AnmRegistry.hpp"
#include "AnmOverlay.hpp"
#include <algorithm>
#include <unordered_map>
namespace th13 {
ChainEntryObserver chain_entry_observer = nullptr;
// 卸载资源时把绑定它的 VM 标待回收（0x46fea0 语义：render_flags |= 0x20）。
void AnmRegistry::retire_resource(const AnmResource* resource)noexcept{if(!resource)return;for(auto& group:groups)for(auto* entry:group)if(entry->vm.resource==resource)entry->vm.visual.render_flags=entry->vm.visual.render_flags|32u;}
bool AnmRegistry::references_resource(const AnmResource* resource)const noexcept{
 if(!resource)return false;const auto uses=[&](auto&& self,const AnmVm& vm)->bool{if(vm.resource==resource||vm.sprite_resource==resource)return true;if(vm.geometry.overlay)for(const auto& panel:vm.geometry.overlay->panels)if(panel&&self(self,*panel))return true;return false;};
 for(const auto& entry:nodes)if(entry.second->active&&uses(uses,entry.second->vm))return true;
 for(const auto& entry:external)if(entry.second->active&&uses(uses,*entry.second->borrowed))return true;return false;
}
AnmRegistry::AnmRegistry(){free_slots.reserve(pool.size());for(u32 i=pool.size();i>0;i--)free_slots.push_back(i-1);}
AnmRegistry::Node* AnmRegistry::node(AnmVm& vm)const noexcept{const auto it=nodes.find(&vm);if(it!=nodes.end())return it->second;const auto borrowed=external.find(&vm);return borrowed==external.end()?nullptr:borrowed->second.get();}
AnmVm* AnmRegistry::allocate(){
    // 池复用路径的键注册必须幂等（operator[]）：回滚 restore 会
    // nodes.clear() 后只按快照 entries 重建键——发散期新分配的槽
    // Node 残留在 pool 且仍在快照 free_slots 里，后续 allocate 复用
    // 该槽时 pool[slot] 非空、旧实现跳过 emplace → 悬空键 → submit
    // "no registry node"（M8 回滚重放实证）。operator[] 对已有键幂等。
    Node* entry=nullptr;if(!free_slots.empty()){const u32 slot=free_slots.back();free_slots.pop_back();if(!pool[slot]){pool[slot]=std::make_unique<Node>();pool[slot]->slot=slot;}entry=pool[slot].get();nodes[&entry->vm]=entry;}
    else{auto extra=std::make_unique<Node>();entry=extra.get();entry->serialization_ordinal=next_serialization_ordinal++;nodes[&entry->vm]=entry;overflow.emplace(entry,std::move(extra));}
    entry->active=true;entry->queued_deletion=false;entry->registered=false;entry->handle=0;entry->parent=nullptr;entry->children.clear();entry->vm.rotation_parent=entry->vm.creation_parent=nullptr;active_count++;return &entry->vm;
}
u32 AnmRegistry::submit(AnmVm& vm,u32 ordering){
    auto* entry=node(vm);
    if(!entry||entry->borrowed||!entry->active||entry->registered){
        // 拒绝原因细分（M8 回滚重放诊断：池残留状态定位）。
        error=entry?"Animation instance cannot be registered ("+
            std::string(!entry->active?"inactive":entry->registered?"registered":"borrowed")+")"
            :"Animation instance has no registry node";
        return 0;
    }
    // 句柄 = generation<<13 | slot（0x46f3f0：低 13 位槽、高 19 位代，代跳 0）。
    generation=(generation+1)&0x7ffff;if(!generation)generation=1;entry->handle=(generation<<13)|(entry->slot&0x1fff);entry->alternate=ordering&4;
    auto& list=groups[entry->alternate];if(ordering&2)entry->position=list.insert(list.begin(),entry);else entry->position=list.insert(list.end(),entry);entry->registered=true;
    // 副链插入清 0xc000（0046f820.c:65,71：bit2/bit1|bit2 两路插入后均清）。
    // port 在 submit 统一执行，同时覆盖 0x46f990 spawn_child（ins_501→ordering4，
    // 暂停菜单子体 117/118 等）：port 模板稳态是 0x4000，若不清 0，这些子体会
    // 被 00462450.c:308 的暂停门冻结；原版 0x46f990 未清位（其模板预热值
    // 0x8000，子体天然不受门影响），故此处清位是 port 侧必要的等价手段。
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
    if(!owner){auto borrowed=std::make_unique<Node>();borrowed->borrowed=&parent;borrowed->active=true;borrowed->serialization_ordinal=next_serialization_ordinal++;owner=borrowed.get();external.emplace(&parent,std::move(borrowed));}
    if(!owner->active){error="Animation parent inactive";return false;}
    for(auto* ancestor=owner;ancestor;ancestor=ancestor->parent)if(ancestor==entry){error="Animation parent cycle";return false;}
    entry->parent=owner;owner->children.push_front(entry);return true;
}
void AnmRegistry::collect(Node& entry){
    // Host bookkeeping must not occupy native render bit6: ins424 writes it.
    if (entry.queued_deletion) return;
    entry.queued_deletion = true;
    for (auto* child : entry.children) collect(*child);
    pending_deletion.push_back(&entry);
    entry.vm.rotation_parent = entry.vm.creation_parent = nullptr;
    entry.vm.visual.render_flags |= 0x20u;
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
    for(auto* child:entry.children){child->parent=nullptr;child->vm.rotation_parent=child->vm.creation_parent=nullptr;child->vm.visual.render_flags=child->vm.visual.render_flags|32u;}entry.children.clear();
    entry.vm.rotation_parent=entry.vm.creation_parent=nullptr;entry.vm.instruction_offset=-1;entry.active=entry.registered=entry.queued_deletion=false;entry.handle=0;active_count--;
    // A freed pool slot owns no script, sprite or allocated geometry. Keeping
    // these bindings after the bank unloads makes a later snapshot dereference
    // released storage. allocate callers install a complete template on reuse.
    entry.vm.script=nullptr;entry.vm.resource=entry.vm.sprite_resource=nullptr;
    entry.vm.sprite=nullptr;entry.vm.sprite_source=nullptr;entry.vm.pre_tick=nullptr;
    entry.vm.geometry={};
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
void AnmRegistry::retire(Node& entry){entry.vm.visual.render_flags=entry.vm.visual.render_flags|32;for(auto* child:entry.children)retire(*child);}
bool AnmRegistry::retire_children(AnmVm& vm){auto* entry=node(vm);if(!entry||!entry->active)return true;for(auto* child:entry->children)retire(*child);return true;}
// 单条链一遍（0x46f170 主链 / 0x46f240 副链同构）：待回收(0x20)不 tick 直接
// 回收；tick 非零回收；零则归桶。副链层 ∉{31,32} 钳到 31（0x46f240:31-33）。
bool AnmRegistry::tick_chain(u32 alternate,GameRandom& random,float rate){
    if(!alternate&&chain_entry_observer)chain_entry_observer(*this,false);
    auto next=groups[alternate].begin();while(next!=groups[alternate].end()){
        auto* entry=*next++;
        if(entry->vm.visual.render_flags&0x20){collect(*entry);continue;}
        if(entry->queued_deletion)continue;
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
void AnmRegistry::append_draw(AnmVm& vm){i32 layer=vm.visual.layer;if(layer<0||layer>30)return;layers[u32(layer)].push_back(&vm);}

// ---- M6：回滚快照面（capture/restore 专用实现）----
u32 AnmRegistry::pool_slot(const AnmVm& vm) const noexcept {
    // node() 查 nodes/external 两表；池内节点回 slot，池外（overflow/borrowed）
    // 返回 0x1fff 与 slot() 同义。
    return slot(const_cast<AnmVm&>(vm));
}
bool AnmRegistry::capture(SnapshotData& out,
                          const std::vector<const AnmVm*>& embedded) const {
    // 全节点序遍历：池内（pool 下标序）+ overflow + external。
    // 池外两类按可恢复的创建序排序；指针序在不同客户端/restore 后不稳定。
    // 快照内序号 = entries 下标。
    for (auto& layer : out.layers) layer.clear();
    out.free_slots = free_slots;
    out.generation = generation;
    out.active_count = active_count;
    out.next_serialization_ordinal = next_serialization_ordinal;
    out.borrowed_roots.clear();
    // 收集全部 Node*（池内 + overflow + external borrowed）。
    std::vector<const Node*> ordered;
    for (const auto& slot_node : pool)
        if (slot_node) ordered.push_back(slot_node.get());
    std::vector<const Node*> overflow_nodes;
    for (const auto& entry : overflow) overflow_nodes.push_back(entry.second.get());
    const auto by_ordinal=[](const Node* a,const Node* b) {
        return a->serialization_ordinal < b->serialization_ordinal;
    };
    std::sort(overflow_nodes.begin(), overflow_nodes.end(), by_ordinal);
    ordered.insert(ordered.end(), overflow_nodes.begin(), overflow_nodes.end());
    std::vector<const Node*> external_nodes;
    for (const auto& entry : external) external_nodes.push_back(entry.second.get());
    std::sort(external_nodes.begin(), external_nodes.end(), by_ordinal);
    ordered.insert(ordered.end(), external_nodes.begin(), external_nodes.end());
    // 节点 → 快照序号映射（指针比较，仅本函数内使用，不落盘）。
    // Pool slots already give stable direct lookup. Only overflow and
    // borrowed nodes need pointer lookup while translating this snapshot.
    std::array<u32, 0x1fff> pool_indices;
    pool_indices.fill(SnapshotData::kNoVm);
    std::unordered_map<const Node*, u32> external_indices;
    external_indices.reserve(overflow_nodes.size() + external_nodes.size());
    for (u32 i = 0; i < ordered.size(); ++i) {
        const auto* node = ordered[i];
        if (node->slot < pool.size()) pool_indices[node->slot] = i;
        else external_indices.emplace(node, i);
    }
    const auto index_of = [&](const Node* node) {
        if (!node) return SnapshotData::kNoVm;
        if (node->slot < pool.size() && pool[node->slot].get() == node)
            return pool_indices[node->slot];
        const auto found = external_indices.find(node);
        return found == external_indices.end() ? SnapshotData::kNoVm : found->second;
    };
    // Record each chain once. Scanning both chains for every pool slot makes
    // a checkpoint quadratic in the number of live effects/bullets.
    std::vector<std::array<u32, 2>> chain_indices(
        ordered.size(), {0xFFFFFFFFu, 0xFFFFFFFFu});
    for (u32 chain = 0; chain < 2; ++chain) {
        u32 position = 0;
        for (const Node* member : groups[chain]) {
            const auto index = index_of(member);
            if (index == SnapshotData::kNoVm) return false;
            chain_indices[index][chain] = position++;
        }
    }
    const auto vm_ref = [&](const AnmVm* vm) {
        if (!vm) return SnapshotData::kNoVm;
        return index_of(node(*const_cast<AnmVm*>(vm)));
    };
    // Checkpoint ring slots are reused. Assign into their existing entries so
    // ANM geometry and child vectors keep capacity across captures, including
    // inactive nodes; resize still removes entries when the source shrinks.
    out.entries.resize(ordered.size());
    for (u32 index = 0; index < ordered.size(); ++index) {
        const Node* node = ordered[index];
        SnapshotData::Entry& entry = out.entries[index];
        entry.vm = node->vm;
        entry.creation_parent = vm_ref(entry.vm.creation_parent);
        entry.rotation_parent = vm_ref(entry.vm.rotation_parent);
        entry.vm.creation_parent = entry.vm.rotation_parent = nullptr;
        entry.slot = node->slot;
        entry.handle = node->handle;
        entry.serialization_ordinal = node->serialization_ordinal;
        entry.active = node->active;
        entry.registered = node->registered;
        entry.alternate = node->alternate;
        entry.borrowed = node->borrowed;
        entry.frozen_root = node->frozen_root;
        entry.queued_deletion = node->queued_deletion;
        entry.in_overflow = node->slot >= pool.size();
        entry.parent = SnapshotData::kNoVm;
        if (node->parent) {
            entry.parent = index_of(node->parent);
        }
        entry.children.clear();
        for (const Node* child : node->children) {
            const auto child_index = index_of(child);
            if (child_index != SnapshotData::kNoVm) entry.children.push_back(child_index);
        }
        // 链序号：groups[alternate] 的列表位置（restore 按序重插）。
        entry.chain_index[0] = chain_indices[index][0];
        entry.chain_index[1] = chain_indices[index][1];
        if (node->borrowed) out.borrowed_roots.push_back(node->borrowed);
    }
    std::unordered_map<const AnmVm*, u32> embedded_index;
    for (u32 i = 0; i < embedded.size(); ++i)
        if (embedded[i]) embedded_index.emplace(embedded[i], i);
    for (size_t layer = 0; layer < layers.size(); ++layer) {
        for (const auto* vm : layers[layer]) {
            SnapshotData::DrawReference reference;
            const auto found = index_of(node(*const_cast<AnmVm*>(vm)));
            if (found != SnapshotData::kNoVm) reference.entry = found;
            else {
                const auto owned = embedded_index.find(vm);
                if (owned == embedded_index.end()) return false;
                reference.embedded = owned->second;
                reference.original = vm;
            }
            out.layers[layer].push_back(reference);
        }
    }
    return true;
}
bool AnmRegistry::restore(const SnapshotData& source,
                          const std::unordered_map<const AnmVm*, AnmVm*>& remapped) {
    // Validate every draw binding before replacing the live registry.
    for (const auto& layer : source.layers) for (const auto& reference : layer) {
        if (reference.entry != SnapshotData::kNoVm) {
            if (reference.embedded != SnapshotData::kNoVm || reference.entry >= source.entries.size()) {
                error = "Animation snapshot draw entry invalid"; return false;
            }
        } else {
            const auto binding = remapped.find(reference.original);
            if (reference.embedded == SnapshotData::kNoVm || binding == remapped.end() || !binding->second) {
                error = "Animation snapshot embedded draw binding missing"; return false;
            }
        }
    }
    // 前置：清空当前全部节点（直毁语义，无 0x20 延迟——快照恢复是确认
    // 栅栏后的整场替换，不走帧内回收路径）。
    for (const auto& slot_node : pool) if (slot_node) { slot_node->active=false; slot_node->registered=false; slot_node->handle=0; slot_node->parent=nullptr; slot_node->children.clear(); slot_node->vm=AnmVm{}; }
    std::array<bool, 0x1fff> captured_slots{};
    for (const auto& entry : source.entries)
        if (!entry.borrowed && !entry.in_overflow && entry.slot < pool.size())
            captured_slots[entry.slot] = true;
    for (u32 slot = 0; slot < pool.size(); ++slot)
        if (!captured_slots[slot]) pool[slot].reset();
    nodes.clear();
    overflow.clear();
    external.clear();
    pending_deletion.clear();
    for (auto& bucket : layers) bucket.clear();
    for (auto& group : groups) group.clear();
    free_slots.clear();
    generation = source.generation;
    next_serialization_ordinal = source.next_serialization_ordinal;
    active_count = 0;
    // 第一遍：按快照重建节点存储（池内复用同槽位——分配顺序一致；
    // 池外节点进 overflow；borrowed 根按指针身份回 external）。
    std::vector<Node*> restored(source.entries.size(), nullptr);
    // borrowed 根按 capture 时的宿主指针序（borrowed_roots 与 entries 中
    // borrowed=true 的条目按序对应）回 external 表——宿主对象由外层持有，
    // 快照存活期不卸载（红线）。
    {
        u32 borrowed_index = 0;
        for (u32 i = 0; i < source.entries.size(); ++i) {
            const SnapshotData::Entry& entry = source.entries[i];
            if (!entry.borrowed) continue;
            if (borrowed_index >= source.borrowed_roots.size()) {
                error = "Animation snapshot borrowed root missing";
                return false;
            }
            auto node = std::make_unique<Node>();
            AnmVm* old_root = source.borrowed_roots[borrowed_index++];
            const auto replacement = remapped.find(old_root);
            if (entry.frozen_root) {
                node->vm = entry.vm;
                node->borrowed = &node->vm;
            } else {
                node->borrowed = replacement == remapped.end() ? old_root : replacement->second;
            }
            node->slot = 0x1fff;
            node->handle = entry.handle;
            node->serialization_ordinal = entry.serialization_ordinal;
            node->active = entry.active;
            node->registered = entry.registered;
            node->alternate = entry.alternate;
            node->frozen_root = entry.frozen_root;
            node->queued_deletion = entry.queued_deletion;
            Node* raw = node.get();
            external.emplace(node->borrowed, std::move(node));
            restored[i] = raw;
        }
    }
    for (u32 i = 0; i < source.entries.size(); ++i) {
        const SnapshotData::Entry& entry = source.entries[i];
        if (entry.borrowed) continue;
        Node* node = nullptr;
        if (!entry.in_overflow && entry.slot < pool.size()) {
            if (!pool[entry.slot]) pool[entry.slot] = std::make_unique<Node>();
            node = pool[entry.slot].get();
            node->slot = entry.slot;
            nodes[&node->vm] = node;
        } else {
            auto extra = std::make_unique<Node>();
            node = extra.get();
            node->slot = 0x1fff;
            nodes.emplace(&node->vm, node);
            overflow.emplace(node, std::move(extra));
        }
        node->vm = entry.vm;
        node->handle = entry.handle;
        node->serialization_ordinal = entry.serialization_ordinal;
        node->active = entry.active;
        node->registered = entry.registered;
        node->alternate = entry.alternate;
        node->frozen_root = entry.frozen_root;
        node->queued_deletion = entry.queued_deletion;
        if (node->active) ++active_count;
        restored[i] = node;
    }
    // 第二遍：父子链（快照内序号 → 节点指针）。
    for (u32 i = 0; i < source.entries.size(); ++i) {
        const SnapshotData::Entry& entry = source.entries[i];
        Node* node = restored[i];
        if (!node) continue;
        const auto vm_at = [&](u32 index) -> AnmVm* {
            if (index >= restored.size() || !restored[index]) return nullptr;
            auto* target = restored[index];
            return target->borrowed ? target->borrowed : &target->vm;
        };
        if (!entry.borrowed || entry.frozen_root) {
            node->vm.creation_parent = vm_at(entry.creation_parent);
            node->vm.rotation_parent = vm_at(entry.rotation_parent);
        }
        if (entry.parent != 0xFFFFFFFFu && entry.parent < restored.size())
            node->parent = restored[entry.parent];
        for (const u32 child : entry.children)
            if (child < restored.size() && restored[child]) node->children.push_back(restored[child]);
    }
    // 第三遍：链表（按快照链序号重插；position 迭代器语义：insert(end)）。
    for (u32 chain = 0; chain < 2; ++chain) {
        // 收集该链的全部节点并按 chain_index 排序（0xFFFFFFFF = 不在该链）。
        std::vector<std::pair<u32, Node*>> members;
        for (u32 i = 0; i < source.entries.size(); ++i) {
            if (source.entries[i].chain_index[chain] == 0xFFFFFFFFu) continue;
            if (!restored[i]) continue;
            members.emplace_back(source.entries[i].chain_index[chain], restored[i]);
        }
        std::sort(members.begin(), members.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });
        for (auto& [position, node] : members) {
            node->position = groups[chain].insert(groups[chain].end(), node);
        }
    }
    for (size_t layer = 0; layer < source.layers.size(); ++layer) {
        for (const auto& reference : source.layers[layer]) {
            AnmVm* vm = nullptr;
            if (reference.entry != SnapshotData::kNoVm) {
                if (reference.embedded != SnapshotData::kNoVm ||
                    reference.entry >= restored.size() || !restored[reference.entry]) {
                    error = "Animation snapshot draw entry invalid"; return false;
                }
                const auto* entry = restored[reference.entry];
                vm = entry->borrowed ? entry->borrowed : const_cast<AnmVm*>(&entry->vm);
            } else {
                const auto binding = remapped.find(reference.original);
                if (reference.embedded == SnapshotData::kNoVm || binding == remapped.end() || !binding->second) {
                    error = "Animation snapshot embedded draw binding missing"; return false;
                }
                vm = binding->second;
            }
            layers[layer].push_back(vm);
        }
    }
    free_slots = source.free_slots;
    return true;
}
}
