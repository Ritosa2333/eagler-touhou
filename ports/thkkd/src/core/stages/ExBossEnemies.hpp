// 原敌136/137及普通弹190，保留追踪、死亡回调和两类环形发射顺序。
#pragma once
#include "core/stages/ExBossEnemiesArithmetic.hpp"
#include "core/stages/ExBossEnemiesEmitter.hpp"
#include "core/stages/ExBossEnemiesRecords.hpp"
#include "core/stages/ExBossEnemyRecord.hpp"
#include "core/stages/ExBossEnemyServices.hpp"
#include "core/entities/EnemyDamage.hpp"
namespace kaikeidou {
inline void update_ex_boss_enemy(std::uint32_t behavior,std::uint32_t index,EnemyPool& enemies,
    ProjectilePools& pool,ObjectRecord<240>& player,PatternParameters& p,BossStageContext& context,
    const EnemyDamageContext& protection,Random& random,RandomPrecision precision,GameMath& math,ExBossEnemyServices& services){
    using namespace ex_boss_enemies_detail;
    const Arithmetic m{static_cast<unsigned>(precision)};
    EnemyRecord e{enemies,context,player,0xa87fe8u+index*248u};
    if(behavior==136){
        auto& center=pool.secondary[0];
        const auto dx=m.store(m.sub(m.q(center.quad(0x10)),m.q(e.quad(0x38))));
        const auto dy=m.store(m.sub(m.q(center.quad(0x18)),m.q(e.quad(0x40))));
        const auto angle=m.store(math.atan2(dy,dx));
        const auto squared=m.store(m.add(m.mul(m.q(dx),m.q(dx)),m.mul(m.q(dy),m.q(dy))));
        const auto distance=m.store(ex_boss_enemies_sqrt(squared,m.precision));
        const auto speed=m.store(m.mul(m.q(distance),m.q(p.reals[0])));
        auto value=math.cosine(angle);e.quad(0x38,m.store(m.add(m.mul(value,m.q(speed)),m.q(e.quad(0x38)))));
        value=math.sine(angle);e.quad(0x40,m.store(m.add(m.mul(value,m.q(speed)),m.q(e.quad(0x40)))));
        if(compare(m.add(m.q(center.quad(0x20)),m.c(0x6d1c18)),m.q(distance))<0){
            e.word(0x14,0);player.word(0x10,player.word(0x10)+10u);services.kill_enemy(index);
            p.integers[2]=140;p.reals[0]=constant(0x6b2620);p.reals[1]=0;
            center.quad(0x28,constant(0x6d18c0));pool.sound_triggers[18]=1;return;
        }
        if(signed_word(e.word(12))<30)return;
        e.word(0x80,e.word(0x80)-1u);if(e.word(0x80))return;
        p.integers[0]=0;p.integers[1]=120;
        ex_boss_enemies_emit(pool,{{9,0,{{0,0,255}},2,255,1,41},e.quad(0x38),e.quad(0x40),11,
            constant(0x6d27b0),constant(0x6d25f0),e.quad(0xa8),0,2},player,p,random,precision,math);
        p.integers[0]=0;p.integers[1]=40;
        ex_boss_enemies_emit(pool,{{9,0,{{255,0,0}},2,255,1,41},e.quad(0x38),e.quad(0x40),8,
            constant(0x6d22d8),constant(0x6d1ad0),e.quad(0xb0),0,2},player,p,random,precision,math);
        e.word(0x80,e.word(0x84));if(signed_word(e.word(0x84))>20)e.word(0x84,e.word(0x84)-4u);
        e.quad(0xa8,m.store(m.add(m.q(e.quad(0xa8)),m.c(0x6d2ee8))));
        e.quad(0xb0,m.store(m.add(m.q(e.quad(0xb0)),m.c(0x6d27f8))));return;
    }
    if(behavior==137){
        if(e.word(0x84)||remainder(e.word(12),120)!=0)return;
        auto angle=m.store(m.mul(random.real_number(constant(0x6d1c60),precision),m.c(0x6d1950)));
        const bool early=signed_word(protection.field_a7eaa4)>=30;
        for(unsigned j=0;j<(early?42u:72u);++j){
            const auto slot=allocate_projectile(pool,early?ProjectileAllocation{1,2,{{255,0,0}},2,255,0,41}:ProjectileAllocation{33,2,{{0,0,0}},0,255,0,41});
            if(slot!=-1){
                auto& b=pool.bullets[static_cast<std::uint32_t>(slot)];
                b.quad(0x40,e.quad(0x38));b.quad(0x48,e.quad(0x40));b.quad(0xa0,angle);
                b.quad(0xa8,constant(early?0x6d2130:0x6d1c90));
                b.quad(0xb0,m.store(m.mul(m.i(e.word(0x88)),m.c(0x6d2128))));b.word(8,190);
                if(early&&e.word(0x80)==1)ex_boss_enemies_recolor(pool,static_cast<std::uint32_t>(slot),0,0,255);
                else if(early&&e.word(0x80)==2)ex_boss_enemies_recolor(pool,static_cast<std::uint32_t>(slot),0,255,0);
            }
            angle=m.store(m.add(m.q(angle),m.c(early?0x6d2120:0x6d2118)));
        }
        e.word(0x88,0u-e.word(0x88));return;
    }
    throw std::out_of_range("EX boss enemy outside136..137");
}
inline void update_ex_boss_enemy_projectile(ProjectilePools& pool,std::uint32_t index,RandomPrecision precision,GameMath& math){
    using namespace ex_boss_enemies_detail;const Arithmetic m{static_cast<unsigned>(precision)};
    PoolRecord b{pool,0xa8e660u+index*240u};
    auto value=math.cosine(b.quad(0xa0));b.quad(0x40,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(b.quad(0x40)))));
    value=math.sine(b.quad(0xa0));b.quad(0x48,m.store(m.add(m.mul(value,m.q(b.quad(0xa8))),m.q(b.quad(0x48)))));
    const auto age=signed_word(b.word(12));
    if(age>=30&&age<=59){b.quad(0xa0,m.store(m.add(m.q(b.quad(0xa0)),m.q(b.quad(0xb0)))));b.quad(0xa8,m.store(m.mul(m.q(b.quad(0xa8)),m.c(0x6d1940))));}
    if(compare(m.c(0x6d1a68),m.q(b.quad(0x48)))<0||compare(m.c(0x6d1a80),m.q(b.quad(0x48)))>0||
       compare(m.c(0x6d1a78),m.q(b.quad(0x40)))<0||compare(m.c(0x6d1a80),m.q(b.quad(0x40)))>0)b.word(0,0);
}
}
