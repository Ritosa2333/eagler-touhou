#include "../th10_web/cpp/multiplayer/HitboxOwnershipHistory.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <cassert>
#include <cstdio>
using th10::multiplayer::HitboxOwnershipHistory;
int main(){
    HitboxOwnershipHistory history;HitboxOwnershipHistory::Owners owners;
    Netplay::RollbackJournal journal;Netplay::RollbackJournalConfig config;
    config.maxFrames=HitboxOwnershipHistory::MaxFrames;config.maxBytesPerFrame=64;config.maxBlocksPerFrame=4;
    assert(journal.Reset(config));std::uint32_t last_id=76;
    assert(journal.BeginFrame(1));history.Capture(1,owners);assert(journal.Touch(&last_id,sizeof(last_id)));
    owners[++last_id]=1;assert(journal.EndFrame());
    std::uint32_t from=0;assert(journal.UndoTo(1,&from));assert(history.Restore(from,owners));
    assert(last_id==76);assert(owners.find(++last_id)==owners.end());
    // A released marker can still be fading at the restore boundary. Its
    // owner must return even if the abandoned future had already removed it.
    for(std::uint32_t view=0;view<3;++view){
        history.Clear();owners={{77,view}};history.Capture(2,owners);
        owners.clear();history.Capture(3,owners);owners[77]=(view+1)%3;
        assert(history.Restore(2,owners));assert(owners.at(77)==view);
        assert(!history.Restore(3,owners));
    }
    // Recapturing the corrected timeline retires future ownership snapshots.
    history.Clear();owners.clear();history.Capture(10,owners);owners[90]=1;history.Capture(11,owners);
    assert(history.Restore(10,owners));owners[90]=2;history.Capture(10,owners);owners.clear();
    assert(history.Restore(10,owners));assert(owners.at(90)==2);
    history.Clear();owners.clear();
    for(std::uint32_t frame=1;frame<=28;++frame){owners={{frame,frame%3}};history.Capture(frame,owners);}
    assert(!history.Restore(14,owners));assert(history.Restore(15,owners));assert(owners.at(15)==0);
    history.Clear();assert(!history.Restore(15,owners));
    std::puts("TH10 hitbox ownership: rollback handle reuse, fade-out, corrected history and bounded retention PASS");
}
