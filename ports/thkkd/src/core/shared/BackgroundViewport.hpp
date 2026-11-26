// 原65F590与42F870的视口偏移和binary32矩阵构造。
#pragma once
#include "core/stages/StageBackgroundServices.hpp"
#include "core/math/BinaryFloat32.hpp"
namespace kaikeidou {
inline std::array<std::uint32_t,16> build_background_viewport_matrix(
    std::uint32_t x,std::uint32_t y,std::uint32_t width,std::uint32_t height,unsigned precision){
    using namespace binary_float;
    std::array<std::uint32_t,16> result{};
    const auto half=binary64(0x3fe0000000000000ull);
    result[0]=to_binary32(multiply(finite_binary32(width),half,precision));
    auto vertical=finite_binary32(height);vertical.negative=!vertical.negative;
    result[5]=to_binary32(multiply(vertical,half,precision));
    result[10]=0x3f800000;result[15]=0x3f800000;
    result[12]=x;result[13]=y;result[14]=0;
    return result;
}
inline std::int32_t set_background_viewport(std::uint64_t x,std::uint64_t y,unsigned precision,StageBackgroundServices& services){
    using namespace binary_float;
    const auto vertical=to_binary32(add(binary64(0x4030000000000000ull),binary64(y),precision));
    const auto horizontal=to_binary32(add(binary64(0x4040000000000000ull),binary64(x),precision));
    const auto matrix=build_background_viewport_matrix(horizontal,vertical,0x44200000,0x43f00000,precision);
    return services.set_viewport_matrix(matrix);
}
}
