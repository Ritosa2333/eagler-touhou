#pragma once
#include "../sprite_renderer/animation.hpp"
namespace th20::source::player_entity::shot_geometry {
bool line_equation(float& slope,float& intercept,float ax,float ay,float bx,float by); //456170
bool segment_intersection(float& x,float& y,float ax,float ay,float bx,float by,float cx,float cy,float dx,float dy); //4545d0
void ray_circle(float& first,float& second,const sprite::Vec3& origin,float angle,const sprite::Vec3& center,float radius); //454480/4aaa80
bool ray_rectangle(sprite::Vec3& first,sprite::Vec3& second,const sprite::Vec3& origin,float ray_angle,float x,float y,float width,float height,float rectangle_angle); //4547e0
}
