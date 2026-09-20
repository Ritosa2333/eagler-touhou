#pragma once
#include "EffectState.hpp"
#include "AnmRenderer.hpp"
namespace th08 {
class EffectGeometry {
    AnmRenderer& renderer;
public:
    enum class Mode:u8 { Circle,Ellipse,Wave };
    static constexpr Mode mode(float height,float frequency){return height==0?Mode::Circle:frequency==0?Mode::Ellipse:Mode::Wave;}
    // Geometry fields are blendable only when the complete interpolation path
    // remains in one topology. Crossing height zero would otherwise synthesize
    // the circle branch even when neither endpoint is circular.
    static constexpr bool interpolation_preserves_topology(float before_height,float before_frequency,i32 before_segments,float current_height,float current_frequency,i32 current_segments){
        const auto before=mode(before_height,before_frequency),current=mode(current_height,current_frequency);
        return before_segments==current_segments&&before==current&&(before==Mode::Circle||(before_height>0)==(current_height>0));
    }
    Vec2 arcade{32,16};bool invalid=false;
    explicit EffectGeometry(AnmRenderer& r):renderer(r){}
    static i32 initialize(EffectState&,EffectDraw callback,bool alternative=false);
    static void release(EffectState&);
    i32 prepare(EffectState&);
    i32 draw(EffectState&);
};
static_assert(EffectGeometry::interpolation_preserves_topology(0,6,64,0,0,64));
static_assert(EffectGeometry::interpolation_preserves_topology(8,6,64,1,6,64));
static_assert(!EffectGeometry::interpolation_preserves_topology(1,6,64,0,0,64));
static_assert(!EffectGeometry::interpolation_preserves_topology(-1,6,64,1,6,64));
static_assert(!EffectGeometry::interpolation_preserves_topology(1,0,64,1,6,64));
static_assert(!EffectGeometry::interpolation_preserves_topology(1,6,32,1,6,64));
}
