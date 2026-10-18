#pragma once
#include "GraphicsMath.hpp"
#include "Background.hpp"
namespace th09 {
struct Camera {
    Matrix4 view,projection;Vec3 right;
    void screen(const Viewport&);
    void scene(const Viewport&,const BackgroundCamera&);
};
}
