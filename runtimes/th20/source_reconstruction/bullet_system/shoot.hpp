#pragma once
#include "command.hpp"
namespace th20::source::bullet {
struct ShotTrajectory {float angle,speed,initial_speed;};
ShotTrajectory shot_trajectory(const ShotParameters&,std::uint16_t pattern,std::uint32_t column,std::uint32_t row,float player_angle); //4818e0 arithmetic
int shoot_one(Controller&,const ShotParameters&,std::shared_ptr<ShotMetadata>,std::uint32_t column,std::uint32_t row,float player_angle); //4818e0
int shoot(Controller&,const ShotParameters&,std::shared_ptr<ShotMetadata>); //481780
}
