#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
void copy_animation_base(Animation&,const Animation&) noexcept; // 0x4371f0
void select_animation_template(AnimationFile&,Animation&,std::int32_t); // 0x438b70
void bind_animation_script(AnimationFile&,Animation&,std::int32_t,Animation* parent); // 0x4382b0
void attach_animation_parent(Animation&,Animation*) noexcept; // parent-field portion of 0x4382b0
void set_animation_layer(Animation&,std::int32_t) noexcept; // 0x450350
int assign_animation_sprite(AnimationFile&,Animation&,std::int32_t); // 0x438620
int initialize_animation_script(AnimationFile&,Animation&,std::int32_t); // 0x438940
void store_sprite_descriptor(AnimationFile&,std::int32_t,const SpriteData&); // 0x4506f0
AnimationFile& script_file(Controller&,const Animation&);    // 0x437470
AnimationFile& sprite_file(Controller&,const Animation&);    // 0x437500
SpriteData& current_sprite(Controller&,const Animation&);   // 0x437cd0/0x437c80
AnmInstruction* script_start(Controller&,const Animation&); // 0x437490
DeviceTexture* texture(Controller&,std::uint32_t);       // 0x445a00
}
