// ScoreDat 格式孪生单测（黄金对照；资产依赖见 run.sh 头注）：
// 1) parse_file(黄金档) → build_image 与黄金未压缩镜像逐字节全等
//    （route_a 默认档 / route_b 三组变异档；首个分歧字节如实报告）；
// 2) defaults(冻结种子 4660, meta.json) 镜像 vs route_a 黄金镜像全等
//    （覆盖 CR 默认 6×10 行、练习记录号/关卡 id、ST LCG 表全量 494 项）；
// 3) route_c 剔除怪癖：槽 1 缺席、镜像 0x15F9C、其余块内容不变；
// 4) 合成损坏：坏一个 CR 魔数 → build_image 缩短 0x56DC 且其余字节原样；
// 5) 字段视图：条目/进度/练习/ST 名/ending 区读写；
// 6) LCG 向量对照（Unicorn 读回值，oracle/scorefmt.lcg_entries 同源）；
// 7) 失败路径：坏魔数/坏版本/截断/坏校验和/未知块魔数。
#include "../harness/test_framework.hpp"

#include "game/ScoreDat.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

using th13::u8;
using th13::u16;
using th13::u32;
using th13::ScoreDat;

bool read_file(const std::string& relative, std::vector<u8>& out) {
    static const char* bases[] = {".", "../../..", "../../../..", "../../../../.."};
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

// 镜像全等；首个分歧字节偏移如实打印。
bool images_equal(const u8* actual, const u8* expected, size_t size, const char* label) {
    for (size_t i = 0; i < size; ++i) {
        if (actual[i] != expected[i]) {
            std::printf("  %s 首个分歧字节 0x%zX: 实际 %02X 期望 %02X\n",
                        label, i, actual[i], expected[i]);
            return false;
        }
    }
    return true;
}

bool load_golden(const char* name, std::vector<u8>& file, std::vector<u8>& image) {
    if (!read_file(std::string("artifacts/score_golden/") + name + ".bin", file)) {
        std::printf("  缺 artifacts/score_golden/%s.bin（容器内应存在，见 run.sh 头注）\n", name);
        return false;
    }
    if (!read_file(std::string("artifacts/score_golden/") + name + ".image.bin", image)) {
        std::printf("  缺 artifacts/score_golden/%s.image.bin（外层 oracle/scorefmt.py 解出）\n", name);
        return false;
    }
    return true;
}

bool parse_and_build(const char* name, ScoreDat& dat, std::vector<u8>& image) {
    std::vector<u8> file;
    if (!load_golden(name, file, image)) return false;
    if (!dat.parse_file(file.data(), file.size())) {
        std::printf("  %s parse_file 失败: %s\n", name, dat.error.c_str());
        return false;
    }
    return true;
}

TEST(parse_golden_route_a_image_equal) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_a_default", dat, image)) {
        CHECK(false);
        return;
    }
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size());
    CHECK(images_equal(built.data(), image.data(), image.size(), "route_a"));
}

TEST(defaults_frozen_seed_route_a_image_equal) {
    // meta.json 冻结种子 4660（= Unicorn 生成黄金时的 u16@0x4dc324）。
    ScoreDat dat;
    dat.set_defaults(4660);
    std::vector<u8> file, image;
    if (!load_golden("route_a_default", file, image)) {
        CHECK(false);
        return;
    }
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size());
    CHECK(images_equal(built.data(), image.data(), image.size(), "defaults(4660)"));
}

TEST(parse_golden_mutated_images_equal) {
    for (const char* name : {"route_b_alpha", "route_b_beta", "route_b_gamma"}) {
        ScoreDat dat;
        std::vector<u8> image;
        if (!parse_and_build(name, dat, image)) {
            CHECK(false);
            return;
        }
        std::vector<u8> built(ScoreDat::kImageSize);
        const size_t length = dat.build_image(built.data(), built.size());
        CHECK_EQ(length, image.size());
        CHECK(images_equal(built.data(), image.data(), image.size(), name));
    }
}

TEST(route_c_dropped_block_quirk) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_c_dropped", dat, image)) {
        CHECK(false);
        return;
    }
    CHECK_EQ(image.size(), size_t(0x15F9C));      // 0x1B678 - 0x56DC
    CHECK(!dat.cr_present[1]);                    // 槽 1 整块剔除
    for (int slot : {0, 2, 3, 4}) CHECK(dat.cr_present[slot]);
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size());
    CHECK(images_equal(built.data(), image.data(), image.size(), "route_c"));
}

