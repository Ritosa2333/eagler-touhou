#pragma once
#include "pool.hpp"
namespace th20::source::sprite {
std::uint32_t select_layer_animations(AnimationList* group,std::int32_t layer,bool secondary) noexcept; //44a090/44a210, actual returned selected count
void configure_animation_layer(Controller&,std::int32_t layer,std::int32_t group); //44f3d0
void draw_selected_animations(Controller&,AnimationList* group); //449fa0
std::int32_t draw_animation_layer(Controller&,std::int32_t layer); //449e40, returns1
void draw_animation(Controller&,Animation&); //443880 renderer dispatch, external until recovered
namespace dispatch_environment {
Controller& controller(); //shared DAT_005c0028, may differ from explicit receiver
void select_camera(std::int32_t preset); //41dce0
void select_layer_camera(std::int32_t preset); //44f720
void select_viewport_camera(std::int32_t preset); //44f700
void disable_fog(); //4dda60
void disable_depth_write(); //4ddf80
void set_render_state(std::uint32_t,std::uint32_t); //4d9db0
}
}
