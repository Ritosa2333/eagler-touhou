// th16 ECL 装载与通用解释器——行为对齐 0x474530 / 0x474740 / 0x472030 / 0x471db0。
#include "Ecl.hpp"
#include "Motion.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace th16 {
namespace {

template <typename T>
T read(const u8* p) {
    T v{};
    std::memcpy(&v, p, sizeof(T));
    return v;
}
bool span(u32 off, u32 n, u32 size) { return off <= size && n <= size - off; }
bool read_name(const u8* b, u32& p, u32 limit, std::string& out) {
    const u32 start = p;
    while (p < limit && b[p]) ++p;
    if (p == limit) return false;
    out.assign(reinterpret_cast<const char*>(b + start), p - start);
    ++p;
    return true;
}

enum class Op : u16 {
    Nop = 0, Delete = 1, Return = 10, Call = 11, Jump = 12, JumpIfZero = 13, JumpIfNonzero = 14,
    Spawn = 15, SpawnIdentified = 16, TerminateThread = 17, PauseThread = 18, ResumeThread = 19,
    SetThreadState = 20, TerminateChildren = 21, DebugBreak = 22, WaitInteger = 23, WaitFloat = 24,
    Printf = 30, PrintStack = 31,
    EnterFrame = 40, LeaveFrame = 41, PushInt = 42, StoreInt = 43, PushFloat = 44, StoreFloat = 45,
    AddInt = 50, AddFloat = 51, SubtractInt = 52, SubtractFloat = 53, MultiplyInt = 54, MultiplyFloat = 55,
    DivideInt = 56, DivideFloat = 57, Remainder = 58,
    EqualInt = 59, EqualFloat = 60, NotEqualInt = 61, NotEqualFloat = 62,
    LessInt = 63, LessFloat = 64, LessEqualInt = 65, LessEqualFloat = 66,
    GreaterInt = 67, GreaterFloat = 68, GreaterEqualInt = 69, GreaterEqualFloat = 70,
    NotInt = 71, NotFloat = 72, LogicalOr = 73, LogicalAnd = 74, Xor = 75, Or = 76, And = 77,
    PostDecrement = 78, Sin = 79, Cos = 80, Polar = 81, NormalizeAngle = 82,
    NegateInt = 83, NegateFloat = 84, LengthSquared = 85, Length = 86, AngleTo = 87, Sqrt = 88,
    AngleDifference = 89, RotateVector = 90, Interpolate = 91, InterpolateHermite = 92, RandomPolar = 93
};

}  // namespace

bool EclResource::open(const u8* source, u32 size) {
    if (!source || size < 36 || read<u32>(source) != 0x54504353 || read<u16>(source + 4) != 1) return false;
    const u32 header = read<u16>(source + 6), count = read<u32>(source + 16), table = 36 + header;
    if (!span(table, count * 4, size)) return false;
    EclResource next;
    if (header) {
        u32 p = 36;
        const u32 limit = table;
        if (!span(p, 8, limit)) return false;
        if (read<u32>(source + p) == 0x4d494e41) {
            const u32 n = read<u32>(source + p + 4);
            p += 8;
            if (n > limit - p) return false;
            for (u32 i = 0; i < n; ++i) {
                std::string item;
                if (!read_name(source, p, limit, item)) return false;
                next.animations.push_back(std::move(item));
            }
            p = (p + 3) & ~3u;
            if (!span(p, 8, limit)) return false;
            if (read<u32>(source + p) == 0x494c4345) {
                const u32 n = read<u32>(source + p + 4);
                p += 8;
                if (n > limit - p) return false;
                for (u32 i = 0; i < n; ++i) {
                    std::string item;
                    if (!read_name(source, p, limit, item)) return false;
                    next.includes.push_back(std::move(item));
                }
            }
        }
    }
    u32 names = table + count * 4;
    std::vector<u32> offsets;
    offsets.reserve(count);
    for (u32 i = 0; i < count; ++i) {
        const u32 off = read<u32>(source + table + i * 4);
        if (!span(off, 16, size) || off < names || std::memcmp(source + off, "ECLH", 4)) return false;
        offsets.push_back(off);
    }
    const u32 names_end = offsets.empty() ? size : *std::min_element(offsets.begin(), offsets.end());
    for (u32 i = 0; i < count; ++i) {
        EclSubroutine sub;
        if (!read_name(source, names, names_end, sub.name)) return false;
        sub.offset = offsets[i];
        u32 end = size;
        for (u32 other : offsets) if (other > sub.offset) end = std::min(end, other);
        sub.size = end - sub.offset;
        for (u32 p = sub.offset + 16; p < end;) {
            if (!span(p, 16, end)) return false;
            const u32 length = read<u16>(source + p + 6);
            if (length < 16 || length % 4 || !span(p, length, end)) return false;
            p += length;
        }
        next.subroutines.push_back(std::move(sub));
    }
    next.bytes.assign(source, source + size);
    *this = std::move(next);
    return true;
}

