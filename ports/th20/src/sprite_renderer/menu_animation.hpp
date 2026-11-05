#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
// Float subrectangle coordinates, used by the two StoneMenu meter strips.
void set_animation_texture_rectangle(Animation&,const SpriteData&,float x,float y,float width,float height); //438f10 descriptor supplied
void set_animation_texture_rectangle(Controller&,Animation&,float x,float y,float width,float height); //438f10
Animation* find_animation_child(Animation&,int script,int occurrence); //44c6b0
Animation* find_animation_child(Controller&,std::uint32_t& handle,int script,int occurrence); //44c670
void execute_animation_interrupt(Controller&,std::uint32_t handle,int event); //44efc0/44f0d0
}
