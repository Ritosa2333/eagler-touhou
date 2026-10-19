#pragma once
#include "../../cpp/game/TextureImage.hpp"
namespace texture_image_test {
using namespace th09;
struct Fixture {AnmResource resource;TextureImage image;bool load(const u8* bytes,u32 size,u32 index,bool low_color){if(!resource.load(0,bytes,size)||index>=resource.textures().size())return false;const auto& s=resource.textures()[index];return image.load(s,s.embedded?resource.data().data()+s.pixel_offset:nullptr,s.pixel_size,low_color);}};
}
