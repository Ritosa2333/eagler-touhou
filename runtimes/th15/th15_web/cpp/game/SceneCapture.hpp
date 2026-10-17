#pragma once
#include "ScreenCompositor.hpp"
namespace th15 {
struct SceneCaptureServices {
 virtual ~SceneCaptureServices()=default;
 virtual bool copy(const ScreenSurface& source,const std::array<i32,4>& source_rect,const AnmResource& destination,u32 texture,const std::array<i32,4>& destination_rect)=0;
};
// Pause captures the previous alternate surface immediately. Results queue a
// copy of the completed presentation surface, consumed before presenting it.
class SceneCapture {
 struct Request {const AnmResource* resource=nullptr;u32 texture=0;std::array<i32,4> source{},destination{};};
 AnmManager& animations;AnmEnvironment& environment;SceneCaptureServices& services;i32 text_bank;
 std::array<Request,4> requests{};bool fail(const char*);
 bool destination(u32,Request&);
public:
 ScreenSurface pause_source,presentation_source;std::string error;
 SceneCapture(AnmManager& a,AnmEnvironment& e,SceneCaptureServices& h,i32 bank):animations(a),environment(e),services(h),text_bank(bank){}
 bool capture(u32& handle,bool results);
 bool queue(u32 handle,i32 x,i32 y,i32 width,i32 height);
 bool finish_frame();
 u32 pending()const noexcept{u32 n=0;for(const auto& r:requests)if(r.resource)n++;return n;}
 void discard()noexcept{requests={};}
};
}
