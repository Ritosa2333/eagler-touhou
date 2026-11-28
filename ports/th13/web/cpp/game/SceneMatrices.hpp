#pragma once

#include "AnmCamera.hpp"

namespace th13 {

struct StageCamera;

Vec3 normalize_scene_vector(Vec3 v) noexcept;
Matrix4 scene_look_at(Vec3 eye, Vec3 target, Vec3 up) noexcept;
Matrix4 scene_perspective(float fov, float aspect, float near_plane, float far_plane) noexcept;
AnmCamera stage_camera(const StageCamera&, const GraphicsViewport&) noexcept;

}
