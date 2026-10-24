#pragma once
#include "AnmGeometry.hpp"
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
#ifdef TH11_MULTIPLAYER
#include "AnmRollback.hpp"
namespace Netplay { class RollbackJournal; }
#endif
namespace th11 {
class AnmManager final:public AnmEnvironment {
public:
    ~AnmManager() override;
    AnmVm* create(AnmResource& resource,i32 script,u16 file_id,u32 layer,bool overlay=false,bool prepend=false,const Vec3* initial_position=nullptr);
    AnmVm* spawn(AnmVm& parent,i32 script,u32 opcode) override;
    AnmVm* find(u32 id) noexcept;
    void destroy(AnmVm& vm);
    void clear();
    void retire_resource(const AnmResource&);
    bool references(const AnmResource&)const noexcept;
    bool update(bool overlay);
    bool change_draw_mode(AnmVm& vm) override;
    bool allocate_geometry(AnmVm& vm,u32 bytes) override;
    bool update_geometry(AnmVm& vm) override;
    void release_geometry(AnmVm& vm);
    AnmVm* layer_first(u32 layer)const noexcept {return layer<layers.size()?layers[layer]:nullptr;}
    u32 active_count()const noexcept {return active.size();}
    // Authoritative VM fingerprint for the Presentation Lab state evidence.
    u32 fingerprint()const noexcept;
    u32 last_error=0;
#ifdef TH11_MULTIPLAYER
    // Orchestrator order: journal.BeginFrame -> Begin -> complete semantic
    // tick/Draw -> End -> journal.EndFrame. Undo bytes before restoring typed
    // metadata, and retain all frame leases until UndoTo has completed.
    bool BeginRollbackFrame(Netplay::RollbackJournal&,AnmRollbackFrame&);
    void EndRollbackFrame()noexcept;
    bool RestoreRollbackFrame(const AnmRollbackFrame&,u32 restored_frame);
    bool RollbackFrameActive()const noexcept{return rollback_journal_!=nullptr;}
    void preserve_script_write(void*,u32)override;
    unsigned multiplayer_owner=0;
    std::unordered_map<const AnmVm*,unsigned> multiplayer_tags;
    unsigned multiplayer_tag(const AnmVm& vm)const noexcept{auto i=multiplayer_tags.find(&vm);return i==multiplayer_tags.end()?0:i->second;}
#endif
private:
    friend struct AnmRollbackTestAccess;
#ifdef TH11_MULTIPLAYER
    // Non-owning logical slots; physical page ownership stays with the
    // manager through rewind. Original slot/ID/free-list order is unchanged.
    struct RollbackVmSlot {
        AnmVm* value;
        AnmVm* get()const noexcept{return value;}
        AnmVm& operator*()const noexcept{return *value;}
    };
    static constexpr std::size_t rollback_page_slots_=32;
    using RollbackVmPage=std::array<AnmVm,rollback_page_slots_>;
    std::vector<RollbackVmSlot> storage;
    std::vector<std::unique_ptr<RollbackVmPage>> rollback_pages_;
    std::vector<unsigned char> rollback_page_captured_;
    bool capture_rollback_page(std::size_t);
#else
    std::vector<std::unique_ptr<AnmVm>> storage;
#endif
    std::vector<AnmVm*> available;
    std::unordered_map<u32,AnmVm*> active;
#ifdef TH11_MULTIPLAYER
    std::unordered_map<AnmVm*,AnmGeometryAllocation> geometry_sizes;
    // Physical slots survive rewind; only this logical frontier is journaled.
    // A replayed allocation reuses the original stable address.
    std::size_t logical_storage_size_=0;
    std::unordered_map<AnmVm*,std::size_t> rollback_slots_;
    Netplay::RollbackJournal* rollback_journal_=nullptr;
    std::shared_ptr<const unsigned char> rollback_identity_;
    std::shared_ptr<const std::unordered_map<u32,AnmVm*>> rollback_registry_snapshot_;
    void preserve_rollback_slot(AnmVm&);
#else
    std::unordered_map<AnmVm*,u32> geometry_sizes;
#endif
    AnmVm::Node* heads[2]{},*tails[2]{};
    std::array<AnmVm*,31> layers{};
    u32 next_id=0,spawn_depth=0;
};
}
