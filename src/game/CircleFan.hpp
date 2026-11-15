#pragma once
#include <cstdint>
#include <optional>
#include <vector>
namespace th128::geometry {
struct ColorVertex {float x=0,y=0,z=0,rhw=1;std::uint32_t color=0;};
std::optional<std::vector<ColorVertex>> circleLine(float horizontal,float vertical,float radius,float angle,int count,std::uint32_t color) noexcept;
std::optional<std::vector<ColorVertex>> circleFan(float horizontal,float vertical,float radius,float angle,int count,std::uint32_t primary,std::uint32_t edge) noexcept;
}