TEST(corrupted_cr_magic_drops_block_and_shortens) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_a_default", dat, image)) {
        CHECK(false);
        return;
    }
    // route_c 怪癖的模型层合成：保存路径逐槽校验 u16=='CR'（0x449ec5），坏块
    // 整块剔除 = cr_present=false（黄金生成时的内存 poke 在文件侧即缺席块）。
    dat.cr_present[2] = false;
    std::vector<u8> built(ScoreDat::kImageSize);
    const size_t length = dat.build_image(built.data(), built.size());
    CHECK_EQ(length, image.size() - ScoreDat::kCRSize);  // uncompSize 缩短 0x56DC
    // 其余块内容不变：缺席块前的字节与其后拼接逐字节对照。
    const size_t cut = 2 * ScoreDat::kCRSize;
    CHECK(images_equal(built.data(), image.data(), cut, "before dropped block"));
    CHECK(images_equal(built.data() + cut, image.data() + cut + ScoreDat::kCRSize,
                       length - cut, "after dropped block"));
    // 逐字节镜像模型再 parse：块 2 缺席语义保真。
    ScoreDat reparsed;
    CHECK(reparsed.parse_image(built.data(), length));
    CHECK(!reparsed.cr_present[2]);
    CHECK_EQ(reparsed.cr_present[0] && reparsed.cr_present[1] && reparsed.cr_present[3] &&
                 reparsed.cr_present[4],
             true);
}

TEST(entry_field_views) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_b_alpha", dat, image)) {
        CHECK(false);
        return;
    }
    // meta.json route_b_alpha: entry block0 chara2 rank3 = score 123456789,
    // stage 6, clear 1, name "PLAYER1\0", time_lo 1600000000, rate_bits 1118765056。
    const u8* block0 = dat.cr[0].data();
    const ScoreDat::Entry entry = ScoreDat::read_entry(block0, 2, 3);
    CHECK_EQ(entry.score, u32(123456789));
    CHECK_EQ(entry.stage, u8(6));
    CHECK_EQ(entry.clear, u8(1));
    CHECK(std::memcmp(entry.name.data(), "PLAYER1\0", 8) == 0);
    CHECK_EQ(entry.name[8], u8(0));
    CHECK_EQ(entry.time_lo, u32(1600000000));
    CHECK_EQ(entry.rate_bits, u32(1118765056));
    // 默认行不受影响：block0 chara0 rank0 = 100000/"--------\0"。
    const ScoreDat::Entry top = ScoreDat::read_entry(block0, 0, 0);
    CHECK_EQ(top.score, u32(100000));
    CHECK(std::memcmp(top.name.data(), "--------\0", 9) == 0);
    // 写视图往返：改写后读回一致且垫字节恒 0。
    ScoreDat::Entry mutated = top;
    mutated.score = 424242;
    mutated.stage = 5;
    mutated.clear = 1;
    std::memcpy(mutated.name.data(), "MEIKO\0\0\0", 9);
    mutated.time_lo = 0x11111111;
    mutated.time_hi = 0x22222222;
    mutated.rate_bits = 0x3F800000;
    ScoreDat::write_entry(dat.cr[0].data(), 0, 0, mutated);
    const ScoreDat::Entry read_back = ScoreDat::read_entry(dat.cr[0].data(), 0, 0);
    CHECK_EQ(read_back.score, u32(424242));
    CHECK_EQ(read_back.stage, u8(5));
    CHECK_EQ(read_back.clear, u8(1));
    CHECK(std::memcmp(read_back.name.data(), "MEIKO\0\0\0", 9) == 0);
    CHECK_EQ(read_back.time_lo, u32(0x11111111));
    CHECK_EQ(read_back.time_hi, u32(0x22222222));
    CHECK_EQ(read_back.rate_bits, u32(0x3F800000));
    CHECK_EQ(dat.cr[0][ScoreDat::kEntriesOff + 15], u8(0));
}

