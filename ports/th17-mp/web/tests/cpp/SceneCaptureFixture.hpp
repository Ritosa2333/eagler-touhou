#include "../../cpp/game/SceneCapture.hpp"
struct SceneCaptureFixture final:SceneCaptureServices {
 AnmManagerFixture& a;SceneCapture capture;std::vector<i32> calls;
 SceneCaptureFixture(AnmManagerFixture& v,i32 bank):a(v),capture(v.manager,v.environment,*this,bank){capture.pause_source={v.manager.resource(bank),3};}
 bool copy(const ScreenSurface& source,const std::array<i32,4>& from,const AnmResource& destination,u32 texture,const std::array<i32,4>& to)override{calls.push_back(source.resource?i32(source.texture):-1);calls.push_back(i32(texture));calls.insert(calls.end(),from.begin(),from.end());calls.insert(calls.end(),to.begin(),to.end());return true;}
};
API(scene_capture_create) SceneCaptureFixture* scene_capture_create(AnmManagerFixture* a,i32 bank){return new SceneCaptureFixture(*a,bank);}
API(scene_capture_delete) void scene_capture_delete(SceneCaptureFixture* p){delete p;}
API(scene_capture_run) bool scene_capture_run(SceneCaptureFixture* p,u32* handle,bool results){return p->capture.capture(*handle,results);}
API(scene_capture_queue) bool scene_capture_queue(SceneCaptureFixture* p,u32 handle,i32 x,i32 y,i32 w,i32 h){return p->capture.queue(handle,x,y,w,h);}
API(scene_capture_finish) bool scene_capture_finish(SceneCaptureFixture* p){return p->capture.finish_frame();}
API(scene_capture_pending) u32 scene_capture_pending(SceneCaptureFixture* p){return p->capture.pending();}
API(scene_capture_calls) i32* scene_capture_calls(SceneCaptureFixture* p){return p->calls.data();}
API(scene_capture_count) u32 scene_capture_count(SceneCaptureFixture* p){return p->calls.size();}
API(scene_capture_reset) void scene_capture_reset(SceneCaptureFixture* p){p->calls.clear();p->capture.discard();}
API(scene_capture_error) const char* scene_capture_error(SceneCaptureFixture* p){return p->capture.error.c_str();}
API(scene_capture_configure) void scene_capture_configure(SceneCaptureFixture* p,float scale,i32 x,i32 y){p->a.environment.resolution_scale=scale;p->a.environment.screen_offsets[0]=x;p->a.environment.screen_offsets[1]=y;}
