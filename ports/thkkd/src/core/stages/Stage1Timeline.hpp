// 一面首段 0x0067e2f0 的完整出敌时间轴，包含转入下一段的帧处理。
#pragma once
#include "core/entities/EnemyPool.hpp"
#include "core/math/BinaryFloat.hpp"
#include <cstring>

namespace kaikeidou {
struct StageTimeline {
    std::uint32_t frame = 0, part = 0, subphase = 0, difficulty = 0;
};
struct StageServices {
    virtual ~StageServices() = default;
    virtual void music(std::uint32_t track, std::uint32_t mode) = 0;
    virtual void title() = 0;
    virtual void clear(std::int32_t mode) = 0;
    virtual binary_float::Number cosine(std::uint64_t argument) = 0;
};
namespace stage_detail {
inline std::uint64_t bits(double value) { std::uint64_t result; std::memcpy(&result, &value, 8); return result; }
struct Arithmetic {
    unsigned precision;
    using Number = binary_float::Number;
    Number n(double value) const { return binary_float::binary64(bits(value)); }
    Number i(std::uint32_t value) const { return binary_float::integer(value); }
    Number add(Number left, Number right) const { return binary_float::add(left, right, precision); }
    Number sub(Number left, Number right) const { right.negative = !right.negative; return add(left, right); }
    Number mul(Number left, Number right) const { return binary_float::multiply(left, right, precision); }
    std::uint64_t store(Number number) const { return binary_float::to_binary64(number); }
};
}

inline void stage1_first_segment(StageTimeline& timeline, EnemyPool& pool, StageServices& services, unsigned precision) {
    using stage_detail::bits;
    const stage_detail::Arithmetic math{precision};
    const auto frame = timeline.frame;
    const auto signed_frame = frame >> 31 ? static_cast<std::int64_t>(frame) - 0x100000000ll : static_cast<std::int64_t>(frame);
    const auto spawn = [&](std::uint32_t kind, std::uint32_t hp, std::uint32_t behavior, std::uint32_t field_1c = 1) -> Enemy* {
        const auto index = allocate_enemy(pool);
        if (index < 0) return nullptr;
        auto& enemy = pool.enemies[static_cast<std::size_t>(index)];
        enemy.word(4, kind); enemy.word(0x18, hp); enemy.word(0x14, hp);
        enemy.word(8, behavior); enemy.word(0x1c, field_1c);
        return &enemy;
    };
    const auto curve = [](Enemy& enemy, std::uint64_t origin, double rate) {
        enemy.quad(0xa8, origin); enemy.quad(0xb0, 0); enemy.quad(0xb8, bits(rate));
    };
    if (frame == 1) services.music(1, 1);
    if (frame == 10 || frame == 30 || frame == 50 || frame == 70 || frame == 90 || frame == 110 || frame == 130) {
        const auto hp = frame == 130 ? 20u : 10u;
        const auto kind = frame == 130 ? 3u : (frame == 110 ? 2u : (frame == 90 ? 1u : 0u));
        for (unsigned side = 0; side < 2; ++side) if (auto* enemy = spawn(kind + (frame == 130 ? side : 0), hp, 1)) {
            enemy->word(0x10, side ? 1u : 2u);
            const auto displacement = math.mul(math.i(frame + 30u), math.n(.75));
            enemy->quad(0x38, math.store(side ? math.add(displacement, math.n(404.)) : math.sub(math.n(-20.), displacement)));
            enemy->quad(0x40, math.store(math.sub(math.n(-10.), math.mul(math.i(frame + 30u), math.n(.5)))));
            enemy->quad(0x48, 0); enemy->word(0x80, side);
        }
    } else if (frame == 260) services.title();
    else if (frame == 500 || frame == 560) {
        for (unsigned i = 0; i < 5; ++i) if (auto* enemy = spawn(i == 0 || i == 4 ? 1u : (i == 2 ? 2u : 0u), 40, 2)) {
            enemy->quad(0x38, math.store(math.i(frame == 500 ? i * 50u + 50u : 334u - i * 50u)));
            enemy->word(0x80, frame == 500 ? i + 4u : 7u - i);
            enemy->quad(0xa8, bits(frame == 500 ? -1.5 : 1.5));
            enemy->quad(0x40, bits(-20.)); enemy->quad(0x48, 0); enemy->quad(0x50, bits(5.));
        }
    } else if (signed_frame >= 630 && signed_frame <= 780) {
        const auto phase = signed_frame % 60;
        if (phase == 0 || phase == 30) if (auto* enemy = spawn(phase == 0 ? 0u : 1u, 60, 3, 2)) {
            enemy->quad(0x38, bits(192.)); enemy->quad(0x40, bits(-20.));
            curve(*enemy, enemy->quad(0x38), phase == 0 ? .03 : -.03);
        }
    } else if (signed_frame >= 870 && signed_frame <= 1020) {
        const auto phase = signed_frame % 60;
        if (phase == 0 || phase == 30) if (auto* enemy = spawn(2, 60, 4, 2)) {
            enemy->quad(0x38, bits(phase == 0 ? 334. : 50.)); enemy->quad(0x40, bits(-20.));
            curve(*enemy, enemy->quad(0x38), phase == 0 ? .03 : -.03);
        }
    } else if (frame == 1150) {
        for (unsigned side = 0; side < 2; ++side) if (auto* enemy = spawn(3u + side, 300, 5)) {
            enemy->quad(0x38, bits(side ? 80. : 304.)); enemy->quad(0x40, bits(-20.));
            curve(*enemy, enemy->quad(0x38), side ? -.03 : .03);
        }
    } else if (signed_frame >= 1200 && signed_frame <= 1380) {
        const auto phase = signed_frame % 40;
        if (phase == 0 || phase == 20) if (auto* enemy = spawn(phase == 0 ? 0u : 2u, 60, 6, 3)) {
            enemy->quad(0x38, bits(phase == 0 ? 434. : -50.)); enemy->quad(0x40, bits(-20.));
            curve(*enemy, enemy->quad(0x40), phase == 0 ? .03 : -.03);
        }
    } else if (signed_frame >= 1440 && signed_frame <= 1744) {
        if (signed_frame % 32 == 0) if (auto* enemy = spawn(0, 60, 7, 2)) {
            enemy->quad(0x38, bits(192.)); enemy->quad(0x40, bits(-20.));
            enemy->quad(0x48, math.store(math.mul(math.sub(math.n(796.), math.mul(math.i(frame), math.n(.5))), math.n(.015))));
        }
    } else if (frame == 1800 || frame == 1840 || frame == 1880) {
        if (auto* enemy = spawn(4, 100, 8)) {
            enemy->quad(0x38, bits(-20.)); enemy->quad(0x40, bits(40.)); enemy->quad(0x50, bits(-1.));
        }
        if (auto* enemy = spawn(3, 100, 9)) {
            enemy->quad(0x38, bits(404.)); enemy->quad(0x40, bits(40.)); enemy->quad(0x50, bits(-1.));
        }
    } else if (signed_frame >= 2250 && signed_frame <= 2700) {
        if (signed_frame % 19 == 12) if (auto* enemy = spawn(1, 50, 10)) {
            const auto argument = math.store(math.mul(math.i(frame), math.n(3.)));
            const auto cosine = services.cosine(argument);
            enemy->quad(0x38, math.store(math.add(math.mul(cosine, math.n(160.)), math.n(192.))));
            enemy->quad(0x40, bits(-20.));
        }
        if (signed_frame % 100 == 0) {
            for (unsigned side = 0; side < 2; ++side) if (auto* enemy = spawn(3u + side, 100, 11u + side)) {
                enemy->quad(0x38, bits(side ? 414. : -30.)); enemy->quad(0x40, bits(30.));
            }
        }
    } else if (frame == 3000) {
        if (auto* enemy = spawn(5, 80, 13)) {
            enemy->quad(0x38, bits(192.));
            enemy->quad(0x40, math.store(math.i(0u - 33u - (timeline.difficulty == 2 ? 10u : 0u))));
            const std::array<double, 6> speeds{{3.5, 4., 4.5, 5.05, 0., 0.}};
            enemy->quad(0x50, bits(speeds[timeline.difficulty < 6 ? timeline.difficulty : 3]));
        }
    } else if (frame == 3300) {
        services.clear(100);
        ++timeline.part; timeline.subphase = 0; timeline.frame = 0;
    }
}
}
