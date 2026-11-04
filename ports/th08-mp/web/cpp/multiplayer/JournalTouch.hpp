#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Journal capture helpers are multiplayer-only
#endif
#include <eagler/netplay/RollbackJournal.hpp>
#include <type_traits>

namespace th08::multiplayer {
#ifdef TH_MULTIPLAYER_FIXTURES
inline bool batch_checkpoint_copies=true;
#endif
// Only synchronous BeginFrame inventory scans may defer copies. Close before
// returning to any native Update/Draw or first-write hook, including failures.
class CheckpointCapture {
    Netplay::RollbackJournal& journal;
    bool open=false,valid=true;
public:
    explicit CheckpointCapture(Netplay::RollbackJournal& value):journal(value){
#ifdef TH_MULTIPLAYER_FIXTURES
        if(!batch_checkpoint_copies)return;
#endif
        valid=journal.BeginReadOnlyCapture();open=valid;
    }
    ~CheckpointCapture(){if(open)journal.EndReadOnlyCapture();}
    bool Valid()const{return valid;}
    bool Finish(){if(!open)return valid;open=false;return journal.EndReadOnlyCapture();}
    CheckpointCapture(const CheckpointCapture&)=delete;
    CheckpointCapture& operator=(const CheckpointCapture&)=delete;
};
template<class T>bool touch(Netplay::RollbackJournal& journal,T& value){
    static_assert(std::is_trivially_copyable_v<T>,"Owning C++ objects need explicit lifetime restoration");
    return journal.Touch(&value,sizeof(value));
}
template<class T>bool before_write(Netplay::RollbackJournal* journal,T& value){
    if(!journal)return true;
    if(journal->Failed())return false;
    if(journal->IsFrameOpen())return touch(*journal,value);
    return journal->FrameCount()==0;
}
}
