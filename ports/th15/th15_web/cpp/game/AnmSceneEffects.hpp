#pragma once
#include "AnmManager.hpp"
namespace th15 {
// Native effect descriptors attach state and callbacks to an existing ANM VM.
// Gathering particles remain independent registry objects with logical handles.
class AnmSceneEffects {
 AnmManager& animations;Rng& game;Rng& visual;i32 effect_bank;
 std::function<bool(AnmVm&,i32)> previous_create;std::function<i32(AnmVm&,float)> previous_update;
 std::function<bool(AnmVm&,i32,float)> previous_interrupt;
public:
 AnmSceneEffects(AnmManager&,Rng& game,Rng& visual,i32 effect_bank);
 ~AnmSceneEffects();
 bool configure(AnmVm&,i32 kind);i32 update(AnmVm&,float rate);
 bool interrupt(AnmVm&,i32 label,float rate);
};
}
