// 二面part2完整67B790..67BF50，依次保留分配、实时帧重读、随机与清弹写入。
#pragma once
#include "core/stages/Stage1Timeline.hpp"
#include "core/entities/ProjectilePattern.hpp"
namespace kaikeidou {
inline void stage2_third_segment(StageTimeline& timeline,EnemyPool& enemies,ProjectilePools& projectiles,
    PatternParameters& parameters,Random& random,RandomPrecision precision){
    const stage_detail::Arithmetic m{static_cast<unsigned>(precision)};
    const auto signed_frame=[&](){return timeline.frame>>31?std::int64_t(timeline.frame)-0x100000000ll:std::int64_t(timeline.frame);};
    const auto spawn=[&](std::uint32_t kind,std::uint32_t hp,std::uint32_t behavior,std::uint32_t items)->Enemy*{
        const auto slot=allocate_enemy(enemies);if(slot==-1)return nullptr;auto& enemy=enemies.enemies[static_cast<std::size_t>(slot)];
        enemy.word(4,kind);enemy.word(0x18,hp);enemy.word(0x14,hp);enemy.word(8,behavior);enemy.word(0x1c,items);return &enemy;
    };
    const auto angle=[&](){return m.store(m.mul(random.real_number(0x40b88b0000000000ull,precision),binary_float::binary64(0x3f50624dd2f1a9fcull)));};
    const auto select=[&](std::array<std::uint64_t,6> values){return values[parameters.difficulty<6?parameters.difficulty:3];};
    if(signed_frame()>=60&&signed_frame()<=130){
        if(signed_frame()%11==0){
            if(auto* enemy=spawn(0,60,30,1)){enemy->quad(0x38,0x4079400000000000ull);enemy->quad(0x40,m.store(m.i(static_cast<std::uint32_t>((signed_frame()%7)*28+10))));enemy->quad(0xa8,0xc010000000000000ull);enemy->quad(0xb0,angle());}
            if(auto* enemy=spawn(1,60,31,1)){enemy->quad(0x38,0xc034000000000000ull);enemy->quad(0x40,m.store(m.i(static_cast<std::uint32_t>((signed_frame()%7)*28+10))));enemy->quad(0xa8,0x4010000000000000ull);enemy->quad(0xb0,angle());}
        }
    }else if(signed_frame()>=380&&signed_frame()<=550){
        if(signed_frame()%20==0){
            if(auto* enemy=spawn(2,90,32,1)){enemy->quad(0x38,m.store(m.i(signed_frame()%40?40u:90u)));enemy->quad(0x40,0xc034000000000000ull);enemy->quad(0xa8,0x4020000000000000ull);enemy->quad(0xb0,angle());}
            if(auto* enemy=spawn(2,90,32,1)){enemy->quad(0x38,m.store(m.i(signed_frame()%40?344u:294u)));enemy->quad(0x40,0xc034000000000000ull);enemy->quad(0xa8,0x4020000000000000ull);enemy->quad(0xb0,angle());}
        }
    }else if(timeline.frame==800){
        if(auto* enemy=spawn(3,170000,33,3)){enemy->quad(0x38,0x4068000000000000ull);enemy->quad(0x40,0xc034000000000000ull);enemy->quad(0xa8,0x4024000000000000ull);}
    }else if(timeline.frame==1200){
        if(auto* enemy=spawn(4,250000,34,2)){enemy->quad(0x38,0x4068000000000000ull);enemy->quad(0x40,0xc034000000000000ull);enemy->word(0x80,0);enemy->quad(0xa8,0x4039000000000000ull);
            enemy->quad(0xb8,select({{0x3ffb333333333333ull,0x3ffe666666666666ull,0x4000cccccccccccdull,0x4002666666666666ull,0,0}}));}
    }else if(timeline.frame==1700){
        if(auto* enemy=spawn(5,250000,35,1)){enemy->quad(0x38,0x4068000000000000ull);enemy->quad(0x40,0xc034000000000000ull);
            const std::array<std::uint32_t,6> rates{{50,30,25,20,0,0}};enemy->word(0x80,rates[parameters.difficulty<6?parameters.difficulty:3]);enemy->word(0x84,0);enemy->quad(0xa8,0x4028000000000000ull);
            enemy->quad(0xb0,select({{0x3ff8000000000000ull,0x3ffb333333333333ull,0x3ffe666666666666ull,0x4000cccccccccccdull,0,0}}));}
    }else if(timeline.frame==2200)clear_projectiles(projectiles,100);
    else if(timeline.frame==2240){++timeline.part;timeline.subphase=0;timeline.frame=0;parameters.game_frame=0;}
}
}
