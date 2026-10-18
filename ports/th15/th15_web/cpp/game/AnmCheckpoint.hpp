#pragma once
#include "AnmRegistry.hpp"
namespace th15 {
class AnmManager;
// The original checkpoints clone only animations owned by snapshotted game
// objects, into a separate pool. They do not copy or rewind the live registry.
class AnmCheckpoint {
    struct Entry {AnmVm vm;u32 handle=0;Entry* parent=nullptr;std::vector<Entry*> children;};
    std::vector<std::unique_ptr<Entry>> entries;std::vector<Entry*> order;u32 generation=0;
    Entry* capture_tree(const AnmVm&,const AnmRegistry&,Entry* parent);
    u32 restore_tree(const Entry&,AnmRegistry&,AnmVm* parent);
    bool write_entry(const Entry&,const AnmManager&,std::vector<u8>&,bool sibling,u32 depth);
    Entry* read_entry(const u8*,u32,u32&,AnmManager&,Entry*,u32 depth);
public:
    struct Mark {size_t entries,order;u32 generation;};
    Mark mark()const noexcept{return {entries.size(),order.size(),generation};}
    void rollback(Mark m){order.erase(order.begin(),order.begin()+(order.size()-m.order));entries.resize(m.entries);generation=m.generation;}
    std::string error;
    void clear(){order.clear();entries.clear();error.clear();}
    u32 capture(const AnmVm*,const AnmRegistry&);u32 capture(u32 handle,const AnmRegistry& r){return capture(r.find(handle),r);}
    const AnmVm* find(u32 handle)const noexcept;
    u32 restore(u32 handle,AnmRegistry&);
    u32 count()const noexcept{return entries.size();}
    u32 ordered_handle(u32 index)const noexcept{return index<order.size()?order[index]->handle:0;}
    u32 child_handle(u32 handle,u32 index)const noexcept;
    u32& generation_counter()noexcept{return generation;}
    bool write_tree(u32 handle,const AnmManager&,std::vector<u8>&);
    u32 read_tree(const u8*,u32,u32& consumed,AnmManager&);
};
}
