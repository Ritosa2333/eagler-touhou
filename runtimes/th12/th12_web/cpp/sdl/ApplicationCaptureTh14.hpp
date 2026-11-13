#pragma once
#include "../game/AnmManager.hpp"
namespace th11::sdl {
struct ImmediateCaptureSourceTh14 {bool text_intermediate=false;i32 rectangle[4]{};};
template<class Session> ImmediateCaptureSourceTh14 th14_immediate_capture_source(const Session& session){
 // Native448bf0 tests GUI+194 after both448d80 Pause and449d30 death.
 // It does not test the public session phase. Normal Pause remains unchanged.
 const bool dialogue=session.battle&&session.battle->dialogue&&session.battle->dialogue->active;
 return {dialogue,{dialogue?128:32,16,dialogue?512:416,464}};
}
template<class Session> AnmResource* th14_deferred_capture_target(Session& session,const DeferredCaptureRequestTh14& request){
 // Both actual44be40 callers use text52 from Supervisor+71c/slot0.
 // Do not invent a generic owner map for normalized player/ANM file tags.
 if(request.file_index!=0||request.texture_index<0)return nullptr;
 auto& text=session.resources.core.text;
 return u32(request.texture_index)<text.textures.size()?&text:nullptr;
}
template<class Session,class Graphics,class Render>
void th14_consume_deferred_captures(DeferredCaptureQueueTh14& queue,Session& session,Graphics& graphics,Render& renderer){
 queue.consume([&](const auto& request){
  auto* resource=th14_deferred_capture_target(session,request);if(!resource)return;
  const u32 target=graphics.texture(*resource,u32(request.texture_index));if(!target)return;
  i32 from[4]={request.source[0],request.source[1],request.source[0]+request.source[2],request.source[1]+request.source[3]};
  i32 to[4]={request.destination[0],request.destination[1],request.destination[0]+request.destination[2],request.destination[1]+request.destination[3]};
  renderer.flush();
  // Return values do not keep a request alive. This is request retirement,
  // not successful GPU completion or a resource-lifetime fence.
  if(graphics.backend.resample(Graphics::screen,from,target,to,nullptr,0,0))graphics.changed(target);
  // Material-cache invalidation is separate from the successful target write.
  renderer.invalidate();
 });
}
}
