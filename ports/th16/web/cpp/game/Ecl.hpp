// th16 ECL：资源 0x474530 / 查找 0x474740；通用解释器 0x472030（操作码 0..0x5d）；
// 调用/异步线程 0x471db0 / 0x474430；变量读写经宿主 vtable（敌人为 0x423810 / 0x423f80）。
#pragma once
#include "AnmVm.hpp"
#include "Rng.hpp"
#include "Types.hpp"
#include <array>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

namespace th16 {

struct EclInstruction {
    i32 time = 0;
    u16 opcode = 0;
    u16 length = 0;
    u16 references = 0;
    u8 difficulty = 0;
    u8 parameter_count = 0;
    u8 stack_adjustment = 0;
    u8 reserved[3]{};

    template <typename T = u32>
    T argument(u32 index = 0) const noexcept {
        T v{};
        std::memcpy(&v, reinterpret_cast<const u8*>(this) + 16 + index * 4, sizeof(T));
        return v;
    }
};
static_assert(sizeof(EclInstruction) == 16, "ECL 指令头 16 字节");

struct EclSubroutine {
    std::string name;
    u32 offset = 0;
    u32 size = 0;
};

class EclResource {
public:
    std::vector<u8> bytes;
    std::vector<EclSubroutine> subroutines;
    std::vector<std::string> animations;
    std::vector<std::string> includes;
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
        const EclResource* file = nullptr;
        u32 index = 0;
    };
    std::vector<std::unique_ptr<EclResource>> files;
    std::vector<Definition> definitions;
    std::string error;

    i32 attach(const u8* data, u32 size, EclResourceProvider* provider = nullptr);
    bool load(const std::string& name, EclResourceProvider& provider);
    i32 find_index(const char* name) const noexcept;
    const EclSubroutine& subroutine(u32 index) const noexcept {
        return definitions[index].file->subroutines[definitions[index].index];
    }
};

class EclContext;
class EclCommands {
public:
    virtual ~EclCommands() = default;
    // 0 继续并前进；1 重读当前上下文（call 后）；-1 本帧结束；-2 错误
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
    bool referenced(u32 argument) const noexcept;
    u32 bits(i32 byte_offset) const noexcept;
    bool local_offset(i32 byte_offset, i32& result) noexcept;
    bool expression(i32 byte_offset, u32& value, u8& type) noexcept;
    bool select_routine(i32 id) noexcept;
    int return_from_call() noexcept;

public:
    // 0x471db0：以本上下文当前指令（call / 异步 spawn）的名字与参数在 target 上建立调用帧；
    // skipped_arguments 为名字之后、参数之前要跳过的 4 字节参数数（op 16 的线程 id）
    bool call_into(EclContext& target, u32 skipped_arguments = 0);
    float time = 0;
    i32 routine = -1;
    i32 instruction_offset = -1;
    std::array<u8, 4096> stack{};
    i32 stack_pointer = 0;
    i32 local_base = 0;
    const EclInstruction* instruction = nullptr;
    EclVariables* variables = nullptr;
    const u8* code = nullptr;
    u32 code_size = 0;
    u8 difficulty = 0xff;
    EclProgram* program = nullptr;
    EclThreadHost* threads = nullptr;
    i32 thread_id = 0;
    i32 thread_state = 0;
    u32 thread_flags = 0;
    EclCommands* commands = nullptr;
    Rng* game_rng = nullptr;
    // ECL 插值的推进速度。原版的计时推进（0x406190）每次直接读全局速度 DAT_004a5788，ECL 547 在本帧改了速度后
    // 收尾阶段的插值就按新值走：Enemy::update 把 live_rate 指向这一份；独立驱动（harness）时用 interpolation_rate
    float interpolation_rate = 1;
    const float* live_rate = nullptr;
    float rate() const noexcept { return live_rate != nullptr ? *live_rate : interpolation_rate; }
    std::array<Interp<float>, 8> interpolators{};
    std::array<const EclInstruction*, 8> interpolation_targets{};
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
    // 0x472030：返回 0 继续（本帧耗尽时间）、-1 删除/结束、-2 错误
    int step(float elapsed);   // 0x473bc0 的 elapsed：调用当时的全局速度，本步内不变
};

}  // namespace th16
