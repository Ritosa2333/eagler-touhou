#pragma once
#include "weapon_services.hpp"
namespace th20::source::overlay {
// Original source diagnostic: weaponyellow.cpp, StoneYellowReimuInf.
struct YellowState {std::uint32_t animation_handles[4],phase_animation,damage_handles[4];};
static_assert(sizeof(YellowState)==0x24);
YellowState& yellow_state(int character) noexcept; //5c6984/5c69a8, distinct actual BSS objects
void update_yellow_option(player_entity::Option&,int character); //536ac0/537ec0
template<int Character>class YellowWeapon final:public StandardWeapon {
public:
 recovered::Timer passive_timer{},end_timer{};
 std::int32_t cooldown=0;
 void reset() override;
 void shoot_main(int,int,int) override;
 void shoot_focused(int,int,int) override;
 void shoot_unfocused(int,int,int) override;
 void activate_main() override;
 void activate_focused() override;
 void activate_unfocused() override{activate_focused();}
 void initialize_focused_option(player_entity::Option*,int) override;
 void initialize_unfocused_option(player_entity::Option*,int) override;
 void initialize_passive() override;
 void update_main() override;
 void update_passive() override;
 int script_variant() override{return stone_id;} //40ff90
 void start_phase() override;
 int update_phase() override;
 int end_phase() override;
 void update_options(int level); //536610/537aa0
 int cancelled_bullet(const sprite::Vec3&,bool passive_callback); //535c50/535ca0
};
static_assert(sizeof(YellowWeapon<0>)==0x5c&&offsetof(YellowWeapon<0>,cooldown)==0x58);
}
