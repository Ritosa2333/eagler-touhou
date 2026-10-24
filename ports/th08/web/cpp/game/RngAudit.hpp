#pragma once
#if defined(TH_PRESENTATION_AUDIT)
#include <array>
#include <cstdint>
namespace th08::rng_audit {
// Read-only diagnostic accounting | calc priorities 0..63, Draw 64..127, other 128.
inline bool enabled=false;
inline const void* watched=nullptr;
inline uint32_t owner=128;
inline std::array<uint32_t,129> counts{};
// Events are inclusive deltas: nested Effect/Item events also belong to their
// surrounding simulation phase. Read from (event_write + capacity - event_count)
// modulo capacity; event_overflow counts overwritten events in this application step.
enum class Category : uint32_t { Timeline=1,Ecl=2,Animation=3,ContactDamage=4,Death=5,Finish=6,Effect=7,Item=8,PlayerEffectOwnerBase=0x100 };
struct Event { uint32_t category; int32_t slot,arg0,arg1; uint32_t calls; };
inline constexpr uint32_t event_capacity=2048;
inline std::array<Event,event_capacity> events{};
inline uint32_t event_write=0,event_count=0,event_overflow=0,total_calls=0;
// Candidate-only enemy snapshots | 15 words, float positions occupy words 11..13.
enum class EnemyPhase : uint32_t { BeforeContact=1,AfterContact=2,BeforeDamage=3,AfterDamage=4,BeforeDeath=5 };
struct EnemySnapshot {
    uint32_t phase;int32_t slot,life;uint32_t flags,flags2;int32_t item_reward;
    uint32_t death0,death1,death2;int32_t parent_slot,subroutine;
    float x,y,z;int32_t last_damage;
};
static_assert(sizeof(EnemySnapshot)==15*sizeof(uint32_t));
inline constexpr uint32_t enemy_capacity=2048;
inline std::array<EnemySnapshot,enemy_capacity> enemy_snapshots{};
inline uint32_t enemy_write=0,enemy_count=0,enemy_overflow=0;
inline void snapshot_enemy(const EnemySnapshot& value){
    if(!enabled)return;enemy_snapshots[enemy_write]=value;enemy_write=(enemy_write+1)%enemy_capacity;
    if(enemy_count<enemy_capacity)++enemy_count;else ++enemy_overflow;
}
inline int32_t enemy_slot=-1;
inline void begin(const void* rng){watched=rng;owner=128;enemy_slot=-1;if(enabled){counts.fill(0);event_write=event_count=event_overflow=total_calls=0;enemy_write=enemy_count=enemy_overflow=0;}}
struct SlotScope {
    int32_t saved=enemy_slot;
    explicit SlotScope(int32_t slot){enemy_slot=slot;}
    ~SlotScope(){enemy_slot=saved;}
};
struct EventScope {
    uint32_t start=total_calls;
    bool active=enabled;
    Event event;
    explicit EventScope(Category category,int32_t arg0=0,int32_t arg1=0):event{uint32_t(category),enemy_slot,arg0,arg1,0}{
        // 0x100..0x180 identify PlayerScene effect leaves and their chain owner.
        if(category==Category::PlayerEffectOwnerBase)event.category+=owner;
    }
    ~EventScope(){
        if(!active||!enabled)return;
        event.calls=total_calls-start;if(!event.calls)return;
        events[event_write]=event;event_write=(event_write+1)%event_capacity;
        if(event_count<event_capacity)++event_count;else ++event_overflow;
    }
};
inline void next(const void* rng){if(enabled&&rng==watched){++counts[owner];++total_calls;}}
struct JobScope {
    uint32_t saved=owner;
    JobScope(int priority,bool draw){owner=priority>=0&&priority<64?uint32_t(priority)+(draw?64u:0u):128u;}
    ~JobScope(){owner=saved;}
};
}
#endif

#if defined(TH_PRESENTATION_AUDIT)
#define TH08_RNG_EVENT(name,category,arg0,arg1) th08::rng_audit::EventScope name(th08::rng_audit::Category::category,arg0,arg1)
#define TH08_RNG_SLOT(slot) th08::rng_audit::SlotScope rng_slot_scope(slot)
#define TH08_RNG_ENEMY(phase) rng_enemy_snapshot(th08::rng_audit::EnemyPhase::phase)
#else
#define TH08_RNG_EVENT(name,category,arg0,arg1)
#define TH08_RNG_SLOT(slot)
#define TH08_RNG_ENEMY(phase)
#endif
