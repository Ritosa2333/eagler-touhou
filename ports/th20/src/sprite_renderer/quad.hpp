#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
extern Vertex28 animation_quad[4]; // source storage replacing DAT_005aef10
extern Vec3 animation_position_scratch; //source storage DAT_005c0018
float animation_scale_x(Animation&); //445940
float animation_scale_y(Animation&); //4458e0
float animation_aux_scale_x(Animation&); //445880
float animation_width(Animation&); //4459d0
float animation_height(Animation&); //4459a0
float round_sprite_coordinate(float); //445b80 ->550060, round halfway away from zero
void calculate_sprite_corners(Animation&,const SpriteData&,Vec3* p0,Vec3* p1,Vec3* p2,Vec3* p3,bool rotated); //43eb40/43ef20
void calculate_animation_corners(Animation&,Vec3(&)[4]); //445770
void submit_animation_quad(Controller&,Animation&,Vertex28(&)[4],std::uint32_t flags); //43f6a0
void draw_axis_aligned_sprite(Controller&,Animation&,bool snap); //43f550/43f5c0
void draw_rotated_sprite(Controller&,Animation&); //43f630
namespace draw_environment {
Controller& controller();
Device& device();
const float* viewport_bounds(); //current viewport +11c, left top right bottom
}
}
