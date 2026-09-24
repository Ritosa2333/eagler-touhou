#pragma once
#include "AnmRenderer.hpp"
#include "Chain.hpp"
#include "Rng.hpp"
#include <unordered_map>
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include <array>
#include <eagler/netplay/RollbackJournal.hpp>
#endif
namespace th08 {
enum class ScreenEffectType:i32 { FadeIn,Shake,ArcadeFadeOut,Flash,FadeOut,MenuFullFade,MenuArcadeFade,EnvelopeShake };
struct ScreenEffectState {
    ScreenEffectType type=ScreenEffectType::FadeIn;
    ChainElement* calculation=nullptr;
    ChainElement* drawing=nullptr;
    u32 unused=0;
    i32 alpha=0,duration=0;
    // Fade: RGB color in a. Shake: start/end amplitude in a/b.
    // Flash: repetition count in a and ARGB in b.
    // EnvelopeShake: amplitude in duration; attack/hold/release in a/b/c.
    i32 a=0,b=0,c=0,phase=0;
    Timer timer;
};
static_assert(sizeof(ScreenEffectState)==0x34);
struct ScreenEffectContext {
    FrameTiming timing;
    bool paused=false,retry=false,frozen=false,shake_disabled=false,terminating=false;
    i32 transition_state=2;
};
class ScreenEffects {
public:
    ScreenEffectContext context;
    ScreenEffects(Chain& chain,AnmRenderer& renderer,Rng& random):chain(chain),renderer(renderer),random(random){}
    ~ScreenEffects();
    void clear();
    ScreenEffectState* create(ScreenEffectType type,i32 duration,i32 a,i32 b,i32 c,i32 draw_priority);
    void remove(ScreenEffectState* state);
    JobResult calculate(ScreenEffectState& state);
    JobResult draw(ScreenEffectState& state);
    u32 active_count()const noexcept{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        u32 count=0;for(const auto& entry:instances)count+=entry.occupied?1u:0u;return count;
#else
        return active.size();
#endif
    }
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    static constexpr u32 MaxInstances=128;
    bool invalid()const{return allocation_failed;}
    // Includes chain links (exact duplicate touches are allowed). A separately
    // owned dynamic chain node must be retired/pinned by its owner first.
    bool capture_rollback(Netplay::RollbackJournal&);
    void rebase_after_rollback(){presentation_previous.clear();}
#endif
private:
    struct Instance {ScreenEffectState state;ScreenEffects* owner=nullptr;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        ChainElement calculation,drawing;bool occupied=false;
#endif
    };
    Chain& chain;
    AnmRenderer& renderer;
    Rng& random;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::array<Instance,MaxInstances> instances;
    bool allocation_failed=false;
#else
    std::vector<Instance*> active;
#endif
    struct PresentationSample {i32 alpha=0,timer=0,phase=0,a=0;ScreenEffectType type=ScreenEffectType::FadeIn;};
    std::unordered_map<ScreenEffectState*,PresentationSample> presentation_previous;
    void shake(float amplitude);
    static JobResult calculate_callback(void*);
    static JobResult draw_callback(void*);
    static i32 added_callback(void*);
    static i32 deleted_callback(void*);
};
}
