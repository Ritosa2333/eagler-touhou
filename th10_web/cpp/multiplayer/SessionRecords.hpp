#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer record ownership must not enter an ordinary build
#endif
#include "../game/ScoreData.hpp"
#include <memory>

namespace th10::multiplayer {
// Native gameplay sees one deterministic run-local record set. Local historic
// scores/names are a persistence owner, never unagreed simulation inputs.
// Only an explicitly confirmed lifecycle fence may merge run contributions.
class SessionRecords {
public:
    bool Begin(ScoreData&,const std::int8_t* spellDifficulties,bool readOnly);
    bool Checkpoint(const ScoreData&,i32 timestamp);
    void Finish(ScoreData&);
    bool Active()const{return persistent_!=nullptr;}
    bool ReadOnly()const{return Active()&&read_only_;}
    ScoreData* Persistent(){return persistent_.get();}
private:
    std::unique_ptr<ScoreData> persistent_,checkpoint_;
    bool read_only_=false;
};
}
