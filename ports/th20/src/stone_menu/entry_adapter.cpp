#include "stone.hpp"
#include "../item_system/collect.hpp"
#include "../startup_scene/startup.hpp"
namespace th20::source::item::unrecovered {
runtime::CallbackOwner* special_owner_0051b960(){return stone_menu::controller;}
}
namespace th20::source::startup::unrecovered {
runtime::CallbackOwner* create_resource_0051cc20(int index){return stone_menu::create_controller(index);}
void release_resource_0051b6d0(){stone_menu::release();}
}
