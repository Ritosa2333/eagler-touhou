#pragma once
#include <cstdint>
namespace kaikeidou {
struct StageTitlePresentationServices {
 virtual ~StageTitlePresentationServices()=default;
 virtual void blend(std::uint32_t mode,std::uint32_t opacity)=0;
 virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
 virtual void box(std::uint32_t left,std::uint32_t top,std::uint32_t right,std::uint32_t bottom,std::uint32_t color,std::uint32_t fill)=0;
 virtual void sprite(std::uint64_t x,std::uint64_t y,std::uint64_t scale,std::uint64_t angle,std::uint32_t handle,std::uint32_t transparent,std::uint32_t flip)=0;
};
}
