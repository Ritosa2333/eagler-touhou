#pragma once
#include "weapon_services.hpp"
namespace th20::source::overlay {
template<int Character>class BarWeapon final:public StandardWeapon { //5306a0/530b90, concrete stone3
public:
 std::uint32_t handle=0;float bar_y=0;
 void reset() override; //530700
 void shoot_main(int,int,int) override; //530750, does not set owner's main_shooting
 void initialize_passive() override; //5309d0
 int script_variant() override{return stone_id;} //40ff90
 void start_phase() override; //530a00/530bd0
 int update_phase() override; //5307e0
 int end_phase() override; //5306e0
 int cancel_phase() override; //530ac0
};
static_assert(sizeof(BarWeapon<0>)==0x40&&offsetof(BarWeapon<0>,handle)==0x38);
}
