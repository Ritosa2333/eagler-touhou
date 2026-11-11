// 场景视图矩阵（th15 StageCamera 的取景族裁剪：仅留 ScreenViews 所需的
// look-at / 透视；stage_camera 随 STD 舞台背景接入（StageScenery 调用））。
#pragma once

#include "AnmCamera.hpp"

namespace th13 {

struct StageCamera;

Vec3 normalize_scene_vector(Vec3 v) noexcept;
Matrix4 scene_look_at(Vec3 eye, Vec3 target, Vec3 up) noexcept;
Matrix4 scene_perspective(float fov, float aspect, float near_plane, float far_plane) noexcept;
// STD 相机状态 → 渲染相机（eye = position+eye_offset；fog 通道直传）。
AnmCamera stage_camera(const StageCamera&, const GraphicsViewport&) noexcept;

}  // namespace th13
