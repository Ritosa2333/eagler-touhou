#include "SceneCapture.hpp"
namespace th15 {
bool SceneCapture::fail(const char* reason){if(error.empty())error=reason;return false;}
bool SceneCapture::destination(u32 handle,Request& out){
 auto* vm=animations.registry.find(handle);if(!vm||!vm->sprite||!vm->sprite_resource)return fail("Screenshot animation has no sprite");
 const auto& sprite=*vm->sprite;out.resource=vm->sprite_resource;out.texture=sprite.texture;
 out.destination={truncate_int(sprite.x),truncate_int(sprite.y),truncate_int(float(sprite.x+sprite.width)),truncate_int(float(sprite.y+sprite.height))};return true;
}
bool SceneCapture::capture(u32& handle,bool results){
 if(!error.empty())return false;
 if(!results&&!animations.retire(handle))return fail(animations.error.c_str());
 handle=animations.create_overlay(text_bank,52);if(!handle)return fail(animations.error.c_str());
 const float scale=environment.resolution_scale;
 if(results)return queue(handle,truncate_int(float(32.f*scale)),truncate_int(float(16.f*scale)),truncate_int(float(384.f*scale)),truncate_int(float(448.f*scale)));
 Request r;if(!destination(handle,r))return false;const float half=float(float(scale*384.f)*.5f),height=float(scale*448.f);
 r.source={truncate_int(float(float(environment.screen_offsets[0])-half)),environment.screen_offsets[1],truncate_int(float(float(environment.screen_offsets[0])+half)),truncate_int(float(float(environment.screen_offsets[1])+height))};
 return services.copy(pause_source,r.source,*r.resource,r.texture,r.destination)||fail("Pause GPU capture failed");
}
bool SceneCapture::queue(u32 handle,i32 x,i32 y,i32 width,i32 height){
 if(!error.empty())return false;Request r;if(!destination(handle,r))return false;
 auto* vm=animations.registry.find(handle);const auto& sprite=*vm->sprite;
 // The deferred helper truncates the origin and extent separately.
 r.destination={truncate_int(sprite.x),truncate_int(sprite.y),wrapping_add(truncate_int(sprite.x),truncate_int(sprite.width)),wrapping_add(truncate_int(sprite.y),truncate_int(sprite.height))};
 r.source={x,y,wrapping_add(x,width),wrapping_add(y,height)};
 for(auto& slot:requests)if(!slot.resource){slot=r;break;}return true;
}
bool SceneCapture::finish_frame(){
 if(!error.empty())return false;for(auto& r:requests)if(r.resource){if(!services.copy(presentation_source,r.source,*r.resource,r.texture,r.destination))return fail("Result GPU capture failed");r=Request{};}return true;
}
}
