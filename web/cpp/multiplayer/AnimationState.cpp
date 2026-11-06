#include "FrameImage.hpp"
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
        AnmVm value;
        u32 handle;
        bool active,registered,alternate,frozen;
        std::vector<Node*> children;
    };
    std::vector<SavedNode> saved;
    std::unordered_set<Node*> seen;
    std::array<std::vector<Node*>,2> order;
    const auto remember=[&](Node* n){
        if(!n||!seen.insert(n).second)return;
        auto& row=saved.emplace_back(SavedNode{n,n->borrowed,n->borrowed?*n->borrowed:n->vm,n->handle,
            n->active,n->registered,n->alternate,n->frozen_root,{}});
        for(auto* child:n->children)row.children.push_back(child);
    };
    for(u32 group=0;group<groups.size();++group){
        for(auto* n:groups[group]){order[group].push_back(n);remember(n);}
    }
    for(const auto& entry:external)remember(entry.second.get());
    for(const auto& layer:layers)for(auto* vm:layer)remember(node(*vm));
    image.save(generation,active_count,error);
    image.restore_with([this,saved=std::move(saved),order=std::move(order),borrowed=external,
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
            (n.borrowed?*n.borrowed:n.vm)=row.value;
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
