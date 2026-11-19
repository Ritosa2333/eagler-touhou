// m1.unit_native_harness —— M1/T1 基建探针（tests/multiplayer/run-native.sh
// 全链：native-runner.mjs 按 workflow-cases.json 映射编译 game 全量 + e2e 同款
// sdl 8 文件 + tests/e2e/native/native_stub.cpp + 本文件，注入
// TH13_NATIVE_ASSET_ROOT/REPO_ROOT/FONT 后真实运行）。
//
// 消费真实生产符号（M1-plan T1 指定面）：
//   - web/cpp/game/Rng.hpp GameRandom（0x472c90 镜像）：reseed/reseed_restore
//     后 next/step16/next32 序列与 index() 语义；
//   - web/cpp/game/GameInput.hpp GameInput::update（GameInput.cpp calculate
//     链接自 game 源集）：held→previous/pressed/released/long_held/repeated 边沿。
// 参照序列由 Rng.hpp 头注释的文档化映射（decomp 0x472c90）独立内联实现，
// 断言生产符号输出与其一致——不是自比较（生产侧走真实链接符号）。
#include "game/GameInput.hpp"
#include "game/Rng.hpp"

#include "../../harness/test_framework.hpp"

#include <cstdint>

namespace {

// 文档化映射的独立参照（Rng.hpp:46-110 注释；0x472c90 / 0x472c20）。
struct RefGameRandom {
    uint16_t seed = 0;
    uint32_t index = 0;

    explicit RefGameRandom(uint16_t initial) : seed(initial) {}
    static uint16_t twist(uint16_t value) {
        return static_cast<uint16_t>((value >> 14) + value * 4);
    }
    uint32_t next() {
        const uint16_t a = static_cast<uint16_t>((seed ^ 0x9630u) - 0x6553u);
        const uint16_t b = static_cast<uint16_t>((twist(a) ^ 0x9630u) - 0x6553u);
        seed = twist(b);
        index += 2;
        return (static_cast<uint32_t>(a) << 16) | b;
    }
    uint16_t step16() {
        const uint16_t a = static_cast<uint16_t>((seed ^ 0x9630u) - 0x6553u);
        seed = twist(a);
        index += 1;
        return seed;
    }
};

}  // namespace

TEST(native_rng_reseed_restore) {
    // reseed_restore(seed)：种子 ← seed 且调用计数清零（0x449070:40-42 恢复语义），
    // 之后 next() 序列与同种子参照全等、index 恰 +2/抽。
    for (const uint16_t seed : {uint16_t{0}, uint16_t{1}, uint16_t{0x1234}, uint16_t{0xFFFF}}) {
        th13::GameRandom game;
        game.reseed_restore(seed);
        RefGameRandom reference(seed);
        for (int i = 0; i < 16; i++) {
            CHECK_EQ(game.next(), reference.next());
            CHECK_EQ(game.index(), static_cast<uint32_t>(2 * (i + 1)));
        }
        // 重复 reseed_restore：计数归零、序列同种子重现（可恢复语义本体）。
        game.reseed_restore(seed);
        CHECK_EQ(game.index(), 0u);
        RefGameRandom replay(seed);
        for (int i = 0; i < 4; i++) CHECK_EQ(game.next(), replay.next());
        CHECK_EQ(game.index(), 8u);
    }
    // reseed(seed) 只写种子不清计数（原版仅 game RNG 恢复清 _DAT_004dc328；
    // reseed 不清 index），序列从新种子延续、index 继续累计。
    th13::GameRandom keep;
    keep.reseed_restore(7);
    RefGameRandom warm(7);
    for (int i = 0; i < 3; i++) keep.next();
    CHECK_EQ(keep.index(), 6u);
    keep.reseed(9);
    CHECK_EQ(keep.index(), 6u);
    RefGameRandom fresh(9);
    CHECK_EQ(keep.next(), fresh.next());
    CHECK_EQ(keep.index(), 8u);
}

TEST(native_rng_step16_next32) {
    // step16()：单步 16 位变体（0x472c20），与 next 共用状态、计数 +1、
    // 返回旋转后的新种子。
    th13::GameRandom game;
    game.reseed_restore(0xBEEF);
    RefGameRandom reference(0xBEEF);
    for (int i = 0; i < 16; i++) {
        CHECK_EQ(game.step16(), reference.step16());
        CHECK_EQ(game.index(), static_cast<uint32_t>(i + 1));
    }
    // 共用状态负控：step16 之后同实例换 next() 续算，必须与「参照侧终态种子
    // 起步的新参照」逐抽全等（Rng.hpp 文档：step16 与 next() 共用状态——
    // 若 step16 私化独立状态或漏转种子，此对照即失败）；计数按
    // 3×step16(+1) + 1×next(+2) 累计。
    th13::GameRandom mixed;
    mixed.reseed_restore(0x0F0D);
    RefGameRandom mixed_reference(0x0F0D);
    for (int i = 0; i < 3; i++) CHECK_EQ(mixed.step16(), mixed_reference.step16());
    RefGameRandom from_state(mixed_reference.seed);
    CHECK_EQ(mixed.next(), from_state.next());
    CHECK_EQ(mixed.index(), 5u);
    // next32() 即 next（th15 ANM 命名沿）：双实例同种子逐抽全等、index 同步。
    th13::GameRandom via_next32;
    via_next32.reseed_restore(0x0042);
    th13::GameRandom via_next;
    via_next.reseed_restore(0x0042);
    for (int i = 0; i < 16; i++) {
        CHECK_EQ(via_next32.next32(), via_next.next());
        CHECK_EQ(via_next32.index(), via_next.index());
    }
    CHECK_EQ(via_next32.index(), 32u);
}