TEST(progress_and_practice_views) {
    // 视图读数对照 meta.json 变异记录（route_b_gamma / route_b_alpha 的
    // block_raw / practice poke），验证视图偏移与镜像字节一致（不臆造语义）。
    auto raw_u32 = [](const u8* at) {
        return u32(at[0]) | u32(at[1]) << 8 | u32(at[2]) << 16 | u32(at[3]) << 24;
    };
    ScoreDat gamma;
    std::vector<u8> image;
    if (!parse_and_build("route_b_gamma", gamma, image)) {
        CHECK(false);
        return;
    }
    const u8* g0 = gamma.cr[0].data();
    // block_raw(0, 1996=b'\x9f\x86\x01\x00')：游戏回数 chara5 = 99999（0x01869F）。
    CHECK_EQ(ScoreDat::progress_u32(g0, 0x7B8, 5), u32(99999));
    // block_raw(0, 2004=b'\x9f\x86\x01\x00')：通关数 chara5 = 99999（+0x7D4）。
    CHECK_EQ(ScoreDat::progress_u32(g0, 0x7C0, 5), u32(99999));
    // block_raw(0, 2032=b'90\x00\x00')：进度行 (chara0,stage0) 最高分 = 0x3039。
    CHECK_EQ(ScoreDat::progress_row_score(g0, 0, 0), u32(12345));
    // block_raw(2, 1696)：mid 区（+0x6A0 未见证）16 字节透传。
    const u8* g2 = gamma.cr[2].data();
    for (int i = 0; i < 16; ++i) CHECK_EQ(g2[0x6A0 + i], u8(i));

    ScoreDat alpha;
    if (!parse_and_build("route_b_alpha", alpha, image)) {
        CHECK(false);
        return;
    }
    // block_raw(1, 1976=b'*')：block1 游戏回数 chara0 = 42。
    CHECK_EQ(ScoreDat::progress_u32(alpha.cr[1].data(), 0x7B8, 0), u32(42));
    // block_raw(1, 1992=b'\x07')：block1 游戏回数 chara4 = 7（+0x7C8）。
    CHECK_EQ(ScoreDat::progress_u32(alpha.cr[1].data(), 0x7B8, 4), u32(7));
    // block_raw(4, 2032=b'1\xde\x0b\x00\x01\x01...')：block4 进度行
    // (chara0,stage0) 最高分 0x000BDE31=777777、+4/+5 旗各 1。
    const u8* a4 = alpha.cr[4].data();
    CHECK_EQ(ScoreDat::progress_row_score(a4, 0, 0), u32(777777));
    CHECK_EQ(ScoreDat::progress_row_flag(a4, 0, 0, 0), u8(1));
    CHECK_EQ(ScoreDat::progress_row_flag(a4, 0, 0, 1), u8(1));
    (void)raw_u32;
    // practice(2, 5, best=987654)。
    CHECK_EQ(ScoreDat::practice_u32(alpha.cr[2].data(), 5, 0x98), u32(987654));

    // 练习记录号/关卡 id 默认表（route_b_gamma 对 defaults 的偏离仅列明处）。
    u8* block4 = gamma.cr[4].data();
    CHECK_EQ(ScoreDat::practice_u32(block4, 126, 0x90), u32(126));
    CHECK_EQ(ScoreDat::practice_u32(block4, 0, 0x94), u32(0));
    CHECK_EQ(ScoreDat::practice_u32(block4, 126, 0x94), u32(5));  // stage_id 突变
    ScoreDat::set_practice_u32(block4, 7, 0x98, 987654);
    CHECK_EQ(ScoreDat::practice_u32(block4, 7, 0x98), u32(987654));
}

TEST(st_name_and_ending_views) {
    ScoreDat dat;
    std::vector<u8> image;
    if (!parse_and_build("route_b_alpha", dat, image)) {
        CHECK(false);
        return;
    }
    const std::array<u8, 9> name = ScoreDat::st_name(dat.st.data());
    CHECK(std::memcmp(name.data(), "NAME0008\0", 9) == 0);
    std::array<u8, 9> renamed = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 0};
    ScoreDat::set_st_name(dat.st.data(), renamed);
    CHECK(std::memcmp(ScoreDat::st_name(dat.st.data()).data(), "ABCDEFGH\0", 9) == 0);
    // ending 区 13 字节（ST+0x16..0x22）往返；黄金里为 0（0x449a30 memset）。
    u8 seen[ScoreDat::kEndingBytes] = {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    ScoreDat::write_ending_bytes(dat.st.data(), seen);
    u8 read_back[ScoreDat::kEndingBytes] = {};
    ScoreDat::read_ending_bytes(dat.st.data(), read_back);
    CHECK(std::memcmp(read_back, seen, ScoreDat::kEndingBytes) == 0);
    u8 zeros[ScoreDat::kEndingBytes] = {};
    ScoreDat::read_ending_bytes(image.data() + 5 * ScoreDat::kCRSize, read_back);
    CHECK(std::memcmp(read_back, zeros, ScoreDat::kEndingBytes) == 0);
}

