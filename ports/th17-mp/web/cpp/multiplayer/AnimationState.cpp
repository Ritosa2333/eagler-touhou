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
        std::vector<Node*> children;
        SavedNode(Node& n,multiplayer::AnmCheckpoint::Curves& curves):
            owner(&n),borrowed(n.borrowed),value(borrowed?*borrowed:n.vm,n.rollback_curves,curves),
            handle(n.handle),active(n.active),registered(n.registered),alternate(n.alternate),frozen(n.frozen_root){
            children.reserve(n.children.size());
            for(auto* child:n.children)children.push_back(child);
        }
    };
    std::vector<Node*> selected;
    std::array<u32,(slot_mask+31)/32> seen_slots{};
    std::unordered_set<Node*> seen_other;
    std::array<std::vector<Node*>,2> order;
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
    for(u32 group=0;group<groups.size();++group){
        order[group].reserve(groups[group].size());
        for(auto* n:groups[group]){order[group].push_back(n);remember(n);}
    }
    for(const auto& entry:external)remember(entry.second.get());
    for(const auto& layer:layers)for(auto* vm:layer)remember(node(*vm));
    // Layers can still reference nodes retired after their draw list was
    // built. Count those before allocating the large VM records.
    std::vector<SavedNode> saved;
    multiplayer::AnmCheckpoint::Curves curves;
    saved.reserve(selected.size());
    curves.changing.reserve(std::count_if(selected.begin(),selected.end(),[](const Node* n){
        return multiplayer::AnmCheckpoint::changing(n->borrowed?*n->borrowed:n->vm);
    }));
    for(auto* n:selected)saved.emplace_back(*n,curves);
#if TH17_ROOM_BENCHMARK
    image.payload[image.Animation]=u32(saved.capacity()*sizeof(SavedNode)+curves.changing.capacity()*sizeof(AnmInterpolators));
    image.unused_capacity+=u32((saved.capacity()-saved.size())*sizeof(SavedNode));
#endif
    image.save(generation,active_count,next_slot,error);
    image.restore_with([this,saved=std::move(saved),curves=std::move(curves),order=std::move(order),borrowed=external,
                        free_slots=free_slots,free_overflow=free_overflow,layers=layers]{
        for(auto& group:groups){
            for(auto* n:group){n->children.clear();n->parent=nullptr;n->active=n->registered=false;n->handle=0;}
            group.clear();
        }
        for(auto& entry:external)entry.second->children.clear();
        external=borrowed;
        overflow_handles.clear();
        pending_deletion.clear();
        this->free_slots=free_slots;
        this->free_overflow=free_overflow;
        // Extra nodes allocated by the abandoned timeline stay recyclable.
        // Snapshot addresses remain valid while the registry is alive.
        for(const auto& entry:overflow){
            auto* n=entry.first;
            if(!n->active&&std::find(this->free_overflow.begin(),this->free_overflow.end(),n)==this->free_overflow.end())
                this->free_overflow.push_back(n);
        }
        for(const auto& row:saved){
            auto& n=*row.owner;
            n.borrowed=row.borrowed;
            row.value.restore(n.borrowed?*n.borrowed:n.vm,n.rollback_curves,curves);
            n.handle=row.handle;n.active=row.active;n.registered=row.registered;n.alternate=row.alternate;n.frozen_root=row.frozen;
            n.parent=nullptr;
        }
        auto& free=this->free_overflow;
        free.erase(std::remove_if(free.begin(),free.end(),[](const Node* n){return n->active;}),free.end());
        for(u32 group=0;group<groups.size();++group){
            auto& list=groups[group];
            for(auto* n:order[group]){
                n->position=list.insert(list.end(),n->group_link);
                if(n->slot==slot_mask)overflow_handles.emplace(n->handle,n);
            }
        }
        for(const auto& row:saved)for(auto* child:row.children){
            auto& n=*row.owner;
            child->parent=&n;child->child_position=n.children.insert(n.children.end(),child->child_link);
        }
        this->layers=layers;
        return true;
    });
    return true;
}
}