i32 EclProgram::attach(const u8* data, u32 size, EclResourceProvider* provider) {
    if (files.size() >= 32) { error = "ECL include limit exceeded"; return -1; }
    auto file = std::make_unique<EclResource>();
    if (!file->open(data, size)) { error = "Invalid ECL resource"; return -1; }
    const i32 result = i32(files.size());
    const auto* current = file.get();
    const bool first = files.empty();
    files.push_back(std::move(file));
    for (u32 i = 0; i < current->subroutines.size(); ++i) {
        Definition def{current, i};
        if (first) definitions.push_back(def);
        else {
            const auto& key = current->subroutines[i].name;
            auto at = definitions.begin();
            while (at != definitions.end() &&
                   std::strcmp(key.c_str(), at->file->subroutines[at->index].name.c_str()) > 0) ++at;
            definitions.insert(at, def);
        }
    }
    if (provider) {
        for (u32 i = 0; i < current->animations.size(); ++i) {
            if (!provider->animation(i + 10, current->animations[i])) {
                error = "Missing animation: " + current->animations[i];
                return result;
            }
        }
        for (const auto& name : current->includes) {
            if (!load(name, *provider)) return result;
        }
    }
    return result;
}

bool EclProgram::load(const std::string& name, EclResourceProvider& provider) {
    std::vector<u8> data;
    if (!provider.read(name, data)) { error = "Missing ECL: " + name; return false; }
    return attach(data.data(), u32(data.size()), &provider) >= 0;
}

