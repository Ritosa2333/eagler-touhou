#pragma once
#include "JournalTouch.hpp"
#include "../game/AnmLibrary.hpp"

namespace th08::multiplayer {
// Stable, fully loaded ANM resource graph. Covers native mutable scripts and
// sprite metadata; texture pixels are presentation owners, not copied here.
class ResourcesJournal {
public:
    static constexpr std::size_t History=8;
    ResourcesJournal()=default;
    ~ResourcesJournal(){Clear();}
    ResourcesJournal(const ResourcesJournal&)=delete;
    ResourcesJournal& operator=(const ResourcesJournal&)=delete;
    bool Bind(AnmLibrary&);
    bool BeginFrame(u32,bool extend=false);
    bool EndFrame(){return !Failed()&&bytes.EndFrame();}
    bool UndoTo(u32 frame){return !failed&&stable()&&bytes.UndoTo(frame);}
    void DiscardBefore(u32 frame){bytes.DiscardBefore(frame);}
    void Clear();
    bool Failed()const{return failed||bytes.Failed()||(library&&library->rollback_failed);}
    std::size_t BytesForFrame(u32 frame)const{return bytes.BytesForFrame(frame);}
    u32 AuditHash()const;
private:
    struct Identity {
        AnmResource* owner=nullptr;const u8* data=nullptr;std::size_t size=0;
        const AnmLoadedSprite* sprites=nullptr;std::size_t sprite_count=0;
        const AnmRawInstr* const* scripts=nullptr;std::size_t script_count=0;
    } identities[256]{};
    AnmLibrary* library=nullptr;Netplay::RollbackJournal bytes;bool failed=false;
    bool stable()const;
    bool Fail(){failed=true;return false;}
};
}
