#pragma once
#include "weapon.hpp"
namespace th20::source::overlay {
// Strategies with the exact common 0x38-byte storage. Character variants have
// separate C++ types even when their recovered method tables are identical.
template<int Character> class FocusBoostWeapon final:public StandardWeapon {
public:void initialize_passive() override;bool passive_active() override{return focused_shooting();} //5323e0/532440
};
template<int Character> class FlagBoostWeapon final:public StandardWeapon {
public:void initialize_passive() override;int script_variant() override {if constexpr(Character==0)return stone_id;else return 0;} //532530;40ff90/412540
};
template<int Character> class OrbitWeapon final:public StandardWeapon {
public:
 void initialize_focused_option(player_entity::Option*,int) override; //52fc80/530d60
 void initialize_unfocused_option(player_entity::Option*,int) override; //52fee0/530df0
 void initialize_passive() override; //52fe80
 int script_variant() override{return stone_id;} //40ff90
 bool passive_active() override{return unfocused_shooting();} //530560
};
void orbit_option(player_entity::Option&,int character); //530010/530e80
Weapon* create_weapon(int character,int stone); //18 actual factory entries5b0ab0
void destroy_weapon(Weapon*); //532740; common nonvirtual destructor532950
}
