#pragma once

#include "ecl_test_fixtures.hpp"

#include "game/EclContext.hpp"
#include "game/EclFile.hpp"
#include "game/EclResource.hpp"
#include "game/EclThreads.hpp"

namespace th13test {
using namespace th13;

struct RecordedCall {
    u16 opcode;
    std::vector<i32> ints;
    std::vector<float> floats;
    bool operator==(const RecordedCall& other) const {
        return opcode == other.opcode && ints == other.ints && floats == other.floats;
    }
};

class RecordingCommands final : public EclCommands {
public:
    std::vector<RecordedCall> calls;
    int execute(EclContext& context, u16 opcode) override {
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
        if (opcode == 548) {
            const i32 index = (context.difficulty & 8)   ? 3
                              : (context.difficulty & 4) ? 2
                              : (context.difficulty & 2) ? 1
                                                         : 0;
            i32 frames = 0;
            if (u32(index) < call.ints.size()) frames = call.ints[index];
            context.time -= float(frames);
        }
        return 0;
    }
};

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

}
