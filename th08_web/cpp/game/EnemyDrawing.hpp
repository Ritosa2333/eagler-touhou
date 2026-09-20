#pragma once
#include "EclVm.hpp"
#include "PresentationVisual.hpp"
#include <unordered_map>
namespace th08 {
struct EnemyDrawActions {
    virtual ~EnemyDrawActions()=default;
    virtual Vec3 position(EclVm&)=0;
    virtual float direction(EclVm&)=0;
    virtual void sprite(AnmVm&)=0;
    virtual void strip(AnmVm&,const SpriteVertex*,i32 count)=0;
    virtual void visual(EclVm&,u32,AnmVm&){}
    virtual void trail(EclVm&,EnemyTrail&){}
    virtual bool discrete_motion(EclVm&){return false;}
};
// Original layer traversal and trail tessellation (0042e140).
bool draw_enemy_layers(EclVm* const* layers,i32 first,i32 last,const Vec2& offset,EnemyDrawActions&);
class EnemyDrawing:private EnemyDrawActions {
    AnmRenderer& renderer;
    struct TrailSample {Vec3 position{};float angle=0;};
    struct PresentationSample {Vec3 position{};float direction=0;i32 age=0,subroutine=-1;bool active=false;presentation::VisualSample visual[3];std::vector<TrailSample> trail;u8 trail_flags=0;i16 trail_step=0;};
    std::unordered_map<EclVm*,PresentationSample> previous;
    presentation::SnapshotMarker presentation_marker;
    Vec3 position(EclVm&)override;
    float direction(EclVm&)override;
    void visual(EclVm&,u32,AnmVm&)override;
    void trail(EclVm&,EnemyTrail&)override;
    bool discrete_motion(EclVm&)override;
    void sprite(AnmVm& vm)override{renderer.draw_2d(vm);}
    void strip(AnmVm& vm,const SpriteVertex* vertices,i32 count)override{renderer.draw_vertices(vm,vertices,count);}
public:
    explicit EnemyDrawing(AnmRenderer& renderer):renderer(renderer){}
    void snapshot(EclVm* const* layers);
#if defined(TH_PRESENTATION_AUDIT)
    const float* audit_presentation_sample(uintptr_t object,u32 index)const;
#endif
    bool draw(EclVm* const* layers,i32 first,i32 last,const Vec2& offset){return draw_enemy_layers(layers,first,last,offset,*this);}
    bool low(EclVm* const* layers,u32 flags,const Vec2& offset){const bool tinted=flags&1024;if(tinted){renderer.mix_enabled=true;renderer.mix_color=0xfff01010;}const bool result=draw(layers,2,4,offset);if(tinted){renderer.mix_enabled=false;renderer.mix_color=0x80808080;}return result;}
};
}
