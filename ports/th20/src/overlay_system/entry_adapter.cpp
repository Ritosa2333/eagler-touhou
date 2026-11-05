#include "environment.hpp"
#include "frame.hpp"
#include "../player_entity/firing.hpp"
#include "../player_entity/power.hpp"
#include "../stone_menu/update.hpp"
#include "../item_system/rewards.hpp"
#include "../gameplay/loading_dependencies.hpp"
namespace th20::source::overlay {void fire_player_shots(player_entity::ShotController& owner,int frame,int secondary,int pattern){player_entity::fire_shots(owner,frame,secondary,pattern);}}
namespace th20::source::player_entity::unrecovered {
int overlay_script_variant_00534130(runtime::CallbackOwner& owner){return overlay::script_variant(static_cast<overlay::WeaponStoneInf&>(owner));}
sprite::Vec2 overlay_option_offset_004ff760(runtime::CallbackOwner& owner,int level,int index){return *overlay::option_offset(static_cast<overlay::WeaponStoneInf&>(owner),level,index,true);}
sprite::Vec2 overlay_option_offset_004ff630(runtime::CallbackOwner& owner,int level,int index){return *overlay::option_offset(static_cast<overlay::WeaponStoneInf&>(owner),level,index,false);}
void overlay_initialize_option_00533180(runtime::CallbackOwner& owner,Option& option,int index){overlay::initialize_option(static_cast<overlay::WeaponStoneInf&>(owner),option,index);}
}
namespace th20::source::stone_menu::unrecovered {void refresh_overlay_selection(runtime::CallbackOwner& owner,int character){overlay::refresh_selection(static_cast<overlay::WeaponStoneInf&>(owner),character,overlay::environment());}}
namespace th20::source::item::unrecovered {void start_special_phase_00534d00(){overlay::start_phase(*overlay::controller());}}
namespace th20::source::gameplay::unrecovered {void configure_overlay(int character){overlay::refresh_selection(*overlay::controller(),character,overlay::environment());}}
namespace th20::source::startup::unrecovered {
runtime::CallbackOwner* create_resource_00534dd0(int view){return overlay::create_controller(view);}
void release_resource_00534110(int view){overlay::release(view);}
}
