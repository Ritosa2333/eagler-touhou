#pragma once
#include "weapon_services.hpp"
namespace th20::source::overlay {
template<int Character>struct CloudTail;
template<>struct CloudTail<0>{sprite::Vec3 position{};float radius=0;sprite::Interpolation<float> interpolation{};};
template<>struct CloudTail<1>{float radius=0;sprite::Interpolation<float> interpolation{};sprite::Vec3 position{};};
template<int Character>class CloudWeapon final:public StandardWeapon { //531650/531cc0 stone7
public:
 std::uint32_t handle=0;recovered::Timer timer_3c{},timer_4c{};CloudTail<Character> tail;
 void reset() override; //531760/531de0
 void shoot_main(int,int,int) override; //531800
 void initialize_passive() override; //5319f0
 int script_variant() override{return stone_id;}
 void start_phase() override; //531a50/532030
 int update_phase() override; //531850/531e80
 int end_phase() override; //5311e0
 int cancel_phase() override; //531550
 const sprite::Vec3* phase_position() override{return &tail.position;} //531bb0/5321a0
 bool passive_active() override; //531bd0
};
static_assert(sizeof(CloudWeapon<0>)==0x98&&sizeof(CloudWeapon<1>)==0x98&&offsetof(CloudWeapon<0>,tail)==0x5c);
template<int Character>int update_cloud_animation(sprite::Animation&); //5316c0/531d30
}
