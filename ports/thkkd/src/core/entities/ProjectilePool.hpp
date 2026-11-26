// 恢复普通弹及第二组弹幕对象的分配、移除与逐帧调度，保持原槽位顺序。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include <stdexcept>

namespace kaikeidou {
using Projectile = ObjectRecord<0xf0>;
using SecondaryProjectile = ObjectRecord<0xc0>;
struct ProjectilePools {
    std::array<Projectile, 3000> bullets{};
    std::array<SecondaryProjectile, 32> secondary{};
    std::array<std::uint32_t, 4> cursors{};
    std::array<std::uint32_t, 100> sound_triggers{};
};
struct ProjectileAllocation {
    std::uint32_t kind = 0;
    std::int32_t group = 0;
    std::array<std::uint32_t, 3> color{};
    std::uint32_t blend = 0, opacity = 0, delayed = 0, sound = 0;
};

inline std::int32_t allocate_projectile(ProjectilePools& pools, const ProjectileAllocation& input) {
    if (input.group > 3 || input.sound >= pools.sound_triggers.size())
        throw std::out_of_range("Projectile allocation arguments outside recovered ranges");
    const auto group = input.group > 0 ? static_cast<std::uint32_t>(input.group) : 0u;
    const auto start = group ? (group - 1) * 1000u : 0u;
    const auto end = group ? group * 1000u : 3000u;
    const auto begin = pools.cursors[group];
    if (begin < start || begin > end) throw std::out_of_range("Projectile cursor outside group");
    const auto initialize = [&](std::uint32_t index) {
        auto& bullet = pools.bullets[index];
        bullet.word(0, input.delayed ? 2u : 1u);
        bullet.word(0xc, 0); bullet.word(0x10, 0); bullet.word(0x14, 0);
        bullet.word(4, input.kind);
        bullet.word(0x2c, input.blend); bullet.word(0x30, input.opacity);
        bullet.word(0x34, 0); bullet.word(0x3c, 0);
        bullet.quad(0x70, 0x3ff0000000000000ull);
        bullet.word(0x38, index);
        pools.cursors[group] = index + 1;
        if (pools.cursors[group] == end) {
            // 原分组分配路径在到达组尾时写的是总游标；保留这一行为。
            pools.cursors[0] = group ? start : 0u;
        }
        for (unsigned c = 0; c < 3; ++c) bullet.word(0x18 + c * 4, input.color[c]);
        pools.sound_triggers[input.sound] = 1;
        return static_cast<std::int32_t>(index);
    };
    for (auto i = begin; i < end; ++i) if (!pools.bullets[i].word(0)) return initialize(i);
    for (auto i = start; i < begin; ++i) if (!pools.bullets[i].word(0)) return initialize(i);
    return -1;
}

// 同时恢复返回寄存器结果，包含调用方未使用的偏移与商值。
inline std::int32_t remove_projectile(ProjectilePools& pools, std::size_t index, std::int32_t mode) {
    if (index >= pools.bullets.size()) throw std::out_of_range("Invalid projectile index");
    auto& bullet = pools.bullets[index];
    auto result = static_cast<std::int32_t>(index * 0xf0);
    if (mode > 99 || !bullet.word(0x34)) {
        result = mode / 100;
        if (mode % 100 == 0) {
            if (bullet.word(0) == 1 || bullet.word(0) == 2) {
                bullet.word(0, 3); bullet.word(0x10, 0); bullet.word(8, 0);
                if (!bullet.word(0x2c)) bullet.word(0x2c, 1);
            }
            result = static_cast<std::int32_t>(index * 0xf0);
        } else if (mode % 100 == 1) {
            if (index >= pools.secondary.size()) throw std::out_of_range("Invalid secondary projectile index");
            pools.secondary[index].word(0x48, 0xffffffffu);
            result = static_cast<std::int32_t>(index * 0xc0);
        }
    }
    return result;
}
inline std::int32_t clear_projectiles(ProjectilePools& pools, std::int32_t mode) {
    auto result = mode / 100;
    if (mode % 100 == 0) {
        for (std::size_t i = 0; i < pools.bullets.size(); ++i) {
            result = static_cast<std::int32_t>(i * 0xf0);
            if (pools.bullets[i].word(0) == 1 || pools.bullets[i].word(0) == 2)
                result = remove_projectile(pools, i, mode);
        }
    } else if (mode % 100 == 1) {
        for (std::size_t i = 0; i < pools.secondary.size(); ++i) result = remove_projectile(pools, i, mode);
    }
    return result;
}

template<class Dispatch> void update_projectiles(ProjectilePools& pools, Dispatch&& dispatch) {
    for (std::size_t i = 0; i < pools.bullets.size(); ++i) {
        auto& bullet = pools.bullets[i];
        const auto state = bullet.word(0);
        const auto kind = bullet.word(4);
        if (state == 1) {
            dispatch(bullet.word(8), static_cast<std::uint32_t>(i));
            bullet.word(0xc, bullet.word(0xc) + 1u);
        } else if (state == 2 || state == 3) {
            const auto time = bullet.word(0x10) + 1u;
            bullet.word(0x10, time);
            if (state == 2) {
                const auto boundary = kind == 22 ? 89u : (kind == 18 || kind == 26 || kind == 30 ? 29u : 9u);
                if (time == boundary) bullet.word(0, 1);
            } else if (time == (kind == 22 ? 90u : 10u)) bullet.word(0, 0);
        }
    }
    for (std::size_t i = 0; i < pools.secondary.size(); ++i) {
        auto& item = pools.secondary[i];
        if (item.word(0) == 1) {
            dispatch(item.word(4), static_cast<std::uint32_t>(i));
            item.word(8, item.word(8) + 1u);
        }
    }
}
}
