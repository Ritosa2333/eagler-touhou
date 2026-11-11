// th13 ECL 线程快照实现（块布局见 EclFile.hpp 文件头）。
#include "EclFile.hpp"

#include <cmath>

namespace th13 {
namespace {
template <class T>
void put(u8* p, u32 at, const T& v) {
    std::memcpy(p + at, &v, sizeof v);
}
template <class T>
void get(const u8* p, u32 at, T& v) {
    std::memcpy(&v, p + at, sizeof v);
}
u32 word(const u8* p, u32 at) {
    u32 v;
    get(p, at, v);
    return v;
}

// 游标合法性：偏移必须落在指令边界上（逐指令推进复核）。
const EclInstruction* locate(EclProgram& p, i32 routine, i32 offset) {
    if (routine < 0 || u32(routine) >= p.definitions.size() || offset < 0) return nullptr;
    const auto& d = p.definitions[routine];
    const auto& s = p.subroutine(routine);
    if (s.size < 32 || u32(offset) > s.size - 32 || offset % 4) return nullptr;
    const auto* code = d.file->bytes.data() + s.offset + 16;
    u32 cursor = 0;
    while (cursor < u32(offset)) {
        const auto* i = reinterpret_cast<const EclInstruction*>(code + cursor);
        if (i->length < 16 || i->length % 4 || i->length > s.size - 16 - cursor) return nullptr;
        cursor += i->length;
    }
    return cursor == u32(offset) ? reinterpret_cast<const EclInstruction*>(code + offset) : nullptr;
}

// 指令指针 → {routine, offset}（快照侧代理；空指针 → {0,0}）。
bool address(const EclContext& c, const EclInstruction* target, i32& routine, i32& offset) {
    if (!target) {
        routine = offset = 0;
        return true;
    }
    if (!c.program) return false;
    const auto ptr = reinterpret_cast<std::uintptr_t>(target);
    for (u32 i = 0; i < c.program->definitions.size(); i++) {
        const auto& d = c.program->definitions[i];
        const auto& sub = c.program->subroutine(i);
        const auto base = reinterpret_cast<std::uintptr_t>(d.file->bytes.data() + sub.offset + 16);
        if (sub.size >= 32 && ptr >= base && ptr - base <= sub.size - 32) {
            routine = i;
            offset = i32(ptr - base);
            return locate(*c.program, routine, offset) == target;
        }
    }
    return false;
}

// 通道目标打包：u32 = routine<<18 | (offset/2)。偏移 4 对齐且 < 0x80000、
// routine < 16384 时无损；越界视为快照损坏。
bool pack_target(i32 routine, i32 offset, u32& out) {
    if (routine < 0 || routine >= 1 << 14 || offset < 0 || offset >= 1 << 19 || offset % 4) return false;
    out = (u32(routine) << 18) | (u32(offset) >> 1);
    return true;
}
bool unpack_target(u32 raw, i32& routine, i32& offset) {
    routine = i32(raw >> 18);
    offset = i32((raw & 0x3ffff) << 1);
    return true;
}
}  // namespace

bool EclFile::write(const EclContext& c, Block& out) {
    error.clear();
    out.fill(0);
    auto* p = out.data();
    put(p, 0x000, c.time);
    put(p, 0x004, c.routine);
    put(p, 0x008, c.stack);
    put(p, 0x1008, c.stack_pointer);
    put(p, 0x100c, c.local_base);
    put(p, 0x1010, c.thread_id);
    put(p, 0x1018, c.thread_state);
    put(p, 0x101c, c.difficulty);
    put(p, 0x1020, c.interpolators);
    put(p, 0x11a0, c.instruction_offset);
    put(p, 0x11c0, c.thread_flags);
    for (u32 i = 0; i < 8; i++) {
        i32 routine, offset;
        if (!address(c, c.interpolation_targets[i], routine, offset)) {
            error = "ECL checkpoint interpolation destination is unavailable";
            return false;
        }
        u32 packed;
        if (!pack_target(routine, offset, packed)) {
            error = "ECL checkpoint interpolation destination is out of range";
            return false;
        }
        put(p, 0x11a4 + i * 4, packed);
    }
    return true;
}

bool EclFile::read(const u8* data, u32 size, EclContext& out, EclProgram& program) {
    error.clear();
    if (!data || size < 0x11c4) {
        error = "Truncated ECL checkpoint context";
        return false;
    }
    const i32 routine = signed_bits(word(data, 0x004)), offset = signed_bits(word(data, 0x11a0));
    EclContext c;
    c.program = &program;
    if (routine >= 0) {
        if (u32(routine) >= program.definitions.size() || !c.bind(program, routine)) {
            error = "ECL checkpoint routine is unavailable";
            return false;
        }
        if (offset >= 0) {
            c.instruction = locate(program, routine, offset);
            if (!c.instruction) {
                error = "ECL checkpoint cursor is not an instruction boundary";
                return false;
            }
        } else
            c.instruction = nullptr;
    } else {
        c.routine = routine;
        c.instruction = nullptr;
    }
    c.instruction_offset = offset;
    get(data, 0x000, c.time);
    get(data, 0x008, c.stack);
    get(data, 0x1008, c.stack_pointer);
    get(data, 0x100c, c.local_base);
    get(data, 0x1010, c.thread_id);
    get(data, 0x1018, c.thread_state);
    get(data, 0x101c, c.difficulty);
    get(data, 0x1020, c.interpolators);
    get(data, 0x11c0, c.thread_flags);
    if (!std::isfinite(c.time) || c.stack_pointer < 0 || c.stack_pointer > 4096 || c.stack_pointer % 4 ||
        c.local_base < 0 || c.local_base > 4096 || c.local_base % 4) {
        error = "Invalid ECL checkpoint context stack/time";
        return false;
    }
    for (u32 i = 0; i < 8; i++) {
        i32 target_routine, target_offset;
        unpack_target(word(data, 0x11a4 + i * 4), target_routine, target_offset);
        c.interpolation_targets[i] = locate(program, target_routine, target_offset);
        if (c.interpolators[i].duration && !c.interpolation_targets[i]) {
            error = "ECL checkpoint interpolation instruction is unavailable";
            return false;
        }
    }
    out = std::move(c);
    return true;
}

bool EclFile::write_threads(const EclThreadsSnapshot& threads, std::vector<u8>& out, u32 owner_offset, u32 child_offset) {
    error.clear();
    constexpr u32 kStride = 0x11d4;  // 0x11C4 块 + 16 字节链表节点
    if (owner_offset > out.size() || out.size() - owner_offset < 0x11dc || child_offset > out.size() ||
        child_offset < owner_offset + 0x11dc || threads.children.size() > 65536) {
        error = "ECL checkpoint owner/child region is unavailable";
        return false;
    }
    Block main_block;
    if (!write(threads.main, main_block)) return false;
    std::memcpy(out.data() + owner_offset + 16, main_block.data(), main_block.size());
    // 子线程存在标志放在主线程块尾节点 +8（node dword 2）。
    put(out.data(), owner_offset + 0x11d8, threads.children.empty() ? u32(0) : u32(1));
    for (u32 i = 0; i < threads.children.size(); i++) {
        Block child;
        if (!write(threads.children[i], child)) return false;
        const u32 at = child_offset + i * kStride;
        if (at > out.size()) {
            error = "ECL checkpoint child region has a gap";
            return false;
        }
        out.resize(at + kStride, 0);
        std::memcpy(out.data() + at, child.data(), child.size());
        // 链继续标志放在块尾 +4（节点首 dword 位置）。
        put(out.data(), at + 0x11c4, i + 1 < threads.children.size() ? u32(1) : u32(0));
    }
    return true;
}

bool EclFile::read_threads(const u8* data, u32 size, EclThreadsSnapshot& out, EclProgram& p, u32 owner_offset,
                           u32 child_offset, u32& consumed) {
    error.clear();
    consumed = 0;
    if (!data || size < 0x11d4 || owner_offset > size || size - owner_offset < 0x11dc || child_offset > size ||
        child_offset < owner_offset + 0x11dc) {
        error = "Truncated ECL checkpoint owner";
        return false;
    }
    const u8* owner = data + owner_offset;
    EclThreadsSnapshot next;
    if (!read(owner + 16, size - owner_offset - 16, next.main, p)) return false;
    u32 at = child_offset;
    if (word(owner, 0x11d8)) {  // 主线程块尾节点 +8 的子线程存在标志
        bool more = true;
        while (more) {
            if (next.children.size() >= 65536 || size - at < 0x11d4) {
                error = "Truncated ECL checkpoint child list";
                return false;
            }
            more = word(data + at, 0x11c4) != 0;
            next.children.emplace_back();
            if (!read(data + at, 0x11c4, next.children.back(), p)) return false;
            at += 0x11d4;
        }
    }
    out = std::move(next);
    consumed = at - child_offset;
    return true;
}

}  // namespace th13
