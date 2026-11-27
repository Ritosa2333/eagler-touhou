#include "ReplayFile.hpp"

#include "Lzss.hpp"
#include "ResourceCrypt.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>

namespace th16 {

bool ReplayFile::open_file(const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        error = "open failed";
        return false;
    }
    std::vector<u8> bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    return open(bytes.data(), u32(bytes.size()));
}

bool ReplayFile::open(const u8* bytes, u32 size) {
    error.clear();
    user.clear();
    stages.clear();
    stage_count = 0;
    std::memset(name, 0, sizeof(name));
    if (bytes == nullptr || size < kHeaderSize) {
        error = "too short";
        return false;
    }
    if (read_u32(bytes) != kMagic) {
        error = "bad magic";
        return false;
    }
    if (read_u16(bytes + 4) != kVersion) {
        error = "bad version";
        return false;
    }
    const u32 packed = read_u32(bytes + 0x1c);
    const u32 unpacked = read_u32(bytes + 0x20);
    if (packed == 0 || unpacked == 0 || size < kHeaderSize + packed) {
        error = "bad sizes";
        return false;
    }

    std::vector<u8> pass1(packed), pass2(packed);
    const CryptParams crypt_a{0x5c, u8(i8(-0x1f)), 0x400, packed};
    const CryptParams crypt_b{0x7d, u8(':'), 0x100, packed};
    if (!resource_crypt(bytes + kHeaderSize, pass1.data(), packed, crypt_a, false) ||
        !resource_crypt(pass1.data(), pass2.data(), packed, crypt_b, false)) {
        error = "crypt failed";
        return false;
    }

    std::vector<u8> decoded(unpacked);
    u32 written = 0;
    Lzss lzss;
    if (!lzss.decode(pass2.data(), packed, decoded.data(), unpacked, written) || written != unpacked) {
        error = "lzss failed";
        return false;
    }
    if (written < kUserSize) {
        error = "userdata truncated";
        return false;
    }

    user.assign(decoded.begin(), decoded.begin() + kUserSize);
    std::memcpy(name, user.data(), 8);
    name[8] = '\0';
    stage_count = read_i32(user.data() + 0x80);
    if (stage_count < 0) stage_count = 0;
    if (stage_count > 7) stage_count = 7;  // 0x448c10 上限按 6 段枚举，但 +0x80 原样保留

    u32 offset = kUserSize;
    const i32 limit = stage_count > 7 ? 6 : stage_count;
    for (i32 i = 0; i < limit; ++i) {
        if (offset + kStageHeaderSize > written) {
            error = "stage header truncated";
            return false;
        }
        ReplayStageSegment seg;
        seg.header.assign(decoded.begin() + offset, decoded.begin() + offset + kStageHeaderSize);
        seg.stage = i16(read_u16(decoded.data() + offset));
        seg.rng_seed = read_u16(decoded.data() + offset + 2);
        seg.frames = read_i32(decoded.data() + offset + 4);
        const i32 extra = read_i32(decoded.data() + offset + 8);
        if (seg.frames < 0 || extra < 0) {
            error = "bad stage sizes";
            return false;
        }
        const u32 body = u32(extra);
        if (offset + kStageHeaderSize + body > written) {
            error = "stage body truncated";
            return false;
        }
        const u32 input_bytes = u32(seg.frames) * 6u;
        const u32 fps_bytes = body >= input_bytes ? body - input_bytes : 0;
        const u8* body_ptr = decoded.data() + offset + kStageHeaderSize;
        seg.inputs.assign(body_ptr, body_ptr + input_bytes);
        seg.fps.assign(body_ptr + input_bytes, body_ptr + input_bytes + fps_bytes);
        stages.push_back(std::move(seg));
        offset += kStageHeaderSize + body;
    }
    return true;
}

namespace {

void put_u16(u8* p, u16 v) { std::memcpy(p, &v, 2); }
void put_i32(u8* p, i32 v) { std::memcpy(p, &v, 4); }
void put_u32(u8* p, u32 v) { std::memcpy(p, &v, 4); }

void append_bytes(std::vector<u8>& out, const void* p, size_t n) {
    const auto* b = static_cast<const u8*>(p);
    out.insert(out.end(), b, b + n);
}

std::vector<u8> synthesize_stage_header(const ReplayStageSegment& seg) {
    std::vector<u8> header(ReplayFile::kStageHeaderSize, 0);
    put_u16(header.data(), u16(seg.stage));
    put_u16(header.data() + 2, seg.rng_seed);
    put_i32(header.data() + 4, seg.frames);
    const i32 extra = i32(seg.inputs.size() + seg.fps.size());
    put_i32(header.data() + 8, extra);
    return header;
}

}  // namespace

