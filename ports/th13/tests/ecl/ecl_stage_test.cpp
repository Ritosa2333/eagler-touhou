// stage1 模拟单测：
// 1) 数学原语（wrap/round001/ease）与原版运算结构一致；
// 2) StageSim 装载真实 st01 依赖清单并跑满 3600 帧（Lunatic）；
// 3) JSONL 事件流与 docs/plans/event-contract.md 的键序/字段/精度一致；
// 4) 同种子两次运行逐字节一致（确定性）。
#ifndef TH13_TEST_PORT_DIR
#define TH13_TEST_PORT_DIR "."
#endif
#include "ecl_test_fixtures.hpp"

#include "game/StageMath.hpp"
#include "game/StageSim.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {
using th13::u32;
using th13::u64;

TEST(stage_math_primitives) {
    // 0x472f30：±π 归一（常量为 float π 的 double 视图）。
    CHECK_NEAR(th13::wrap_angle(3.5f), float(3.5 - 6.2831854820251465), 1e-6);
    CHECK_NEAR(th13::wrap_angle(-3.5f), float(-3.5 + 6.2831854820251465), 1e-6);
    CHECK_NEAR(th13::wrap_angle(1.0f), 1.0f, 1e-6);
    // 0x41c137 镜像：右（0）翻转成左（π），竖直（π/2）不变。
    CHECK_NEAR(th13::mirror_angle(0.0f), 3.1415927f, 1e-6);
    CHECK_NEAR(th13::mirror_angle(1.5707964f), 1.5707964f, 1e-6);
    // 0x497690：floor 量化 0.01（向负无穷；oracle 写断点实证 -163.783264 → -163.79）。
    CHECK_NEAR(th13::round001(0.125f), 0.12f, 1e-9);   // floor(12.5)=12
    CHECK_NEAR(th13::round001(0.119f), 0.11f, 1e-9);   // floor(11.9)=11
    CHECK_NEAR(th13::round001(-163.783264f), -163.79f, 1e-9);
    CHECK_NEAR(th13::round001(1.0f), 1.0f, 1e-9);
    // 0x473a40：mode 0 线性、1 平方、4 ease-out 平方。
    CHECK_NEAR(th13::ease_interp(0, 1.0f, 4.0f), 0.25f, 1e-6);
    CHECK_NEAR(th13::ease_interp(1, 1.0f, 4.0f), 0.0625f, 1e-6);
    CHECK_NEAR(th13::ease_interp(4, 1.0f, 4.0f), 0.4375f, 1e-6);
}

bool run_stage1(th13::StageSim& sim, std::vector<char>& output) {
    const char* out_path = TH13_TEST_PORT_DIR "/tests/ecl/build/stage1-test.jsonl";
    if (!sim.load("artifacts/reference/raw", out_path)) return false;
    if (!sim.run(3600)) return false;
    std::FILE* file = std::fopen(out_path, "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    output.resize(size_t(size < 0 ? 0 : size));
    const size_t got = output.empty() ? 1 : std::fread(output.data(), 1, output.size(), file);
    std::fclose(file);
    return got == output.size();
}

TEST(stage1_sim_contract_smoke) {
    th13::StageSim first, second;
    std::vector<char> a, b;
    CHECK(run_stage1(first, a));
    CHECK(run_stage1(second, b));
    CHECK(!a.empty());
    CHECK_EQ(a.size(), b.size());  // 同种子确定性
    CHECK(std::equal(a.begin(), a.end(), b.begin()));
    // 依赖清单装载核验：st01 ANIM 两项起（enemy.anm / st01enm.anm）。
    CHECK(first.anim_list.size() >= 2u);
    CHECK_EQ(first.anim_list[0], std::string("enemy.anm"));
    CHECK_EQ(first.anim_list[1], std::string("st01enm.anm"));
    // 首行契约：root 创建趟（frame 0）的第一个 enmCreate 事件。
    const char* kHead =
        "{\"frame\":0,\"seq\":0,\"type\":\"enemy_spawn\",\"enemy\":1,\"routine\":\"MapleEnemy\"";
    CHECK(a.size() > std::strlen(kHead));
    CHECK(std::memcmp(a.data(), kHead, std::strlen(kHead)) == 0);
    // 逐行校验：键序与类型枚举（合同 §4）。
    u64 lines = 0, spawns = 0, moves = 0, bullets = 0;
    u32 last_frame = 0, last_seq = 0;
    bool first_line = true;
    for (size_t i = 0; i < a.size();) {
        size_t end = i;
        while (end < a.size() && a[end] != '\n') ++end;
        std::string line(a.data() + i, end - i);
        CHECK(!line.empty());
        const auto prefix = [&](const char* keys) {
            return line.rfind(keys, 0) == 0;
        };
        // 公共头 {"frame":N,"seq":M,"type":"..."
        CHECK(prefix("{\"frame\":"));
        const size_t type_at = line.find(",\"type\":\"");
        CHECK(type_at != std::string::npos);
        const std::string kind = line.substr(type_at + 9, line.find('"', type_at + 9) - (type_at + 9));
        CHECK(kind == "enemy_spawn" || kind == "enemy_move" || kind == "bullet_spawn");
        if (kind == "enemy_spawn") {
            ++spawns;
            CHECK(prefix("{\"frame\":"));
            // 键序抽查（合同 §4.1）
            CHECK(line.find("\"routine\":") < line.find("\"x\":"));
            CHECK(line.find("\"x\":") < line.find("\"y\":"));
            CHECK(line.find("\"life\":") < line.find("\"parent\":"));
        } else if (kind == "enemy_move") {
            ++moves;
            CHECK(line.find("\"enemy\":") < line.find("\"op\":"));
            CHECK(line.find("\"op\":") < line.find("\"args\":["));
            CHECK(line.find("]}") == line.size() - 2);
        } else {
            ++bullets;
            CHECK(line.find("\"slot\":") < line.find("\"way\":"));
            CHECK(line.find("\"way\":") < line.find("\"idx\":"));
            CHECK(line.find("\"idx\":") < line.find("\"x\":"));
            CHECK(line.find("\"angle\":") < line.find("\"speed\":"));
            CHECK(line.find("\"sprite\":") < line.find("\"subtype\":"));
        }
        // (frame, seq) 严格递增：帧内 seq 连续、跨帧归零。
        const u32 frame = u32(std::strtoul(line.c_str() + 9, nullptr, 10));
        const size_t seq_at = line.find(",\"seq\":");
        const u32 seq = u32(std::strtoul(line.c_str() + seq_at + 7, nullptr, 10));
        if (first_line) {
            CHECK(seq == 0);
        } else if (frame == last_frame) {
            CHECK(seq == last_seq + 1);
        } else {
            CHECK(frame > last_frame);
            CHECK(seq == 0);
        }
        first_line = false;
        last_frame = frame;
        last_seq = seq;
        ++lines;
        i = end + 1;
    }
    CHECK_EQ(u64(first.event_count()), lines);
    CHECK(spawns > 80);      // MainFront 波次 + MBoss 族
    CHECK(moves > 1000);     // 含 TraceEnemyPos 逐帧 433
    CHECK(bullets > 500);    // GirlA01_at 族起
    CHECK(last_frame == 3599);  // 跑满 3600 帧
    std::printf("  stage1 冒烟：lines=%llu spawns=%llu moves=%llu bullets=%llu\n",
                (unsigned long long)lines, (unsigned long long)spawns, (unsigned long long)moves,
                (unsigned long long)bullets);
}

}  // namespace

int main() { return th13test::run_all("ecl-stage"); }
