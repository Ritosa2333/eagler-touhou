#pragma once
#include "ScoreFile.hpp"
namespace th11 {
// Control4360e0 calls456f60 synchronously before retiring owners or starting
// the incoming worker. It does so even when score bytes have not changed,
// and ignores the writer's return value before continuing the transition.
template<class Write>
inline void th14_stage_score_save(ScoreFile& scores,const SpellRecords& spells,
                                 const ClearRecords& clears,Write&& write){
    scores.write_records(spells,clears);
    std::vector<u8> bytes;
    if(scores.save(bytes))(void)write(bytes);
}
}
