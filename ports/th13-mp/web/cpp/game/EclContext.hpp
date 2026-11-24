#pragma once

#include "EclResource.hpp"
#include "Interpolation.hpp"
#include "Rng.hpp"
#include "Types.hpp"

#include <array>

namespace th13 {

class EclContext;
class EclThreads;

class EclCommands {
public:
    virtual ~EclCommands() = default;
    virtual int execute(EclContext& context, u16 opcode) = 0;
};

class EclThreadHost {
public:
    virtual ~EclThreadHost() = default;
    virtual bool spawn(EclContext& source, i32 id, u32 skipped_arguments) = 0;
    virtual EclContext* find(i32 id) = 0;
    virtual void terminate_children() = 0;
};

class EclVariables {
public:
    virtual ~EclVariables() = default;
    virtual bool integer(i32 id, i32& value) = 0;
    virtual bool floating(i32 id, float& value) = 0;
    virtual i32* integer_destination(i32 id) = 0;
    virtual float* float_destination(i32 id) = 0;
};

class EclContext {
    friend class EclThreads;
    bool referenced(u32 argument) const noexcept;
    u32 bits(i32 byte_offset) const noexcept;
    bool local_offset(i32 byte_offset, i32& result) noexcept;
    bool expression(i32 byte_offset, u32& value, u8& type) noexcept;
    bool select_routine(i32 id) noexcept;
    bool call_into(EclContext& target, u32 skipped_arguments = 0);
    int return_from_call() noexcept;

public:
    float time = 0;
    i32 routine = -1, instruction_offset = -1;
    std::array<u8, 4096> stack{};
    i32 stack_pointer = 0, local_base = 0;
    const EclInstruction* instruction = nullptr;
    EclVariables* variables = nullptr;
    const u8* code = nullptr;
    u32 code_size = 0;
    u8 difficulty = 255;
    EclProgram* program = nullptr;
    EclThreadHost* threads = nullptr;
    i32 thread_id = -1, thread_state = 0;
    u32 thread_flags = 0;
    EclCommands* commands = nullptr;
    GameRandom* random = nullptr;
    std::array<EclInterpolation, 8> interpolators{};
    std::array<const EclInstruction*, 8> interpolation_targets{};
    struct Interrupt {
        i32 life = -1;
        i32 timeout = 0;
        std::string health_label;
        std::string timeout_label;
    };
    std::string error;

    bool integer(u32 argument, i32 literal, i32& value, bool pop = false) noexcept;
    bool floating(u32 argument, float literal, float& value, bool pop = false) noexcept;
    i32* integer_destination(u32 argument) noexcept;
    float* float_destination(u32 argument) noexcept;
    bool push(i32 value) noexcept;
    bool push(float value) noexcept;
    bool pop(i32& value) noexcept;
    bool pop(float& value) noexcept;
    bool enter_frame(i32 size) noexcept;
    bool leave_frame() noexcept;
    bool bind_code(const u8* bytes, u32 size, i32 routine_id = 0) noexcept;
    bool bind(EclProgram& source, i32 routine_id) noexcept;
    int step(float elapsed);
};

}
