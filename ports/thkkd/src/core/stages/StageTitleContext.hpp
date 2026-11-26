// 原A78FD8..A78FE8四个32位字段，图像句柄保留原位型。
#pragma once
#include <cstdint>
namespace kaikeidou {
struct StageTitleContext {std::uint32_t active=0,graph_handle=0,age=0,opacity=0;};
static_assert(sizeof(StageTitleContext)==16);
}
