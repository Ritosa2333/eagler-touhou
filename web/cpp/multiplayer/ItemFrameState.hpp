#pragma once
#include "../game/ItemManager.hpp"
namespace th15::multiplayer {
class ItemFrameState {
    struct Data;
    std::unique_ptr<Data> saved_;
public:
    mutable std::string error;
    ItemFrameState();~ItemFrameState();
    ItemFrameState(ItemFrameState&&)noexcept;
    ItemFrameState& operator=(ItemFrameState&&)noexcept;
    void discard()noexcept;
    static void append_roots(ItemManager&,const AnmRegistry&,std::vector<AnmVm*>&);
    bool capture(ItemManager&);
    bool restore(ItemManager&)const;
};
}
