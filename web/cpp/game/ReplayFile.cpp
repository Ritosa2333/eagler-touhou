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

// ---- 0x458c30 压缩器（bit-exact 移植，规格 = capstone 直读反汇编）----
// 树/环布局与 0x458f30 解压侧严格镜像；与 oracle/replay_compress.py 完全同构
// （该实现已过 8/8 真 .rpy 逐字节往返 + 35/35 Unicorn 直调向量）。
namespace {
constexpr u32 kDeflateRing = 0x2000;     // 文本环 0x4da2f8
constexpr u32 kDeflateMask = 0x1FFF;     // 绝对环位掩码
constexpr u32 kDeflateMaxMatch = 0x12;   // 18：窗口超前量 = 最大匹配长
constexpr u32 kDeflateMinMatch = 3;
constexpr u32 kDeflateSentinel = 0x2000; // 下标 8192：rgt[8192]=根（0x4da2f0）

// 0x4593a0/0x4593f0/0x459440/0x4594d0：BST 删除。单子顶替父槽先查 rgt；
// 两子取左子树最右 v（先递归删 v，v 必无右子），再拼到 x 位（父槽先查 lft）。
void deflate_tree_remove(std::vector<i32>& dad, std::vector<i32>& lft,
                         std::vector<i32>& rgt, u32 x) {
    if (dad[x] == 0) return;
    u32 repl;
    if (rgt[x] == 0) {
        repl = u32(lft[x]);
    } else if (lft[x] == 0) {
        repl = u32(rgt[x]);
    } else {
        u32 v = u32(lft[x]);                    // 0x4594d0：左子树最右
        while (rgt[v] != 0) v = u32(rgt[v]);
        deflate_tree_remove(dad, lft, rgt, v);  // v 无右子 → 简单顶替摘除
        const i32 dd = dad[x];                  // 0x459440：v 拼到 x 位（先查 lft）
        if (lft[dd] == i32(x)) lft[dd] = i32(v); else rgt[dd] = i32(v);
        dad[v] = dd;
        lft[v] = lft[x];
        rgt[v] = rgt[x];
        dad[lft[v]] = i32(v);
        dad[rgt[v]] = i32(v);
        dad[x] = 0;
        return;
    }
    const i32 dd = dad[x];                      // 0x4593f0：简单顶替（先查 rgt）
    dad[repl] = dd;
    if (rgt[dd] == i32(x)) rgt[dd] = i32(repl); else lft[dd] = i32(repl);
    dad[x] = 0;
}
}  // namespace

