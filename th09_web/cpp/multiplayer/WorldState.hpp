#pragma once
#include "DynamicState.hpp"
#include "../game/GameSession.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <functional>
#include <bitset>
#include <type_traits>

namespace th09::multiplayer {
// One pre-tick checkpoint. Only named, trivially copyable state enters the
// byte journal. Containers and polymorphic owners have explicit copy rules.
// Resource and pool addresses must stay stable until this record is retired.
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
    bool Touch(void* data,std::size_t size){return good=journal.Touch(data,size)&&good;}
    bool Good() const { return good && !journal.Failed(); }
    std::size_t Bytes() const { return journal.BytesForFrame(0)+ownedBytes; }
    template<class... T> void Fields(T&... fields) {
        static_assert((std::is_trivially_copyable_v<T> && ...), "checkpoint field owns memory");
        if(hashEnabled)digest.Add(fields...);
        ((good = journal.Touch(&fields,sizeof(fields)) && good), ...);
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
    std::vector<PoolCheckpoint**> observers;
    void Bind(PoolCheckpoint*& p){p=this;observers.push_back(&p);}
    void Unbind(){for(auto** p:observers)if(*p==this)*p=nullptr;observers.clear();}
    struct EffectRange { EffectManager* owner=nullptr;std::size_t offset=0; };
    std::array<EffectRange,3> effectRanges{};
    std::size_t effectRangeCount=0,effectSlots=0;
    std::bitset<1400> effectSaved;
    std::bitset<257> attackSaved;
    struct BulletRange {
        BulletManager* owner=nullptr;
        BulletVisuals* visuals=nullptr;
        Netplay::SparsePoolCapture<BulletManager::update_count+1> capture;
        // First-write ownership is per complete VM, never a heuristic subset
        // of VM fields. Full-slot reset/type change uses the same bitmap.
        std::bitset<(BulletManager::update_count+1)*5> visualSaved;
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
    bool good=true;
    void Enemy(EnemyManager&);
    void Effects(EffectManager&);
    void Attacks(AttackQueue&);
    void PlayerState(Player&);
    void BackgroundState(Background&);
    void Recording(ReplayArchive&);
};
}
