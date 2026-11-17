#include "../../cpp/game/ScreenCompositor.hpp"
struct ScreenCompositorFixture {
 AnmManagerFixture& animation;AnmRendererFixture& render;FrameScheduler scheduler;ScreenViews views;ScreenCompositor compositor;
 ScreenCompositorFixture(AnmManagerFixture& a,AnmRendererFixture& r):animation(a),render(r),views(r.renderer,a.environment),compositor(scheduler,a.manager,a.environment,r.renderer,r.graphics,views){r.graphics.targets_enabled=true;compositor.main={a.manager.resource(5),0};compositor.alternate={a.manager.resource(5),1};}
};
API(screen_compositor_create) ScreenCompositorFixture* screen_compositor_create(AnmManagerFixture* a,AnmRendererFixture* r){return new ScreenCompositorFixture(*a,*r);}
API(screen_compositor_delete) void screen_compositor_delete(ScreenCompositorFixture* p){delete p;}
API(screen_compositor_capture) void screen_compositor_capture(ScreenCompositorFixture* p,u32 i,AnmVm* vm){p->compositor.captures[i]=vm;}
API(screen_compositor_configure) void screen_compositor_configure(ScreenCompositorFixture* p,u32 active,u32 color,u32 width,u32 height,float scale){p->compositor.active=active;p->compositor.clear_color=color;p->animation.environment.screen_width=width;p->animation.environment.screen_height=height;p->animation.environment.resolution_scale=scale;p->views.configure(p->animation.environment);p->render.graphics.target_events.clear();p->render.graphics.target=0;p->render.graphics.history.clear();p->render.graphics.submissions.clear();p->render.graphics.calls=0;}
API(screen_compositor_offset) void screen_compositor_offset(ScreenCompositorFixture* p,u32 index,float x,float y){p->views.view(DrawCamera(index)).offset={x,y};}
API(screen_compositor_view_offset) const Vec2* screen_compositor_view_offset(ScreenCompositorFixture* p,u32 index){return &p->views.view(DrawCamera(index)).offset;}
API(screen_compositor_pass) u32 screen_compositor_pass(ScreenCompositorFixture* p,u32 i){return p->compositor.pass(i);}
API(screen_compositor_events) const u32* screen_compositor_events(ScreenCompositorFixture* p){return p->render.graphics.target_events.data();}
API(screen_compositor_event_count) u32 screen_compositor_event_count(ScreenCompositorFixture* p){return p->render.graphics.target_events.size();}
API(screen_compositor_size) const Vec2* screen_compositor_size(ScreenCompositorFixture* p){return &p->compositor.playfield_size;}
API(screen_compositor_error) const char* screen_compositor_error(ScreenCompositorFixture* p){return p->compositor.error.c_str();}
API(screen_compositor_replacement) void screen_compositor_replacement(ScreenCompositorFixture* p,u32 index,u32 enabled){p->compositor.replacement[index]=enabled?std::function<bool()>([p,index](){p->render.graphics.target_event(3,index,nullptr);return true;}):std::function<bool()>();}

#include "../../cpp/game/StageDrawFrame.hpp"
struct StageDrawFrameFixture final:th17::StageDrawServices {
 th17::FrameScheduler scheduler;th17::ScreenViews views;th17::StageDrawState state;th17::StageDrawFrame draw;std::vector<i32> fades;
 StageDrawFrameFixture(StageSceneFixture& s,AnmManagerFixture& a,AnmRendererFixture& r):views(r.renderer,a.environment),draw(scheduler,s.scene,state,a.manager,r.renderer,r.graphics,views,*this){r.graphics.targets_enabled=true;}
 bool background_fade(i32 duration,i32 update_priority,i32 draw_priority)override{fades.insert(fades.end(),{duration,update_priority,draw_priority});return true;}
};
API(stage_draw_frame_create) StageDrawFrameFixture* stage_draw_frame_create(StageSceneFixture* s,AnmManagerFixture* a,AnmRendererFixture* r){return new StageDrawFrameFixture(*s,*a,*r);}
API(stage_draw_frame_delete) void stage_draw_frame_delete(StageDrawFrameFixture* p){delete p;}
API(stage_draw_frame_configure) void stage_draw_frame_configure(StageDrawFrameFixture* p,StageSceneFixture* s,const GraphicsViewport* viewport,u32 flags,u32 tint,i32 age,float rate,float x,float y){p->draw.viewport=*viewport;p->state.flags=flags;p->state.tint_color=tint;p->state.transition.set(age);p->draw.rate=rate;p->views.view(th17::DrawCamera::Playfield).offset={x,y};s->scene.draw_objects=(flags&1)!=0;p->fades.clear();}
API(stage_draw_frame_pass) u32 stage_draw_frame_pass(StageDrawFrameFixture* p,u32 pass){return p->draw.draw(pass);}
API(stage_draw_frame_state) const th17::StageDrawState* stage_draw_frame_state(StageDrawFrameFixture* p){return &p->state;}
API(stage_draw_frame_fades) const i32* stage_draw_frame_fades(StageDrawFrameFixture* p){return p->fades.data();}
API(stage_draw_frame_fade_count) u32 stage_draw_frame_fade_count(StageDrawFrameFixture* p){return p->fades.size();}
API(stage_draw_frame_error) const char* stage_draw_frame_error(StageDrawFrameFixture* p){return p->draw.error.c_str();}
#include "../../cpp/game/ScreenTargets.hpp"
struct ScreenTargetsFixture {th17::ScreenTargets targets;ScreenTargetsFixture(AnmManagerFixture& a,ScreenCompositorFixture& c,i32 bank):targets(a.manager,a.environment,c.views,c.compositor,bank){}};
API(screen_targets_create) ScreenTargetsFixture* screen_targets_create(AnmManagerFixture* a,ScreenCompositorFixture* c,i32 bank){return new ScreenTargetsFixture(*a,*c,bank);}
API(screen_targets_delete) void screen_targets_delete(ScreenTargetsFixture* p){delete p;}
API(screen_targets_prepare) u32 screen_targets_prepare(ScreenTargetsFixture* p){return p->targets.prepare();}
API(screen_targets_deactivate) void screen_targets_deactivate(ScreenTargetsFixture* p){p->targets.deactivate();}
API(screen_targets_capture) AnmVm* screen_targets_capture(ScreenTargetsFixture* p,u32 index){return index<4?&p->targets.captures[index]:nullptr;}
API(screen_targets_error) const char* screen_targets_error(ScreenTargetsFixture* p){return p->targets.error.c_str();}
