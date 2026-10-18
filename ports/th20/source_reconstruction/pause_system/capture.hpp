#pragma once
#include "pause.hpp"
#include "../runtime_state/state.hpp"
namespace th20::source::pause {
void copy_background_texture(sprite::Controller&,int file,int texture,DeviceTexture*,const RECT&,const RECT&,int noise); //44be40
void capture_background(PauseInf&); //4e60c0
void capture_practice_background(PauseInf&); //4e59e0 ->4e6600
void show_animation_tree(sprite::Animation&); //44fa70
void show_animation(std::uint32_t,bool); //44fa30/450120
}
