#pragma once
#include "weapon_services.hpp"
namespace th20::source::overlay {
struct ShieldState {std::uint32_t animation_handles[4],phase_animation,damage_handles[4];};
static_assert(sizeof(ShieldState)==0x24);
ShieldState& shield_state(int character) noexcept; // actual BSS5c6960..6984 /5c69cc..69f0
template<int Character>class ShieldWeapon final:public StandardWeapon { //534ee0/538260 stone4
public:
 recovered::Timer timer_38{};
 void reset() override; //5350b0/538370
 void shoot_main(int,int,int) override; //531260
 void shoot_focused(int,int,int) override; //5351f0/538490
 void shoot_unfocused(int,int,int) override; //535370/5385b0
 void activate_main() override; //535130/5383f0
 void activate_focused() override{activate_main();}void activate_unfocused() override{activate_main();}
 void initialize_focused_option(player_entity::Option*,int) override; //5351d0 or originalbaseNOP
 void initialize_unfocused_option(player_entity::Option*,int) override; //535350 or originalbaseNOP
 void initialize_passive() override; //535310
 int script_variant() override{return stone_id;}
 void start_phase() override; //535680/5388c0
 int update_phase() override; //535260/538500
 int end_phase() override; //534fd0/538290
 int cancel_phase() override; //535740/538980
 void update_options(int level); //5353e0/538620
};
static_assert(sizeof(ShieldWeapon<0>)==0x48);
int update_shield_animation(sprite::Animation&); //534e30/534f10
}
