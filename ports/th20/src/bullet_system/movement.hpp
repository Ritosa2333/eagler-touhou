#pragma once
#include "bullet.hpp"
namespace th20::source::bullet {
void retire(Bullet&); //47da40
float angle_to_player(const Bullet&); //4ff350
int update_launch_acceleration(Bullet&); //4840e0
int update_linear_acceleration(Bullet&,bool secondary=false); //483f00/483d20
int update_angular_acceleration(Bullet&); //483920
int update_repeated_turn(Bullet&); //483580
int update_homing(Bullet&); //480db0
int update_position_interpolation(Bullet&); //4827e0
int update_position_offset(Bullet&); //47c800
int update_screen_wrap(Bullet&); //483270
namespace unrecovered {
void execute_extended_commands_0047dcf0(Bullet&);
int update_bounce_00482a60(Bullet&);
int update_offscreen_delay_00481200(Bullet&);
}
}