bool ReplayFile::encode(std::vector<u8>& out) {
    out.clear();
    error.clear();
    if (user.size() != kUserSize) {
        error = "bad user size";
        return false;
    }
    std::vector<u8> decoded;
    decoded.reserve(kUserSize + stages.size() * (kStageHeaderSize + 64));
    append_bytes(decoded, user.data(), kUserSize);
    for (const ReplayStageSegment& seg : stages) {
        std::vector<u8> header = seg.header.size() == kStageHeaderSize ? seg.header : synthesize_stage_header(seg);
        if (header.size() != kStageHeaderSize) header.resize(kStageHeaderSize, 0);
        put_u16(header.data(), u16(seg.stage));
        put_u16(header.data() + 2, seg.rng_seed);
        put_i32(header.data() + 4, seg.frames);
        put_i32(header.data() + 8, i32(seg.inputs.size() + seg.fps.size()));
        append_bytes(decoded, header.data(), kStageHeaderSize);
        if (!seg.inputs.empty()) append_bytes(decoded, seg.inputs.data(), seg.inputs.size());
        if (!seg.fps.empty()) append_bytes(decoded, seg.fps.data(), seg.fps.size());
    }

    Lzss lzss;
    const std::vector<u8> packed_plain = lzss.encode(decoded.data(), u32(decoded.size()));
    if (packed_plain.empty() && !decoded.empty()) {
        error = "lzss encode empty";
        return false;
    }
    const u32 packed = u32(packed_plain.size());
    std::vector<u8> pass_b(packed), pass_a(packed);
    const CryptParams crypt_b{0x7d, u8(':'), 0x100, packed};
    const CryptParams crypt_a{0x5c, u8(i8(-0x1f)), 0x400, packed};
    if (!resource_crypt(packed_plain.data(), pass_b.data(), packed, crypt_b, true) ||
        !resource_crypt(pass_b.data(), pass_a.data(), packed, crypt_a, true)) {
        error = "crypt encode failed";
        return false;
    }

    out.assign(kHeaderSize, 0);
    put_u32(out.data(), kMagic);
    put_u16(out.data() + 4, kVersion);
    put_u32(out.data() + 0xc, packed + kHeaderSize);
    put_u32(out.data() + 0x1c, packed);
    put_u32(out.data() + 0x20, u32(decoded.size()));
    append_bytes(out, pass_a.data(), packed);
    return true;
}

bool ReplayFile::write_file(const char* path) {
    std::vector<u8> bytes;
    if (!encode(bytes)) return false;
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        error = "write open failed";
        return false;
    }
    out.write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
    if (!out) {
        error = "write failed";
        return false;
    }
    return true;
}

bool replay_slot_path(const char* directory, int slot, char* out, size_t out_n) {
    if (directory == nullptr || out == nullptr || out_n == 0 || slot < 1 || slot > 100) return false;
    std::snprintf(out, out_n, "%s/th16_%02d.rpy", directory, slot);
    if (std::FILE* f = std::fopen(out, "rb")) { std::fclose(f); return true; }
    std::snprintf(out, out_n, "%s/replay/th16_%02d.rpy", directory, slot);
    if (std::FILE* f = std::fopen(out, "rb")) { std::fclose(f); return true; }
    // 返回首选路径（即使尚不存在），供 save 写入
    std::snprintf(out, out_n, "%s/th16_%02d.rpy", directory, slot);
    return false;
}

bool replay_slot_exists(const char* directory, int slot) {
    char path[512];
    return replay_slot_path(directory, slot, path, sizeof(path));
}

bool replay_playback_step(const ReplayFile& file, ReplayPlaybackCursor& cursor,
                          u32& held_low16, u32& aux0, u32& aux1, u8& fps_byte) {
    held_low16 = 0;
    aux0 = 0;
    aux1 = 0;
    if (cursor.ended || cursor.stage_index < 0 ||
        cursor.stage_index >= i32(file.stages.size())) {
        cursor.ended = true;
        fps_byte = cursor.fps_byte;
        return false;
    }
    const ReplayStageSegment& seg = file.stages[size_t(cursor.stage_index)];
    if (cursor.frame_index < 0 || cursor.frame_index >= seg.frames) {
        cursor.ended = true;
        fps_byte = cursor.fps_byte;
        return false;
    }
    const u32 at = u32(cursor.frame_index) * 6u;
    if (at + 6 <= seg.inputs.size()) {
        held_low16 = read_u16(seg.inputs.data() + at);
        aux0 = read_u16(seg.inputs.data() + at + 2);
        aux1 = read_u16(seg.inputs.data() + at + 4);
        if (held_low16 == 0xffffu && aux0 == 0xffffu && aux1 == 0xffffu) {
            // 终止帧：输入全清零，不推进帧计数与 FPS 字节，此后不再读段
            held_low16 = aux0 = aux1 = 0;
            cursor.ended = true;
            cursor.terminated = true;
            fps_byte = cursor.fps_byte;
            return false;
        }
    }
    // 0x448246–0x44827b：先读帧率字节，再于第 0、30、60 帧推进指针。
    const u32 fps_index = (u32(cursor.frame_index) + 29u) / 30u;
    if (fps_index < seg.fps.size()) cursor.fps_byte = seg.fps[fps_index];
    fps_byte = cursor.fps_byte;
    ++cursor.frame_index;
    return true;
}

}  // namespace th16
