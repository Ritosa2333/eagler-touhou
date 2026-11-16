// ReplayFile::deflate（0x458c30 bit-exact 移植）验证：
// 1) Unicorn 黄金向量（deflate_golden.hpp，容器内直调原版压缩器生成）；
// 2) 真机 .rpy：解码平流 → deflate → 与文件内解密后的压缩段逐字节相等
//    （C.1 核心口径：每份样本 = 双趟 XOR 解密后的密文段）；
// 3) deflate ↔ inflate 互逆（合成与真机平流）；
// 4) 终止符/自复位语义：空输入 = 单个 0x00（14 个 0 位不满字节不落盘），
//    同一进程多次调用结果与首调一致（0x458c30 入口自初始化）。
#include "../harness/test_framework.hpp"

#include "game/ReplayFile.hpp"
#include "deflate_golden.hpp"

#include <cstdio>
#include <string>
#include <vector>

namespace {
using th13::u8;
using th13::u32;

bool read_file(const std::string& relative, std::vector<u8>& out) {
    static const char* bases[] = {".", "../../../..", "../../../../.."};
    for (const char* base : bases) {
        std::FILE* file = std::fopen((std::string(base) + "/" + relative).c_str(), "rb");
        if (!file) continue;
        std::fseek(file, 0, SEEK_END);
        const long size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        out.resize(size_t(size < 0 ? 0 : size));
        const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
        std::fclose(file);
        if (got == out.size()) return true;
        return false;
    }
    return false;
}

std::vector<u8> pattern_bytes(size_t size, u32 seed) {
    std::vector<u8> bytes(size);
    u32 state = seed;
    for (auto& byte : bytes) {
        state = state * 0x343FD + 0x269EC3;
        byte = u8(state >> 16);
    }
    return bytes;
}

TEST(deflate_matches_original_on_unicorn_golden_vectors) {
    for (std::size_t i = 0; i < deflate_golden::kCaseCount; ++i) {
        const auto& item = deflate_golden::kCases[i];
        const std::vector<u8> packed =
            th13::ReplayFile::deflate(item.plain, item.plain_size);
        CHECK_EQ(packed.size(), item.packed_size);
        bool equal = packed.size() == item.packed_size;
        for (std::size_t b = 0; equal && b < item.packed_size; ++b) {
            if (packed[b] != item.packed[b]) equal = false;
        }
        if (!equal && item.packed_size > 0) {
            std::printf("# first divergence in %s at byte %zu: got 0x%02X want 0x%02X\n",
                        item.name, [&] {
                            for (std::size_t b = 0; b < item.packed_size; ++b) {
                                if (b >= packed.size() || packed[b] != item.packed[b]) return b;
                            }
                            return item.packed_size;
                        }(),
                        packed.size() > 0 ? packed[0] : 0, item.packed[0]);
        }
        CHECK(equal);
    }
}

TEST(deflate_empty_input_is_single_zero_byte) {
    // 结尾 14 个 0 位：8 位落入首字节，余 6 位不满字节不落盘（0x458f1d 出口
    // 写实际字节数）。
    const std::vector<u8> packed = th13::ReplayFile::deflate(nullptr, 0);
    CHECK_EQ(packed.size(), size_t(1));
    CHECK_EQ(packed[0], u8(0x00));
}

TEST(deflate_is_self_resetting_across_calls) {
    // 入口自初始化（环 memset + 树清零），同进程重复调用与首调逐字节一致。
    const std::vector<u8> data = pattern_bytes(3000, 42);
    const std::vector<u8> first = th13::ReplayFile::deflate(data.data(), data.size());
    for (int round = 0; round < 3; ++round) {
        th13::ReplayFile::deflate(nullptr, 0);  // 中间穿插空调用
        const std::vector<u8> again = th13::ReplayFile::deflate(data.data(), data.size());
        CHECK(again == first);
    }
}

TEST(deflate_inflate_roundtrip_synthetic) {
    const std::vector<u8> data = pattern_bytes(70000, 7);
    const std::vector<u8> packed = th13::ReplayFile::deflate(data.data(), data.size());
    std::vector<u8> out;
    CHECK(th13::ReplayFile::inflate(packed.data(), packed.size(), out, data.size()));
    CHECK(out == data);
}

// 真机样本清单（games/ 只读；与 replay_test.cpp 同一解析约定）。
struct Sample {
    const char* path;
};

const Sample kSamples[] = {
    {"games/replays/th13_01.rpy"},
    {"games/replays/th13_udMy25.rpy"},
    {"games/replays/th13_udN01.rpy"},
    {"games/replays/th13_udPn16.rpy"},
    {"games/replays/th13_udSr01.rpy"},
    {"games/replays/th13_udWi11.rpy"},
    {"games/th13/replay/th13_01.rpy"},
    {"games/th13/replay/th13_udSr01.rpy"},
};

bool decode_sample(const Sample& sample, th13::ReplayFile& replay, std::vector<u8>& packed) {
    std::vector<u8> bytes;
    if (!read_file(sample.path, bytes)) return false;
    if (!replay.decode(bytes.data(), bytes.size())) return false;
    if (bytes.size() < size_t(replay.comp_size) + 0x24) return false;
    packed.assign(bytes.begin() + ptrdiff_t(0x24),
                  bytes.begin() + ptrdiff_t(0x24 + replay.comp_size));
    // 装载解密序（0x448d43/0x448d5d 两趟）还原压缩流。
    th13::ReplayFile::xor_decrypt(packed.data(), packed.size(), 0x5C, 0xE1, 0x400);
    th13::ReplayFile::xor_decrypt(packed.data(), packed.size(), 0x7D, 0x3A, 0x100);
    return true;
}

// 解码结构 → 完整平流（顶层 0x74 + stage 块链）。平流必须与文件头声明的
// uncomp_size 一致，否则解码侧丢了字节。
std::vector<u8> rebuild_plain(const th13::ReplayFile& replay) {
    std::vector<u8> plain = replay.top_header;
    for (const th13::ReplayStage& stage : replay.stages) {
        plain.insert(plain.end(), stage.header.begin(), stage.header.end());
        plain.insert(plain.end(), stage.input.begin(), stage.input.end());
        plain.insert(plain.end(), stage.events.begin(), stage.events.end());
    }
    return plain;
}

TEST(deflate_reproduces_real_replay_packed_stream) {
    for (const Sample& sample : kSamples) {
        th13::ReplayFile replay;
        std::vector<u8> packed;
        if (!decode_sample(sample, replay, packed)) {
            std::printf("# missing or undecodable sample: %s\n", sample.path);
            CHECK(false);
            continue;
        }
        std::vector<u8> plain = rebuild_plain(replay);
        CHECK_EQ(plain.size(), size_t(replay.uncomp_size));
        const std::vector<u8> compressed = th13::ReplayFile::deflate(plain.data(), plain.size());
        bool equal = compressed.size() == packed.size();
        size_t diverge = packed.size();
        for (size_t b = 0; b < compressed.size() && b < packed.size(); ++b) {
            if (compressed[b] != packed[b]) {
                diverge = b;
                equal = false;
                break;
            }
        }
        if (!equal) {
            std::printf("# %s: packed %zu bytes, deflate %zu bytes, first divergence %zu\n",
                        sample.path, packed.size(), compressed.size(), diverge);
        }
        CHECK(equal);
        // 互逆：deflate 输出经 inflate 还原平流。
        std::vector<u8> restored;
        CHECK(th13::ReplayFile::inflate(compressed.data(), compressed.size(), restored,
                                        plain.size()));
        CHECK(restored == plain);
    }
}

}  // namespace

int main() { return th13test::run_all("ReplayFile::deflate 0x458c30"); }
