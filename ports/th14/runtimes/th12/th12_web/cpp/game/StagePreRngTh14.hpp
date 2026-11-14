#pragma once
#include "StageRestoreMemoryTh14.hpp"
namespace th11 {
#if PORT_TITLE==14
template<class Load,class Store> inline void th14_stage_high_score_words(Load load,Store store){
    u32 high=load(0);
    // Retail435e68 compares score,435e6e conditionally reloads it. Preserve
    // both reads and the unconditional435e75 high-score store; this is no CAS.
    if(load(1)>high)high=load(1);
    store(high);
}
inline void th14_stage_high_score(i32& high,const i32& score)noexcept{
    th14_stage_high_score_words([&](u32 field){
        const auto* word=field?&score:&high;
#if defined(TH14_CPP_STAGE_WORKER)
        return u32(__atomic_load_n(word,__ATOMIC_SEQ_CST));
#else
        return u32(*word);
#endif
    },[&](u32 value){th14_stage_store(high,signed_bits(value));});
}
template<class Rate> inline void th14_stage_pre_rng(Rate& rate,bool& ending,i32& high,const i32& score)noexcept{
    th14_stage_float_bits(rate,0x3f800000u);
    th14_stage_store(ending,false);
    th14_stage_high_score(high,score);
}
#endif
}
