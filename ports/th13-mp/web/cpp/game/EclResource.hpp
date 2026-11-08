// th13 ECL 资源解析：SCPT 容器（§1.1）与多文件聚合查找。
// 布局依据 docs/plans/ecl-vm.md §1.1/§1.2（raw/st01.ecl 逐字节走查）：
//   头 0x24 字节 {“SCPT”, u16 version=1, u16 include_length, u32 include_offset=0x24,
//   u32 0, u16 sub_count, u16[8] 0}，随后 ANIM/ECLI 名单与子程序偏移表/名字表，
//   子程序块 “ECLH”+u32 0x10+u32 zero[2]，指令流自块内 +0x10 起。
#pragma once

#include "Types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace th13 {

// 指令记录 16 字节头（文件内即运行时格式）：
//   +0x00 u32 time；+0x04 u16 opcode；+0x06 u16 length；
//   +0x08 u16 param_mask（bit i = 参数 i 需解析）；
//   +0x0A u8 rank_mask（1111LHNE：bit0 E … bit3 L）；
//   +0x0B u8 param_count；+0x0C u32 stack_adjust（th13 起记录弹出的表达式栈字节数）。
struct EclInstruction {
    i32 time;
    u16 opcode, length, parameter_mask;
    u8 rank_mask, parameter_count;
    u32 stack_adjust;

    template <class T = u32>
    T argument(u32 index = 0) const noexcept {
        T value;
        std::memcpy(&value, reinterpret_cast<const u8*>(this) + 16 + index * 4, sizeof value);
        return value;
    }
};
static_assert(sizeof(EclInstruction) == 16);

struct EclSubroutine {
    std::string name;
    u32 offset = 0, size = 0;
};

// 单个 .ecl 文件。
class EclResource {
public:
    std::vector<u8> bytes;
    std::vector<EclSubroutine> subroutines;
    std::vector<std::string> animations, includes;
    bool open(const u8* source, u32 size);
};

// include（ECLI 名单）与 ANM 挂接的外部供给接口。
class EclResourceProvider {
public:
    virtual ~EclResourceProvider() = default;
    virtual bool read(const std::string& name, std::vector<u8>& data) = 0;
    virtual bool animation(u32 slot, const std::string& name) = 0;
};

// 多文件聚合：首文件顺序 + 后续按名插入（原版 0x4795b0 注册序），名字二分查找（0x4797d0）。
class EclProgram {
public:
    struct Definition {
        const EclResource* file;
        u32 index;
    };
    std::vector<std::unique_ptr<EclResource>> files;
    std::vector<Definition> definitions;
    std::string error;
    i32 attach(const u8* data, u32 size, EclResourceProvider* provider = nullptr);
    bool load(const std::string& name, EclResourceProvider& provider);
    const EclInstruction* find(const char* name) const noexcept;
    i32 find_index(const char* name) const noexcept;
    const EclSubroutine& subroutine(u32 index) const noexcept {
        return definitions[index].file->subroutines[definitions[index].index];
    }
};

}  // namespace th13
