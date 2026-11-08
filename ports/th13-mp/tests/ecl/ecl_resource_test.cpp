// EclResource/EclProgram 单测：
// 1) 真实 st01.ecl 容器解析（59 子程序、ANIM/ECLI 名单、GirlA01 偏移 0x3e0）；
// 2) GirlA01 指令流与 §1.3 走查表逐字段一致；
// 3) 多文件聚合与名字二分查找。
#include "ecl_test_fixtures.hpp"

#include <algorithm>
#include <cstdio>

namespace {

th13test::TestFileReader& reader() {
    static th13test::TestFileReader instance;
    return instance;
}

TEST(st01_container_parse) {
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/st01.ecl", bytes));
    th13::EclResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    CHECK_EQ(resource.subroutines.size(), 59u);   // §1.1: sub_count = 0x3b
    CHECK_EQ(resource.animations.size(), 2u);     // anim { enemy.anm; st01enm.anm }
    CHECK_EQ(resource.animations[0], std::string("enemy.anm"));
    CHECK_EQ(resource.animations[1], std::string("st01enm.anm"));
    CHECK_EQ(resource.includes.size(), 2u);  // ecli { default.ecl; st01bs.ecl }
    CHECK_EQ(resource.includes[0], std::string("default.ecl"));
    CHECK_EQ(resource.includes[1], std::string("st01bs.ecl"));
    CHECK_EQ(resource.subroutines[0].name, std::string("GirlA01"));
    CHECK_EQ(resource.subroutines[0].offset, 0x3e0u - 0x10u);  // ECLH 头 0x3d0
}

TEST(girl_a01_instruction_walk) {
    // §1.3 走查表：{文件偏移, time, opcode, len, mask, rank, count, stack_adjust}
    struct Row {
        th13::u32 offset, time, opcode, length, mask, rank, count, adjust;
    };
    const Row rows[] = {
        {0x3e0, 0, 40, 20, 0, 0xff, 1, 0},   {0x3f4, 0, 500, 24, 0, 0xff, 2, 0},
        {0x40c, 0, 501, 24, 0, 0xff, 2, 0},  {0x424, 0, 404, 24, 0, 0xff, 2, 0},
        {0x43c, 0, 405, 32, 0, 0xff, 4, 0},  {0x45c, 0, 15, 32, 0, 0xcc, 1, 0},
        {0x47c, 0, 15, 36, 0, 0xc8, 1, 0},  {0x4a0, 0, 23, 20, 0, 0xff, 1, 0},
        {0x4b4, 0, 44, 20, 1, 0xff, 1, 0},  {0x4c8, 0, 44, 20, 1, 0xff, 1, 0},
        {0x4dc, 0, 44, 20, 0, 0xff, 1, 0},  {0x4f0, 0, 57, 16, 0, 0xff, 0, 0},
        {0x500, 0, 51, 16, 0, 0xff, 0, 0},  {0x510, 0, 429, 32, 4, 0xff, 4, 8},
        {0x530, 0, 12, 24, 0, 0xff, 2, 0},  {0x548, 0, 23, 20, 0, 0xff, 1, 0},
        {0x55c, 0, 42, 20, 0, 0xff, 1, 0},  {0x570, 0, 14, 24, 0, 0xff, 2, 0},
        {0x588, 0, 10, 16, 0, 0xff, 0, 0},
    };
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/st01.ecl", bytes));
    th13::EclResource resource;
    CHECK(resource.open(bytes.data(), th13::u32(bytes.size())));
    for (const auto& row : rows) {
        const auto* instruction = reinterpret_cast<const th13::EclInstruction*>(bytes.data() + row.offset);
        CHECK_EQ(th13::u32(instruction->time), row.time);
        CHECK_EQ(th13::u32(instruction->opcode), row.opcode);
        CHECK_EQ(th13::u32(instruction->length), row.length);
        CHECK_EQ(th13::u32(instruction->parameter_mask), row.mask);
        CHECK_EQ(th13::u32(instruction->rank_mask), row.rank);
        CHECK_EQ(th13::u32(instruction->parameter_count), row.count);
        CHECK_EQ(instruction->stack_adjust, row.adjust);
    }
    // 跳转目标算术：jmp(0x530) Δ=0x2C → 指令流坐标 0x17C（thecl 标签 GirlA01_396 = 0x17C+0x10）。
    const auto* jump = reinterpret_cast<const th13::EclInstruction*>(bytes.data() + 0x530);
    CHECK_EQ(jump->argument<th13::i32>(0), 0x2c);
    const auto* landed = reinterpret_cast<const th13::EclInstruction*>(bytes.data() + 0x3e0 + (0x530 - 0x3e0) + 0x2c);
    CHECK_EQ(th13::u32(landed->opcode), 42u);  // GirlA01_396 处是 pushi 1（if (1)）
    // jmpNeq(0x570) Δ=-40 → 回到 wait(1000)（0x548）。
    const auto* back = reinterpret_cast<const th13::EclInstruction*>(bytes.data() + 0x570 - 40);
    CHECK_EQ(th13::u32(back->opcode), 23u);
    CHECK_EQ(back->argument<th13::i32>(0), 1000);
}

TEST(embedded_fixture_matches_st01_bytes) {
    // 内嵌的 GirlA01/GirlA01_at 指令流必须与真实 st01.ecl 字节一致（固定用例的自证）。
    std::vector<th13::u8> bytes;
    CHECK(reader().read("artifacts/reference/raw/st01.ecl", bytes));
    const auto girl_a01 = th13test::girl_a01_stream();
    CHECK_EQ(girl_a01.size(), 0x598u - 0x3e0u);
    CHECK(std::equal(girl_a01.begin(), girl_a01.end(), bytes.begin() + 0x3e0));
    const auto girl_a01_at = th13test::girl_a01_at_stream();
    CHECK_EQ(girl_a01_at.size(), 0x77cu - 0x5a8u);
    CHECK(std::equal(girl_a01_at.begin(), girl_a01_at.end(), bytes.begin() + 0x5a8));
}

TEST(program_aggregation_and_lookup) {
    // 用内嵌固定序列构造两文件聚合：首文件序 + 后续按名插入 → 二分可命中。
    auto main_script = th13test::girl_a01_stream();
    auto other_script = th13test::girl_a01_at_stream();
    const auto a = th13test::build_scpt({{"GirlA01", main_script}});
    const auto b = th13test::build_scpt({{"GirlA01_at", other_script}});
    th13::EclProgram program;
    CHECK_EQ(program.attach(a.data(), th13::u32(a.size())), 0);
    CHECK_EQ(program.attach(b.data(), th13::u32(b.size())), 1);
    CHECK_EQ(program.definitions.size(), 2u);
    // 字典序：GirlA01 < GirlA01_at。
    CHECK_EQ(program.find_index("GirlA01"), 0);
    CHECK_EQ(program.find_index("GirlA01_at"), 1);
    CHECK_EQ(program.find_index("Missing"), -1);
    const auto* entry = program.find("GirlA01_at");
    CHECK(entry != nullptr);
    CHECK_EQ(th13::u32(entry->opcode), 40u);  // 首指令 stackAlloc(4)
    CHECK_EQ(entry->argument<th13::i32>(0), 4);
    // 真实文件 + include 供给：st01.ecl 聚合 default.ecl/st01bs.ecl 后名字可查。
    std::vector<th13::u8> st01, default_ecl, st01bs;
    CHECK(reader().read("artifacts/reference/raw/st01.ecl", st01));
    CHECK(reader().read("artifacts/reference/raw/default.ecl", default_ecl));
    CHECK(reader().read("artifacts/reference/raw/st01bs.ecl", st01bs));
    class Provider final : public th13::EclResourceProvider {
    public:
        std::vector<th13::u8> default_bytes, bs_bytes;
        std::vector<std::string> animations;
        bool read(const std::string& name, std::vector<th13::u8>& data) override {
            if (name == "default.ecl") {
                data = default_bytes;
                return true;
            }
            if (name == "st01bs.ecl") {
                data = bs_bytes;
                return true;
            }
            return false;
        }
        bool animation(th13::u32, const std::string& name) override {
            animations.push_back(name);
            return true;
        }
    } provider;
    provider.default_bytes = default_ecl;
    provider.bs_bytes = st01bs;
    th13::EclProgram full;
    CHECK(full.attach(st01.data(), th13::u32(st01.size()), &provider) == 0);
    CHECK(full.error.empty());
    CHECK(full.files.size() >= 3u);  // st01 + ecli 名单递归（default/st01bs 及其再包含）
    CHECK(provider.animations.size() >= 2u);  // 各文件 ANIM 名单都会挂接
    CHECK_EQ(provider.animations[0], std::string("enemy.anm"));
    CHECK_EQ(provider.animations[1], std::string("st01enm.anm"));
    // default.ecl 的子程序并入全局名字表（default.txt dump 里可见 EnemyCenter 等）。
    th13::EclResource default_resource;
    CHECK(default_resource.open(default_ecl.data(), th13::u32(default_ecl.size())));
    CHECK(!default_resource.subroutines.empty());
    for (const auto& sub : default_resource.subroutines) CHECK(full.find_index(sub.name.c_str()) >= 0);
    for (const auto& sub : default_resource.subroutines) CHECK(full.find(sub.name.c_str()) != nullptr);
    // 名字表全局非降序（二分前提；跨文件同名子程序按原版注册序相邻保留）。
    for (th13::u32 i = 1; i < full.definitions.size(); ++i)
        CHECK(std::strcmp(full.subroutine(i - 1).name.c_str(), full.subroutine(i).name.c_str()) <= 0);
}

}  // namespace

int main() { return th13test::run_all("ecl-resource"); }
