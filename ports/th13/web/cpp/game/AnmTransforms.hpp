#pragma once
#include "AnmCoordinates.hpp"
namespace th13 {
Matrix4 matrix_product(const Matrix4&, const Matrix4&) noexcept;
Matrix4 axis_rotation(u32 axis, float angle) noexcept;
bool anm_billboard_transform(AnmVm&, Matrix4&) noexcept;
bool anm_world_quad_transform(AnmVm&, Matrix4&) noexcept;
bool anm_mesh_transform(AnmVm&, Matrix4&) noexcept;
Matrix4 anm_texture_transform(const AnmVm&) noexcept;
}
