// UI 基座单测夹具：合成 ANM 资源（外置纹理 + 指定脚本体）与平台服务桩。
// 合成资源只覆盖 AnmResource::open 接受的布局，不模拟真实美术；
// 状态机断言不依赖脚本视觉效果。
#pragma once

#include "game/AnmManager.hpp"
#include "game/AnmEnvironment.hpp"
#include "game/Rng.hpp"

#include <cstring>
#include <string>
#include <vector>

namespace th13::test {

// 一条 ANM 指令：opcode/length/time/mask + i32 参数序列。
struct AnmIns {
    i16 opcode;
    i16 time = 0;
    std::vector<i32> args;
    u16 mask = 0;
};

inline AnmIns label(i32 id) { return AnmIns{5, 0, {id}}; }
inline AnmIns wait() { return AnmIns{3, 0, {}}; }
inline AnmIns stop() { return AnmIns{-1, 0, {}}; }
inline AnmIns sprite(i32 index) { return AnmIns{300, 0, {index}}; }
inline AnmIns spawn(i32 script) { return AnmIns{500, 0, {script}}; }

inline u32 instruction_length(const AnmIns& ins) { return u32(8 + ins.args.size() * 4); }

// 合成单 entry ANM：外置纹理 "s.png"（64×64）、ns 个 sprite、给定脚本体。
inline std::vector<u8> synthetic_anm(u32 sprites, const std::vector<std::vector<AnmIns>>& scripts) {
    const u32 ns = sprites, nc = u32(scripts.size());
    const u32 table_end = 64 + ns * 4 + nc * 8;
    u32 sprite_area = ns * 20;
    std::vector<u8> body;  // sprite 记录 + 脚本体，拼接在表后
    std::vector<u32> sprite_offsets(ns), script_offsets(nc);
    u32 cursor = table_end;
    for (u32 i = 0; i < ns; i++) {
        sprite_offsets[i] = cursor;
        const u8 record[20]{};  // source_id=0, x=y=w=h=0
        body.insert(body.end(), record, record + 20);
        cursor += 20;
    }
    for (u32 i = 0; i < nc; i++) {
        script_offsets[i] = cursor;
        for (const auto& ins : scripts[i]) {
            u8 buffer[8]{};
            std::memcpy(buffer + 0, &ins.opcode, 2);
            const u16 length = u16(instruction_length(ins));
            std::memcpy(buffer + 2, &length, 2);
            std::memcpy(buffer + 4, &ins.time, 2);
            std::memcpy(buffer + 6, &ins.mask, 2);
            body.insert(body.end(), buffer, buffer + 8);
            for (const auto arg : ins.args) {
                u8 argument[4];
                std::memcpy(argument, &arg, 4);
                body.insert(body.end(), argument, argument + 4);
            }
            cursor += instruction_length(ins);
        }
    }
    const char name[] = "s.png";
    const u32 name_at = cursor;
    std::vector<u8> chunk(table_end + body.size() + sizeof name, 0);
    auto put16 = [&](u32 at, u16 value) { std::memcpy(chunk.data() + at, &value, 2); };
    auto put32 = [&](u32 at, u32 value) { std::memcpy(chunk.data() + at, &value, 4); };
    put32(0, 8);      // version
    put16(4, u16(ns));
    put16(6, u16(nc));
    put16(10, 64);    // width
    put16(12, 64);    // height
    put16(14, 5);     // format（外置时不用）
    put32(16, name_at);
    chunk[32] = 0;    // hasData=0 → External
    put32(36, 0);     // stride=0（单 chunk）
    for (u32 i = 0; i < ns; i++) put32(64 + i * 4, sprite_offsets[i]);
    for (u32 i = 0; i < nc; i++) {
        put32(64 + ns * 4 + i * 8 + 0, 0);                  // source_id
        put32(64 + ns * 4 + i * 8 + 4, script_offsets[i]);  // offset
    }
    std::memcpy(chunk.data() + table_end, body.data(), body.size());
    std::memcpy(chunk.data() + name_at, name, sizeof name);
    return chunk;
}

// n 个平凡脚本（park 即停）：供状态机全链路使用。
inline std::vector<std::vector<AnmIns>> trivial_scripts(u32 n, u32 sprite_count = 8) {
    std::vector<std::vector<AnmIns>> scripts(n);
    for (u32 i = 0; i < n; i++) scripts[i] = {wait(), stop()};
    (void)sprite_count;
    return scripts;
}

// 装一个合成 bank；失败返回 false。
inline bool load_bank(AnmManager& manager, i32 id, const std::vector<u8>& bytes) {
    return manager.load(id, bytes.data(), u32(bytes.size()));
}

}  // namespace th13::test
