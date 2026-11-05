#pragma once
#include "BulletState.hpp"
#include <vector>
#include <functional>
#include <bitset>
namespace th15 {
namespace multiplayer {class BulletFrameState;}
struct BulletManagerHost:BulletSoundHost {virtual ~BulletManagerHost()=default;virtual BulletFrameHost& bullet(u32 slot)=0;virtual void prepare(u32 slot,BulletFrameContext& context)=0;virtual const std::string* failure()const noexcept{return nullptr;}};
class BulletManager final:public BulletEmissionHost {
    friend class BulletCheckpoint;
    friend class BulletScene;
    friend class multiplayer::BulletFrameState;
    struct Slot {BulletState state;u32 free_next=UINT32_MAX,active_next=UINT32_MAX,active_previous=UINT32_MAX;bool active=false;};
    std::vector<Slot> slots;u32 free_head=UINT32_MAX,active_head=UINT32_MAX;
    // Copy cold slots only when their content changes. The serial never rewinds:
    // divergent futures must not reuse the identity of an abandoned snapshot.
    inline static u64 snapshot_serial=0;
    std::vector<u64> snapshot_versions;
    std::bitset<2000> snapshot_exposed;
    void changed(u32 index)noexcept{snapshot_versions[index]=++snapshot_serial;}
    BulletManagerHost& host;
    u32 acquire()noexcept;
public:
    static constexpr u32 capacity=2000,none=UINT32_MAX;
    std::function<Vec3(const Vec3&)> multiplayer_target;
    BulletFrameContext context;Rng* random=nullptr;float minimum_distance_squared=0;bool world_paused=false;
    BulletCancellationRewards cancellation_rewards;
    std::string error;u32 visible_count=0;std::array<std::vector<u32>,6> draw_groups;
    explicit BulletManager(BulletManagerHost& host);
    void reset();void recycle(u32 slot)noexcept;
    bool emit(const BulletShooter&,float minimum_distance_squared)override;
    bool update();
    BulletState* state(u32 slot)noexcept{if(slot>=capacity)return nullptr;snapshot_exposed.set(slot);return &slots[slot].state;}
    const BulletState* state(u32 slot)const noexcept{return slot<capacity?&slots[slot].state:nullptr;}
    bool active(u32 slot)const noexcept{return slot<capacity&&slots[slot].active;}
    u32 first_active()const noexcept{return active_head;}
    u32 first_free()const noexcept{return free_head;}
    u32 next_active(u32 slot)const noexcept{return slot<capacity?slots[slot].active_next:none;}
};
}
