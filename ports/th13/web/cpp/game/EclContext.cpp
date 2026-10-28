// th13 ECL 取参/栈机/调用帧（锚点：0x478e60 整型取参、0x478ee0 浮点取参、
// 0x479a20 stackAlloc、0x479a60 stackFree、0x477030 调用帧压入——均经容器内
// capstone 直读 th13.exe 反汇编逐条核对，浮点判定常量 -100.0/-1.0/8.0
// 位于 .rdata 0x4ae900/0x4ae908/0x4ae910）。
#include "EclContext.hpp"

namespace th13 {
namespace {
float as_float(u32 bits) {
    float value;
    std::memcpy(&value, &bits, 4);
    return value;
}
}  // namespace

bool EclContext::referenced(u32 argument) const noexcept {
    return instruction && (instruction->parameter_mask & (1u << (argument & 31)));
}

u32 EclContext::bits(i32 offset) const noexcept {
    u32 value;
    std::memcpy(&value, stack.data() + offset, 4);
    return value;
}

bool EclContext::local_offset(i32 offset, i32& result) noexcept {
    const i64 address = i64(local_base) + offset;
    if (address < 0 || address > i64(stack.size()) - 4) {
        error = "ECL local variable outside stack";
        return false;
    }
    result = i32(address);
    return true;
}

bool EclContext::expression(i32 offset, u32& value, u8& type) noexcept {
    if (offset < 0 || offset > i32(stack.size()) - 8) {
        error = "ECL expression stack underflow";
        return false;
    }
    type = stack[offset];
    value = bits(offset + 4);
    return true;
}

bool EclContext::integer(u32 argument, i32 literal, i32& value, bool pop) noexcept {
    if (!referenced(argument)) {
        value = literal;
        return true;
    }
    // 0x478e80：raw >= 0 → 局部变量（地址 = 栈基 + FP + raw）。
    if (literal >= 0) {
        i32 address;
        if (!local_offset(literal, address)) return false;
        value = signed_bits(bits(address));
        return true;
    }
    // 0x478e9b：raw >= -100 → 表达式栈（SP + raw*8 + 4 为值、-4 为类型；'f' 经 0x4971f0 向零截断）。
    if (literal >= -100) {
        u32 raw;
        u8 type;
        const i32 offset = pop ? wrapping_sub(stack_pointer, 8) : wrapping_add(stack_pointer, literal * 8);
        if (!expression(offset, raw, type)) return false;
        if (pop) stack_pointer = offset;
        value = type == 'f' ? truncate_int(as_float(raw)) : signed_bits(raw);
        return true;
    }
    // 0x478ecb：raw < -100 → 宿主全局码（虚表 +4 → 0x420380）。
    if (!variables || !variables->integer(literal, value)) {
        error = "ECL integer global not bound";
        return false;
    }
    return true;
}

bool EclContext::floating(u32 argument, float literal, float& value, bool pop) noexcept {
    if (!referenced(argument)) {
        value = literal;
        return true;
    }
    // 0x478efe：param > 0（含 ==0 落局部[0]）→ 局部变量 float 视图。
    if (!(literal < 0.f)) {
        i32 address;
        if (!local_offset(truncate_int(literal), address)) return false;
        value = as_float(bits(address));
        return true;
    }
    // 0x478f24：param ∈ [-100.0, -1.0] → 表达式栈（偏移 = SP + truncate(param×8.0)，
    // -1.0 为栈顶；'i' 条目经 fild 转浮点）。
    if (literal >= -100.f && literal <= -1.f) {
        u32 raw;
        u8 type;
        const i32 scaled = truncate_int(static_cast<float>(literal * 8.f));
        const i32 offset = pop ? wrapping_sub(stack_pointer, 8) : wrapping_add(stack_pointer, scaled);
        if (!expression(offset, raw, type)) return false;
        if (pop) stack_pointer = offset;
        value = type == 'i' ? static_cast<float>(signed_bits(raw)) : as_float(raw);
        return true;
    }
    // 0x478f7f：其余（<-100，如 -9989.0f）→ 全局码（虚表 +0xC → 0x420d00）。
    if (!variables || !variables->floating(truncate_int(literal), value)) {
        error = "ECL float global not bound";
        return false;
    }
    return true;
}

i32* EclContext::integer_destination(u32 argument) noexcept {
    if (!referenced(argument)) {
        error = "ECL integer destination is not a reference";
        return nullptr;
    }
    const i32 id = instruction->argument<i32>(argument);
    if (id >= 0) {
        i32 address;
        if (!local_offset(id, address)) return nullptr;
        return reinterpret_cast<i32*>(stack.data() + address);
    }
    if (!variables) {
        error = "ECL integer global not bound";
        return nullptr;
    }
    auto* result = variables->integer_destination(id);
    if (!result) error = "ECL integer global destination not bound";
    return result;
}

float* EclContext::float_destination(u32 argument) noexcept {
    if (!referenced(argument)) {
        error = "ECL float destination is not a reference";
        return nullptr;
    }
    const float literal = instruction->argument<float>(argument);
    const i32 id = truncate_int(literal);
    if (!(literal < 0.f)) {
        i32 address;
        if (!local_offset(id, address)) return nullptr;
        return reinterpret_cast<float*>(stack.data() + address);
    }
    if (!variables) {
        error = "ECL float global not bound";
        return nullptr;
    }
    auto* result = variables->float_destination(id);
    if (!result) error = "ECL float global destination not bound";
    return result;
}

bool EclContext::push(i32 value) noexcept {
    if (stack_pointer < 0 || stack_pointer > i32(stack.size()) - 8) {
        error = "ECL expression stack overflow";
        return false;
    }
    stack[stack_pointer] = 'i';
    std::memcpy(stack.data() + stack_pointer + 4, &value, 4);
    stack_pointer += 8;
    return true;
}

bool EclContext::push(float value) noexcept {
    if (stack_pointer < 0 || stack_pointer > i32(stack.size()) - 8) {
        error = "ECL expression stack overflow";
        return false;
    }
    stack[stack_pointer] = 'f';
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
    value = type == 'f' ? truncate_int(as_float(raw)) : signed_bits(raw);
    return true;
}

bool EclContext::pop(float& value) noexcept {
    u32 raw;
    u8 type;
    const i32 offset = wrapping_sub(stack_pointer, 8);
    if (!expression(offset, raw, type)) return false;
    stack_pointer = offset;
    value = type == 'i' ? static_cast<float>(signed_bits(raw)) : as_float(raw);
    return true;
}

bool EclContext::enter_frame(i32 size) noexcept {
    // 0x479a20：SP+size 溢出 0x1000 拒绝；压旧 FP，FP=旧 SP。
    const i64 end = i64(stack_pointer) + size;
    if (end < 0 || end > i64(stack.size()) - 4) {
        error = "ECL local frame overflow";
        return false;
    }
    const i32 old = stack_pointer;
    stack_pointer = i32(end);
    std::memcpy(stack.data() + stack_pointer, &local_base, 4);
    stack_pointer += 4;
    local_base = old;
    return true;
}

bool EclContext::leave_frame() noexcept {
    // 0x479a60：SP-=4 取回保存的 FP，SP 恢复为 FP。
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
    if (!bytes || size < 16) {
        error = "ECL routine unavailable";
        return false;
    }
    code = bytes;
    code_size = size;
    routine = routine_id;
    instruction_offset = 0;
    time = 0;
    instruction = reinterpret_cast<const EclInstruction*>(bytes);
    return true;
}

bool EclContext::select_routine(i32 id) noexcept {
    if (!program || id < 0 || u32(id) >= program->definitions.size()) {
        error = "ECL routine unavailable";
        return false;
    }
    const auto& definition = program->definitions[id];
    const auto& sub = program->subroutine(id);
    code = definition.file->bytes.data() + sub.offset + 16;
    code_size = sub.size - 16;
    routine = id;
    return true;
}

bool EclContext::bind(EclProgram& source, i32 routine_id) noexcept {
    program = &source;
    if (!select_routine(routine_id)) return false;
    return bind_code(code, code_size, routine_id);
}

// 同步调用（0x477030）：被调者（target）为新线程或本线程切换；调用实参按名字后的
// 8 字节描述符 {char 源类型; char 目标类型; u32 原始值} 逐个解析写入被调者栈帧，
// 栈底首帧在栈头保留 4 字零字（saved_pointer==0 支路）。
bool EclContext::call_into(EclContext& target, u32 skipped_arguments) {
    if (!instruction || instruction->length < 24 || !program) {
        error = "ECL call program unavailable";
        return false;
    }
    const u32 name_size = instruction->argument<u32>(0);
    const auto* bytes = reinterpret_cast<const u8*>(instruction);
    if (name_size == 0 || name_size % 4 || name_size > u32(instruction->length - 20) || !std::memchr(bytes + 20, 0, name_size)) {
        error = "Invalid ECL call name";
        return false;
    }
    const i32 next = program->find_index(reinterpret_cast<const char*>(bytes + 20));
    if (next < 0) {
        error = "ECL called routine unavailable";
        return false;
    }
    const i32 saved_pointer = target.stack_pointer;
    if (saved_pointer < 0 || saved_pointer > i32(target.stack.size()) - 20) {
        error = "ECL call frame overflow";
        return false;
    }
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
        const u32 offset = 20 + name_size + skipped_arguments * 4 + u32(n) * 8, reference = u32(n) + skipped_arguments + 1;
        u32 raw;
        std::memcpy(&raw, bytes + offset + 4, 4);
        if (bytes[offset] == 'f' || bytes[offset] == 'g') {
            float value;
            std::memcpy(&value, &raw, 4);
            if (!floating(reference, value, value, true)) return false;
            if (bytes[offset + 1] == 'f')
                std::memcpy(target.stack.data() + arguments_offset + n * 4, &value, 4);
            else {
                const i32 converted = truncate_int(value);
                std::memcpy(target.stack.data() + arguments_offset + n * 4, &converted, 4);
            }
        } else {
            i32 value;
            if (!integer(reference, signed_bits(raw), value, true)) return false;
            if (bytes[offset + 1] == 'f') {
                const float converted = static_cast<float>(value);
                std::memcpy(target.stack.data() + arguments_offset + n * 4, &converted, 4);
            } else
                std::memcpy(target.stack.data() + arguments_offset + n * 4, &value, 4);
        }
    }
    const i32 resume_pointer = target.stack_pointer;
    if (saved_pointer) {
        if (resume_pointer < 4 || resume_pointer > i32(target.stack.size())) {
            error = "ECL call stack underflow";
            return false;
        }
        const u32 previous_base = target.bits(resume_pointer - 4);
        std::memcpy(target.stack.data() + saved_pointer - 4, &previous_base, 4);
        target.stack_pointer = saved_pointer;
    } else
        target.stack_pointer = 4;
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
        save(UINT32_MAX);
        save(UINT32_MAX);
        save(UINT32_MAX);
    }
    target.program = program;
    if (!target.select_routine(next)) {
        error = target.error;
        return false;
    }
    target.instruction_offset = 0;
    target.time = 0;
    target.instruction = reinterpret_cast<const EclInstruction*>(target.code);
    return true;
}

// 返回（0x4772a0 case 10）：弹帧恢复 {SP,time,Q,routine}；栈底帧返回 = 线程结束。
int EclContext::return_from_call() noexcept {
    //4772ff calls479a60 even for a root with SP=FP=0. It restores SP
    // from FP, then477307 sees zero and terminates. No caller frame exists.
    if (stack_pointer==0 && local_base==0) {
        routine=instruction_offset=-1;instruction=nullptr;return -1;
    }
    if (!leave_frame()) return -2;
    if (!stack_pointer) {
        routine = instruction_offset = -1;
        instruction = nullptr;
        return -1;
    }
    if (stack_pointer < 16) {
        error = "ECL return frame underflow";
        return -2;
    }
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

}  // namespace th13
