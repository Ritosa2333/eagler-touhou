// th13 ECL 线程状态块（§2.1）与取参/压弹栈/帧切换。
// 字段对应原版 0x11C4 线程块（0x4793e0 分配）：time +0x0、当前指令 +0x4、
// 栈区 +0x8..+0x1007（条目 8 字节 {char 'i'/'f'; u32 值}）、SP +0x1008、
// FP +0x100C、线程 id +0x1010、宿主指针 +0x1014（此处以 EclCommands/EclVariables
// 接口呈现）、线程状态 +0x1018、难度位 +0x101C、8 插值通道 +0x1020..+0x119F、
// 通道挂接指令 +0x11A0、线程标志 +0x11C0（bit0 暂停）。
#pragma once

#include "EclResource.hpp"
#include "Interpolation.hpp"
#include "Rng.hpp"
#include "Types.hpp"

#include <array>

namespace th13 {

class EclContext;
class EclThreads;

// 游戏指令（opcode > 93）回调：原版经 EnemyInf 虚表槽 +0（0x41b250→0x41b260）分发。
// 返回契约：0 前移；1 冻结指令及脚本时钟（519/520，原版返回-1）；
// -1 本次解释器让出；-2 错误。让出仍推进插值通道。
class EclCommands {
public:
    virtual ~EclCommands() = default;
    virtual int execute(EclContext& context, u16 opcode) = 0;
};

// 线程宿主：callAsync/callAsyncId 派生（0x4793e0 头插、新线程下帧参战）。
class EclThreadHost {
public:
    virtual ~EclThreadHost() = default;
    virtual bool spawn(EclContext& source, i32 id, u32 skipped_arguments) = 0;
    virtual EclContext* find(i32 id) = 0;
    virtual void terminate_children() = 0;
};

// 全局变量（gvar）供给：原版经 EnemyInf 虚表 +4/+8/+0xC/+0x10（0x420380/0x420b80/
// 0x420d00/0x421670），码表 -10000..-9904 见 artifacts/reference/eclmap/th13.eclm；
// 其中 I0-I3/F0-F3（-9985..-9978）读宿主敌人参数槽（§2.2），由宿主实现方承担。
class EclVariables {
public:
    virtual ~EclVariables() = default;
    virtual bool integer(i32 id, i32& value) = 0;
    virtual bool floating(i32 id, float& value) = 0;
    virtual i32* integer_destination(i32 id) = 0;
    virtual float* float_destination(i32 id) = 0;
};

// 单线程上下文。纯脚本状态（栈/时钟/游标），不含进程地址，可整体拷贝存档。
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
    GameRandom* random = nullptr;  // 指令 93 随机源（原版 0x472e10 → 0x472c90 游戏侧 RNG）
    std::array<EclInterpolation, 8> interpolators{};
    std::array<const EclInstruction*, 8> interpolation_targets{};
    // Slot layout of the ENEMY-owned interrupt table (0x5234 + id*16).
    // 514 registers {life threshold, enemy-age timeout, health label, timeout label}.
    // 521 overrides ONLY the timeout label (+0x5240), not the health label.
    // ECL thread time is not the timeout clock: wait/goto can rewind it forever.
    struct Interrupt {
        i32 life = -1;  // +0x5234; negative = inactive
        i32 timeout = 0;  // +0x5238; zero = no timeout
        std::string health_label;  // +0x523c
        std::string timeout_label;  // +0x5240
    };
    std::string error;

    // 取参（0x478e60 整型 / 0x478ee0 浮点，判定已对反汇编逐条核对）：
    // 未置 mask 位 → 字面量；mask 置位且值 >= 0 → 局部变量字节偏移（FP 相对）；
    // 值 ∈ [-100,-1] → 表达式栈引用（偏移 = SP + 值×8，-1 为栈顶，pop=true 时弹栈）；
    // 值 < -100 → 全局变量码（整型原值 / 浮点位模式的整数值，如 -9989.0f → -9989）。
    bool integer(u32 argument, i32 literal, i32& value, bool pop = false) noexcept;
    bool floating(u32 argument, float literal, float& value, bool pop = false) noexcept;
    i32* integer_destination(u32 argument) noexcept;
    float* float_destination(u32 argument) noexcept;
    bool push(i32 value) noexcept;
    bool push(float value) noexcept;
    bool pop(i32& value) noexcept;
    bool pop(float& value) noexcept;
    bool enter_frame(i32 size) noexcept;  // 0x479a20 stackAlloc
    bool leave_frame() noexcept;          // 0x479a60 stackFree
    bool bind_code(const u8* bytes, u32 size, i32 routine_id = 0) noexcept;
    bool bind(EclProgram& source, i32 routine_id) noexcept;
    // 每帧步进（§3.2 主循环，实现见 EclInterpreter.cpp）。
    // 返回：0 本帧完成；-1 线程结束；-2 脚本错误（error 已置位）。
    int step(float elapsed);
};

}  // namespace th13
