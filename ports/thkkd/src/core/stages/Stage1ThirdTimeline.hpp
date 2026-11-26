// 一面part2的完整67D0E0..67D7C8，保留失败分配后的旧槽检查和转段计时写入。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
#include "core/stages/StagePatternContext.hpp"

namespace kaikeidou {
inline void stage1_third_segment(StageTimeline& timeline, EnemyPool& enemies, PatternParameters& parameters,
    StagePatternContext& context, const Enemy& enemy_prefix, StageServices& services, unsigned precision) {
    const stage_detail::Arithmetic arithmetic{precision};
    const auto frame = timeline.frame;
    const auto signed_frame = frame >> 31 ? std::int64_t(frame) - 0x100000000ll : std::int64_t(frame);
    const auto spawn = [&](std::uint32_t kind, std::uint32_t hp, std::uint32_t behavior, std::uint32_t items) -> Enemy* {
        const auto slot = allocate_enemy(enemies);
        if (slot == -1) return nullptr;
        auto& enemy = enemies.enemies[static_cast<std::size_t>(slot)];
        enemy.word(4, kind); enemy.word(0x18, hp); enemy.word(0x14, hp); enemy.word(8, behavior); enemy.word(0x1c, items);
        return &enemy;
    };
    const auto selected = [&](std::uint32_t a, std::uint32_t b, std::uint32_t c, std::uint32_t d) {
        const std::array<std::uint32_t, 6> values{{a, b, c, d, 0, 0}};
        return values[parameters.difficulty < 6 ? parameters.difficulty : 3];
    };
    if (signed_frame >= 0 && signed_frame < 500) {
        const auto cycle = frame % 100;
        if (cycle < 60 && frame % 10 == 0) if (auto* enemy = spawn(2, 40, 18, 2)) {
            enemy->quad(0x38, 0xc034000000000000ull); enemy->quad(0x40, 0x4051800000000000ull);
            enemy->quad(0x48, 0x4010000000000000ull); enemy->word(0xc, 0u - cycle / 2u);
            enemy->word(0x80, parameters.difficulty == 0 ? std::uint32_t(cycle < 21) :
                (parameters.difficulty == 1 ? std::uint32_t(cycle < 31) : 1u));
        }
        if (cycle > 39 && frame % 10 == 0) if (auto* enemy = spawn(1, 40, 18, 1)) {
            enemy->quad(0x38, 0x4079400000000000ull); enemy->quad(0x40, 0x405b800000000000ull);
            enemy->quad(0x48, 0xc010000000000000ull); enemy->word(0xc, 0u - (cycle - 40u) / 2u);
            enemy->word(0x80, parameters.difficulty == 0 ? std::uint32_t(cycle < 61) :
                (parameters.difficulty == 1 ? std::uint32_t(cycle < 71) : 1u));
        }
        if (cycle == 50) if (auto* enemy = spawn(0, 40, 19, 5)) {
            enemy->quad(0x38, 0xc034000000000000ull); enemy->quad(0x40, 0x403e000000000000ull);
            enemy->quad(0x48, 0x4010000000000000ull); enemy->word(0xc, 0); enemy->word(0x84, 0);
        }
    } else if (signed_frame >= 600 && signed_frame < 1100) {
        auto& side = context.integers_6_to_19[2];
        auto& kind = context.integers_6_to_19[3];
        if (frame == 700) { kind = 0; side = 0; }
        if (frame % 200 < 81 && frame % 8 == 0) {
            const auto slot = allocate_enemy(enemies);
            if (slot != -1) {
                auto& enemy = enemies.enemies[static_cast<std::size_t>(slot)];
                enemy.word(4, kind);
                const auto signed_kind = kind >> 31 ? std::int64_t(kind) - 0x100000000ll : std::int64_t(kind);
                enemy.word(0x18, signed_kind > 2 ? 80u : 40u); enemy.word(0x14, enemy.word(0x18));
                enemy.word(8, 20); enemy.word(0x1c, 1);
                enemy.quad(0x38, arithmetic.store(arithmetic.i(side != 0 ? 0xffffffecu : 404u)));
                enemy.quad(0x40, arithmetic.store(arithmetic.i((frame % 28u) * 9u + 30u)));
                enemy.quad(0x48, arithmetic.store(arithmetic.i(side != 0 ? 3u : 0xfffffffdu)));
                enemy.word(0xc, 0);
                ++kind; if (kind == 5) kind = 0;
                side = 1u - side;
            }
        }
    } else if (signed_frame >= 1200 && signed_frame <= 2300) {
        if (frame == 1200) {
            const auto slot = allocate_enemy(enemies);
            if (slot != -1) {
                auto& enemy = enemies.enemies[static_cast<std::size_t>(slot)];
                enemy.word(4, 5); enemy.word(0x18, 4000); enemy.word(0x14, 4000); enemy.word(8, 21); enemy.word(0x1c, 1);
                enemy.quad(0x38, 0x4068000000000000ull); enemy.quad(0x40, 0xc03e000000000000ull); enemy.word(0xc, 0);
                parameters.integers[0] = selected(65, 50, 22, 18);
                parameters.reals[0] = 0x4008000000000000ull; parameters.reals[1] = 0x3fed70a3d70a3d71ull;
                parameters.integers[1] = static_cast<std::uint32_t>(slot);
            }
        }
        const auto tracked = parameters.integers[1];
        const auto& enemy = tracked == 0xffffffffu ? enemy_prefix : enemies.enemies[tracked];
        if (enemy.word(0) == 0) { timeline.frame = 2400; parameters.game_frame = 2400; }
    } else if (frame == 2560) services.clear(100);
    else if (frame == 2600) {
        ++timeline.part; timeline.subphase = 0; timeline.frame = 0; parameters.game_frame = 0;
    }
}
}
