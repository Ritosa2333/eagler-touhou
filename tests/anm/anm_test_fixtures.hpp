// ANM 单测共享夹具：仓库文件读取、ANM 归档构造器、指令流构造器。
// golden 数值取自 artifacts/reference/raw/front.anm 逐字节走查
// （docs/plans/anm-pipeline.md §二 与本轮 python 复核输出）。
#pragma once

#include "../harness/test_framework.hpp"

#include "game/AnmResource.hpp"
#include "game/AnmManager.hpp"

#include <array>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace th13test {
using namespace th13;

// ---------- 仓库文件读取：从当前目录或本文件上溯定位仓库根 ----------
class TestFileReader {
public:
    bool read(const std::string& relative, std::vector<th13::u8>& out) {
        if (cache_.count(relative)) {
            out = cache_[relative];
            return true;
        }
        for (const auto& base : bases()) {
            FILE* file = std::fopen((base + "/" + relative).c_str(), "rb");
            if (!file) continue;
            std::fseek(file, 0, SEEK_END);
            const long size = std::ftell(file);
            std::fseek(file, 0, SEEK_SET);
            out.resize(size_t(size < 0 ? 0 : size));
            size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
            std::fclose(file);
            if (got == out.size()) {
                cache_[relative] = out;
                return true;
            }
            return false;
        }
        return false;
    }

private:
    static const std::vector<std::string>& bases() {
        static const std::vector<std::string> list = {".", "../../../..", "../../../../.."};
        return list;
    }
    std::map<std::string, std::vector<th13::u8>> cache_;
};

// ---------- 指令流构造：{i16 opcode; u16 size; i16 time; u16 mask; i32 args[]} ----------
inline void push_insn(std::vector<u8>& out, i16 opcode, i16 time, u16 mask = 0,
                      std::vector<i32> args = {}) {
    const u16 size = u16(8 + 4 * args.size());
    u8 head[8];
    std::memcpy(head, &opcode, 2);
    std::memcpy(head + 2, &size, 2);
    std::memcpy(head + 4, &time, 2);
    std::memcpy(head + 6, &mask, 2);
    out.insert(out.end(), head, head + 8);
    for (i32 value : args) {
        u8 raw[4];
        std::memcpy(raw, &value, 4);
        out.insert(out.end(), raw, raw + 4);
    }
}

// 标签伪指令（opcode 5）：{5, 12, time, 0, i32 id}。
inline void push_label(std::vector<u8>& out, i32 id, i16 time = 0) {
    push_insn(out, 5, time, 0, {id});
}

inline void push_terminator(std::vector<u8>& out) {
    const i16 opcode = -1;
    const u16 size = 0;
    u8 head[8];
    std::memcpy(head, &opcode, 2);
    std::memcpy(head + 2, &size, 2);
    std::memset(head + 4, 0, 4);
    out.insert(out.end(), head, head + 8);
}

inline AnmScript make_script(std::vector<u8> bytes) {
    AnmScript script;
    script.bytes = std::move(bytes);
    return script;
}

// ---------- 合成 ANM 归档（单 entry；布局对齐 AnmResource::open 校验） ----------
struct SyntheticTexture {
    std::string name = "test/tex.png";
    u16 width = 2, height = 2, format = 1;
    bool has_data = true;
};

// sprites：{id,x,y,w,h}；scripts：指令流（自带终止符亦可，这里不追加）。
inline std::vector<u8> build_anm(const SyntheticTexture& texture,
                                 const std::vector<std::array<float, 5>>& sprites,
                                 const std::vector<std::pair<i32, std::vector<u8>>>& scripts) {
    std::vector<u8> out;
    std::vector<u8> body;  // name + 精灵记录 + THTX + 脚本流（entry 内连续区）
    // 追加目标一律是 body，最后整体拼到 out 的表区之后。
    auto append = [&body](const void* data, size_t size) {
        const auto* bytes = static_cast<const u8*>(data);
        body.insert(body.end(), bytes, bytes + size);
    };
    auto put32 = [&](size_t at, u32 value) { std::memcpy(out.data() + at, &value, 4); };

    const u32 ns = u32(sprites.size()), nc = u32(scripts.size());
    const u32 table_end = 64 + ns * 4 + nc * 8;
    const u32 name_offset = table_end;
    std::string name = texture.name;
    body.insert(body.end(), name.begin(), name.end());
    body.push_back(0);
    while (body.size() % 4) body.push_back(0);

    std::vector<u32> sprite_offsets(ns);
    for (u32 i = 0; i < ns; i++) {
        sprite_offsets[i] = name_offset + u32(body.size());
        u32 id = u32(sprites[i][0]);
        append(&id, 4);
        for (int k = 1; k < 5; k++) append(&sprites[i][k], 4);
    }
    u32 thtx_offset = 0;
    if (texture.has_data) {
        thtx_offset = name_offset + u32(body.size());
        append("THTX", 4);
        const u16 zero = 0, fmt = texture.format, w = texture.width, h = texture.height;
        append(&zero, 2);
        append(&fmt, 2);
        append(&w, 2);
        append(&h, 2);
        const u32 bpp = texture.format == 1 ? 4 : texture.format == 7 ? 1 : 2;
        const u32 pixels = u32(w) * h * bpp;
        append(&pixels, 4);
        body.insert(body.end(), pixels, 0x55);
    }
    std::vector<u32> script_offsets(nc);
    for (u32 i = 0; i < nc; i++) {
        script_offsets[i] = name_offset + u32(body.size());
        const auto& bytes = scripts[i].second;
        body.insert(body.end(), bytes.begin(), bytes.end());
    }

    out.resize(table_end);  // header + 表区；body 随后整体拼接
    const u32 version = 8;
    std::memcpy(out.data() + 0, &version, 4);
    std::memcpy(out.data() + 4, &ns, 2);
    std::memcpy(out.data() + 6, &nc, 2);
    std::memcpy(out.data() + 0xA, &texture.width, 2);
    std::memcpy(out.data() + 0xC, &texture.height, 2);
    std::memcpy(out.data() + 0xE, &texture.format, 2);
    put32(0x10, name_offset);
    const i16 sx = 0, sy = 0;
    std::memcpy(out.data() + 0x14, &sx, 2);
    std::memcpy(out.data() + 0x16, &sy, 2);
    const u32 priority = 1;
    put32(0x18, priority);
    put32(0x1C, thtx_offset);
    out[0x20] = texture.has_data ? 1 : 0;
    const u16 low = 0;
    std::memcpy(out.data() + 0x22, &low, 2);
    put32(0x24, 0);  // nextentry=0：链尾
    for (u32 i = 0; i < ns; i++) put32(64 + i * 4, sprite_offsets[i]);
    for (u32 i = 0; i < nc; i++) {
        put32(64 + ns * 4 + i * 8, u32(scripts[i].first));
        put32(64 + ns * 4 + i * 8 + 4, script_offsets[i]);
    }
    out.insert(out.end(), body.begin(), body.end());
    return out;
}

// 便捷：单精灵（覆盖全纹理）+ N 个脚本 的归档。
inline std::vector<u8> build_simple_anm(const std::vector<std::vector<u8>>& scripts) {
    SyntheticTexture texture;
    std::vector<std::array<float, 5>> sprites{{0.f, 0.f, 0.f, 2.f, 2.f}};
    std::vector<std::pair<i32, std::vector<u8>>> table;
    for (u32 i = 0; i < scripts.size(); i++) table.emplace_back(i32(i), scripts[i]);
    return build_anm(texture, sprites, table);
}

}  // namespace th13test