TEST(native_game_input_edges) {
    // GameInput::update(current)：previous=held; held=current; calculate()。
    // pressed=新增位、released=消失位；long_held=连续 ≥8 帧、repeated=连续
    // ≥26 帧且此后每 +8 帧重发（GameInput.cpp calculate）。
    th13::GameInput input;
    input.update(0);
    CHECK_EQ(input.held, 0u);
    CHECK_EQ(input.previous, 0u);
    CHECK_EQ(input.pressed, 0u);
    CHECK_EQ(input.released, 0u);

    input.update(0x01);
    CHECK_EQ(input.previous, 0u);
    CHECK_EQ(input.held, 0x01u);
    CHECK_EQ(input.pressed, 0x01u);
    CHECK_EQ(input.released, 0u);

    input.update(0x01);
    CHECK_EQ(input.pressed, 0u);
    CHECK_EQ(input.released, 0u);
    CHECK_EQ(input.previous, 0x01u);
    CHECK_EQ(input.held, 0x01u);

    // 新增位才进 pressed（0x01→0x03 只按 0x02）。
    input.update(0x03);
    CHECK_EQ(input.pressed, 0x02u);
    CHECK_EQ(input.previous, 0x01u);
    CHECK_EQ(input.held, 0x03u);

    // 连续保持计龄：bit0 第 8 帧 long_held 置位（bit1 第 6 帧未到）。
    for (int i = 0; i < 5; i++) input.update(0x03);
    CHECK_EQ(input.long_held, 0x01u);
    CHECK_EQ(input.repeated, 0u);
    // 再 2 帧：bit1 也到 8 → long_held=0x03；repeated 仍 0（<26）。
    input.update(0x03);
    input.update(0x03);
    CHECK_EQ(input.long_held, 0x03u);
    CHECK_EQ(input.repeated, 0u);
    // bit0 先到 26（f26）：repeated=0x01 且龄 -8；bit1（晚 2 帧起计）龄 24 未到。
    for (int i = 0; i < 16; i++) input.update(0x03);
    CHECK_EQ(input.repeated, 0x01u);
    input.update(0x03);
    CHECK_EQ(input.repeated, 0u);  // bit0 龄 19、bit1 龄 25，均 <26
    // 再 1 帧：bit1 到 26 → repeated=0x02（bit0 龄 20 不重发；repeat 窗 8 帧
    // 使两 bit 相隔 2 帧起计后永不共帧重发）。
    input.update(0x03);
    CHECK_EQ(input.repeated, 0x02u);
    // 双龄回落 <26：repeated 清零，long_held 保持连续置位。
    input.update(0x03);
    CHECK_EQ(input.repeated, 0u);
    CHECK_EQ(input.long_held, 0x03u);

    // 释放沿：held 全落 → released=0x03、pressed=0、long_held 随龄清零。
    input.update(0);
    CHECK_EQ(input.held, 0u);
    CHECK_EQ(input.previous, 0x03u);
    CHECK_EQ(input.released, 0x03u);
    CHECK_EQ(input.pressed, 0u);
    CHECK_EQ(input.long_held, 0u);

    // 释放清龄负控（重按同一位）：松开后再按 bit0，7 帧内不得 long_held/
    // repeated，重按后恰第 8 帧重新置位——若 calculate 的 else 分支漏清
    // repeat_age，旧龄（释放前已达 21）会在重按首帧越阈误触发长按/重复。
    input.update(0x01);
    CHECK_EQ(input.pressed, 0x01u);
    CHECK_EQ(input.released, 0u);
    for (int i = 0; i < 6; i++) input.update(0x01);
    CHECK_EQ(input.long_held, 0u);
    CHECK_EQ(input.repeated, 0u);
    input.update(0x01);
    CHECK_EQ(input.long_held, 0x01u);

    // 同时沿：held 0x01→0x02 同帧一释一按（changed=0x03），released 与
    // pressed 各取 changed 的半边——两掩码混用（如 released=changed&held）
    // 即失败；换位后 long_held 从零重计（旧位清龄、新位龄 1）。
    input.update(0x02);
    CHECK_EQ(input.released, 0x01u);
    CHECK_EQ(input.pressed, 0x02u);
    CHECK_EQ(input.held, 0x02u);
    CHECK_EQ(input.long_held, 0u);
}

int main() { return th13test::run_all("m1.unit_native_harness"); }
