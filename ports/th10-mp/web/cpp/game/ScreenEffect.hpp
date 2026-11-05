#pragma once
#include "Camera.hpp"
#include "UpdateChain.hpp"
#include "Rng.hpp"
namespace th10 {
struct ScreenEffectEnvironment;
enum class ScreenEffectKind : u32 {
    RevealScreen,ShakeLinear,HidePlayfield,RevealPlayfield,FlashPlayfield,
    HideScreen,DimScreen,DimPlayfield,ShakeEnvelope
};
struct ScreenEffect {
    u32 flags,state;
    UpdateChainEntry* update_entry;
    UpdateChainEntry* draw_entry;
    ScreenEffectKind kind;
    u32 reserved_014;
    i32 alpha,duration,parameters[3];
    u32 releasing;
    Timer timer;u32 timer_flags;
    void initialize() noexcept;
    void start(ScreenEffectKind type,i32 frames,i32 first,i32 second,i32 third,i32 layer,ScreenEffectEnvironment& environment);
    static ScreenEffect* create(ScreenEffectKind type,i32 frames,i32 first,i32 second,i32 third,i32 layer,ScreenEffectEnvironment& environment);
    void release(ScreenEffectEnvironment& environment);
    void fade_out(float* rate) noexcept;
    i32 update(ScreenEffectEnvironment& environment);
    i32 reveal(ScreenEffectEnvironment& environment);
    i32 hide(ScreenEffectEnvironment& environment);
    i32 dim();
    i32 flash(ScreenEffectEnvironment& environment);
    i32 shake_linear(ScreenEffectEnvironment& environment);
    i32 shake_envelope(ScreenEffectEnvironment& environment);
    i32 draw(ScreenEffectEnvironment& environment);
    i32 draw_region(ScreenEffectEnvironment& environment,bool fullscreen,bool set_viewport,bool flash);
};
static_assert(sizeof(ScreenEffect)==0x44&&offsetof(ScreenEffect,timer)==0x30);
struct ScreenRect {float left,top,right,bottom;};
struct ScreenEffectEnvironment {
    float* rate;
    const u32* quitting;
    const u32* controller_flags;
    Rng* random;
    Vec2* camera_offset;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken update_callbacks[9],draw_callbacks[9],delete_callback;
    virtual ScreenEffect* allocate()=0;
    virtual void destroy(ScreenEffect* effect)=0;
    virtual i32 presentation_alpha(const ScreenEffect& effect)=0;
    virtual void fullscreen_viewport()=0;
    virtual void rectangle(const ScreenRect& bounds,u32 color)=0;
};
// Diffuse-only screen overlays use the same geometry and cache invalidation for
// single-color and four-corner gradients.
void draw_screen_rectangle(const ScreenRect& bounds,const u32 colors[4],AnmManager** manager,AnmRenderEnvironment& environment);
}
