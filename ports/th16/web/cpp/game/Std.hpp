// th16 STD 关卡背景文件——0x40ac30 装载时的内存布局（文件原样驻留，偏移表就地重定位）。
//
//   头 0x10：i16 对象数  i16 面 VM 数（所有四边形脚本槽总数）  u32 实例表偏移  u32 脚本偏移  u32 保留
//   +0x10   char anm_name[0x80]：背景 ANM（装入槽 3 + (stage & 1)，0x40ac30）
//   +0x90   u32 object_offset[对象数]（相对文件头，装载时加上基址）
//   对象（0x1c 头 + 四边形串）：u16 id  u8 layer  u8 flags（bit0 本帧有活动面 VM，bit1 本帧被绘制）
//           Vec3 pos  Vec3 size；四边形 {i16 type(-1 结束) i16 size i16 anm_script i16 vm_index Vec3 pos Vec2 wh}
//           vm_index 为运行时字段：0x40add0 按遍历顺序给每个四边形分配面 VM 下标
//   实例表：{u16 object u16 flags(bit0 可见) Vec3 pos} × N，以 object == 0xffff 结束
//   脚本：{i32 time i16 opcode i16 size args…}，0x40b3b0 解释
#pragma once
#include "Types.hpp"

#include <string>
#include <vector>

namespace th16 {

struct StdQuad {
    i16 type = 0;
    i16 size = 0;
    i16 anm_script = 0;   // 背景 ANM 脚本号
    i16 vm_index = 0;     // 运行时：面 VM 下标（0x40add0 赋值）
    Vec3 pos;
    float width = 0, height = 0;
};

struct StdObject {
    u16 id = 0;
    u8 layer = 0;
    u8 flags = 0;      // 运行时位：bit0 有活动面 VM（0x40aed0 清），bit1 本帧绘制（0x40af70 置）
    Vec3 pos;
    Vec3 size;
    std::vector<StdQuad> quads;
};

struct StdInstance {
    u16 object = 0;
    u16 flags = 0;     // bit0：上帧通过视锥测试（0x40af70 置/清）
    Vec3 pos;
};

struct StdInstruction {
    i32 time = 0;
    i16 opcode = 0;
    i16 size = 0;
    u32 offset = 0;    // 相对脚本起点（ins_1 跳转目标用）
    const u8* args = nullptr;
    i32 arg_i(int n) const;
    float arg_f(int n) const;
};

class StdFile {
public:
    std::string anm_name;
    i32 face_vm_count = 0;
    std::vector<StdObject> objects;
    std::vector<StdInstance> instances;
    std::vector<StdInstruction> script;
    std::string error;

    bool parse(const u8* bytes, u32 size);
    // 按脚本内偏移找指令下标；找不到返回 -1
    i32 instruction_at(u32 offset) const;

private:
    std::vector<u8> bytes_;
};

}  // namespace th16
