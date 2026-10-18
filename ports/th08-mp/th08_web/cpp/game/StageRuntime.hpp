#pragma once
#include "AnmLibrary.hpp"
#include "BackgroundScript.hpp"

namespace th08 {
// Resource-owning adapter for the original-verified background interpreter.
// Effects are requests for the application to consume, not replacement effects.
struct StageRequest {
    enum Kind:u32 { Moon=1,MoveEffects=2,Sparkle=3 };
    u32 kind;Vec3 position;
};
class StageRuntime final:private BackgroundActions {
    TextureStore textures;
    AnmLibrary animations{textures};
    AnmExecutor executor;
    BackgroundState state;
    BackgroundContext context;
    BackgroundScript script{state,context,executor,*this};
    AnmVm moon_vm{};
    std::vector<StageRequest> requests;
    bool loaded=false;
    AnmVm* moon() override;
    void move_effects(const Vec3& offset) override;
    void sparkle(const Vec3& position) override;
    bool integrity_failed() override{return false;}
public:
    explicit StageRuntime(Rng& random):executor(random){}
    void reset();
    // Slots: 0=text.anm; 4=the selected stage's background ANM.
    bool load_animation(i32 slot,const u8* bytes,u32 size);
    bool load(const u8* bytes,u32 size,i32 stage=0,bool practice=false);
    bool update(const FrameTiming& timing,bool paused=false,bool youkai=false);
    void interrupt(i32 label){if(loaded)state.pending_interrupt=label;}
    bool ready()const noexcept{return loaded;}
    const BackgroundState& status()const noexcept{return state;}
    const std::vector<StageRequest>& pending_requests()const noexcept{return requests;}
    TextureStore& texture_store()noexcept{return textures;}
};
}
