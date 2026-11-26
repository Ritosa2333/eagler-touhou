// 关卡背景资源的原句柄与数值状态，平台句柄保持32位原位型。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include <string_view>
namespace kaikeidou {
struct StageResourceContext {
    std::array<std::uint32_t, 8> handles{}; // A7E5CC..A7E5E8
    std::array<std::uint64_t, 13> reals{}; // A73400..A73460
    std::array<std::uint32_t, 4> integers{}; // A75830..A7583C
    std::uint32_t stage = 0, part = 0, character = 0;
};
struct BackgroundMeshPool {
    std::array<ObjectRecord<0x6f0>, 10> meshes{}; // A82F90..A874EF
    std::uint32_t count = 0; // A773D8
};
struct StageResourceServices {
    virtual ~StageResourceServices() = default;
    virtual std::int32_t delete_graph(std::uint32_t handle, std::uint32_t flag) = 0;
    virtual std::uint32_t load_graph(std::string_view path, std::uint32_t flag) = 0;
    virtual std::uint32_t platform_random(std::uint32_t maximum) = 0;
    virtual std::int32_t movie_seek(std::uint32_t handle, std::uint32_t position) = 0;
    virtual std::int32_t movie_play(std::uint32_t handle, std::uint32_t mode, std::uint32_t flag) = 0;
};
}
