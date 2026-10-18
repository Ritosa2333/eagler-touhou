#pragma once
#include "Types.hpp"
namespace th09 {
struct TransferParameters {float speed=0;i16 source_kind=0,target_kind=0,source_side=0;u16 owner_flags=0;};
struct PlayfieldGeometry {
    u32 viewport_x=0,viewport_y=0;Vec2 camera;float width=384;
    Vec3 to_screen(const Vec3& p)const noexcept {
        return {float(float(double(viewport_x)+double(p.x))-camera.x),float(float(double(viewport_y)+double(p.y))-camera.y),0};
    }
    Vec3 to_local(const Vec3& p)const noexcept {
        return {float(float(double(p.x)-double(viewport_x))+camera.x),float(float(double(p.y)-double(viewport_y))+camera.y),0};
    }
};
}
