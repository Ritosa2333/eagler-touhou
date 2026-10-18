#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Journal capture helpers are multiplayer-only
#endif
#include <eagler/netplay/RollbackJournal.hpp>
#include <type_traits>

namespace th08::multiplayer {
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
