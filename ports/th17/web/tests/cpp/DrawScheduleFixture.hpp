#pragma once
#include "../../cpp/game/ScreenViews.hpp"
#include "../../cpp/game/StageDrawFrame.hpp"
struct DrawScheduleFixture final:AnmDrawServices {
    AnmManagerFixture& animations;AnmRendererFixture& target;ScreenViews views;FrameScheduler scheduler;std::vector<i32> events;
    AnmDrawSchedule layers;std::array<u32,43> handles{};
    DrawScheduleFixture(AnmManagerFixture& a,AnmRendererFixture& r):animations(a),target(r),views(r.renderer,a.environment),layers(scheduler,a.manager,r.renderer,r.graphics,*this){}
    bool camera(DrawCamera which,bool refresh)override{events.insert(events.end(),{i32(which),refresh});return views.camera(which,refresh);}
    bool populate(i32 bank,i32 script){
        for(u32 i=0;i<handles.size();i++){handles[i]=animations.manager.create(bank,script,i,i>=36?4:0);if(!handles[i])return false;}
        if(!animations.manager.update(false)||!animations.manager.update(true))return false;
        for(u32 i=0;i<handles.size();i++){auto* vm=animations.manager.registry.find(handles[i]);if(!vm)return false;vm->visual.flags=3;vm->visual.render_flags=0;vm->visual.color=0xff000000|i;vm->visual.secondary_color=vm->visual.color;}
        return true;
    }
};
API(draw_schedule_create) DrawScheduleFixture* draw_schedule_create(AnmManagerFixture* a,AnmRendererFixture* r){return new DrawScheduleFixture(*a,*r);}
API(draw_schedule_delete) void draw_schedule_delete(DrawScheduleFixture* p){delete p;}
API(draw_schedule_populate) u32 draw_schedule_populate(DrawScheduleFixture* p,i32 bank,i32 script){return p->populate(bank,script);}
API(draw_schedule_run) i32 draw_schedule_run(DrawScheduleFixture* p){p->events.clear();const i32 result=p->scheduler.draw();p->target.renderer.flush();return result;}
API(draw_schedule_events) const i32* draw_schedule_events(DrawScheduleFixture* p){return p->events.data();}
API(draw_schedule_event_count) u32 draw_schedule_event_count(DrawScheduleFixture* p){return p->events.size();}
API(draw_schedule_error) const char* draw_schedule_error(DrawScheduleFixture* p){return p->layers.error.c_str();}
API(screen_view_configure) void screen_view_configure(DrawScheduleFixture* p,u32 width,u32 height,float scale){p->animations.environment.screen_width=width;p->animations.environment.screen_height=height;p->animations.environment.resolution_scale=scale;p->views.configure(p->animations.environment);}
API(screen_view_viewport) const GraphicsViewport* screen_view_viewport(DrawScheduleFixture* p,u32 index){return &p->views.view(DrawCamera(index)).viewport;}
API(screen_view_matrix) const Matrix4* screen_view_matrix(DrawScheduleFixture* p,u32 index,u32 kind){auto& camera=p->views.view(DrawCamera(index)).camera;return kind?&camera.projection:&camera.view;}
