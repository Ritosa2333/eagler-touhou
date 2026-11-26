// 六面新增的已采状态；向量保存原float32位型，结局文本保留96字节缓冲的旧尾部。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include "core/shared/EffectPool.hpp"
#include <cstdint>
namespace kaikeidou {
struct Stage6RuntimeContext {
    ObjectRecord<144> vectors; // B3E350，12个float3。
    Effect effect_prefix; // A73550，特效分配失败后原-1槽。
    ObjectRecord<420> ending; // A7E658，9个dword及4个96字节文本缓冲。
    std::uint32_t outer_scene=100; // A8CE58，外层画面分派编号。
};
static_assert(sizeof(Stage6RuntimeContext)==680);
}
