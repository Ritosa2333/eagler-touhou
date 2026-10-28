// ReplayFile 实现。全部常量与控制流对应原版地址，见 ReplayFile.hpp 文件头。
#include "ReplayFile.hpp"

namespace th13 {

namespace {
constexpr u32 kHeaderSize = 0x24;
constexpr u32 kTopSize = 0x74;
constexpr u32 kStageHeaderSize = 0xC4;
constexpr u32 kFrameBytes = 6;

u16 read_u16(const u8* p) {
    return u16(p[0]) | u16(p[1]) << 8;
}
u32 read_u32(const u8* p) {
    return u32(p[0]) | u32(p[1]) << 8 | u32(p[2]) << 16 | u32(p[3]) << 24;
}
i32 read_i32(const u8* p) {
    return i32(read_u32(p));
}
float read_f32(const u8* p) {
    const u32 bits = read_u32(p);
    float value;
    std::memcpy(&value, &bits, sizeof value);
    return value;
}
}  // namespace

ReplayFrame ReplayStage::frame(u32 index) const {
    ReplayFrame result;
    if (index < frame_count && (index + 1) * kFrameBytes <= input.size()) {
        const u8* p = input.data() + index * kFrameBytes;
        result.state = read_u16(p);
        result.pressed = read_u16(p + 2);
        result.released = read_u16(p + 4);
    }
    return result;
}

// 0x472280/0x4723a0 公共骨架。两方向互逆（各自单独非自逆）：
//   解密 0x472280：顺序读 → 写块内交织反转位（块尾-1,-3,… 前 ceil 半，再
//   块尾-2,-4,…），密钥逐字节 +step、跨块连续；
//   加密 0x4723a0：顺序读位反过来（读交织反转位 → 顺序写）。
// 读写交叠，原版经 malloc 拷贝读侧，此处同构。尾部门槛：余数 < block/4 时
// 该余数不变换；奇尺寸末字节不变换（iVar4 = size-(size&1)-tail，有符号可
// 至负 → 无变换区间）。
namespace {
// 两种方向共用的长度/块划分。
std::vector<size_t> transform_plan(size_t size, u32 block) {
    const size_t remainder = size % block;
    const size_t tail = remainder < block / 4 ? remainder : 0;
    const size_t aligned = size - (size & 1);
    size_t remaining = aligned >= tail ? aligned - tail : 0;
    std::vector<size_t> chunks;
    while (remaining > 0) {
        const size_t chunk = remaining < block ? remaining : block;
        chunks.push_back(chunk);
        remaining -= chunk;
    }
    return chunks;
}
}  // namespace

void ReplayFile::xor_decrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block) {
    const std::vector<size_t> chunks = transform_plan(size, block);
    const std::vector<u8> source(buf, buf + size);
    u32 key = init_key & 0xFF;
    size_t pos = 0;
    for (size_t chunk : chunks) {
        const size_t base = pos;
        const size_t half = (chunk + 1) / 2;
        size_t write = base + chunk - 1;  // 前半：奇距位（块尾-1,-3,…）
        for (size_t i = 0; i < half; ++i) {
            buf[write] = u8(source[pos] ^ key);
            key = (key + step) & 0xFF;
            write -= 2;
            ++pos;
        }
        write = base + chunk;  // 后半：偶距位（块尾-2,-4,…）
        for (size_t i = 0; i < chunk - half; ++i) {
            write -= 2;
            buf[write] = u8(source[pos] ^ key);
            key = (key + step) & 0xFF;
            ++pos;
        }
    }
}

void ReplayFile::xor_encrypt(u8* buf, size_t size, u32 init_key, u32 step, u32 block) {
    const std::vector<size_t> chunks = transform_plan(size, block);
    const std::vector<u8> source(buf, buf + size);
    u32 key = init_key & 0xFF;
    size_t pos = 0;
    for (size_t chunk : chunks) {
        const size_t base = pos;
        const size_t half = (chunk + 1) / 2;
        size_t read = base + chunk - 1;  // 前半：奇距位（块尾-1,-3,…）
        for (size_t i = 0; i < half; ++i) {
            buf[pos] = u8(source[read] ^ key);
            key = (key + step) & 0xFF;
            read -= 2;
            ++pos;
        }
        read = base + chunk;  // 后半：偶距位（块尾-2,-4,…）
        for (size_t i = 0; i < chunk - half; ++i) {
            read -= 2;
            buf[pos] = u8(source[read] ^ key);
            key = (key + step) & 0xFF;
            ++pos;
        }
    }
}

