#pragma once
#include "PlayerMotion.hpp"
#include "Interpolation.hpp"
#include "AnmVm.hpp"
namespace th15 {
struct PlayerBox {Vec3 minimum{},maximum{};};
class PlayerBounds {
public:
    Vec3 hit_half_size{},item_half_size{},graze_half_size{};
    PlayerBox hit,item_near,graze,item_full;ScalarInterpolation enlargement;
    void update(PlayerMotion&,const Timer& age,AnmVm&,float rate);
};
}
