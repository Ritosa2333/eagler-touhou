#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error TH08 enemy journaling is multiplayer-only
#endif
#include "../game/EnemyPopulation.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <array>
#include <deque>

namespace th08::multiplayer {
// One owner in the eventual world journal, not a standalone world restore.
// In-process instruction/familiar/effect references remain valid through the
// stage fence. Owning C++ pointers are retained/restored as owners, not memcpy.
class EnemyJournal {
public:
    static constexpr std::size_t History=8;
    EnemyJournal()=default;
    ~EnemyJournal(){Clear();}
    EnemyJournal(const EnemyJournal&)=delete;
    EnemyJournal& operator=(const EnemyJournal&)=delete;
    bool Bind(EnemyPopulation&);
    bool BeginFrame(u32 frame,bool extend=false);
    bool Capture(u32 slot);
    bool EndFrame();
    bool UndoTo(u32 frame);
    void DiscardBefore(u32 frame);
    void Clear();
    bool HasHistory()const{return !records.empty();}
    bool Failed()const{return failed||bytes.Failed();}
    std::size_t BytesForFrame(u32 frame)const{return bytes.BytesForFrame(frame);}
    u32 AuditHash()const;
private:
    struct ContextOwners {
        u32 slot=0;
        std::array<std::shared_ptr<EclContext>,4> contexts{};
    };
    struct Record {
        u32 frame=0,end=0;
        std::array<bool,481> existed{},captured{};
        std::vector<ContextOwners> owners;
    };
    EnemyPopulation* population=nullptr;
    EclProgram* program=nullptr;
    u8* program_data=nullptr;u32 program_size=0;
    Netplay::RollbackJournal bytes;
    std::deque<Record> records;
    bool failed=false;
    bool Fail(){failed=true;return false;}
};
}
