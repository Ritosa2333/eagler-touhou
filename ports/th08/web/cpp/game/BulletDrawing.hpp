#pragma once
#include "PresentationVisual.hpp"
#include "BulletState.hpp"
#include <array>
namespace th08 {
struct BulletDrawingActions {
    virtual ~BulletDrawingActions()=default;
    virtual void tint(u32 color)=0;
    virtual void clear_tint()=0;
    virtual void items()=0;
    virtual void effects()=0;
    virtual void draw(AnmVm& vm)=0;
};
// Original 00432b50 / 00432f20, including the six linked drawing layers,
// laser origins and the deathbomb tint shared with the item/effect passes.
class BulletDrawing {
    presentation::SnapshotMarker presentation_marker;
    u32 last_submitted_opacity_factor=128,previous_submitted_opacity_factor=128;
    BulletManagerState& state;BulletDrawingActions& actions;
    struct BulletPresentation {Vec3 position{};float angle=0;int age=0;u16 state=0;i16 script=-1, sprite=-1;bool active=false;presentation::VisualSample visual;};
    struct LaserPresentation {Vec3 position{};float angle=0,start_offset=0,end_offset=0,scale_x=1,scale_y=1;int age=0;u8 state=0;i16 script=-1,color=0;bool active=false;presentation::VisualSample visual[2];};
    std::array<BulletPresentation,1537> previous_bullets{};std::array<LaserPresentation,256> previous_lasers{};
    void laser(LaserState&,const Vec2&);
public:
    BulletDrawing(BulletManagerState& s,BulletDrawingActions& a):state(s),actions(a){}
    void snapshot();
    void bullet(BulletState&,const Vec2&,u32 previous_opacity_factor=128,u32 current_opacity_factor=128);
    bool draw(u32 game_flags,const Vec2& arcade);
};
}
