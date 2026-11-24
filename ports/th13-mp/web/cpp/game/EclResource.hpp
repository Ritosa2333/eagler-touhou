#pragma once

#include "Types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace th13 {

struct EclInstruction {
    i32 time;
    u16 opcode, length, parameter_mask;
    u8 rank_mask, parameter_count;
    u32 stack_adjust;

    template <class T = u32> T argument(u32 index = 0) const noexcept {
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

class EclResource {
public:
    std::vector<u8> bytes;
    std::vector<EclSubroutine> subroutines;
    std::vector<std::string> animations, includes;
    bool open(const u8* source, u32 size);
};

class EclResourceProvider {
public:
    virtual ~EclResourceProvider() = default;
    virtual bool read(const std::string& name, std::vector<u8>& data) = 0;
    virtual bool animation(u32 slot, const std::string& name) = 0;
};

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

}
