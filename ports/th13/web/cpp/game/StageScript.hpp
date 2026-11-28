#pragma once
#include "Interpolation.hpp"
#include "StageResource.hpp"
#include "Timer.hpp"
#include <array>
#include <functional>
namespace th13 {
struct StageFog {
    float near_distance = 0, far_distance = 0;
    std::array<float, 4> channels{};
    u32 color = 0;
    void set(u32 packed, float near_value, float far_value) noexcept;
    void pack() noexcept;
};
struct StageFogInterpolation {
    StageFog start{}, end{}, control1{}, control2{}, current{};
    Timer timer{0, 0, 0, 0, 0};
    i32 duration = 0, mode = 0;
    StageFog step(float rate = 1) noexcept;
};
struct StageCamera {
    Vec3 position{}, direction{}, up{}, eye_offset{}, target_offset{}, animation_delta{};
    float fov = 0;
    StageFog fog{};
};
struct StageBackgroundAnm {
    i32 script = -1;
    bool visible = true;
};
struct StageScriptState {
    Timer timer{0, 0, 0, 0, 0};
    u32 instruction_offset = 0;
    u8 camera_effect = 0;
    Timer effect_timer{0, 0, 0, 0, 0};
    Vec3Interpolation direction{}, position{}, up{};
    StageFogInterpolation fog{};
    StageCamera camera{};
    float culling_distance_squared = 0;
    std::array<StageBackgroundAnm, 8> background{};
};
class StageScript {
public:
    StageScriptState state;
    std::string error;
    std::function<bool(i32, i32)> background_command;
    explicit StageScript(const StageResource& file) : file_(file) {}
    bool step(float rate = 1);
    bool interrupt(i32 id);

private:
    const StageResource& file_;
    void camera_effect(float rate) noexcept;
};
}
