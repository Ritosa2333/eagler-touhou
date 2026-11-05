#pragma once
#include "item.hpp"
namespace th20::source::item {
void collect_item(Item&);
void activate_special_item(Item&); //4c4420
void activate_special_item(Item&,Environment&); //4c4420
void spawn_item_effect(Item&); //4c42c0
namespace unrecovered {
runtime::CallbackOwner* special_owner_0051b960();
}
}
