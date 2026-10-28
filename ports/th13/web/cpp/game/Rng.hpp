// th13 随机数发生器——MSVC CRT rand() 行为镜像。
// 依据：th13.exe .text 中唯一一处 LCG 常量对（0x47BBAE/0x47BBB3，imul/add 相邻），
// 即静态链接的 CRT rand()；游戏侧调用路径与状态地址由 Ghidra xref 锁定后
// 在 AddressMap.hpp 登记，并由 Unicorn oracle 逐值验证。
#pragma once

#include <cstdint>

namespace th13 {

class Rng {
public:
    explicit Rng(uint32_t seed = 1)
        : state_(seed) {}

    void reseed(uint32_t seed) { state_ = seed; }

    // MSVC rand()：state = state * 0x343FD + 0x269EC3；返回 (state >> 16) & 0x7FFF
    int next() {
        state_ = state_ * kMultiplier + kIncrement;
        return static_cast<int>((state_ >> 16) & 0x7FFF);
    }

    // 供 oracle 比对：不推进状态的观察
    uint32_t state() const { return state_; }

    static constexpr uint32_t kMultiplier = 0x343FDu;
    static constexpr uint32_t kIncrement = 0x269EC3u;

private:
    uint32_t state_;
};

// th13 游戏侧随机数发生器（0x472c90，容器内 capstone 反汇编逐指令还原）：
// 与上方 MSVC CRT rand() 是两个独立发生器。每步对 16 位种子做
//   A = u16((seed ^ 0x9630) - 0x6553)
//   B = u16((u16((A >> 14) + A * 4) ^ 0x9630) - 0x6553)
//   seed = u16((B >> 14) + B * 4)
// 返回 (A << 16) | B；伴随调用计数 +2（原版 [rng+4]，重放对齐用）。
// 状态对象归属与种子初始化（敌人管理器侧）待 oracle 对照定域，此处先还原映射本身。
// 浮点化见 0x472e10：以无符号解释乘 2^-31 再减 1，得 [-1, 1) 的对称单位随机。
class GameRandom {
public:
    explicit GameRandom(uint16_t seed = 0) : seed_(seed) {}

    void reseed(uint16_t seed) { seed_ = seed; }

    // 0x449070:40-42 恢复语义：种子 ← stage 头 +0x02 戳，并把本对象的调用
    // 计数清零。原版只对 0x4dc324（game RNG）清计数（_DAT_004dc328=0），
    // 0x4dc31c（ANM/畸变侧）只写种子不清计数——调用方据此分别选用两个入口。
    void reseed_restore(uint16_t seed) {
        seed_ = seed;
        index_ = 0;
    }

    uint32_t next() {
        const uint16_t a = static_cast<uint16_t>((seed_ ^ 0x9630u) - 0x6553u);
        const uint16_t step = static_cast<uint16_t>((a >> 14) + a * 4);
        const uint16_t b = static_cast<uint16_t>((step ^ 0x9630u) - 0x6553u);
        seed_ = static_cast<uint16_t>((b >> 14) + b * 4);
        index_ += 2;
        return (static_cast<uint32_t>(a) << 16) | b;
    }

    // 0x472c20：单步 16 位变体（rand16）。与 next() 共用状态；计数 +1；
    // 返回旋转后的新种子。moveRand（412）象限掷币用（% 3 判定）。
    uint16_t step16() {
        const uint16_t a = static_cast<uint16_t>((seed_ ^ 0x9630u) - 0x6553u);
        seed_ = static_cast<uint16_t>((a >> 14) + a * 4);
        index_ += 1;
        return seed_;
    }

    float signed_unit() {  // 0x472e10 的浮点化（fild→×2^-31→−1.0）
        const double raw = static_cast<double>(next());
        return static_cast<float>(raw * (1.0 / 2147483648.0) - 1.0);
    }

    // ANM 引擎沿 th15 命名：next32 即 next；unit 为 [0,1) 浮点化（×2^-32）。
    uint32_t next32() { return next(); }
    float unit() {
        const double raw = static_cast<double>(next());
        return static_cast<float>(raw * (1.0 / 4294967296.0));
    }

    uint32_t index() const { return index_; }
    uint16_t seed() const { return seed_; }

private:
    uint16_t seed_;
    uint32_t index_ = 0;
};

}  // namespace th13
