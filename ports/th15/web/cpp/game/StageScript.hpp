#pragma once
#include "StageResource.hpp"
#include "StageFog.hpp"
#include <array>
namespace th15 {
struct StageCamera {
    Vec3 position{},direction{},up{},eye_offset{},target_offset{},animation_delta{};float fov=0;StageFog fog{};
};
struct StageScriptState {
    Timer timer{0,0,0,0,0};u32 instruction_offset=0;u8 camera_effect=0;Timer effect_timer{0,0,0,0,0};
    Vec3Interpolation direction{},position{},up{};StageFogInterpolation fog{};StageCamera camera{};
    std::array<i32,8> animation_layers{};float culling_distance_squared=0;
    float deformation_target=0,deformation_radius=0;u32 deformation_color=0;float phase_x=0,phase_y=0;i32 deformation_mode=0;
};
class StageScriptWorld {
public:
    virtual ~StageScriptWorld()=default;
    virtual bool clear_color(u32)=0;
    virtual bool animation(u32 index,i32 script,i32 layer)=0;
    virtual bool deformation(i32 mode,i32 script)=0;
    virtual bool object_interrupt(i32)=0;
};
class StageScript {
public:
    StageScriptState state;std::string error;
    explicit StageScript(const StageResource& file,StageScriptWorld& world):file_(file),world_(world){}
    bool step(float rate=1);
    // The original interrupt selects an offset from the matched label's length.
    bool interrupt(i32 id);
    void interpolate_fog(i32 duration,i32 mode,const StageFog& target)noexcept;
private:
    const StageResource& file_;StageScriptWorld& world_;
    void camera_effect(float rate)noexcept;
};
}
