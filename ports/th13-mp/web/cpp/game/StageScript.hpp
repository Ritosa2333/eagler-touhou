// STD 相机脚本 VM（st01 实测指令面：ins_1 跳转(offset,time)/ins_2,3 位置/
// ins_4,5 视线/ins_6 上向量/ins_7 视场角/ins_8,9 雾/ins_12 镜头摇晃/
// ins_14 背景子 ANM 槽隐藏与重挂 / ins_16 中断标号）。
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
// stage+0x20c 的八个内嵌 ANM 槽；指令按原始顺序交给 StageScenery。
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
}  // namespace th13
