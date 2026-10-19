#pragma once
#include "EffectTransforms.hpp"
#include "EffectSpace.hpp"
#include "EffectGeometry.hpp"
#include "EffectBomb.hpp"
#include "AnmExecutor.hpp"
#include "GameValues.hpp"
#include "PresentationVisual.hpp"
#include <array>
namespace th08 {
struct EffectPoolState {
    i32 cursor;u32 reserved4;i32 active_count;u32 reserved0c[4];
    EffectState objects[654],sentinels[5];EffectState* tails[5];i32 frames;AnmLoaded* base_animation;AnmLoaded* stage_animation;
    EffectPoolState(){std::memset(this,0,sizeof(*this));}
};
static_assert(sizeof(EffectPoolState)==0x8b05c&&offsetof(EffectPoolState,objects)==0x1c);
static_assert(offsetof(EffectPoolState,sentinels)==0x89f5c&&offsetof(EffectPoolState,frames)==0x8b050);
struct EffectDefinition {i32 script;EffectUpdate update,initialize;};
class EffectSystem {
    AnmExecutor& anm;AnmRenderer& renderer;GameValues& values;u16& replay_flags;
    struct PresentationSample {
        Vec3 position{},center{};float radius=0,angle=0,width=0,height=0,angle_y=0,frequency=0;
        i32 age=0,segments=0;u8 kind=0;bool active=false;presentation::VisualSample visual;Vec3 projected_offset{};
    };
    std::array<PresentationSample,654> presentation_previous{};
    presentation::SnapshotMarker presentation_marker;
    Vec3 presentation_position(const EffectState&)const;
    void presentation_geometry(const EffectState& source,EffectState& draw)const;
    void presentation_visual(const EffectState& source,EffectState& draw)const;
    void begin(EffectState&,i32 kind,u32 color,bool depth);
    void initialize(EffectState&,i32 kind);
    void draw_list(u32 index,float depth,bool offset_before_depth);
    static void projected(AnmVm&,Vec3&,void*);
public:
    EffectPoolState& state;EffectEnvironment& environment;Vec2 arcade{32,16};bool paused=false,invalid=false;u8 quality=2;
    EffectTransforms transforms;EffectSpace space;EffectGeometry geometry;EffectBomb bomb;
    EffectSystem(EffectPoolState&,EffectEnvironment&,AnmExecutor&,AnmRenderer&,Rng&,ScreenEffects&,DamageRegions&,GameValues&,u16& replay_flags);
    ~EffectSystem(){release();}
    static const EffectDefinition& definition(u32 kind);
    void reset();void release();
    // Capture the authoritative end state from the previous 60 Hz tick before
    // any owner mutates shared effects during the next tick. Some effects,
    // notably the spell-card boss ring, are driven by SpellSystem before the
    // EffectSystem calculation job runs, so snapshotting inside update() is too
    // late for presentation interpolation.
    void snapshot_presentation();
    // Custom owner callbacks do not pass through draw_list(), but they still
    // need the same lifecycle-gated presentation endpoint as ordinary effects.
    EffectState presentation_copy(const EffectState&)const;
    EffectState* spawn(i32 kind,Vec3 position,i32 count,u32 color,const Vec3* parameters=nullptr);
    EffectState* fixed(i32 kind,Vec3 position,i32 slot,u32 color,const Vec3* parameters=nullptr);
    EffectState* overlay(i32 kind,Vec3 position,i32 count,u32 color);
    EffectState* group(i32 slot){return slot>=0&&slot<13?&state.objects[640+slot]:nullptr;}
    void shift_glows(const Vec3& offset);
    JobResult update();JobResult draw();JobResult draw_alternative();JobResult draw_background();
#if defined(TH_PRESENTATION_AUDIT)
    const float* audit_presentation_sample(uintptr_t object)const;
#endif
};
}