TEST(lcg_vectors) {
    // Unicorn 读回向量（oracle/scorefmt.lcg_entries 同源）。
    u16 state = 0;
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0xC374));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0xBFC7));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0x1293));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0x7D40));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0x1876));
    CHECK_EQ(ScoreDat::lcg_next(state), u16(0xA3CC));
    u16 seed_state = 4660;
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0x7AC4));
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0x1E86));
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0x8D8C));
    CHECK_EQ(ScoreDat::lcg_next(seed_state), u16(0xD9A6));
    // 494 项后状态收敛路径（route_a 黄金末项前的状态推演由 defaults 全等覆盖）。
}

TEST(parse_failure_paths) {
    std::vector<u8> file, image;
    if (!load_golden("route_a_default", file, image)) {
        CHECK(false);
        return;
    }
    ScoreDat dat;
    CHECK(dat.parse_file(file.data(), file.size()));

    std::vector<u8> bad = file;
    bad[0] = 'X';
    CHECK(!dat.parse_file(bad.data(), bad.size()));  // 坏魔数
    bad = file;
    bad[8] = 5;                                      // 版本 ≠ 4
    CHECK(!dat.parse_file(bad.data(), bad.size()));
    CHECK(!dat.parse_file(file.data(), 4));          // 截断头
    CHECK(!dat.parse_file(file.data(), 0x18));       // 缺密文体

    // 坏校验和：镜像内 ST 表翻转 1 字节（校验和区 [+8,0x42C)）。
    std::vector<u8> broken_image = image;
    broken_image[image.size() - 4] ^= 0x40;
    CHECK(!dat.parse_image(broken_image.data(), broken_image.size()));
    // 未知块魔数：镜像首块头改坏（游戏侧语义 = 提前停走）。
    broken_image = image;
    broken_image[0] = 'X';
    broken_image[1] = 'X';
    CHECK(!dat.parse_image(broken_image.data(), broken_image.size()));
    // 纯镜像（无头）也走 parse_image；带 'TH31' 头的字节不应进 parse_image。
    CHECK(!dat.parse_image(file.data(), file.size()));
}

TEST(build_file_round_trip_golden) {
    // 阶段 B：build_file(parse(黄金)) 整文件逐字节相等（含头/压缩流/密文）。
    // ReplayFile::deflate 为 0x458c30 bit-exact（35/35 Unicorn 向量），黄金由
    // Unicorn 直调原版压缩器产出。
    for (const char* name :
         {"route_a_default", "route_b_alpha", "route_b_beta", "route_b_gamma",
          "route_c_dropped"}) {
        ScoreDat dat;
        std::vector<u8> image;
        if (!parse_and_build(name, dat, image)) {
            CHECK(false);
            return;
        }
        std::vector<u8> rebuilt;
        if (!dat.build_file(rebuilt)) {
            std::printf("  %s build_file 失败: %s\n", name, dat.error.c_str());
            CHECK(false);
            return;
        }
        std::vector<u8> golden;
        CHECK(read_file(std::string("artifacts/score_golden/") + name + ".bin", golden));
        if (rebuilt.size() != golden.size()) {
            std::printf("  %s 尺寸分歧: 实际 %zu 期望 %zu\n", name, rebuilt.size(),
                        golden.size());
        }
        CHECK_EQ(rebuilt.size(), golden.size());
        CHECK(images_equal(rebuilt.data(), golden.data(),
                           rebuilt.size() < golden.size() ? rebuilt.size() : golden.size(),
                           name));
        // 再 parse 往返：build 产物自身结构自洽。
        ScoreDat reparsed;
        CHECK(reparsed.parse_file(rebuilt.data(), rebuilt.size()));
        std::vector<u8> image2(ScoreDat::kImageSize);
        CHECK_EQ(reparsed.build_image(image2.data(), image2.size()), image.size());
        CHECK(images_equal(image2.data(), image.data(), image.size(), name));
    }
}

}  // namespace

int main() {
    return th13test::run_all("scoreth13.dat 格式孪生");
}