bool ReplayFile::inflate(const u8* src, size_t src_size, std::vector<u8>& out, size_t limit) {
    out.clear();
    if (limit == 0) return true;
    out.reserve(limit);
    u8 ring[0x2000] = {};
    u32 ring_pos = 1;  // 原版游标初值 1（0x458f30:20）
    size_t bit = 0;
    const auto read_bit = [&]() -> u32 {
        const u32 byte = bit >> 3 < src_size ? src[bit >> 3] : 0;  // 越界读 0
        const u32 value = byte >> (7 - (bit & 7)) & 1;
        ++bit;
        return value;
    };
    const auto read_bits = [&](u32 count) {
        u32 value = 0;
        for (u32 i = 0; i < count; ++i) value = value << 1 | read_bit();
        return value;
    };
    while (true) {
        if (read_bit()) {
            const u32 byte = read_bits(8);
            out.push_back(u8(byte));
            ring[ring_pos] = u8(byte);
            ring_pos = (ring_pos + 1) & 0x1FFF;
        } else {
            const u32 address = read_bits(13);
            if (address == 0) break;  // 终止符
            const u32 count = read_bits(4);
            for (u32 i = 0; i < count + 3; ++i) {  // 长 = n + 3（0x458f30 拷贝循环）
                const u8 byte = ring[(address + i) & 0x1FFF];
                out.push_back(byte);
                ring[ring_pos] = byte;
                ring_pos = (ring_pos + 1) & 0x1FFF;
            }
        }
        if (out.size() > limit) return false;  // 超出声明尺寸即解码错
    }
    return out.size() == limit;
}

bool ReplayFile::decode(const u8* data, size_t size) {
    if (size < kHeaderSize) return fail("file too small");
    if (std::memcmp(data, "t13r", 4) != 0) return fail("bad magic");
    version = read_u16(data + 4);
    if (version != 2) return fail("unsupported replay version");
    user_offset = read_u32(data + 0xC);
    comp_size = read_u32(data + 0x1C);
    uncomp_size = read_u32(data + 0x20);
    if (user_offset != comp_size + kHeaderSize) return fail("user offset mismatch");
    if (size < size_t(comp_size) + kHeaderSize) return fail("truncated stage data");
    if (uncomp_size == 0 || uncomp_size > 0x4000000) return fail("bad uncompressed size");

    // 装载解密序（0x448d43/0x448d5d 两趟；保存序为其逆）。
    std::vector<u8> body(data + kHeaderSize, data + kHeaderSize + comp_size);
    xor_decrypt(body.data(), body.size(), 0x5C, 0xE1, 0x400);
    xor_decrypt(body.data(), body.size(), 0x7D, 0x3A, 0x100);

    std::vector<u8> plain;
    if (!inflate(body.data(), body.size(), plain, uncomp_size)) return fail("inflate failed");

    // 顶层 0x74 头。
    if (plain.size() < kTopSize) return fail("stage data too small");
    top_header.assign(plain.begin(), plain.begin() + ptrdiff_t(kTopSize));
    player_name.assign(reinterpret_cast<const char*>(plain.data()), std::size_t(12));
    // 保存侧 0x20 填充至 ≥8 字符（0x4484d0:50-52）：按 NUL 或尾随空格裁剪。
    const auto cut = player_name.find('\0');
    if (cut != std::string::npos) player_name.resize(cut);
    while (!player_name.empty() && player_name.back() == ' ') player_name.pop_back();
    score = read_i32(plain.data() + 0x14);
    slow_rate = read_f32(plain.data() + 0x54);
    stage_count = plain[0x58];
    character = read_i32(plain.data() + 0x5C);
    shot_type = read_i32(plain.data() + 0x60);
    rank = read_i32(plain.data() + 0x64);
    practice_stage = read_i32(plain.data() + 0x70);
    stages.clear();
    stages.reserve(stage_count);

    // stage 块链（0x448c40:67-82 的步进与容量语义）。
    size_t pos = kTopSize;
    for (u32 s = 0; s < stage_count; ++s) {
        if (pos + kStageHeaderSize > plain.size()) return fail("stage header out of range");
        ReplayStage stage;
        stage.header.assign(plain.begin() + ptrdiff_t(pos),
                            plain.begin() + ptrdiff_t(pos + kStageHeaderSize));
        stage.number = read_u16(plain.data() + pos);
        stage.stamp = read_u16(plain.data() + pos + 2);
        stage.frame_count = read_u32(plain.data() + pos + 4);
        stage.score = read_i32(plain.data() + pos + 0x1C);
        stage.blob_size = read_u32(plain.data() + pos + 8);
        const size_t input_bytes = size_t(stage.frame_count) * kFrameBytes;
        if (input_bytes > stage.blob_size) return fail("stage input exceeds blob");
        if (pos + kStageHeaderSize + stage.blob_size > plain.size()) return fail("stage blob out of range");
        stage.input.assign(plain.begin() + ptrdiff_t(pos + kStageHeaderSize),
                           plain.begin() + ptrdiff_t(pos + kStageHeaderSize + input_bytes));
        stage.events.assign(plain.begin() + ptrdiff_t(pos + kStageHeaderSize + input_bytes),
                            plain.begin() + ptrdiff_t(pos + kStageHeaderSize + stage.blob_size));
        if (stage.events.size() != (stage.frame_count + 29) / 30)
            return fail("stage event stream length mismatch");
        stages.push_back(std::move(stage));
        pos += kStageHeaderSize + stage.blob_size;
    }

    // USER 段（0x4484d0:170-247 落盘格式："USER"+u32 块长+u8 类型+3 填充+文本）。
    user_blocks.clear();
    size_t user = user_offset;
    while (user + 12 <= size && std::memcmp(data + user, "USER", 4) == 0) {
        const u32 block = read_u32(data + user + 4);
        if (block < 12 || user + block > size) return fail("user block out of range");
        user_blocks.emplace_back(reinterpret_cast<const char*>(data + user + 12),
                                 std::size_t(block - 12));
        user += block;
    }
    return true;
}

}  // namespace th13
