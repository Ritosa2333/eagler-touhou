#pragma once
#include "Rng.hpp"
#include "RateScalarTh14.hpp"
#include "Replay.hpp"
#include "GameEconomy.hpp"
#include <array>
#include <cstddef>

namespace th11 {
#if PORT_TITLE == 14
// Worker builds use ordered, aligned scalar stores to the real shared fields.
// The ordinary main-thread implementation preserves its plain-store ABI.
template<class T> inline void th14_stage_store(T& destination,T value)noexcept{
#if defined(TH14_STAGE_MEMORY_WORKER) || defined(TH14_CPP_STAGE_WORKER)
    __atomic_store_n(&destination,value,__ATOMIC_SEQ_CST);
#else
    destination=value;
#endif
}
inline u16 th14_stage_seed(const u32& seed)noexcept{
#if defined(TH14_STAGE_MEMORY_WORKER) || defined(TH14_CPP_STAGE_WORKER)
    return u16(__atomic_load_n(&seed,__ATOMIC_SEQ_CST));
#else
    return u16(seed);
#endif
}
inline void th14_playback_gameplay_seed(Rng& gameplay,const u32& seed)noexcept{th14_stage_store(gameplay.seed,th14_stage_seed(seed));}
inline void th14_playback_script_seed(Rng& script,const u32& seed)noexcept{th14_stage_store(script.seed,th14_stage_seed(seed));}
inline u16 th14_record_gameplay_seed(Rng& gameplay,u32& recording)noexcept{
#if defined(TH14_STAGE_MEMORY_WORKER) || defined(TH14_CPP_STAGE_WORKER)
    const u16 captured=__atomic_load_n(&gameplay.seed,__ATOMIC_SEQ_CST);
#else
    const u16 captured=gameplay.seed;
#endif
    th14_stage_store(recording,u32(captured));return captured;
}
inline void th14_record_script_seed(Rng& script,u16 captured)noexcept{th14_stage_store(script.seed,captured);}
inline void th14_clear_gameplay_calls(Rng& gameplay)noexcept{th14_stage_store(gameplay.calls,u32(0));}
inline void th14_stage_float_bits(float& destination,u32 bits)noexcept{
#if defined(TH14_STAGE_MEMORY_WORKER) || defined(TH14_CPP_STAGE_WORKER)
    __atomic_store_n(reinterpret_cast<u32*>(&destination),bits,__ATOMIC_SEQ_CST);
#else
    std::memcpy(&destination,&bits,4);
#endif
}
#if defined(TH14_CPP_STAGE_WORKER)
inline void th14_stage_float_bits(AnmRateTh14& destination,u32 bits)noexcept{
    float value;std::memcpy(&value,&bits,4);destination=value;
}
#endif

// Named projections follow the original global-word order. Selection targets
// are outside GameEconomy, as are practice, continues and minimum point value.
// Distinct typed fields are still not a contiguous raw112-byte destination.
inline void th14_project_stage_globals(const ReplayStageState& entry,GameEconomy& e,
    i32* practice=nullptr,i32* continues=nullptr,i32* minimum_point_value=nullptr,
    i32* character=nullptr,i32* subtype=nullptr)noexcept{
    const auto value=[&](u32 index){return signed_bits(entry.th14_globals[index]);};
    if(character)th14_stage_store(*character,value(0));
    if(subtype)th14_stage_store(*subtype,value(1));
    th14_stage_store(e.score_units,entry.score);th14_stage_store(e.difficulty,value(3));
    if(continues)th14_stage_store(*continues,entry.continues);
    th14_stage_store(e.rank,entry.rank);th14_stage_store(e.graze,entry.graze);
    if(practice)th14_stage_store(*practice,entry.th14_practice_mode);
    th14_stage_store(e.th14_point_items,value(8));th14_stage_store(e.point_value,entry.points);
    if(minimum_point_value)th14_stage_store(*minimum_point_value,value(10));
    th14_stage_store(e.max_point_value,value(11));th14_stage_store(e.power,entry.power);
    th14_stage_store(e.max_power,value(13));th14_stage_store(e.power_step,value(14));
    th14_stage_store(e.lives,entry.lives);th14_stage_store(e.life_fragments,entry.fragments);
    th14_stage_store(e.th14_life_awards,entry.extends);th14_stage_store(e.th14_bombs,value(18));
    th14_stage_store(e.th14_bomb_fragments,value(19));th14_stage_store(e.th14_combo.total,value(20));
    th14_stage_store(e.th14_reserved_54,value(21));th14_stage_store(e.th14_combo.count,value(22));
    th14_stage_store(e.th14_combo.timer,value(23));
    th14_stage_float_bits(e.th14_combo.position.x,entry.th14_globals[24]);
    th14_stage_float_bits(e.th14_combo.position.y,entry.th14_globals[25]);
    th14_stage_float_bits(e.th14_combo.position.z,entry.th14_globals[26]);
    th14_stage_store(e.th14_special_drop_count,value(27));
}

// The packet and every pointed-to object must outlive the worker's terminal
// acknowledgement. It contains real pointers, not copied RNG or expected state.
struct StageRestoreMemoryTh14 {
    Rng* gameplay=nullptr;
    Rng* script=nullptr;
    const ReplayStageState* entry=nullptr;
    u32* recording_seed=nullptr;
    GameEconomy* economy=nullptr;
    i32* practice=nullptr;
    i32* continues=nullptr;
    i32* minimum_point_value=nullptr;
    i32* character=nullptr;
    i32* subtype=nullptr;
};

constexpr u32 th14_stage_memory_layout()noexcept{
    constexpr u32 words[]={sizeof(Rng),offsetof(Rng,seed),offsetof(Rng,calls),sizeof(StageRestoreMemoryTh14),
        offsetof(StageRestoreMemoryTh14,gameplay),offsetof(StageRestoreMemoryTh14,script),offsetof(StageRestoreMemoryTh14,entry),
        offsetof(StageRestoreMemoryTh14,recording_seed),offsetof(StageRestoreMemoryTh14,economy),offsetof(StageRestoreMemoryTh14,practice),
        offsetof(StageRestoreMemoryTh14,continues),offsetof(StageRestoreMemoryTh14,minimum_point_value),
        offsetof(StageRestoreMemoryTh14,character),offsetof(StageRestoreMemoryTh14,subtype),
        sizeof(ReplayStageState),offsetof(ReplayStageState,seed),offsetof(ReplayStageState,score),offsetof(ReplayStageState,power),
        offsetof(ReplayStageState,points),offsetof(ReplayStageState,lives),offsetof(ReplayStageState,fragments),offsetof(ReplayStageState,rank),
        offsetof(ReplayStageState,continues),offsetof(ReplayStageState,extends),offsetof(ReplayStageState,graze),
        offsetof(ReplayStageState,th14_globals),offsetof(ReplayStageState,th14_practice_mode),sizeof(GameEconomy),
        offsetof(GameEconomy,score_units),offsetof(GameEconomy,power),offsetof(GameEconomy,point_value),offsetof(GameEconomy,difficulty),
        offsetof(GameEconomy,rank),offsetof(GameEconomy,graze),offsetof(GameEconomy,max_point_value),offsetof(GameEconomy,max_power),
        offsetof(GameEconomy,power_step),offsetof(GameEconomy,lives),offsetof(GameEconomy,life_fragments),
        offsetof(GameEconomy,th14_point_items),offsetof(GameEconomy,th14_special_drop_count),offsetof(GameEconomy,th14_reserved_54),
        offsetof(GameEconomy,th14_bombs),offsetof(GameEconomy,th14_bomb_fragments),offsetof(GameEconomy,th14_life_awards),
        offsetof(GameEconomy,th14_combo),sizeof(Th14ItemCombo),offsetof(Th14ItemCombo,total),offsetof(Th14ItemCombo,count),
        offsetof(Th14ItemCombo,timer),offsetof(Th14ItemCombo,position)};
    u32 result=2166136261u;for(const auto word:words){result^=word;result*=16777619u;}return result;
}
#endif
}
