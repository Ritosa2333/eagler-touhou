#pragma once
#include "EclVm.hpp"
namespace th09 {
struct CaptureArea {float radius=0,angle=0,spread=0;};
class CharacterCapture {
public:
    static bool contains(i32 character,const Vec3& player,const Vec3& target,const CaptureArea&)noexcept;
    static bool update_motion(EclVm&,i32 character,const FrameTiming&)noexcept;
};
}
