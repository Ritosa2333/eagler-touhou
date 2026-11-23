#include "Std.hpp"

#include <cstring>

namespace th16 {

namespace {

template <typename T>
T read_at(const u8* p) {
    T v;
    std::memcpy(&v, p, sizeof(T));
    return v;
}

}  // namespace

i32 StdInstruction::arg_i(int n) const { return read_at<i32>(args + n * 4); }
float StdInstruction::arg_f(int n) const { return read_at<float>(args + n * 4); }

bool StdFile::parse(const u8* bytes, u32 size) {
    error.clear();
    objects.clear();
    instances.clear();
    script.clear();
    if (size < 0x90) { error = "std: header truncated"; return false; }
    bytes_.assign(bytes, bytes + size);
    const u8* base = bytes_.data();

    const i16 object_count = read_at<i16>(base);
    face_vm_count = read_at<i16>(base + 2);
    const u32 instance_offset = read_at<u32>(base + 4);
    const u32 script_offset = read_at<u32>(base + 8);
    anm_name.assign(reinterpret_cast<const char*>(base + 0x10), strnlen(reinterpret_cast<const char*>(base + 0x10), 0x80));

    if (object_count < 0 || 0x90 + u32(object_count) * 4 > size) { error = "std: object table truncated"; return false; }
    objects.resize(size_t(object_count));
    for (i32 i = 0; i < object_count; ++i) {
        const u32 off = read_at<u32>(base + 0x90 + u32(i) * 4);
        if (off + 0x1c > size) { error = "std: object truncated"; return false; }
        StdObject& obj = objects[size_t(i)];
        obj.id = read_at<u16>(base + off);
        obj.layer = base[off + 2];
        obj.flags = base[off + 3];
        obj.pos = {read_at<float>(base + off + 4), read_at<float>(base + off + 8), read_at<float>(base + off + 12)};
        obj.size = {read_at<float>(base + off + 16), read_at<float>(base + off + 20), read_at<float>(base + off + 24)};
        u32 q = off + 0x1c;
        for (;;) {
            if (q + 4 > size) { error = "std: quad list truncated"; return false; }
            const i16 type = read_at<i16>(base + q);
            const i16 qsize = read_at<i16>(base + q + 2);
            if (type < 0) break;
            if (qsize < 0x1c || q + u32(qsize) > size) { error = "std: quad truncated"; return false; }
            StdQuad quad;
            quad.type = type;
            quad.size = qsize;
            quad.anm_script = read_at<i16>(base + q + 4);
            quad.vm_index = read_at<i16>(base + q + 6);
            quad.pos = {read_at<float>(base + q + 8), read_at<float>(base + q + 12), read_at<float>(base + q + 16)};
            quad.width = read_at<float>(base + q + 20);
            quad.height = read_at<float>(base + q + 24);
            if (quad.anm_script < 0) { error = "std: quad script out of range"; return false; }
            obj.quads.push_back(quad);
            q += u32(qsize);
        }
    }

    for (u32 p = instance_offset;; p += 16) {
        if (p + 4 > size) { error = "std: instance table truncated"; return false; }
        const u16 object = read_at<u16>(base + p);
        if (object == 0xffff) break;
        if (p + 16 > size) { error = "std: instance truncated"; return false; }
        if (object >= objects.size()) { error = "std: instance references missing object"; return false; }
        StdInstance inst;
        inst.object = object;
        inst.flags = read_at<u16>(base + p + 2);
        inst.pos = {read_at<float>(base + p + 4), read_at<float>(base + p + 8), read_at<float>(base + p + 12)};
        instances.push_back(inst);
    }

    for (u32 p = script_offset; p + 8 <= size;) {
        StdInstruction ins;
        ins.time = read_at<i32>(base + p);
        ins.opcode = read_at<i16>(base + p + 4);
        ins.size = read_at<i16>(base + p + 6);
        ins.offset = p - script_offset;
        ins.args = base + p + 8;
        // 文件以 20 字节 0xff 收尾（time/opcode/size 全 -1）；脚本末尾总是 jmp 回环，原版不会执行到此
        if (ins.opcode == -1 && ins.size == -1) break;
        if (ins.size < 8 || p + u32(ins.size) > size) { error = "std: instruction truncated"; return false; }
        script.push_back(ins);
        p += u32(ins.size);
    }
    if (script.empty()) { error = "std: empty script"; return false; }
    return true;
}

i32 StdFile::instruction_at(u32 offset) const {
    for (size_t i = 0; i < script.size(); ++i)
        if (script[i].offset == offset) return i32(i);
    return -1;
}

}  // namespace th16
