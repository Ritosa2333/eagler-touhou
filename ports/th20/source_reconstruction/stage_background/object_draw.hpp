#pragma once
#include "background.hpp"
namespace th20::source::background {
int cull_object(const Object&,const sprite::Vec3& instance_position,float squared_limit,const program_entry::ViewportState&); //473b20
void update_perspective_camera(program_entry::ViewportState&); //4d9f60
void select_background_viewport(program_entry::GraphicsStatePrefix&,int); //477520
void reset_sprite_draw_cache(sprite::Controller&) noexcept; //4750a0
void draw_embedded_layer(ScriptState&,int); //474780
void draw_object_layer(Background&,int); //474880
}
