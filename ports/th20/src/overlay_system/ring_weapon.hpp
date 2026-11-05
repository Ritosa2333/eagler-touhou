#pragma once
#include "weapon_services.hpp"
namespace th20::source::overlay {
template<int Character>class RingWeapon final:public StandardWeapon { //531100/532270, stone6
public:
 std::uint32_t handle=0;
 recovered::Timer timer_3c{},timer_4c{};
 void reset() override; //531200
 void shoot_main(int,int,int) override; //531260
 void initialize_focused_option(player_entity::Option*,int) override; //5312b0
 void initialize_passive() override; //531400
 int script_variant() override{return stone_id;}
 void start_phase() override; //531460
 int update_phase() override; //5312d0
 int end_phase() override; //5311e0
 int cancel_phase() override; //531550
};
static_assert(sizeof(RingWeapon<0>)==0x5c&&offsetof(RingWeapon<0>,handle)==0x38);
void anchored_option(player_entity::Option&); //5322c0
int update_ring_animation(sprite::Animation&); //531150/531050
}
