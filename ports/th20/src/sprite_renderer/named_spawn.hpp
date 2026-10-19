#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
// 0x450cb0. The caller owns output_handle; output_animation is optional.
// name is compared against AnimationFile::stem, whose diagnostic is an actual
// no-op in this release. flags select position, list and insertion behavior.
void spawn_named_animation(Controller&,AnimationFile&,std::uint32_t& output_handle,
    const char* name,std::int32_t script,const Vec3* position,float rotation_z,
    std::int32_t layer,std::uint32_t flags,Animation** output_animation=nullptr);
// 0x450c70: null position, zero rotation, flags zero.
std::uint32_t spawn_named_animation(Controller&,AnimationFile&,const char* name,
    std::int32_t script,std::int32_t layer=-1,Animation** output_animation=nullptr);
}
