// 恢复 0x00660860..0x00660b10 的完整角色状态机；外部作用按原顺序交给服务接口。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include "core/math/Random.hpp"

namespace kaikeidou {
struct LifecycleContext {
    std::uint32_t character = 0, difficulty = 0, practice = 0;
    std::uint32_t stage = 0, part = 0, subphase = 0, boss_state = 0;
    std::uint32_t hit_flag = 0, scene = 0, scene_phase = 0, scene_selection = 0;
    std::uint8_t flash = 0;
    std::uint64_t secondary_first_field_28 = 0;
};
struct LifecycleServices {
    virtual ~LifecycleServices() = default;
    virtual void drop_items() = 0;
    virtual std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) = 0;
    virtual void effect(std::uint32_t kind, std::uint32_t color, std::uint64_t x, std::uint64_t y) = 0;
    virtual void reset_options(std::uint32_t first, std::uint32_t last) = 0;
    virtual void reset_speed(std::uint64_t scale) = 0;
    virtual void original_action(std::uint32_t action, std::uint32_t mode) = 0;
};
namespace lifecycle_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
}
}
inline void update_player_lifecycle(ObjectRecord<240>& player, LifecycleContext& context, Random& random,
    RandomPrecision precision, LifecycleServices& services) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto signed_value = lifecycle_detail::signed_word;
    if (player.word(0) == 1) {
        context.flash = 80;
        random.integer_bits(1, precision, true);
        const auto threshold = context.character == 1 || context.character == 2 ? 8 : 16;
        if (signed_value(player.word(4)) > threshold) {
            player.word(0, 2); player.word(4, 0); player.word(0x28, 310);
            context.hit_flag = 1; context.flash = 255;
            if (context.difficulty == 5) {
                context.scene = 5; context.scene_phase = 0; context.scene_selection = 0;
            } else {
                const auto lives = player.word(0x14);
                player.word(0x14, lives - 1u);
                if (lives == 0) {
                    context.scene = (context.practice != 0 ? 1u : 0u) + 2u;
                    services.original_action(17, 1);
                    context.scene_phase = 0; context.scene_selection = 0;
                } else {
                    services.drop_items();
                    const auto effect_y = player.quad(0x98);
                    const auto effect_x = player.quad(0x90);
                    const auto color = services.color(255, 255, 255);
                    services.effect(11, color, effect_x, effect_y);
                    player.word(8, 0);
                    services.reset_options(0, 0);
                    if (signed_value(player.word(0x18)) < 2) player.word(0x18, 2);
                    player.quad(0x90, 0x4068000000000000ull);
                    player.quad(0x98, 0x407e000000000000ull);
                    if (context.stage == 5 && context.part == 3 && context.subphase == 11 && context.boss_state == 2)
                        context.secondary_first_field_28 = 0x4049000000000000ull;
                }
            }
        }
    }
    if (player.word(0) == 2) {
        random.integer_bits(1, precision, true);
        if (signed_value(player.word(4)) < 40) {
            auto decrease = binary64(0x3ff8000000000000ull);
            decrease.negative = true;
            player.quad(0x98, to_binary64(add(binary64(player.quad(0x98)), decrease, working)));
        } else if (signed_value(player.word(4)) >= 280) {
            player.word(0, 0); player.word(4, 0);
        }
    }
    if (signed_value(player.word(0x28)) > 0) {
        player.word(0x28, player.word(0x28) - 1u);
        if (signed_value(player.word(0x2c)) > 0) {
            player.word(0x2c, player.word(0x2c) - 1u);
            if (player.word(0x2c) == 0) {
                context.flash = 255;
                services.reset_speed(0x3ff0000000000000ull);
            }
        }
    }
    player.word(4, player.word(4) + 1u);
    player.word(0xa4, player.word(0xa4) + 1u);
    const auto phase = multiply(integer(player.word(4)), binary64(0x3fb999999999999aull), working);
    const auto converted = truncate(binary64(to_binary64(phase)));
    player.word(0x88, static_cast<std::uint32_t>(signed_value(converted) % 4));
    if (player.word(0xe8) != 0) {
        player.word(0xe8, player.word(0xe8) - 1u);
        if (player.word(0xe8) == 0 && context.practice != 0) {
            context.scene = 4; context.scene_phase = 0; context.scene_selection = 0;
        }
    }
}
}
