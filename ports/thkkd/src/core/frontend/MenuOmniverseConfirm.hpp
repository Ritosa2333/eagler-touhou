#pragma once
#include "core/frontend/MenuExtraConfirm.hpp"
namespace kaikeidou {
inline void update_menu_omniverse_confirm(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api){menu_start_detail::update_special_confirm(c,save,precision,api,5);}
inline void present_menu_omniverse_confirm(MenuStartContext& c,const SaveData& save,unsigned precision,MenuStartServices& api){
 api.present_component(MenuStartComponent::routine_5e71e0);api.present_component(MenuStartComponent::routine_5e6ff0);update_menu_omniverse_confirm(c,save,precision,api);menu_start_detail::finish_wrapper(api,0x6c79ec,0x6c79e0);
}
}
