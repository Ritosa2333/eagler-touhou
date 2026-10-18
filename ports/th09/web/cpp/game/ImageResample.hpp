#pragma once
#include "TextureImage.hpp"
namespace th09 {
struct ImageRect {i32 left=0,top=0,right=0,bottom=0;};
// Positive float arithmetic used by the original triangle image filter. This
// is an image operation, independent of the graphics API and game simulation.
class ImageResample {
public:
    static bool triangle(TextureImage& destination,const ImageRect&,const TextureImage& source,const ImageRect&);
};
}
