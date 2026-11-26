// 背景逐帧更新的平台边界；视口矩阵按原16个binary32位型提交。
#pragma once
#include "core/stages/StageResourceContext.hpp"
namespace kaikeidou {
struct StageBackgroundServices : StageResourceServices {
    virtual std::int32_t movie_pause(std::uint32_t handle,std::uint32_t flag)=0;
    virtual std::int32_t set_viewport_matrix(const std::array<std::uint32_t,16>& matrix)=0;
};
}