std::vector<u8> ReplayFile::deflate(const u8* src, size_t size) {
    std::vector<u8> out;
    u32 acc = 0;
    u32 mask = 0x80;
    const auto put_bit = [&](u32 bit) {         // MSB 先行位流（0x458d0c 出位序）
        if (bit) acc |= mask;
        mask >>= 1;
        if (!mask) {
            out.push_back(u8(acc));
            acc = 0;
            mask = 0x80;
        }
    };

    // 文本环翻倍镜像（0x4da2f8 起memset 0；翻倍仅省环绕切片，不改语义）。
    std::vector<u8> ring(size_t(2) * kDeflateRing, 0);
    // dad/lft/rgt 每环位 12 字节交错（0x4c22e8 起）；含下标 8192 的越界哨兵项。
    std::vector<i32> dad(kDeflateSentinel + 1, 0);
    std::vector<i32> lft(kDeflateSentinel + 1, 0);
    std::vector<i32> rgt(kDeflateSentinel + 1, 0);

    // 预入环（0x458c96..0x458cbf）：n=min(18,size) 字节进 ring[1..n]，wp=1。
    u32 n = 0;
    size_t pos = 0;
    while (n < kDeflateMaxMatch && pos < size) {
        ring[1 + n] = src[pos];
        ring[1 + size_t(n) + kDeflateRing] = src[pos];
        ++pos;
        ++n;
    }
    u32 lookahead = n;
    size_t fill = pos;
    u32 wp = 1;

    // 树初始化（0x458cc5..0x458cdf）：root=rgt[8192]=1、dad[1]=8192（删根经
    // 哨兵别名改根）、lft[1]=rgt[1]=0。每次调用自初始化，与调用历史无关。
    rgt[kDeflateSentinel] = 1;
    dad[1] = i32(kDeflateSentinel);

    u32 match_len = 0;   // 首个 token 前为 0 → 必为字面量
    u32 match_pos = 0;

    // 0x459160：自根顺序比较 18 字节，首失配处 i>=best 含并列更新（后访问者
    // 胜）；i==18 幽灵顶替（0x4592df：node 被 p 原位顶替，返回 18）；wp==0 不入树。
    const auto tree_insert = [&](u32 p) -> u32 {
        if (p == 0) return 0;
        u32 node = u32(rgt[kDeflateSentinel]);
        u32 best = 0;
        while (true) {
            u32 i = 0;
            i32 c = 0;
            for (; i < kDeflateMaxMatch; ++i) {
                c = i32(ring[p + i]) - i32(ring[node + i]);
                if (c != 0) break;
            }
            if (i >= best) {
                best = i;
                match_pos = node;
            }
            if (i >= kDeflateMaxMatch) {
                const i32 dd = dad[node];       // 0x4592df..0x459355
                if (lft[dd] == i32(node)) lft[dd] = i32(p); else rgt[dd] = i32(p);
                dad[p] = dd;
                lft[p] = lft[node];
                rgt[p] = rgt[node];
                dad[lft[p]] = i32(p);
                dad[rgt[p]] = i32(p);
                dad[node] = 0;
                return kDeflateMaxMatch;
            }
            const u32 child = c < 0 ? u32(lft[node]) : u32(rgt[node]);
            if (child == 0) {                   // 0x459369..0x459394：挂叶子
                if (c < 0) lft[node] = i32(p); else rgt[node] = i32(p);
                dad[p] = i32(node);
                lft[p] = 0;
                rgt[p] = 0;
                return best;
            }
            node = child;
        }
    };

    // 主循环（0x458ceb..0x458eec）：贪心无 lazy，ml=min(match_len, lookahead)。
    while (lookahead > 0) {
        const u32 ml = match_len < lookahead ? match_len : lookahead;
        u32 count;
        if (ml <= 2) {
            put_bit(1);                          // 字面量：1 位标志 + 8 位 ring[wp]
            const u8 byte = ring[wp];
            for (int shift = 7; shift >= 0; --shift) put_bit(u32(byte >> shift) & 1);
            count = 1;
        } else {
            put_bit(0);                          // 拷贝：13 位绝对环址 + 4 位 (ml-3)
            for (int shift = 12; shift >= 0; --shift) put_bit(match_pos >> shift & 1);
            for (int shift = 3; shift >= 0; --shift) put_bit((ml - kDeflateMinMatch) >> shift & 1);
            count = ml;
        }
        for (u32 step = 0; step < count; ++step) {  // 0x458dd2..0x458ee6 前进
            const u32 r = (wp + kDeflateMaxMatch) & kDeflateMask;
            if (dad[r] != 0) deflate_tree_remove(dad, lft, rgt, r);
            if (fill < size) {                   // 超前 18 预填（fill 指针 ++）
                const u8 byte = src[fill];
                ++fill;
                ring[r] = byte;
                ring[size_t(r) + kDeflateRing] = byte;
            } else {
                --lookahead;
            }
            wp = (wp + 1) & kDeflateMask;
            if (lookahead != 0) match_len = tree_insert(wp);
        }
    }

    // 终止（0x458ef2..0x458f15）：1+13 = 14 个 0 位（解压侧地址 0 即终止）。
    put_bit(0);
    for (int i = 0; i < 13; ++i) put_bit(0);
    return out;
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
        // 事件流长度：录制期每 30 帧一字节（0x4480a0:25-37）；中断保存的 EOF
        // 帧（0x4484d0:53-58 追加的三字全 F）不产生事件字节，故以末帧是否为
        // EOF 计录制帧数基准（判定与回放消费方 0x448200:26 一致）。完整录像
        // 无 EOF，基准不变。
        u32 recorded = stage.frame_count;
        if (recorded > 0 && stage.frame(recorded - 1).eof()) --recorded;
        if (stage.events.size() != (recorded + 29) / 30)
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
