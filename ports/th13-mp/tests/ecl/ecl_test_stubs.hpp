// ECL 虚拟机单测共享桩：记录型游戏指令 / 全局变量供给（依赖 VM 全模块）。
#pragma once

#include "ecl_test_fixtures.hpp"

#include "game/EclContext.hpp"
#include "game/EclFile.hpp"
#include "game/EclResource.hpp"
#include "game/EclThreads.hpp"

namespace th13test {
using namespace th13;
// ---------- 记录型桩 ----------

struct RecordedCall {
    u16 opcode;
    std::vector<i32> ints;
    std::vector<float> floats;
    bool operator==(const RecordedCall& other) const { return opcode == other.opcode && ints == other.ints && floats == other.floats; }
};

// 游戏指令桩：按 eclmap 记录每条指令按签名解析后的实参（int/float 各一路），
// 用于验证取参路径（字面量/局部/表达式栈/全局）对真实脚本产出的值。
class RecordingCommands final : public EclCommands {
public:
    std::vector<RecordedCall> calls;
    int execute(EclContext& context, u16 opcode) override {
        // 每条指令最多取 8 个参数：先按 float 解析（值域无损），同时记录 int 视图。
        // 真实脚本中 int/float 参数的排布由断言侧按 dump 决定，桩只负责留痕。
        RecordedCall call;
        call.opcode = opcode;
        const EclInstruction* instruction = context.instruction;
        for (u32 i = 0; i < instruction->parameter_count && i < 8; ++i) {
            if (16 + i * 4 + 4 > instruction->length) break;
            i32 integer = 0;
            float floating = 0;
            context.integer(i, instruction->argument<i32>(i), integer, false);
            context.floating(i, instruction->argument<float>(i), floating, false);
            call.ints.push_back(integer);
            call.floats.push_back(floating);
        }
        calls.push_back(call);
        // ins_548 diffWait（0x41f2c4 ★）：thread.time -= 按难度选取的参数。
        // 桩必须实现它，否则依赖 548 停帧的脚本（GirlA02_at 等）会无 wait 死循环。
        if (opcode == 548) {
            const i32 index = (context.difficulty & 8) ? 3 : (context.difficulty & 4) ? 2 : (context.difficulty & 2) ? 1 : 0;
            i32 frames = 0;
            if (u32(index) < call.ints.size()) frames = call.ints[index];
            context.time -= float(frames);
        }
        return 0;
    }
};

// 全局变量桩：-9989 ANGLE_PLAYER / -9988 TIME / -10000 RAND 等按需注入；
// 目标槽以 map 兜底（st01 冒烟里有 `[-9949] = 0` 这类全局写）。
class StubVariables final : public EclVariables {
public:
    std::vector<std::pair<i32, i32>> integers;
    std::vector<std::pair<i32, float>> floats;
    i32 default_integer = 0;
    float default_float = 0;
    bool integer(i32 id, i32& value) override {
        for (const auto& pair : integers)
            if (pair.first == id) {
                value = pair.second;
                return true;
            }
        const auto slot = integer_slots.find(id);
        if (slot != integer_slots.end()) {
            value = slot->second;
            return true;
        }
        value = default_integer;
        return true;
    }
    bool floating(i32 id, float& value) override {
        for (const auto& pair : floats)
            if (pair.first == id) {
                value = pair.second;
                return true;
            }
        const auto slot = float_slots.find(id);
        if (slot != float_slots.end()) {
            value = slot->second;
            return true;
        }
        value = default_float;
        return true;
    }
    i32* integer_destination(i32 id) override { return &integer_slots[id]; }
    float* float_destination(i32 id) override { return &float_slots[id]; }

private:
    std::map<i32, i32> integer_slots;
    std::map<i32, float> float_slots;
};

}  // namespace th13test
