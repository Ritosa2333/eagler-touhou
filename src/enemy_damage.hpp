#pragma once
#include "enemy_slots.hpp"
#include "enemy_math.hpp"
#include <optional>
namespace soew {
struct EnemyKillRequest {unsigned type=0;int left=0,top=0;std::uint8_t drop=0;std::uint32_t score=0;};
// State-only translation of HP threshold sub_1766E and24-tick sub_175E8.
// Item/sound/score/spark effects MUST have an explicit caller; no guessed drops.
struct EnemyDamage {
    std::uint64_t hit_queries=0,kills=0;
    std::optional<EnemyKillRequest> apply(EnemySlots::Slot &s,const StageScript::EnemyTemplate &type,unsigned dealt,unsigned page){
        ++hit_queries;const auto &r=type.raw;
        const unsigned hp=unsigned(r[22])|(unsigned(r[23])<<8);
        if(!dealt||hp==65535||s.raw[0x22])return {};
        if(hp==65534)return {}; // hit feedback only, no accumulating HP damage.
        const auto damage=static_cast<std::uint16_t>(s.word(0x20)+dealt);s.word(0x20,damage);
        if(enemy_s16(damage)<enemy_s16(hp))return {};
        s.word(0xa,0);s.raw[0x10]=1;++kills;
        EnemyKillRequest event;event.type=s.word(0xc);event.left=enemy_s16(s.word(page*4));event.top=enemy_s16(s.word(page*4+2));event.drop=r[16];
        event.score=unsigned(r[18])|(unsigned(r[19])<<8)|(std::uint32_t(r[20])<<16)|(std::uint32_t(r[21])<<24);return event;
    }
    static unsigned death_pattern(const EnemySlots::Slot &s,const StageScript::EnemyTemplate &type){const unsigned alternate=unsigned(type.raw[14])|(unsigned(type.raw[15])<<8);return (alternate==0?10:0)+unsigned(enemy_s16(s.word(0xa))/3);}
    static void death_tick(EnemySlots::Slot &s){s.word(0xa,static_cast<std::uint16_t>(s.word(0xa)+1));if(enemy_s16(s.word(0xa))>=24)s.raw[0xe]=2;}
};
}