i32 EclProgram::find_index(const char* name) const noexcept {
    if (!name) return -1;
    i32 low = 0, high = i32(definitions.size()) - 1;
    while (low <= high) {
        const i32 mid = low + (high - low) / 2;
        const int order = std::strcmp(name, subroutine(u32(mid)).name.c_str());
        if (!order) return mid;
        if (order < 0) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}

bool EclContext::referenced(u32 argument) const noexcept {
    return instruction && (instruction->references & (1u << (argument & 31)));
}
u32 EclContext::bits(i32 byte_offset) const noexcept {
    u32 value;
    std::memcpy(&value, stack.data() + byte_offset, 4);
    return value;
}
bool EclContext::local_offset(i32 offset, i32& result) noexcept {
    const i64 address = i64(local_base) + offset;
    if (address < 0 || address > i64(stack.size()) - 4) { error = "ECL local variable outside stack"; return false; }
    result = i32(address);
    return true;
}
bool EclContext::expression(i32 offset, u32& value, u8& type) noexcept {
    if (offset < 0 || offset > i32(stack.size()) - 8) { error = "ECL expression stack underflow"; return false; }
    type = stack[u32(offset)];
    value = bits(offset + 4);
    return true;
}
bool EclContext::integer(u32 argument, i32 literal, i32& value, bool pop) noexcept {
    if (!referenced(argument)) { value = literal; return true; }
    if (literal >= 0) {
        i32 address;
        if (!local_offset(literal, address)) return false;
        value = signed_bits(bits(address));
        return true;
    }
    if (literal >= -100) {
        u32 raw;
        u8 type;
        const i32 offset = pop ? wrapping_sub(stack_pointer, 8)
                               : wrapping_add(stack_pointer, signed_bits(u32(literal) * 8));
        if (!expression(offset, raw, type)) return false;
        if (pop) stack_pointer = offset;
        value = type == 'f' ? truncate_int(float_from_bits(raw)) : signed_bits(raw);
        return true;
    }
    if (!variables || !variables->integer(literal, value)) { error = "ECL integer global not bound"; return false; }
    return true;
}
bool EclContext::floating(u32 argument, float literal, float& value, bool pop) noexcept {
    if (!referenced(argument)) { value = literal; return true; }
    if (literal >= 0) {
        i32 address;
        if (!local_offset(truncate_int(literal), address)) return false;
        value = float_from_bits(bits(address));
        return true;
    }
    if (literal <= -1 && literal >= -100) {
        u32 raw;
        u8 type;
        const i32 offset = pop ? wrapping_sub(stack_pointer, 8)
                               : wrapping_add(stack_pointer, signed_bits(u32(truncate_int(literal)) * 8));
        if (!expression(offset, raw, type)) return false;
        if (pop) stack_pointer = offset;
        value = type == 'i' ? float(signed_bits(raw)) : float_from_bits(raw);
        return true;
    }
    if (!variables || !variables->floating(truncate_int(literal), value)) { error = "ECL float global not bound"; return false; }
    return true;
}
i32* EclContext::integer_destination(u32 argument) noexcept {
    if (!referenced(argument)) { error = "ECL integer destination is not a reference"; return nullptr; }
    const i32 id = instruction->argument<i32>(argument);
    if (id >= 0) {
        i32 address;
        if (!local_offset(id, address)) return nullptr;
        return reinterpret_cast<i32*>(stack.data() + address);
    }
    if (!variables) { error = "ECL integer global not bound"; return nullptr; }
    auto* result = variables->integer_destination(id);
    if (!result) error = "ECL integer global destination not bound";
    return result;
}
float* EclContext::float_destination(u32 argument) noexcept {
    if (!referenced(argument)) { error = "ECL float destination is not a reference"; return nullptr; }
    const float literal = instruction->argument<float>(argument);
    const i32 id = truncate_int(literal);
    if (literal >= 0) {
        i32 address;
        if (!local_offset(id, address)) return nullptr;
        return reinterpret_cast<float*>(stack.data() + address);
    }
    if (!variables) { error = "ECL float global not bound"; return nullptr; }
    auto* result = variables->float_destination(id);
    if (!result) error = "ECL float global destination not bound";
    return result;
}
bool EclContext::push(i32 value) noexcept {
    if (stack_pointer < 0 || stack_pointer > i32(stack.size()) - 8) { error = "ECL expression stack overflow"; return false; }
    stack[u32(stack_pointer)] = 'i';
    std::memcpy(stack.data() + stack_pointer + 4, &value, 4);
    stack_pointer += 8;
    return true;
}
bool EclContext::push(float value) noexcept {
    if (stack_pointer < 0 || stack_pointer > i32(stack.size()) - 8) { error = "ECL expression stack overflow"; return false; }
    stack[u32(stack_pointer)] = 'f';
    std::memcpy(stack.data() + stack_pointer + 4, &value, 4);
    stack_pointer += 8;
    return true;
}
bool EclContext::pop(i32& value) noexcept {
    u32 raw;
    u8 type;
    const i32 offset = wrapping_sub(stack_pointer, 8);
    if (!expression(offset, raw, type)) return false;
    stack_pointer = offset;
    value = type == 'f' ? truncate_int(float_from_bits(raw)) : signed_bits(raw);
    return true;
}
bool EclContext::pop(float& value) noexcept {
    u32 raw;
    u8 type;
    const i32 offset = wrapping_sub(stack_pointer, 8);
    if (!expression(offset, raw, type)) return false;
    stack_pointer = offset;
    value = type == 'i' ? float(signed_bits(raw)) : float_from_bits(raw);
    return true;
}
bool EclContext::enter_frame(i32 size) noexcept {
    const i64 end = i64(stack_pointer) + size;
    if (end < 0 || end > i64(stack.size()) - 4) { error = "ECL local frame overflow"; return false; }
    const i32 old = stack_pointer;
    stack_pointer = i32(end);
    std::memcpy(stack.data() + stack_pointer, &local_base, 4);
    stack_pointer += 4;
    local_base = old;
    return true;
}
bool EclContext::leave_frame() noexcept {
    if (stack_pointer < 4 || stack_pointer > i32(stack.size()) || local_base < 0 || local_base > i32(stack.size())) {
        error = "ECL local frame underflow";
        return false;
    }
    const i32 old = local_base;
    local_base = signed_bits(bits(stack_pointer - 4));
    stack_pointer = old;
    return true;
}
bool EclContext::bind_code(const u8* bytes, u32 size, i32 routine_id) noexcept {
    error.clear();
    if (!bytes || size < 16) { error = "ECL routine unavailable"; return false; }
    code = bytes;
    code_size = size;
    routine = routine_id;
    instruction_offset = 0;
    time = 0;
    instruction = reinterpret_cast<const EclInstruction*>(bytes);
    return true;
}
bool EclContext::select_routine(i32 id) noexcept {
    if (!program || id < 0 || u32(id) >= program->definitions.size()) { error = "ECL routine unavailable"; return false; }
    const auto& def = program->definitions[u32(id)];
    const auto& sub = program->subroutine(u32(id));
    code = def.file->bytes.data() + sub.offset + 16;
    code_size = sub.size - 16;
    routine = id;
    return true;
}
bool EclContext::bind(EclProgram& source, i32 routine_id) noexcept {
    program = &source;
    if (!select_routine(routine_id)) return false;
    return bind_code(code, code_size, routine_id);
}
bool EclContext::call_into(EclContext& target, u32 skipped_arguments) {
    if (!instruction || instruction->length < 24 || !program) { error = "ECL call program unavailable"; return false; }
    const u32 name_size = instruction->argument<u32>(0);
    const auto* bytes = reinterpret_cast<const u8*>(instruction);
    if (name_size == 0 || name_size % 4 || name_size > instruction->length - 20 ||
        !std::memchr(bytes + 20, 0, name_size)) { error = "Invalid ECL call name"; return false; }
    const i32 next = program->find_index(reinterpret_cast<const char*>(bytes + 20));
    if (next < 0) { error = "ECL called routine unavailable"; return false; }
    const i32 saved_pointer = target.stack_pointer;
    if (saved_pointer < 0 || saved_pointer > i32(target.stack.size()) - 20) { error = "ECL call frame overflow"; return false; }
    const i32 arguments_offset = saved_pointer + (saved_pointer ? 16 : 20);
    if (!saved_pointer) {
        const i32 zero = 0;
        std::memcpy(target.stack.data(), &zero, 4);
        target.stack_pointer = 4;
    }
    const i32 arguments = i32(instruction->parameter_count) - i32(skipped_arguments) - 1;
    if (arguments < 0 || i64(arguments_offset) + i64(arguments) * 4 > i64(target.stack.size()) ||
        20u + name_size + skipped_arguments * 4u + u32(arguments) * 8u > instruction->length) {
        error = "Invalid ECL call arguments";
        return false;
    }
    for (i32 n = 0; n < arguments; n++) {
        const u32 offset = 20 + name_size + skipped_arguments * 4 + u32(n) * 8;
        const u32 reference = u32(n) + skipped_arguments + 1;
        u32 raw;
        std::memcpy(&raw, bytes + offset + 4, 4);
        if (bytes[offset] == 'f' || bytes[offset] == 'g') {
            float value;
            std::memcpy(&value, &raw, 4);
            if (!floating(reference, value, value, true)) return false;
            if (bytes[offset + 1] == 'f') std::memcpy(target.stack.data() + arguments_offset + n * 4, &value, 4);
            else {
                const i32 converted = truncate_int(value);
                std::memcpy(target.stack.data() + arguments_offset + n * 4, &converted, 4);
            }
        } else {
            i32 value;
            if (!integer(reference, signed_bits(raw), value, true)) return false;
            if (bytes[offset + 1] == 'f') {
                const float converted = float(value);
                std::memcpy(target.stack.data() + arguments_offset + n * 4, &converted, 4);
            } else std::memcpy(target.stack.data() + arguments_offset + n * 4, &value, 4);
        }
    }
    const i32 resume_pointer = target.stack_pointer;
    if (saved_pointer) {
        if (resume_pointer < 4 || resume_pointer > i32(target.stack.size())) { error = "ECL call stack underflow"; return false; }
        const u32 previous_base = target.bits(resume_pointer - 4);
        std::memcpy(target.stack.data() + saved_pointer - 4, &previous_base, 4);
        target.stack_pointer = saved_pointer;
    } else target.stack_pointer = 4;
    auto save = [&](u32 value) {
        std::memcpy(target.stack.data() + target.stack_pointer, &value, 4);
        target.stack_pointer += 4;
    };
    save(u32(resume_pointer));
    if (saved_pointer) {
        u32 saved_time;
        std::memcpy(&saved_time, &time, 4);
        save(saved_time);
        save(u32(instruction_offset));
        save(u32(routine));
    } else {
        save(0xffffffffu);
        save(0xffffffffu);
        save(0xffffffffu);
    }
    target.program = program;
    if (!target.select_routine(next)) { error = target.error; return false; }
    target.instruction_offset = 0;
    target.time = 0;
    target.instruction = reinterpret_cast<const EclInstruction*>(target.code);
    return true;
}
int EclContext::return_from_call() noexcept {
    if (!leave_frame()) return -2;
    if (!stack_pointer) { routine = instruction_offset = -1; instruction = nullptr; return -1; }
    if (stack_pointer < 16) { error = "ECL return frame underflow"; return -2; }
    auto restore = [&]() {
        stack_pointer -= 4;
        return bits(stack_pointer);
    };
    const i32 resumed_routine = signed_bits(restore());
    instruction_offset = signed_bits(restore());
    const u32 restored_time = restore();
    std::memcpy(&time, &restored_time, 4);
    stack_pointer = signed_bits(restore());
    if (instruction_offset < 0 || resumed_routine < 0) {
        routine = instruction_offset = -1;
        instruction = nullptr;
        return -1;
    }
    if (!select_routine(resumed_routine)) return -2;
    if (code_size < 16 || u32(instruction_offset) > code_size - 16) {
        error = "ECL return instruction outside routine";
        return -2;
    }
    instruction = reinterpret_cast<const EclInstruction*>(code + instruction_offset);
    return 0;
}

int EclContext::step(float elapsed) {
    if (!code || routine < 0 || instruction_offset < 0) return -1;
    auto fail = [&](const char* message) { error = message; return -2; };
    auto finish = [&]() {
        const auto* saved = instruction;
        for (u32 slot = 0; slot < interpolators.size(); slot++) {
            if (interpolators[slot].end_time) {
                instruction = interpolation_targets[slot];
                if (!instruction) return fail("ECL interpolation target unavailable");
                auto* destination = float_destination(1);
                if (!destination) return -2;
                *destination = step_interp_float(interpolators[slot], rate());
            }
        }
        instruction = saved;
        return 0;
    };
    for (u32 steps = 0; steps < 100000; steps++) {
        const u32 offset = u32(instruction_offset);
        if (offset + 16 > code_size) return fail("ECL instruction outside routine");
        instruction = reinterpret_cast<const EclInstruction*>(code + offset);
        if (float(instruction->time) > time) {
            time = float(time + elapsed);
            return finish();
        }
        u32 length = instruction->length;
        if (length < 16 || length % 4 || offset + length > code_size) return fail("Invalid ECL instruction length");
        const Op op = Op(instruction->opcode);
        const bool enabled = (instruction->difficulty & difficulty) != 0;
        bool ok = true, adjust = false;
        auto integer_arg = [&](u32 n, bool consume = false) {
            i32 value = 0;
            if (16 + n * 4 + 4 > length) { error = "Truncated ECL arguments"; ok = false; }
            else if (!integer(n, instruction->argument<i32>(n), value, consume)) ok = false;
            return value;
        };
        auto float_arg = [&](u32 n, bool consume = false) {
            float value = 0;
            if (16 + n * 4 + 4 > length) { error = "Truncated ECL arguments"; ok = false; }
            else if (!floating(n, instruction->argument<float>(n), value, consume)) ok = false;
            return value;
        };
        auto pop_int = [&]() { i32 value = 0; if (!pop(value)) ok = false; return value; };
        auto pop_float = [&]() { float value = 0; if (!pop(value)) ok = false; return value; };
        auto push_int = [&](i32 value) { if (!push(value)) ok = false; };
        auto push_float = [&](float value) { if (!push(value)) ok = false; };
        auto store_int = [&](u32 n, i32 value) {
            if (16 + n * 4 + 4 > length) { error = "Truncated ECL destination"; ok = false; return; }
            auto* destination = integer_destination(n);
            if (!destination) ok = false;
            else *destination = value;
        };
        auto store_float = [&](u32 n, float value) {
            if (16 + n * 4 + 4 > length) { error = "Truncated ECL destination"; ok = false; return; }
            auto* destination = float_destination(n);
            if (!destination) ok = false;
            else *destination = value;
        };
        if (enabled) switch (op) {
            case Op::Nop: case Op::DebugBreak: case Op::Printf: case Op::PrintStack: adjust = true; break;
            case Op::Delete: routine = instruction_offset = -1; instruction = nullptr; return -1;
            case Op::Return: {
                const int result = return_from_call();
                if (result) return result;
                length = instruction->length;
                adjust = true;
                break;
            }
            case Op::Call:
                const_cast<EclInstruction*>(instruction)->stack_adjustment = 0;
                if (!call_into(*this)) return -2;
                continue;
            case Op::Spawn: case Op::SpawnIdentified: {
                if (!threads) return fail("ECL thread host not bound");
                i32 id = -1;
                if (op == Op::SpawnIdentified) {
                    const u32 name_off = instruction->argument<u32>(0) + 20;
                    if (name_off + 4 > length) return fail("Truncated ECL thread identifier");
                    i32 literal;
                    std::memcpy(&literal, reinterpret_cast<const u8*>(instruction) + name_off, 4);
                    if (!integer(1, literal, id, true)) return -2;
                }
                if (!threads->spawn(*this, id, op == Op::SpawnIdentified ? 1u : 0u)) return -2;
                break;
            }
            case Op::TerminateThread: case Op::PauseThread: case Op::ResumeThread: case Op::SetThreadState: {
                if (!threads) return fail("ECL thread host not bound");
                const i32 id = integer_arg(0);
                if (auto* target = threads->find(id)) {
                    if (op == Op::TerminateThread) target->instruction_offset = -1;
                    else if (op == Op::PauseThread) target->thread_flags |= 1;
                    else if (op == Op::ResumeThread) target->thread_flags &= ~1u;
                    else target->thread_state = integer_arg(1);
                }
                adjust = true;
                break;
            }
            case Op::TerminateChildren:
                if (!threads) return fail("ECL thread host not bound");
                threads->terminate_children();
                break;
            case Op::Jump: case Op::JumpIfZero: case Op::JumpIfNonzero: {
                const bool take = op == Op::Jump ? true : (pop_int() == 0) == (op == Op::JumpIfZero);
                if (take) {
                    if (length < 24) return fail("Truncated ECL jump");
                    const i32 relative = instruction->argument<i32>(0), target_time = instruction->argument<i32>(1);
                    if (!ok) return -2;
                    instruction_offset = wrapping_add(instruction_offset, relative);
                    time = float(target_time);
                    continue;
                }
                adjust = true;
                break;
            }
            case Op::WaitInteger: time = float(time - float(integer_arg(0))); adjust = true; break;
            case Op::WaitFloat: time = float(time - float_arg(0)); adjust = true; break;
            case Op::EnterFrame: if (!enter_frame(integer_arg(0))) ok = false; adjust = true; break;
            case Op::LeaveFrame: if (!leave_frame()) ok = false; adjust = true; break;
            case Op::PushInt: push_int(integer_arg(0, true)); break;
            case Op::PushFloat: push_float(float_arg(0, true)); break;
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
            case Op::AddInt: case Op::SubtractInt: case Op::MultiplyInt: case Op::DivideInt: case Op::Remainder:
            case Op::EqualInt: case Op::NotEqualInt: case Op::LessInt: case Op::LessEqualInt:
            case Op::GreaterInt: case Op::GreaterEqualInt:
            case Op::LogicalOr: case Op::LogicalAnd: case Op::Xor: case Op::Or: case Op::And: {
                const i32 right = pop_int(), left = pop_int();
                i32 value = 0;
                switch (op) {
                    case Op::AddInt: value = wrapping_add(left, right); break;
                    case Op::SubtractInt: value = wrapping_sub(left, right); break;
                    case Op::MultiplyInt: value = signed_bits(u32(left) * u32(right)); break;
                    case Op::DivideInt: case Op::Remainder:
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
                if (u16(op) <= 58) const_cast<EclInstruction*>(instruction)->stack_adjustment = 0;
                break;
            }
            case Op::AddFloat: case Op::SubtractFloat: case Op::MultiplyFloat: case Op::DivideFloat: {
                const float right = pop_float(), left = pop_float();
                push_float(op == Op::AddFloat ? float(left + right)
                         : op == Op::SubtractFloat ? float(left - right)
                         : op == Op::MultiplyFloat ? float(left * right)
                         : float(left / right));
                const_cast<EclInstruction*>(instruction)->stack_adjustment = 0;
                break;
            }
            case Op::EqualFloat: case Op::NotEqualFloat: case Op::LessFloat: case Op::LessEqualFloat:
            case Op::GreaterFloat: case Op::GreaterEqualFloat: {
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
            case Op::NotFloat: push_int(pop_float() == 0.0f); break;
            case Op::NegateInt: push_int(signed_bits(0u - u32(pop_int()))); break;
            case Op::NegateFloat: push_float(float(-pop_float())); break;
            case Op::PostDecrement: {
                const i32 value = integer_arg(0);
                store_int(0, wrapping_add(value, -1));
                push_int(value);
                break;
            }
            case Op::Sin: case Op::Cos: case Op::Sqrt: {
                const double value = double(pop_float());
                push_float(float(op == Op::Sin ? std::sin(value) : op == Op::Cos ? std::cos(value) : std::sqrt(value)));
                break;
            }
            case Op::Polar: {
                const float angle = wrap_angle(float_arg(2)), radius = float_arg(3);
                store_float(0, float(std::cos(double(angle)) * double(radius)));
                store_float(1, float(std::sin(double(angle)) * double(radius)));
                adjust = true;
                break;
            }
            case Op::NormalizeAngle: store_float(0, wrap_angle(float_arg(0))); adjust = true; break;
            case Op::LengthSquared: case Op::Length: {
                const float x = float_arg(1), y = float_arg(2), squared = float(float(y * y) + float(x * x));
                store_float(0, op == Op::Length ? float(std::sqrt(double(squared))) : squared);
                adjust = true;
                break;
            }
            case Op::AngleTo: {
                const float ax = float_arg(1), ay = float_arg(2), bx = float_arg(3), by = float_arg(4);
                store_float(0, float(std::atan2(double(float(by - ay)), double(float(bx - ax)))));
                adjust = true;
                break;
            }
            case Op::AngleDifference: {
                const float from = float_arg(1), to = float_arg(2);
                float difference = float(to - from);
                if (difference > 3.1415927410125732f) difference = float(to - float(from + 6.2831854820251465f));
                else if (float(from - to) > 3.1415927410125732f) difference = float(to - float(from - 6.2831854820251465f));
                store_float(0, difference);
                adjust = true;
                break;
            }
            case Op::RotateVector: {
                const float x = float_arg(2), y = float_arg(3), angle = wrap_angle(float_arg(4));
                const float sine = float(std::sin(double(angle))), cosine = float(std::cos(double(angle)));
                store_float(0, float(float(cosine * x) - float(sine * y)));
                store_float(1, float(float(cosine * y) + float(sine * x)));
                adjust = true;
                break;
            }
            case Op::Interpolate: case Op::InterpolateHermite: {
                const i32 slot = integer_arg(0);
                if (slot < 0 || u32(slot) >= interpolators.size()) return fail("ECL interpolation slot outside context");
                auto& interpolation = interpolators[u32(slot)];
                interpolation_targets[u32(slot)] = instruction;
                step_interp_float(interpolation, rate());
                const i32 frames = integer_arg(2), mode = integer_arg(3);
                const float from = float_arg(4), to = float_arg(5);
                interpolation.initial = from;
                interpolation.goal = to;
                interpolation.bezier1 = op == Op::InterpolateHermite ? float_arg(6) : 0;
                interpolation.bezier2 = op == Op::InterpolateHermite ? float_arg(7) : 0;
                interpolation.mode = mode;
                interpolation.end_time = frames;
                interpolation.timer.reset();
                store_float(1, from);
                adjust = true;
                break;
            }
            case Op::RandomPolar: {
                if (!game_rng) return fail("ECL game random generator not bound");
                const float minimum = float_arg(2), maximum = float_arg(3);
                const float angle = float(game_rng->signed_unit() * 3.1415927410125732f);
                const float radius = float(float(game_rng->signed_unit() * float(maximum - minimum)) + minimum);
                store_float(0, float(std::cos(double(angle)) * double(radius)));
                store_float(1, float(std::sin(double(angle)) * double(radius)));
                adjust = true;
                break;
            }
            default:
                if (commands) {
                    const int result = commands->execute(*this, u16(op));
                    if (result == -2) return result;
                    if (result == -1) return finish();
                    if (result == 1) continue;
                    adjust = true;
                } else {
                    error = "Unimplemented ECL opcode " + std::to_string(u16(op));
                    return -2;
                }
                break;
        }
        if (!ok) return -2;
        if (enabled && adjust && instruction->stack_adjustment) {
            if (stack_pointer < instruction->stack_adjustment) return fail("ECL instruction stack cleanup underflow");
            stack_pointer -= instruction->stack_adjustment;
        }
        instruction_offset = wrapping_add(instruction_offset, i32(length));
    }
    return fail("ECL instruction budget exceeded");
}

}  // namespace th16
