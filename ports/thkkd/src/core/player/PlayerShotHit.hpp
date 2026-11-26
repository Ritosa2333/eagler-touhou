// 恢复完整 0x0065b8e0、0x0065a030 和 0x00643fe0，保留命中后即时伤害与继续扫描顺序。
#pragma once
#include "core/player/PlayerShotFireContext.hpp"
#include "core/player/PlayerShotPools.hpp"
#include "core/player/PlayerShotUpdate.hpp"
#include "core/player/PlayerCollision.hpp"
#include "core/math/FiniteSqrt.hpp"
#include "core/math/FiniteDivide.hpp"
#include <cmath>

namespace kaikeidou {
struct PlayerShotHitContext { PlayerShotFireContext fire; std::uint32_t mode_a7ea98 = 0; };
struct PlayerShotHitServices {
    virtual ~PlayerShotHitServices() = default;
    virtual void damage(std::uint32_t slot, std::uint32_t amount) = 0;
};
namespace player_shot_hit_detail {
struct Arithmetic {
    using Number = binary_float::Number;
    unsigned precision;
    Number q(std::uint64_t value) const { return binary_float::binary64(value); }
    Number n(double value) const { return q(crt_sse2::bits(value)); }
    Number i(std::uint32_t value) const { return binary_float::integer(value); }
    Number add(Number a, Number b) const { return binary_float::add(a,b,precision); }
    Number sub(Number a, Number b) const { b.negative=!b.negative;return add(a,b); }
    Number mul(Number a, Number b) const { return binary_float::multiply(a,b,precision); }
    Number div(Number a, Number b) const { return binary_float::divide(a,b,precision); }
    std::uint64_t store(Number value) const { return binary_float::to_binary64(value); }
    bool le(Number a, Number b) const {
        const bool negative_a=a.negative&&a.mantissa,negative_b=b.negative&&b.mantissa;
        if(negative_a!=negative_b)return negative_a;
        const auto magnitude=[](Number x,Number y) {
            if(!x.mantissa||!y.mantissa)return x.mantissa?1:(y.mantissa?-1:0);
            if(x.exponent!=y.exponent)return x.exponent<y.exponent?-1:1;
            return x.mantissa==y.mantissa?0:(x.mantissa<y.mantissa?-1:1);
        };
        const auto compared=magnitude(a,b);return negative_a?compared>=0:compared<=0;
    }
    std::uint32_t rounded_integer(Number value,bool upward) const {
        const auto stored=crt_sse2::real(store(value));
        const auto rounded=upward?std::ceil(stored):std::floor(stored);
        if(rounded<-2147483648.||rounded>2147483647.)throw std::out_of_range("Player shot integer conversion outside declared range");
        return static_cast<std::uint32_t>(static_cast<std::int64_t>(rounded));
    }
};
inline std::int64_t signed_word(std::uint32_t value) { return value>>31?static_cast<std::int64_t>(value)-0x100000000ll:value; }
inline std::uint32_t count_visible(const EnemyPool& enemies) {
    std::uint32_t count=0;
    for(const auto& enemy:enemies.enemies)if(enemy.word(0)==1&&player_shot_target_in_view(enemy))++count;
    return count;
}
template<class Target> inline bool rectangle(const ObjectRecord<56>& shot,const Target& enemy,const std::array<std::uint32_t,4>& extents,unsigned precision) {
    const Arithmetic m{precision};
    const auto radius=[&](){return m.i(enemy_contact_radius(enemy.word(4)));};
    const auto left=m.store(m.sub(m.q(shot.quad(0x10)),m.i(extents[0])));
    if(!m.le(m.q(left),m.add(radius(),m.q(enemy.quad(0x38)))))return false;
    const auto right=m.store(m.add(m.i(extents[1]),m.q(shot.quad(0x10))));
    if(!m.le(m.sub(m.q(enemy.quad(0x38)),radius()),m.q(right)))return false;
    const auto top=m.store(m.sub(m.q(shot.quad(0x18)),m.i(extents[2])));
    if(!m.le(m.q(top),m.add(radius(),m.q(enemy.quad(0x40)))))return false;
    const auto bottom=m.store(m.add(m.i(extents[3]),m.q(shot.quad(0x18))));
    return m.le(m.sub(m.q(enemy.quad(0x40)),radius()),m.q(bottom));
}
inline std::uint64_t distance(const Arithmetic& m,std::uint64_t ax,std::uint64_t ay,std::uint64_t bx,std::uint64_t by) {
    const auto dx=m.q(m.store(m.sub(m.q(ax),m.q(bx))));
    const auto dy=m.q(m.store(m.sub(m.q(ay),m.q(by))));
    return m.store(finite_square_root(m.store(m.add(m.mul(dx,dx),m.mul(dy,dy))),m.precision));
}
inline std::uint32_t input(PlayerShotHitContext& context,PlayerShotFireInput& source,std::uint32_t binding) {
    auto& fire=context.fire;
    if(fire.replay_variant&&fire.replay_mode!=0)return (fire.replay_buttons>>(binding==11?7u:binding))&1u;
    return source.read_counter(binding);
}
}
inline void hit_player_shots(PlayerShotPools& pools,const OptionsPool& options,EnemyPool& enemies,const ObjectRecord<240>& player,
    PlayerShotHitContext& context,PlayerShotFireInput& source,PlayerShotHitServices& services,RandomPrecision precision,const Enemy* enemy_prefix=nullptr) {
    using namespace player_shot_hit_detail;
    const auto working=static_cast<unsigned>(precision);const Arithmetic m{working};
    const auto eligible=[](const auto& enemy){return enemy.word(0)==1&&player_shot_target_in_view(enemy);};
    const auto radius=[&](const Enemy& enemy){return m.i(enemy_contact_radius(enemy.word(4)));};
    for(auto& shot:pools.common)if(shot.word(0)==1) {
        for(std::uint32_t slot=0;slot<80;++slot) {
            const auto& enemy=enemies.enemies[slot];if(!eligible(enemy))continue;
            if(!m.le(m.sub(m.q(shot.quad(0x10)),m.n(32.)),m.add(radius(enemy),m.q(enemy.quad(0x40)))))continue;
            if(!m.le(m.sub(m.q(shot.quad(8)),m.n(6.)),m.add(radius(enemy),m.q(enemy.quad(0x38)))))continue;
            const auto right=m.store(m.add(m.q(shot.quad(8)),m.n(6.)));
            if(!m.le(m.sub(m.q(enemy.quad(0x38)),radius(enemy)),m.q(right)))continue;
            const auto bottom=m.store(m.add(m.q(shot.quad(0x10)),m.n(32.)));
            if(!m.le(m.sub(m.q(enemy.quad(0x40)),radius(enemy)),m.q(bottom)))continue;
            shot.word(0,0);services.damage(slot,shot.word(4));break;
        }
    }
    const auto selector=context.fire.options.character*10u+context.fire.options.weapon;
    const auto entity_shots=[&](std::size_t count,std::array<std::uint32_t,4> extents) {
        for(std::size_t index=0;index<count;++index) {
            auto& shot=pools.weapon[index];if(shot.word(0)!=1)continue;
            for(std::uint32_t slot=0;slot<80;++slot) {
                if(!eligible(enemies.enemies[slot])||!rectangle(shot,enemies.enemies[slot],extents,working))continue;
                shot.word(0,0);services.damage(slot,shot.word(4));break;
            }
        }
    };
    const auto firing=[&](){return input(context,source,context.fire.fire_binding)!=0;};
    const auto option_count=[&](){return signed_word(m.rounded_integer(m.mul(m.i(player.word(8)),m.n(.01)),false));};
    switch(selector) {
    case 0: entity_shots(40,{{4,4,32,32}});break;
    case 1:
        for(std::size_t group=0;group<4;++group) {
            const auto slot=options.options[group].word(0x48);
            const PlayerShotEnemyView target(enemies,slot,enemy_prefix,&player);
            if(!eligible(target))continue;
            for(std::size_t index=0;index<20;++index) {
                auto& shot=pools.weapon[group*20+index];
                if(shot.word(0)!=1||!rectangle(shot,target,{{2,2,2,2}},working))continue;
                shot.word(0,0);services.damage(slot,shot.word(4));
            }
        }
        break;
    case 2: entity_shots(40,{{16,16,16,16}});break;
    case 10: case 12: entity_shots(40,{{8,8,8,8}});break;
    case 11:
        if(firing()&&context.fire.fire_lock==0)for(std::int64_t index=0,count=option_count();index<count;++index) {
            const auto& option=options.options.at(static_cast<std::size_t>(index));
            for(std::uint32_t slot=0;slot<80;++slot) {
                const auto& enemy=enemies.enemies[slot];if(!eligible(enemy))continue;
                if(!m.le(m.sub(m.q(option.quad(8)),m.n(2.)),m.add(radius(enemy),m.q(enemy.quad(0x38)))))continue;
                const auto right=m.store(m.add(m.q(option.quad(8)),m.n(2.)));
                if(!m.le(m.sub(m.q(enemy.quad(0x38)),radius(enemy)),m.q(right)))continue;
                const auto bottom=m.store(m.add(m.q(option.quad(0x10)),m.n(8.)));
                if(m.le(m.sub(m.q(enemy.quad(0x40)),radius(enemy)),m.q(bottom)))services.damage(slot,2);
            }
        }
        break;
    case 20:
        if(firing()&&input(context,source,context.fire.slow_binding)==0&&signed_word(player.word(8))>=100&&context.fire.fire_lock==0) {
            const auto divisor=m.add(m.mul(m.i(count_visible(enemies)),m.n(.2)),m.n(1.));
            auto amount=m.rounded_integer(m.div(m.q(options.options[0].quad(0x50)),divisor),true);
            if(player.word(0x2c)!=0)amount=m.rounded_integer(m.mul(m.i(amount),m.n(.5)),true);
            for(std::size_t index=0;index<3;++index)for(std::uint32_t slot=0;slot<80;++slot) {
                const auto& enemy=enemies.enemies[slot];if(!eligible(enemy))continue;
                const auto& option=options.options[index];
                if(circles_overlap(option.quad(8),option.quad(0x10),enemy.quad(0x38),enemy.quad(0x40),crt_sse2::bits(60.),m.store(radius(enemy)),working))services.damage(slot,amount);
            }
        }
        entity_shots(80,{{8,8,8,8}});break;
    case 21:
        if(firing()&&context.fire.fire_lock==0)for(std::int64_t index=0,count=option_count();index<count;++index) {
            const auto& option=options.options.at(static_cast<std::size_t>(index));
            const bool alternate=context.mode_a7ea98!=0;
            const auto divisor=m.add(m.mul(m.i(count_visible(enemies)),m.n(alternate?.2:.1)),m.n(1.));
            const auto amount=m.rounded_integer(m.add(m.div(m.q(option.quad(alternate?0x58:0x50)),divisor),m.n(alternate?.1:1.)),true);
            for(std::uint32_t slot=0;slot<80;++slot) {
                const auto& enemy=enemies.enemies[slot];if(!eligible(enemy))continue;
                const auto range=m.q(distance(m,player.quad(0x90),player.quad(0x98),enemy.quad(0x38),enemy.quad(0x40)));
                if(!m.le(m.sub(m.q(option.quad(0x40)),radius(enemy)),range))continue;
                if(m.le(range,m.add(radius(enemy),m.q(option.quad(0x40)))))services.damage(slot,amount);
            }
        }
        break;
    case 22:
        for(std::size_t index=0;index<16;++index) {
            const auto& shot=pools.weapon[index];if(shot.word(0)!=1)continue;
            for(std::uint32_t slot=0;slot<80;++slot) {
                const auto& enemy=enemies.enemies[slot];if(!eligible(enemy))continue;
                const auto range=m.q(distance(m,shot.quad(0x10),shot.quad(0x18),enemy.quad(0x38),enemy.quad(0x40)));
                const auto size=m.q(m.store(m.mul(m.q(shot.quad(0x30)),m.n(50.))));
                if(m.le(range,m.add(radius(enemy),size)))services.damage(slot,shot.word(4));
            }
        }
        break;
    default:break;
    }
}
}
