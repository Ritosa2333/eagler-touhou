// STD 舞台背景资源（st01.std 容器；布局依据 thstd.h 同族结构与 raw/st01.std
// 逐字节走查：头 0x90 = 对象数/图元数/实例区偏移/脚本区偏移/保留 + ANM 名 128
// 字节；对象目录 u32×N 于 0x90；对象头 28 字节 = id/layer/flags + 位置 + 尺寸；
// 图元 28 字节 = type/size/script/pad + 位置 + 宽高；实例 16 字节 = 对象/flags +
// 位置；相机脚本 = {u32 time, u16 opcode, u16 size} 流，0xffffffff 终止）。
#pragma once
#include "Types.hpp"
#include <cstring>
#include <string>
#include <vector>
namespace th13 {
struct StagePrimitive {
    i16 type = 0, script = 0;
    u32 animation = 0;
    Vec3 position{};
    Vec2 size{};
};
struct StageObject {
    i16 id = 0;
    u8 layer = 0, flags = 0;
    Vec3 position{}, size{};
    std::vector<StagePrimitive> primitives;
};
struct StageInstance {
    i16 object = 0, flags = 0;
    Vec3 position{};
};
struct StageInstruction {
    i32 time = 0;
    i16 opcode = 0, length = 0;
    u32 offset = 0;  // 相对脚本区起点的字节偏移（ins_1 跳转目标）
    std::vector<u32> arguments;
    template <class T>
    T argument(u32 index) const noexcept {
        T out{};
        if (index < arguments.size()) std::memcpy(&out, &arguments[index], 4);
        return out;
    }
};
class StageResource {
public:
    std::vector<StageObject> objects;
    std::vector<StageInstance> instances;
    std::vector<StageInstruction> instructions;
    std::string animation_name, error;
    u32 animation_count = 0;
    bool open(const u8*, u32);
    const StageInstruction* instruction(u32 offset) const noexcept;
};
}  // namespace th13
