// 背景网格显示的图形边界，对应40DD90与41DD00。
#pragma once
#include <cstdint>
namespace kaikeidou {
struct BackgroundMeshPresentationServices {
 virtual ~BackgroundMeshPresentationServices()=default;
 virtual std::int32_t set_clip(std::int32_t left,std::int32_t top,std::int32_t right,std::int32_t bottom)=0;
 virtual std::int32_t draw_triangles(const std::uint8_t* vertices,std::uint32_t count,std::uint32_t handle,std::uint32_t transparent)=0;
};
}
