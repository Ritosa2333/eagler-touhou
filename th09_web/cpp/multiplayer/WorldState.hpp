#pragma once
#include "DynamicState.hpp"
#include "../game/GameSession.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <functional>
#include <bitset>
#include <type_traits>
#include <cassert>
#include <memory>

namespace th09::multiplayer {
// One pre-tick checkpoint. Named trivially copyable state uses the byte
// journal; complete bullet VMs use fixed-identity pages. Containers and
// polymorphic owners have explicit copy rules. Resource and pool addresses
// must stay stable until this record is retired.
class WorldState : public PoolCheckpoint {
public:
    WorldState();
    ~WorldState(){Unbind();}
    bool sparse=true;
    void BeforeEnemy(EclVm&) override;
    void BeforeEffect(EffectManager&,std::size_t) override;
    void BeforeAttack(AttackQueue&,std::size_t) override;
    void BeforeBullet(BulletManager&,std::size_t) override;
    void BeforeBulletVisual(BulletVisuals&,std::size_t) override;
    void BeforeBulletAnimation(BulletVisuals&,std::size_t,std::size_t) override;
    void BeforePlayerShot(PlayerShots&,std::size_t) override;
    void BeforeBytes(void* p,std::size_t n)override{Touch(p,n);}
    bool Save(GameSession&);
    u32 Fingerprint()const{return digest.value;}
    bool hashEnabled=false;
    std::vector<std::pair<std::string,u32>> hashParts;
    template<class... T>void NamedFields(const char* name,T&... fields){Fields(fields...);if(hashEnabled)hashParts.emplace_back(name,digest.value);}
    void HashPart(const char* name,u32 value){if(hashEnabled){digest.Add(value);hashParts.emplace_back(name,digest.value);}}
    bool Restore();
    void AfterRestore(std::function<void()> action){undo.push_back(std::move(action));}
    bool Seal(){Unbind();return journal.EndFrame() && Good();}
    // Sparse bullet VM bytes belong only to BeforeBulletAnimation/Visual.
    // Fields, Touch and BeforeBytes must not register overlapping VM ranges.
    bool Touch(void* data,std::size_t size){
#if !defined(NDEBUG) && (!defined(TH09_DEVELOPMENT_HARNESS) || TH09_DEVELOPMENT_HARNESS)
        assert(JournalRangeDisjoint(data,size));
#endif
        good=journal.Touch(data,size)&&good;
        if(visualFirstWriteBlocks)good=WithinFirstWriteBudget()&&good;
        return good;
    }
    bool Good() const { return good && !journal.Failed() && (!visualFirstWriteBlocks||WithinFirstWriteBudget()); }
    std::size_t FirstWriteBytes() const { return journal.BytesForFrame(0)+visualFirstWriteBlocks*sizeof(AnmVm); }
    std::size_t FirstWriteBlocks() const { return journal.BlocksForFrame(0)+visualFirstWriteBlocks; }
    std::size_t Bytes() const { return FirstWriteBytes()+ownedBytes; }
    // Retained allocation is separate from the current checkpoint's live bytes.
    std::size_t VisualPageCapacityBytes() const { return visualPageCount*BulletRange::PageBytes; }
    template<class... T> void Fields(T&... fields) {
        static_assert((std::is_trivially_copyable_v<T> && ...), "checkpoint field owns memory");
        if(hashEnabled)digest.Add(fields...);
#if !defined(NDEBUG) && (!defined(TH09_DEVELOPMENT_HARNESS) || TH09_DEVELOPMENT_HARNESS)
        (assert(JournalRangeDisjoint(&fields,sizeof(fields))), ...);
#endif
        ((good = journal.Touch(&fields,sizeof(fields)) && good), ...);
        if(visualFirstWriteBlocks)good=WithinFirstWriteBudget()&&good;
    }
    template<class T> void Vector(std::vector<T>& values) {
        static_assert(std::is_trivially_copyable_v<T>);
        if(hashEnabled)for(auto& v:values)digest.Add(v);
        ownedBytes+=values.size()*sizeof(T);
        undo.emplace_back([&values,saved=values] { values=saved; });
    }
    template<class T> void AppendOnly(std::vector<T>& values) {
        undo.emplace_back([&values,size=values.size()] { values.resize(size); });
    }
    void Text(std::string& value) { undo.emplace_back([&value,saved=value]{value=saved;}); }
private:
    // The two versus enemy pools have stable slot identity for a checkpoint.
    // Unknown owners retain the original pointer-list fallback in BeforeEnemy.
    // This index owns only first-write metadata, never ECL state or pointers.
    friend struct WorldStateEnemyCaptureTest;
    friend struct WorldStateShotCaptureTest;
    static constexpr std::size_t EnemySlotsPerRange=EnemyManager::capacity+2;
    struct EnemyRange { EclVm* prototype=nullptr;EclVm* pool=nullptr; };
    std::array<EnemyRange,2> enemyRanges{};
    std::bitset<2*EnemySlotsPerRange> enemySaved;
    void ResetEnemyCapture(){enemyCount=0;enemyRanges={};enemySaved.reset();}
    // These were the journal's limits before VM bytes moved into pages. Keep
    // one combined first-write budget; owning-container accounting is unchanged.
    static constexpr std::size_t FirstWriteByteBudget=32*1024*1024,FirstWriteBlockBudget=8192;
    bool WithinFirstWriteBudget() const { return FirstWriteBytes()<=FirstWriteByteBudget&&FirstWriteBlocks()<=FirstWriteBlockBudget; }
#if !defined(NDEBUG) && (!defined(TH09_DEVELOPMENT_HARNESS) || TH09_DEVELOPMENT_HARNESS)
    bool JournalRangeDisjoint(const void*,std::size_t) const;
#endif
    std::vector<PoolCheckpoint**> observers;
    void Bind(PoolCheckpoint*& p){p=this;observers.push_back(&p);}
    void Unbind(){for(auto** p:observers)if(*p==this)*p=nullptr;observers.clear();}
    struct EffectRange { EffectManager* owner=nullptr;std::size_t offset=0; };
    std::array<EffectRange,3> effectRanges{};
    std::size_t effectRangeCount=0,effectSlots=0;
    std::bitset<1400> effectSaved;
    std::bitset<257> attackSaved;
    // Player shot pools have stable addresses for the same match horizon as
    // bullets. Capture complete live slots and protect a cold slot on creation.
    struct ShotRange {
        PlayerShots* owner=nullptr;
        Netplay::SparsePoolCapture<PlayerShots::capacity> capture;
    };
    std::array<ShotRange,2> shotRanges{};
    std::size_t shotRangeCount=0;
    struct BulletRange {
        static constexpr std::size_t VisualCount=(BulletManager::update_count+1)*5;
        static constexpr std::size_t VMsPerPage=64,PageBytes=VMsPerPage*sizeof(AnmVm);
        static_assert(std::is_trivially_copyable_v<AnmVm>);
        static_assert(VisualCount<=0xffff);
        BulletManager* owner=nullptr;
        BulletVisuals* visuals=nullptr;
        Netplay::SparsePoolCapture<BulletManager::update_count+1> capture;
        // First-write ownership is per complete VM, never a heuristic subset
        // of VM fields. Full-slot reset/type change uses the same bitmap.
        std::bitset<VisualCount> visualSaved;
        // Lazy, uninitialized pages never relocate existing snapshots. The
        // bitmap/order prefix is per checkpoint; allocations survive ring reuse.
        std::array<std::unique_ptr<u8[]>,(VisualCount+VMsPerPage-1)/VMsPerPage> visualPages{};
        std::array<u16,VisualCount> visualOrder;
        std::size_t visualCount=0;
    };
    std::array<BulletRange,2> bulletRanges;
    std::size_t bulletRangeCount=0;
    StateChecksum digest;
    Netplay::RollbackJournal journal;
    std::vector<std::function<void()>> undo;
    std::vector<std::pair<EclVm*,EnemyState>> enemies;
    std::vector<std::pair<EffectActor*,EffectState>> effects;
    std::vector<std::pair<AttackActor*,AttackStateSnapshot>> attacks;
    // Retain each record's owning storage across ring reuse. Only this prefix
    // belongs to the current checkpoint; older targets must never be restored.
    std::size_t enemyCount=0,effectCount=0,attackCount=0;
    std::size_t ownedBytes=0;
    std::size_t visualFirstWriteBlocks=0,visualPageCount=0;
    bool good=true;
    void Enemy(EnemyManager&);
    void Effects(EffectManager&);
    void Attacks(AttackQueue&);
    void PlayerState(Player&);
    void Shots(PlayerShots&);
    void BackgroundState(Background&);
    void Recording(ReplayArchive&);
};
}
