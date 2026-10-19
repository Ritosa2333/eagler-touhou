#pragma once
#include "EnemyFrame.hpp"
#ifdef TH11_MULTIPLAYER
#include "EnemyRollback.hpp"
#endif
#include <memory>
#include <unordered_map>
namespace th11 {
struct EnemySpawn {
    Vec3 position{};i32 score=0,drop=0,health=0;u32 mirrored=0,world_coordinates=0;
    i32 integers[4]{};float floats[4]{},extra_floats[4]{};
};
static_assert(sizeof(EnemySpawn)==0x50);
class EnemyManager {
public:
    EnemyManager(EclProgram&,EnemyEnvironment&,EnemyCommandEnvironment&,EnemyFrameWorld&);
    ~EnemyManager();
    Enemy* spawn(const char*,const EnemySpawn&);
    bool destroy(Enemy&);
    bool reset(EclProgram&);
    bool update();
#ifdef TH11_MULTIPLAYER
    // Fresh per-frame journal. External world, commands, environment, ANM and
    // deformation owners must participate separately before prediction is safe.
    bool BeginRollbackFrame(Netplay::RollbackJournal&,EnemyRollbackFrame&);
    void EndRollbackFrame()noexcept;
    bool RestoreRollbackFrame(const EnemyRollbackFrame&,u32 restored_frame);
    bool RollbackFrameActive()const noexcept{return rollback_journal_!=nullptr;}
#endif
    bool erase_stage_enemies(bool matching_tag=false,u32 tag=0);
    bool cancel_circle(Vec3 center,float radius,bool reward,bool convert=false);
    bool cancel_beam(Vec3 center,float half_width,bool reward);
    i32 command(EnemyState&,EclContext&,EnemyGlobals&);
    EnemyLink* first=nullptr;EnemyLink* last=nullptr;
    u32 count=0,total_created=0;Timer timer;
    i32 last_error=0;EnemyScriptError script_error;
private:
#ifdef TH11_MULTIPLAYER
    friend struct EnemyRollbackTestAccess;
    using Entry=EnemyRollbackEntry;
#else
    struct Entry {Enemy enemy{};EnemyScriptServices script;Entry(EnemyEnvironment& w,EnemyCommandEnvironment& c):script(enemy.state,w,c){}};
#endif
    EclProgram* program;EnemyEnvironment& environment;EnemyCommandEnvironment& commands;EnemyFrameWorld& world;
#ifdef TH11_MULTIPLAYER
    EclRollbackAllocator rollback_allocator_;
    std::vector<std::shared_ptr<Entry>> rollback_storage_;
    std::unordered_map<Enemy*,u32> rollback_slots_;
    std::vector<u32> rollback_available_;
    u32 rollback_logical_size_=0;
    std::shared_ptr<const unsigned char> rollback_identity_;
    Netplay::RollbackJournal* rollback_journal_=nullptr;
    std::unordered_map<Enemy*,std::shared_ptr<Entry>> entries;
    void preserve_rollback_entry(Entry&);
    std::shared_ptr<Entry> acquire_rollback_entry();
    void release_rollback_entry(const std::shared_ptr<Entry>&);
#else
    std::unordered_map<Enemy*,std::unique_ptr<Entry>> entries;
#endif
    u32 spawn_depth=0;
};
}
