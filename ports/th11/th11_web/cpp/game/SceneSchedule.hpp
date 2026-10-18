#pragma once
#include "Types.hpp"
namespace th11 {
enum class SceneDrawKind:u32 {Animation,Player,Items,Lasers,Bullets,Spell,Begin,TargetB,CompositeA,TargetA,CompositeB,TargetScreen,CompositeScreen,End,StageBackground,StageForeground,HudInner,HudOuter,AsciiInner,AsciiOuter,ScorePopups};
struct SceneDrawPass {u32 priority;SceneDrawKind kind;u32 layer;};
// Original callback registrations, including 4298c0's render-target passes.
// Layers 27/28 are drawn inside the corresponding composite callback.
inline constexpr SceneDrawPass scene_draw_passes[]={
    {1,SceneDrawKind::Begin,0},
    {2,SceneDrawKind::StageBackground,0},{4,SceneDrawKind::Animation,0},
    {5,SceneDrawKind::StageForeground,0},{6,SceneDrawKind::Animation,1},
    {8,SceneDrawKind::Animation,2},{9,SceneDrawKind::Spell,0},{10,SceneDrawKind::Animation,3},
    {11,SceneDrawKind::TargetB,0},{12,SceneDrawKind::Animation,4},
    {13,SceneDrawKind::CompositeA,27},{15,SceneDrawKind::Animation,5},
    {16,SceneDrawKind::Animation,6},{17,SceneDrawKind::Animation,7},
    {18,SceneDrawKind::Animation,8},{19,SceneDrawKind::Animation,9},
    {21,SceneDrawKind::Animation,10},{22,SceneDrawKind::Player,0},
    {23,SceneDrawKind::Animation,11},{24,SceneDrawKind::Animation,12},
    {25,SceneDrawKind::Items,0},{26,SceneDrawKind::Animation,13},
    {27,SceneDrawKind::Lasers,0},{28,SceneDrawKind::Animation,14},
    {29,SceneDrawKind::Bullets,0},{32,SceneDrawKind::Animation,15},
    {34,SceneDrawKind::Animation,16},{35,SceneDrawKind::TargetA,0},
    {36,SceneDrawKind::Animation,17},{37,SceneDrawKind::CompositeB,28},
    {39,SceneDrawKind::Animation,18},{40,SceneDrawKind::Animation,19},
    {41,SceneDrawKind::ScorePopups,0},
    {42,SceneDrawKind::HudInner,0},
    {43,SceneDrawKind::AsciiInner,0},
    {46,SceneDrawKind::TargetScreen,0},{47,SceneDrawKind::CompositeScreen,0},
    {48,SceneDrawKind::Animation,20},{49,SceneDrawKind::Animation,21},
    {50,SceneDrawKind::Animation,22},{57,SceneDrawKind::HudOuter,0},{60,SceneDrawKind::Animation,23},
    {61,SceneDrawKind::Animation,24},{62,SceneDrawKind::Animation,30},
    {63,SceneDrawKind::Animation,29},{66,SceneDrawKind::AsciiOuter,0},{68,SceneDrawKind::End,0}
};
}
