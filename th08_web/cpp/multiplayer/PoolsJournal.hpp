#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error TH08 pool journals are multiplayer-only
#endif
#include "JournalTouch.hpp"
#include "../game/Types.hpp"

namespace th08 {
class BulletSystem;class ItemSystem;class EffectSystem;struct Rng;
namespace multiplayer {
// Native fixed pools and their first-write allocation paths. This owner is
// composed with EnemyJournal, players, scene and presentation state by the
// world adapter; it is not a second simulator or a full-world checkpoint.
class PoolsJournal {
public:
    static constexpr std::size_t History=8;
    ~PoolsJournal(){Clear();}
    PoolsJournal()=default;
    PoolsJournal(const PoolsJournal&)=delete;
    PoolsJournal& operator=(const PoolsJournal&)=delete;
    bool Bind(BulletSystem&,ItemSystem&,EffectSystem&,Rng&);
    bool BeginFrame(u32 frame);
    bool EndFrame();
    bool UndoTo(u32 frame);
    void DiscardBefore(u32 frame){bytes.DiscardBefore(frame);}
    void Clear();
    bool HasHistory()const{return bytes.FrameCount()!=0;}
    bool Failed()const{return failed||bytes.Failed();}
    std::size_t BytesForFrame(u32 frame)const{return bytes.BytesForFrame(frame);}
    // Independent whole-pool oracle used only by native ownership fixtures.
    // Capture is sparse; this deliberately hashes inactive slots too.
    u32 AuditHash()const;
private:
    BulletSystem* bullets=nullptr;ItemSystem* items=nullptr;EffectSystem* effects=nullptr;Rng* rng=nullptr;
    Netplay::RollbackJournal bytes;
    bool failed=false;
    bool Fail(){failed=true;return false;}
};
}
}
