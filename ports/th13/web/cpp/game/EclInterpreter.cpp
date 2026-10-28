// th13 ECL 基础解释器（0x4772a0 主循环，§3.2/§4.1 行为表）。
// opcode <= 93 内联执行；> 93 经 EclCommands（原版 EnemyInf 虚表槽 +0）分发。
// 无指令条数预算的原版会因纯 goto 环挂死，此处按 th15 经验保留 100000 步保险。
#include "EclContext.hpp"

#include <cmath>
#include <cstring>

namespace th13 {
namespace {
enum class Op : u16 {
    Nop = 0,
    Delete = 1,
    Return = 10,
    Call = 11,
    Jump = 12,
    JumpIfZero = 13,
    JumpIfNonzero = 14,
    CallAsync = 15,
    CallAsyncId = 16,
    KillAsync = 17,
    PauseAsync = 18,
    ResumeAsync = 19,
    SetThreadState = 20,
    KillAllAsync = 21,
    Nop22 = 22,  // §4.1：22/30/31 为 nop 变体
    WaitInteger = 23,
    WaitFloat = 24,
    EnterFrame = 40,
    LeaveFrame = 41,
    PushInt = 42,
    StoreInt = 43,
    PushFloat = 44,
    StoreFloat = 45,
    AddInt = 50,
    AddFloat = 51,
    SubtractInt = 52,
    SubtractFloat = 53,
    MultiplyInt = 54,
    MultiplyFloat = 55,
    DivideInt = 56,
    DivideFloat = 57,
    Remainder = 58,
    EqualInt = 59,
    EqualFloat = 60,
    NotEqualInt = 61,
    NotEqualFloat = 62,
    LessInt = 63,
    LessFloat = 64,
    LessEqualInt = 65,
    LessEqualFloat = 66,
    GreaterInt = 67,
    GreaterFloat = 68,
    GreaterEqualInt = 69,
    GreaterEqualFloat = 70,
    NotInt = 71,
    NotFloat = 72,
    LogicalOr = 73,
    LogicalAnd = 74,
    Xor = 75,
    Or = 76,
    And = 77,
    PostDecrement = 78,
    Sin = 79,
    Cos = 80,
    CirclePos = 81,
    ValidRad = 82,
    NegateInt = 83,
    NegateFloat = 84,
    SquareSum = 85,
    SquareSumRoot = 86,
    GetAngle = 87,
    StackSqrt = 88,
    Nop30 = 30,
    Nop31 = 31,
    LinearFunc = 89,
    PointRotate = 90,
    Interpolate = 91,
    InterpolateHermite = 92,
    RandomPolar = 93
};
constexpr float kPi = 3.1415927410125732421875f;
constexpr float kTwoPi = 6.283185482025146484375f;
}  // namespace

int EclContext::step(float elapsed) {
    if (!code || routine < 0 || instruction_offset < 0) return -1;
    auto fail = [&](const char* message) {
        error = message;
        return -2;
    };
    // 帧尾步进 8 条插值通道（§3.2）：duration != 0 的通道以挂接指令为取参上下文
    // 向目标变量（参数 1）写当前值。
    auto finish = [&](float rate) {
        const auto* saved = instruction;
        for (u32 slot = 0; slot < interpolators.size(); slot++)
            if (interpolators[slot].duration) {
                instruction = interpolation_targets[slot];
                if (!instruction) return fail("ECL interpolation target unavailable");
                auto* destination = float_destination(1);
                if (!destination) return -2;
                *destination = interpolators[slot].step(rate);
            }
        instruction = saved;
        return 0;
    };
    for (u32 steps = 0; steps < 100000; steps++) {
        // 游标恰在例程末尾：原版继续读后续字节（相邻 ECLH 头的时钟值）而挂起，
        // th13 脚本恒以 ret 收尾，此处按线程结束处理（内存安全优先）。
        if (instruction_offset == i32(code_size)) {
            routine = instruction_offset = -1;
            instruction = nullptr;
            return -1;
        }
        if (code_size < 16 || instruction_offset < 0 || u32(instruction_offset) > code_size - 16)
            return fail("ECL instruction outside routine");
        instruction = reinterpret_cast<const EclInstruction*>(code + instruction_offset);
        if (float(instruction->time) > time) {
            time = float(time + elapsed);
            return finish(elapsed);
        }
        u32 length = instruction->length;
        if (length < 16 || length % 4 || length > code_size - u32(instruction_offset))
            return fail("Invalid ECL instruction length");
        const Op op = Op(instruction->opcode);
        // 难度门（§3.4）：rank_mask 与线程难度字节按位与为零则仅前移。
        const bool enabled = (instruction->rank_mask & difficulty) != 0;
        bool ok = true, adjust = false;
        auto integer_arg = [&](u32 n, bool consume = false) {
            i32 value = 0;
            if (16 + n * 4 + 4 > length) {
                error = "Truncated ECL arguments";
                ok = false;
            } else if (!integer(n, instruction->argument<i32>(n), value, consume))
                ok = false;
            return value;
        };
        auto float_arg = [&](u32 n, bool consume = false) {
            float value = 0;
            if (16 + n * 4 + 4 > length) {
                error = "Truncated ECL arguments";
                ok = false;
            } else if (!floating(n, instruction->argument<float>(n), value, consume))
                ok = false;
            return value;
        };
        auto pop_int = [&]() {
            i32 value = 0;
            if (!pop(value)) ok = false;
            return value;
        };
        auto pop_float = [&]() {
            float value = 0;
            if (!pop(value)) ok = false;
            return value;
        };
        auto push_int = [&](i32 value) {
            if (!push(value)) ok = false;
        };
        auto push_float = [&](float value) {
            if (!push(value)) ok = false;
        };
        auto store_int = [&](u32 n, i32 value) {
            if (16 + n * 4 + 4 > length) {
                error = "Truncated ECL destination";
                ok = false;
                return;
            }
            auto* destination = integer_destination(n);
            if (!destination) ok = false;
            else *destination = value;
        };
        auto store_float = [&](u32 n, float value) {
            if (16 + n * 4 + 4 > length) {
                error = "Truncated ECL destination";
                ok = false;
                return;
            }
            auto* destination = float_destination(n);
            if (!destination) ok = false;
            else *destination = value;
        };
        if (enabled)
            switch (op) {
                case Op::Nop:
                case Op::Nop22:
                case Op::Nop30:
                case Op::Nop31:
                    adjust = true;
                    break;
                case Op::Delete:
                    routine = instruction_offset = -1;
                    instruction = nullptr;
                    return -1;
                case Op::Return: {
                    const int result = return_from_call();
                    if (result) return result;
                    length = instruction->length;
                    adjust = true;
                    break;
                }
                // 004772a0 case0xb passes the current context to00477030.
                // The callee shares its stack and suspends the caller until ret.
                case Op::Call:
                    const_cast<EclInstruction*>(instruction)->stack_adjust = 0;
                    if (!call_into(*this)) return -2;
                    continue;
                case Op::CallAsync:
                case Op::CallAsyncId: {
                    if (!threads) return fail("ECL thread host not bound");
                    i32 id = -1;
                    if (op == Op::CallAsyncId) {  // id 为名字后的内联参数（0x479480 按 id 查找）
                        const u32 offset = instruction->argument<u32>(0) + 20;
                        if (offset + 4 > length) return fail("Truncated ECL thread identifier");
                        i32 literal;
                        std::memcpy(&literal, reinterpret_cast<const u8*>(instruction) + offset, 4);
                        if (!integer(1, literal, id, true)) return -2;
                    }
                    if (!threads->spawn(*this, id, op == Op::CallAsyncId ? 1 : 0)) return -2;
                    break;
                }
                case Op::KillAsync:
                case Op::PauseAsync:
                case Op::ResumeAsync:
                case Op::SetThreadState: {
                    if (!threads) return fail("ECL thread host not bound");
                    const i32 id = integer_arg(0);
                    if (auto* target = threads->find(id)) {
                        if (op == Op::KillAsync)
                            target->instruction_offset = -1;
                        else if (op == Op::PauseAsync)
                            target->thread_flags |= 1;
                        else if (op == Op::ResumeAsync)
                            target->thread_flags &= ~1u;
                        else
                            target->thread_state = integer_arg(1);
                    }
                    adjust = true;
                    break;
                }
                case Op::KillAllAsync:
                    if (!threads) return fail("ECL thread host not bound");
                    threads->terminate_children();
                    break;
                // 跳转双参数：{相对本指令的 Δ, 绝对重置的 time}。
                case Op::Jump:
                case Op::JumpIfZero:
                case Op::JumpIfNonzero: {
                    if (length < 24) return fail("Truncated ECL jump");
                    const bool take = op == Op::Jump ? true : (pop_int() == 0) == (op == Op::JumpIfZero);
                    if (take) {
                        const i32 relative = instruction->argument<i32>(0), target_time = instruction->argument<i32>(1);
                        if (!ok) return -2;
                        instruction_offset = wrapping_add(instruction_offset, relative);
                        time = float(target_time);
                        continue;
                    }
                    adjust = true;
                    break;
                }
                case Op::WaitInteger:
                    time = float(time - float(integer_arg(0)));
                    adjust = true;
                    break;
                case Op::WaitFloat:
                    time = float(time - float_arg(0));
                    adjust = true;
                    break;
                case Op::EnterFrame:
                    if (!enter_frame(integer_arg(0))) ok = false;
                    adjust = true;
                    break;
                case Op::LeaveFrame:
                    if (!leave_frame()) ok = false;
                    adjust = true;
                    break;
                case Op::PushInt:
                    push_int(integer_arg(0, true));
                    break;
                case Op::PushFloat:
                    push_float(float_arg(0, true));
                    break;
                case Op::StoreInt: {
                    auto* destination = integer_destination(0);
                    if (!destination) ok = false;
                    const i32 value = pop_int();
                    if (destination) *destination = value;
                    break;
                }
                case Op::StoreFloat: {
                    auto* destination = float_destination(0);
                    if (!destination) ok = false;
                    const float value = pop_float();
                    if (destination) *destination = value;
                    break;
                }
                case Op::AddInt:
                case Op::SubtractInt:
                case Op::MultiplyInt:
                case Op::DivideInt:
                case Op::Remainder:
                case Op::EqualInt:
                case Op::NotEqualInt:
                case Op::LessInt:
                case Op::LessEqualInt:
                case Op::GreaterInt:
                case Op::GreaterEqualInt:
                case Op::LogicalOr:
                case Op::LogicalAnd:
                case Op::Xor:
                case Op::Or:
                case Op::And: {
                    const i32 right = pop_int(), left = pop_int();
                    i32 value = 0;
                    switch (op) {
                        case Op::AddInt: value = wrapping_add(left, right); break;
                        case Op::SubtractInt: value = wrapping_sub(left, right); break;
                        case Op::MultiplyInt: value = wrapping_mul(left, right); break;
                        case Op::DivideInt:
                        case Op::Remainder:
                            if (!right || (left == INT32_MIN && right == -1)) return fail("ECL integer divide error");
                            value = op == Op::DivideInt ? left / right : left % right;
                            break;
                        case Op::EqualInt: value = left == right; break;
                        case Op::NotEqualInt: value = left != right; break;
                        case Op::LessInt: value = left < right; break;
                        case Op::LessEqualInt: value = left <= right; break;
                        case Op::GreaterInt: value = left > right; break;
                        case Op::GreaterEqualInt: value = left >= right; break;
                        case Op::LogicalOr: value = left || right; break;
                        case Op::LogicalAnd: value = left && right; break;
                        case Op::Xor: value = left ^ right; break;
                        case Op::Or: value = left | right; break;
                        default: value = left & right; break;
                    }
                    push_int(value);
                    if (u16(op) <= 58) const_cast<EclInstruction*>(instruction)->stack_adjust = 0;
                    break;
                }
                case Op::AddFloat:
                case Op::SubtractFloat:
                case Op::MultiplyFloat:
                case Op::DivideFloat: {
                    const float right = pop_float(), left = pop_float();
                    push_float(op == Op::AddFloat ? float(left + right)
                                 : op == Op::SubtractFloat ? float(left - right)
                                 : op == Op::MultiplyFloat ? float(left * right)
                                                           : float(left / right));
                    const_cast<EclInstruction*>(instruction)->stack_adjust = 0;
                    break;
                }
                case Op::EqualFloat:
                case Op::NotEqualFloat:
                case Op::LessFloat:
                case Op::LessEqualFloat:
                case Op::GreaterFloat:
                case Op::GreaterEqualFloat: {
                    const float right = pop_float(), left = pop_float();
                    push_int(op == Op::EqualFloat ? left == right
                             : op == Op::NotEqualFloat ? left != right
                             : op == Op::LessFloat ? left < right
                             : op == Op::LessEqualFloat ? left <= right
                             : op == Op::GreaterFloat ? left > right
                                                       : left >= right);
                    break;
                }
                case Op::NotInt: push_int(pop_int() == 0); break;
                case Op::NotFloat: push_int(pop_float() == 0); break;
                case Op::NegateInt: push_int(signed_bits(0u - u32(pop_int()))); break;
                case Op::NegateFloat: push_float(float(-pop_float())); break;
                case Op::PostDecrement: {  // $A--：压旧值，目标写回 旧值-1
                    const i32 value = integer_arg(0);
                    store_int(0, wrapping_add(value, -1));
                    push_int(value);
                    break;
                }
                case Op::Sin:
                case Op::Cos:
                case Op::StackSqrt: {
                    const double value = double(pop_float());
                    push_float(float(op == Op::Sin ? std::sin(value) : op == Op::Cos ? std::cos(value) : std::sqrt(value)));
                    break;
                }
                // circlePos（0x479540）：角度/半径 → 目标 0/1 写 cosθ·r, sinθ·r。
                case Op::CirclePos: {
                    const float angle = float_arg(2), radius = float_arg(3);
                    const float x = float(std::cos(double(angle)) * double(radius));
                    const float y = float(std::sin(double(angle)) * double(radius));
                    store_float(0, x);
                    store_float(1, y);
                    adjust = true;
                    break;
                }
                case Op::ValidRad: store_float(0, normalize_angle(float_arg(0))); adjust = true; break;
                case Op::SquareSum:
                case Op::SquareSumRoot: {
                    const float x = float_arg(1), y = float_arg(2);
                    // x87「积和宽精度、和存 f32 再开方」链（三点实证 e77/e78/e80）：
                    // s = f32(x²+y²)（积与和在 80 位精确，存和时一次舍入），
                    // r = f32(sqrt(s))。f32 逐级舍入与全程 double 均与 oracle
                    // 存在单 ulp 偏差，勿改。
                    const float sum = float(double(y) * double(y) + double(x) * double(x));
                    store_float(0, op == Op::SquareSumRoot ? float(std::sqrt(double(sum)))
                                                           : sum);
                    adjust = true;
                    break;
                }
                case Op::GetAngle: {  // 0x4977aa 两点 atan2
                    const float ax = float_arg(1), ay = float_arg(2), bx = float_arg(3), by = float_arg(4);
                    store_float(0, float(std::atan2(double(float(by - ay)), double(float(bx - ax)))));
                    adjust = true;
                    break;
                }
                case Op::LinearFunc: {  // 0x4794d0 角差 wrap ±π
                    const float from = float_arg(1), to = float_arg(2);
                    float difference = float(to - from);
                    if (difference > kPi)
                        difference = float(to - float(from + kTwoPi));
                    else if (float(from - to) > kPi)
                        difference = float(to - float(from - kTwoPi));
                    store_float(0, difference);
                    adjust = true;
                    break;
                }
                case Op::PointRotate: {
                    const float x = float_arg(2), y = float_arg(3), angle = normalize_angle(float_arg(4));
                    const float sine = float(std::sin(double(angle))), cosine = float(std::cos(double(angle)));
                    const float rx = float(float(cosine * x) - float(sine * y));
                    const float ry = float(float(cosine * y) + float(sine * x));
                    store_float(0, rx);
                    store_float(1, ry);
                    adjust = true;
                    break;
                }
                // 插值通道（0x4772a0 case 0x5b/0x5c）：参数 0=槽号、2=帧数、3=模式、
                // 4/5=起止；目标 1 写起始值；本指令挂接到通道（帧尾取参上下文）。
                case Op::Interpolate:
                case Op::InterpolateHermite: {
                    const i32 slot = integer_arg(0);
                    if (slot < 0 || u32(slot) >= interpolators.size()) return fail("ECL interpolation slot outside context");
                    auto& interpolation = interpolators[slot];
                    interpolation_targets[slot] = instruction;
                    const i32 frames = integer_arg(2), mode = integer_arg(3);
                    const float from = float_arg(4), to = float_arg(5);
                    interpolation.begin(frames, mode, from, to);
                    store_float(1, from);
                    if (op == Op::InterpolateHermite) {
                        interpolation.control1 = float_arg(6);
                        interpolation.control2 = float_arg(7);
                    }
                    adjust = true;
                    break;
                }
                case Op::RandomPolar: {  // 0x472e10 两次 rand 合成环内随机点
                    if (!random) return fail("ECL random generator not bound");
                    const float minimum = float_arg(2), maximum = float_arg(3);
                    const float angle = float(random->signed_unit() * kPi);
                    const float radius = float(float(random->signed_unit() * float(maximum - minimum)) + minimum);
                    store_float(0, float(std::cos(double(angle)) * double(radius)));
                    store_float(1, float(std::sin(double(angle)) * double(radius)));
                    adjust = true;
                    break;
                }
                default:
                    // 游戏指令（>93）：0 前移 / 1 让出本帧 / -1 终止 / -2 错误。
                    // Port result1 is StageCommands' blocked519/520 result.
                    // Retail returns-1 here and jumps478bac ->4773a3,
                    // bypassing the time increment; interpolation still ticks.
                    if (commands) {
                        const int result = commands->execute(*this, u16(op));
                        if (result == -2) return result;
                        if (result == -1) return finish(elapsed);
                        if (result == 1) {
                            return finish(elapsed);
                        }
                        adjust = true;
                    } else {
                        error = "Unimplemented ECL opcode " + std::to_string(u16(op));
                        return -2;
                    }
                    break;
            }
        if (!ok) return -2;
        // 执行成功后统一扣 stack_adjust（0x4772a0 caseD_0；难度过滤跳过的指令不扣）。
        if (enabled && adjust && instruction->stack_adjust) {
            if (stack_pointer < 0 || u32(stack_pointer) < instruction->stack_adjust)
                return fail("ECL instruction stack cleanup underflow");
            stack_pointer -= i32(instruction->stack_adjust);
        }
        instruction_offset = wrapping_add(instruction_offset, i32(length));
    }
    return fail("ECL instruction budget exceeded");
}

}  // namespace th13
