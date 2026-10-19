#include "special.hpp"
#include "../item_system/rewards.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/enemy_frame.hpp"
namespace th20::source::item::unrecovered {special_state::Controller* special_state_00513dd0(){return special_state::controller;}}
namespace th20::source::gameplay::unrecovered {
void update_special_objects_005120d0(){special_state::update(*special_state::controller);}
void draw_enemy_overlay_00512aa0(){special_state::draw(*special_state::controller);}
void create_auxiliary_owner(){special_state::create();}
}
