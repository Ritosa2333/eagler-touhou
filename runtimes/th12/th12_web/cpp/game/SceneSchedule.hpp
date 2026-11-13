#pragma once
#include "Types.hpp"
#include "PortTitle.hpp"
namespace th11 {
enum class SceneDrawKind:u32 {Animation,Player,Items,Lasers,Bullets,Spell,Begin,TargetB,CompositeA,TargetA,CompositeB,TargetScreen,CompositeScreen,End,StageBackground,StageForeground,HudInner,HudOuter,AsciiInner,AsciiOuter,ScorePopups,TargetC,CompositeC,AsciiPlayfield};
struct SceneDrawPass {u32 priority;SceneDrawKind kind;u32 layer;};
constexpr bool scene_compositor_pass(SceneDrawKind kind){return (kind>=SceneDrawKind::Begin&&kind<=SceneDrawKind::End)||kind==SceneDrawKind::TargetC||kind==SceneDrawKind::CompositeC;}
#if PORT_TITLE == 16
// Retail registrations: ANM46a769, Supervisor43ba8a, Stage4098e9,
// ASCII407d3f, player4410b2, bullets411a94 and spell41771f.
// Other managers still need their own ports; these are the measured callbacks.
inline constexpr SceneDrawPass scene_draw_passes[]={
    {1,SceneDrawKind::Begin,0},{3,SceneDrawKind::StageBackground,0},
    {5,SceneDrawKind::Animation,0},{6,SceneDrawKind::StageForeground,0},
    {7,SceneDrawKind::Animation,1},{9,SceneDrawKind::Animation,2},
    {10,SceneDrawKind::Animation,3},{11,SceneDrawKind::Animation,4},{12,SceneDrawKind::Spell,0},
    {13,SceneDrawKind::Animation,5},{14,SceneDrawKind::TargetB,0},{15,SceneDrawKind::CompositeA,34},
    {16,SceneDrawKind::Animation,6},{18,SceneDrawKind::Animation,7},
    {20,SceneDrawKind::Animation,8},{21,SceneDrawKind::Animation,9},{22,SceneDrawKind::Animation,10},
    {24,SceneDrawKind::Animation,11},{25,SceneDrawKind::TargetA,0},{26,SceneDrawKind::CompositeB,0},
    {27,SceneDrawKind::Animation,12},{28,SceneDrawKind::Animation,13},{29,SceneDrawKind::Player,0},
    {31,SceneDrawKind::Animation,14},{32,SceneDrawKind::Animation,15},{34,SceneDrawKind::Animation,16},
    {36,SceneDrawKind::Animation,17},{37,SceneDrawKind::Bullets,0},{39,SceneDrawKind::Animation,18},
    {42,SceneDrawKind::Animation,19},{43,SceneDrawKind::TargetC,0},{44,SceneDrawKind::CompositeC,0},
    {45,SceneDrawKind::Animation,20},{46,SceneDrawKind::Animation,21},
    {52,SceneDrawKind::Animation,22},{53,SceneDrawKind::AsciiInner,0},{54,SceneDrawKind::Animation,23},
    {55,SceneDrawKind::Animation,36},{56,SceneDrawKind::TargetScreen,0},{57,SceneDrawKind::CompositeScreen,0},
    {58,SceneDrawKind::Animation,24},{59,SceneDrawKind::Animation,25},{60,SceneDrawKind::Animation,37},
    {61,SceneDrawKind::Animation,26},{62,SceneDrawKind::Animation,27},{63,SceneDrawKind::Animation,38},
    {64,SceneDrawKind::Animation,28},{65,SceneDrawKind::Animation,39},{66,SceneDrawKind::AsciiPlayfield,0},
    {77,SceneDrawKind::Animation,29},{78,SceneDrawKind::Animation,40},{79,SceneDrawKind::Animation,30},
    {80,SceneDrawKind::Animation,41},{81,SceneDrawKind::AsciiOuter,0},{82,SceneDrawKind::Animation,31},
    {83,SceneDrawKind::Animation,42},{85,SceneDrawKind::End,0}
};
#elif PORT_TITLE == 14
// Supervisor 444eaa, ANM 47aae5, Stage 40d53b and ASCII 40b5c3.
// Stage draws layers 30/31; CompositeA draws 32. Layer 33 has no draw callback.
inline constexpr SceneDrawPass scene_draw_passes[]={
    {1,SceneDrawKind::Begin,0},{3,SceneDrawKind::StageBackground,0},
    {5,SceneDrawKind::Animation,0},{6,SceneDrawKind::StageForeground,0},
    {7,SceneDrawKind::Animation,1},{9,SceneDrawKind::Animation,2},
    {10,SceneDrawKind::Animation,3},{11,SceneDrawKind::Animation,4},{12,SceneDrawKind::Spell,0},
    {13,SceneDrawKind::Animation,5},{14,SceneDrawKind::TargetB,0},{15,SceneDrawKind::CompositeA,32},
    {16,SceneDrawKind::Animation,6},{18,SceneDrawKind::Animation,7},{19,SceneDrawKind::Animation,8},
    {20,SceneDrawKind::Animation,9},{21,SceneDrawKind::Animation,10},{23,SceneDrawKind::Animation,11},
    {24,SceneDrawKind::TargetA,0},{25,SceneDrawKind::CompositeB,0},{26,SceneDrawKind::Animation,12},
    {27,SceneDrawKind::Animation,13},{28,SceneDrawKind::Player,0},{29,SceneDrawKind::Animation,14},
    {30,SceneDrawKind::Animation,15},{31,SceneDrawKind::Items,0},{32,SceneDrawKind::Animation,16},
    {33,SceneDrawKind::Lasers,0},{34,SceneDrawKind::Animation,17},{35,SceneDrawKind::Bullets,0},
    {37,SceneDrawKind::Animation,18},{39,SceneDrawKind::Animation,19},
    {40,SceneDrawKind::TargetC,0},{41,SceneDrawKind::CompositeC,0},
    {42,SceneDrawKind::Animation,20},{43,SceneDrawKind::Animation,21},{44,SceneDrawKind::ScorePopups,0},
    {45,SceneDrawKind::HudInner,0},{49,SceneDrawKind::Animation,22},{50,SceneDrawKind::AsciiInner,0},
    {51,SceneDrawKind::Animation,23},{52,SceneDrawKind::TargetScreen,0},{53,SceneDrawKind::CompositeScreen,0},
    {54,SceneDrawKind::Animation,24},{55,SceneDrawKind::Animation,25},{56,SceneDrawKind::Animation,26},
    {57,SceneDrawKind::Animation,27},{58,SceneDrawKind::AsciiPlayfield,0},
    {66,SceneDrawKind::Animation,28},{67,SceneDrawKind::Animation,29},
    {68,SceneDrawKind::Animation,35},{69,SceneDrawKind::Animation,34},
    {72,SceneDrawKind::AsciiOuter,0},{74,SceneDrawKind::End,0}
};
#elif PORT_TITLE == 13
// TH13 registrations: Supervisor 43a4b8, ANM 46bf25, ASCII 403a00.
// Layers 27/28 belong to StageForeground; 29/30 to the first two copies.
inline constexpr SceneDrawPass scene_draw_passes[]={
    {1,SceneDrawKind::Begin,0},{2,SceneDrawKind::StageBackground,0},
    {4,SceneDrawKind::Animation,0},{5,SceneDrawKind::StageForeground,0},
    {6,SceneDrawKind::Animation,1},{8,SceneDrawKind::Animation,2},
    {9,SceneDrawKind::Animation,3},{10,SceneDrawKind::Spell,0},{11,SceneDrawKind::Animation,4},
    {12,SceneDrawKind::TargetB,0},{13,SceneDrawKind::Animation,5},{14,SceneDrawKind::CompositeA,29},
    {15,SceneDrawKind::Animation,6},{17,SceneDrawKind::Animation,7},{18,SceneDrawKind::Animation,8},
    {19,SceneDrawKind::Animation,9},{20,SceneDrawKind::Animation,10},{22,SceneDrawKind::Animation,11},
    {24,SceneDrawKind::Animation,12},{25,SceneDrawKind::Animation,13},{26,SceneDrawKind::Items,0},
    {27,SceneDrawKind::Player,0},{28,SceneDrawKind::Animation,14},{29,SceneDrawKind::Lasers,0},
    {30,SceneDrawKind::Animation,15},{31,SceneDrawKind::Bullets,0},{35,SceneDrawKind::Animation,16},
    {37,SceneDrawKind::Animation,17},{38,SceneDrawKind::TargetA,0},{39,SceneDrawKind::Animation,18},
    {40,SceneDrawKind::CompositeB,30},{41,SceneDrawKind::TargetC,0},{42,SceneDrawKind::CompositeC,0},
    {43,SceneDrawKind::Animation,19},{44,SceneDrawKind::Animation,20},{45,SceneDrawKind::ScorePopups,0},
    {47,SceneDrawKind::AsciiInner,0},{50,SceneDrawKind::HudInner,0},
    {52,SceneDrawKind::TargetScreen,0},{53,SceneDrawKind::CompositeScreen,0},
    {54,SceneDrawKind::Animation,21},{55,SceneDrawKind::Animation,22},{57,SceneDrawKind::Animation,23},
    {58,SceneDrawKind::Animation,24},{67,SceneDrawKind::HudOuter,0},{68,SceneDrawKind::Animation,25},
    {69,SceneDrawKind::Animation,26},{70,SceneDrawKind::Animation,32},{71,SceneDrawKind::Animation,31},
    {74,SceneDrawKind::AsciiOuter,0},{76,SceneDrawKind::End,0}
};
#else
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
#endif
}
