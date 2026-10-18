#pragma once
#include "ItemManager.hpp"
namespace th15 {
class ItemCheckpoint {
    struct Slot {ItemState state;std::unique_ptr<AnmVm> body,arrow;bool arrow_ended=false;};
    ItemManager& manager;std::vector<Slot> slots;float scale=1;bool available=false;
public:
    std::string error;explicit ItemCheckpoint(ItemManager& m):manager(m),slots(ItemManager::pool_size){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&,AnmManager&);
    bool read_file(const u8*,u32,u32& consumed,AnmManager&);
    const ItemState* state(u32 id)const noexcept{return id&&id<=slots.size()?&slots[id-1].state:nullptr;}
    const AnmVm* animation(u32 id,bool arrow)const noexcept{return id&&id<=slots.size()?(arrow?slots[id-1].arrow.get():slots[id-1].body.get()):nullptr;}
};
}
