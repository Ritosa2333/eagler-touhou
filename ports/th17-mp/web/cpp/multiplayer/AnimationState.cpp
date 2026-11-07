#include "FrameImage.hpp"
#include "AnmCheckpoint.hpp"
#include "../game/AnmManager.hpp"
#include <algorithm>
#include <unordered_set>

namespace th17 {
bool AnmManager::save_rollback(multiplayer::FrameImage& image){
    if(!registry.save_rollback(image))return false;
    // Retired banks can still back scripts in an earlier registry image.
    ++rollback_leases;
    auto lease=std::shared_ptr<AnmManager>(this,[](AnmManager* owner){--owner->rollback_leases;});
    image.restore_with([lease=std::move(lease)]{return true;});
    image.save(error,rate);
    return true;
}

bool AnmRegistry::save_rollback(multiplayer::FrameImage& image){
    if(!pending_deletion.empty()||active_count!=groups[0].size()+groups[1].size())return false;

    struct SavedNode {
        Node* owner;
        AnmVm* borrowed;
        multiplayer::AnmCheckpoint value;
        u32 handle;
        bool active,registered,alternate,frozen;
        u32 child_start,child_count;
        SavedNode(Node& n,multiplayer::AnmCheckpoint::Curves& curves,std::vector<Node*>& children):
            owner(&n),borrowed(n.borrowed),value(borrowed?*borrowed:n.vm,n.rollback_curves,curves),
            handle(n.handle),active(n.active),registered(n.registered),alternate(n.alternate),frozen(n.frozen_root),
            child_start(u32(children.size())),child_count(n.children.size()){
            for(auto* child:n.children)children.push_back(child);
        }
    };
    struct Snapshot {
        std::vector<Node*> selected,children,free_overflow;
        std::vector<SavedNode> saved;
        multiplayer::AnmCheckpoint::Curves curves;
        std::unordered_map<AnmVm*,std::shared_ptr<Node>> borrowed;
        std::vector<u16> free_slots;
        std::array<std::vector<AnmVm*>,44> layers;

        void reset(){
            // A cancellation burst can fill every history slot. Retain
            // ordinary capture capacity and release storage from large bursts.
            size_t bytes=saved.capacity()*sizeof(SavedNode)+curves.changing.capacity()*sizeof(AnmInterpolators)+
                (selected.capacity()+children.capacity()+free_overflow.capacity()+borrowed.bucket_count())*sizeof(Node*)+
                free_slots.capacity()*sizeof(u16);
            for(const auto& layer:layers)bytes+=layer.capacity()*sizeof(AnmVm*);
            if(bytes>256*1024){*this=Snapshot{};return;}
            selected.clear();children.clear();saved.clear();curves.changing.clear();curves.last_resting.reset();
            borrowed.clear();free_slots.clear();free_overflow.clear();
            for(auto& layer:layers)layer.clear();
        }
    };
    auto& snapshot=image.reuse<Snapshot>();
    auto& selected=snapshot.selected;
    std::array<u32,(slot_mask+31)/32> seen_slots{};
    std::unordered_set<Node*> seen_other;
    selected.reserve(active_count+external.size());
    const auto remember=[&](Node* n){
        if(!n)return;
        if(n->slot<slot_mask){
            auto& word=seen_slots[n->slot/32];
            const auto bit=u32(1)<<(n->slot%32);
            if(word&bit)return;
            word|=bit;
        }else if(!seen_other.insert(n).second)return;
        selected.push_back(n);
    };
    for(const auto& group:groups)for(auto* n:group)remember(n);
    for(const auto& entry:external)remember(entry.second.get());
    for(const auto& layer:layers)for(auto* vm:layer)remember(node(*vm));
    // Layers can still reference nodes retired after their draw list was
    // built. Count those before allocating the large VM records.
    auto& saved=snapshot.saved;
    auto& children=snapshot.children;
    auto& curves=snapshot.curves;
    size_t child_count=0,changing=0;
    for(const auto* n:selected){
        child_count+=n->children.size();
        changing+=multiplayer::AnmCheckpoint::changing(n->borrowed?*n->borrowed:n->vm);
    }
    saved.reserve((selected.size()+63)/64*64);
    children.reserve(child_count);
    curves.changing.reserve(changing);
    for(auto* n:selected)saved.emplace_back(*n,curves,children);
    snapshot.borrowed=external;
    snapshot.free_slots=free_slots;
    snapshot.free_overflow=free_overflow;
    snapshot.layers=layers;
#if TH17_ROOM_BENCHMARK
    image.payload[image.Animation]=u32(saved.capacity()*sizeof(SavedNode)+children.capacity()*sizeof(Node*)+curves.changing.capacity()*sizeof(AnmInterpolators));
    image.unused_capacity+=u32((saved.capacity()-saved.size())*sizeof(SavedNode));
#endif
    image.save(generation,active_count,next_slot,error);
    image.restore_with([this,checkpoint=&snapshot]{
        for(auto& group:groups){
            for(auto* n:group){n->children.clear();n->parent=nullptr;n->active=n->registered=false;n->handle=0;}
            group.clear();
        }
        for(auto& entry:external)entry.second->children.clear();
        external=checkpoint->borrowed;
        overflow_handles.clear();
        pending_deletion.clear();
        free_slots=checkpoint->free_slots;
        free_overflow=checkpoint->free_overflow;
        // Extra nodes allocated by the abandoned timeline stay recyclable.
        // Snapshot addresses remain valid while the registry is alive.
        for(const auto& entry:overflow){
            auto* n=entry.first;
            if(!n->active&&std::find(this->free_overflow.begin(),this->free_overflow.end(),n)==this->free_overflow.end())
                this->free_overflow.push_back(n);
        }
        for(const auto& row:checkpoint->saved){
            auto& n=*row.owner;
            n.borrowed=row.borrowed;
            row.value.restore(n.borrowed?*n.borrowed:n.vm,n.rollback_curves,checkpoint->curves);
            n.handle=row.handle;n.active=row.active;n.registered=row.registered;n.alternate=row.alternate;n.frozen_root=row.frozen;
            n.parent=nullptr;
        }
        auto& free=this->free_overflow;
        free.erase(std::remove_if(free.begin(),free.end(),[](const Node* n){return n->active;}),free.end());
        // Saved nodes begin with both registration groups in queue order.
        for(const auto& row:checkpoint->saved)if(row.registered){
            auto* n=row.owner;
            auto& list=groups[row.alternate];
            n->position=list.insert(list.end(),n->group_link);
            if(n->slot==slot_mask)overflow_handles.emplace(n->handle,n);
        }
        for(const auto& row:checkpoint->saved){
            auto& n=*row.owner;
            for(u32 i=row.child_start;i<row.child_start+row.child_count;++i){
                auto* child=checkpoint->children[i];
                child->parent=&n;child->child_position=n.children.insert(n.children.end(),child->child_link);
            }
        }
        layers=checkpoint->layers;
        return true;
    });
    return true;
}
}
