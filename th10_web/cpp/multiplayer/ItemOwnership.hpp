#pragma once
#include "../game/Item.hpp"
#include <cstdint>

namespace th10::multiplayer {
struct ItemOwnership {
    std::int8_t homing=-1;
    std::int8_t recipient=-1;
};
struct ItemCandidate {
    bool eligible=false,auto_collect=false,focused=false;
    Vec3 position{};
    ItemRegion pickup{},slow{},fast{};
};
// A targeted transfer keeps its recipient through temporary death. Ordinary
// homing loses its claim when the collector becomes unavailable. Equal-distance
// claims resolve by seat, independent of local player or packet arrival order.
int select_item_collector(ItemOwnership&,const Item&,const ItemCandidate*,u32 count) noexcept;
}
