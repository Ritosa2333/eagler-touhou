#pragma once
#include "PlayerMotion.hpp"
#include <vector>
#include <string>
namespace th09 {
enum class ShotCreation:u32 {standard,marisa_beam,sakuya_delayed,youmu_wave,lyrica_effect,lyrica_normal,lyrica_effect_shot,komachi_aim};
enum class ShotUpdate:u32 {standard,reimu_homing,marisa_beam,sakuya_turn,youmu_wave,reserved,medicine_clockwise,medicine_counterclockwise};
enum class ShotDrawing:u32 {standard,marisa_beam,layered_trail};
enum class ShotHit:u32 {standard,reserved,area_only};
struct ShotDefinition {
    i16 frame=0,reserved=0;Vec2 offset,hitbox;float angle=0,speed=0;i16 damage=0,reserved1e=0,option=0,type=0,animation=0,sound=-1;
    ShotCreation creation=ShotCreation::standard;ShotUpdate update=ShotUpdate::standard;ShotDrawing drawing=ShotDrawing::standard;ShotHit hit=ShotHit::standard;
};
class ShotResource {
public:
    float hit_size=0,graze_size=0,capture_size=0,item_size=0;MovementSpeeds movement;
    float charge_speed=0,charge_duration=0;std::array<std::string,3> spell_names;
    std::vector<std::vector<ShotDefinition>> sets;
    bool load(const u8*,u32 size);
};
}
