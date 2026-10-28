// th13 SCPT 容器解析与聚合查找（规格见 EclResource.hpp 文件头；th15 EclResource 骨架
// 按 §1.1 逐字段对 th13 复核后沿用——两代容器头同构，st01.ecl 头部字节已实证）。
#include "EclResource.hpp"

#include <algorithm>

namespace th13 {
namespace {
template <class T>
T read(const u8* p) {
    T value;
    std::memcpy(&value, p, sizeof value);
    return value;
}
bool span(u32 offset, u32 count, u32 size) { return offset <= size && count <= size - offset; }
bool name(const u8* bytes, u32& position, u32 limit, std::string& out) {
    const u32 start = position;
    while (position < limit && bytes[position]) ++position;
    if (position == limit) return false;
    out.assign(reinterpret_cast<const char*>(bytes + start), position - start);
    ++position;
    return true;
}
}  // namespace

bool EclResource::open(const u8* source, u32 size) {
    // “SCPT” + version 1；include_length 给出 ANIM+ECLI 区长度（st01 = 0x40）。
    if (!source || size < 36 || read<u32>(source) != 0x54504353 || read<u16>(source + 4) != 1) return false;
    const u32 header = read<u16>(source + 6), count = read<u16>(source + 16), table = 36 + header;
    if (!span(table, count * 4, size)) return false;
    EclResource next;
    if (header) {
        u32 position = 36;
        const u32 limit = table;
        if (!span(position, 8, limit)) return false;
        if (read<u32>(source + position) == 0x4d494e41) {  // “ANIM”
            const u32 animations = read<u32>(source + position + 4);
            position += 8;
            if (animations > limit - position) return false;
            for (u32 i = 0; i < animations; ++i) {
                std::string item;
                if (!name(source, position, limit, item)) return false;
                next.animations.push_back(std::move(item));
            }
            position = (position + 3) & ~3u;
            if (!span(position, 8, limit)) return false;
            if (read<u32>(source + position) == 0x494c4345) {  // “ECLI”
                const u32 includes = read<u32>(source + position + 4);
                position += 8;
                if (includes > limit - position) return false;
                for (u32 i = 0; i < includes; ++i) {
                    std::string item;
                    if (!name(source, position, limit, item)) return false;
                    next.includes.push_back(std::move(item));
                }
            }
        }
    }
    // 子程序偏移表 + 名字表 + 指令流长度校验（每条指令 4 对齐、>=16 字节）。
    u32 names = table + count * 4;
    std::vector<u32> offsets;
    offsets.reserve(count);
    for (u32 i = 0; i < count; ++i) {
        const u32 offset = read<u32>(source + table + i * 4);
        if (!span(offset, 16, size) || offset < names || std::memcmp(source + offset, "ECLH", 4)) return false;
        offsets.push_back(offset);
    }
    const u32 names_end = offsets.empty() ? size : *std::min_element(offsets.begin(), offsets.end());
    for (u32 i = 0; i < count; ++i) {
        EclSubroutine sub;
        if (!name(source, names, names_end, sub.name)) return false;
        sub.offset = offsets[i];
        u32 end = size;
        for (u32 other : offsets)
            if (other > sub.offset) end = std::min(end, other);
        sub.size = end - sub.offset;
        for (u32 position = sub.offset + 16; position < end;) {
            if (!span(position, 16, end)) return false;
            const u32 length = read<u16>(source + position + 6);
            if (length < 16 || length % 4 || !span(position, length, end)) return false;
            position += length;
        }
        next.subroutines.push_back(std::move(sub));
    }
    next.bytes.assign(source, source + size);
    *this = std::move(next);
    return true;
}

i32 EclProgram::attach(const u8* data, u32 size, EclResourceProvider* provider) {
    if (files.size() >= 32) {
        error = "ECL include limit exceeded";
        return -1;
    }
    auto file = std::make_unique<EclResource>();
    if (!file->open(data, size)) {
        error = "Invalid ECL resource";
        return -1;
    }
    const i32 result = files.size();
    const auto* current = file.get();
    const bool first = files.empty();
    files.push_back(std::move(file));
    // 首文件保持原序，后续文件按名插入维持全局字典序（原版 0x4795b0 注册序）。
    for (u32 i = 0; i < current->subroutines.size(); ++i) {
        Definition definition{current, i};
        if (first)
            definitions.push_back(definition);
        else {
            const auto& key = current->subroutines[i].name;
            auto at = definitions.begin();
            while (at != definitions.end() &&
                   std::strcmp(key.c_str(), at->file->subroutines[at->index].name.c_str()) > 0)
                ++at;
            definitions.insert(at, definition);
        }
    }
    if (provider) {
        for (u32 i = 0; i < current->animations.size(); ++i) {
            if (!provider->animation(i + 10, current->animations[i])) {
                error = "Missing animation: " + current->animations[i];
                return result;
            }
        }
        for (const auto& include : current->includes) load(include, *provider);
    }
    return result;
}

bool EclProgram::load(const std::string& name, EclResourceProvider& provider) {
    std::vector<u8> data;
    if (!provider.read(name, data)) {
        error = "Missing ECL: " + name;
        return false;
    }
    return attach(data.data(), data.size(), &provider) >= 0;
}

i32 EclProgram::find_index(const char* name) const noexcept {
    if (!name) return -1;
    i32 low = 0, high = i32(definitions.size()) - 1;
    while (low <= high) {
        const i32 middle = low + (high - low) / 2;
        const auto& sub = subroutine(middle);
        const int order = std::strcmp(name, sub.name.c_str());
        if (!order) return middle;
        if (order < 0)
            high = middle - 1;
        else
            low = middle + 1;
    }
    return -1;
}

const EclInstruction* EclProgram::find(const char* name) const noexcept {
    const i32 index = find_index(name);
    if (index < 0) return nullptr;
    const auto& definition = definitions[index];
    return reinterpret_cast<const EclInstruction*>(definition.file->bytes.data() + subroutine(index).offset + 16);
}

}  // namespace th13
