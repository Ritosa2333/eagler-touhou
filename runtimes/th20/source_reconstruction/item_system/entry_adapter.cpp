#include "item.hpp"
#include "../damage_regions/hit_callbacks.hpp"
#include "../gameplay/loading_dependencies.hpp"
namespace th20::source::damage::unrecovered {
void spawn_item_004c45b0(void* owner,const sprite::Vec3& position,int count,int type){item::spawn_many(*static_cast<item::ItemInf*>(owner),position,count,type);}
}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_004c5010(int index){return item::create_controller(index);}
}
