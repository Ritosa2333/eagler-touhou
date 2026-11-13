#pragma once
#include "AnmProjection.hpp"
#include "FogInterpolation.hpp"
namespace th11 {
// Native renderer camera, 0x118 bytes (42a810 / 42a970 / 42aae0).
struct SceneCamera {
    Vec3 position{},direction{},up{0,1,0},reserved24{},right{},eye_offset{};
    float fov=0.5235987901687622f;
    Matrix4 view{},projection{};
    GraphicsViewport viewport;
    u32 mode=0;
    Vec2 offset{};
    Vec3 animation_delta{};
    SceneFog fog;
#if PORT_TITLE == 13 || PORT_TITLE == 14
    Vec3 target_offset{};
#endif
#if PORT_TITLE == 14
    // Native +0x58/+0x5c retain the render dimensions from 4f7a64/68.
    u32 render_width=0,render_height=0;
#endif
    void perspective();
    void screen();
};
static_assert(sizeof(SceneCamera)==0x118+(kPortTitle==13||kPortTitle==14?12:0)+(kPortTitle==14?8:0));
#if PORT_TITLE == 14
// Original446400 publication phases. This scope observes one camera without
// changing SceneCamera's native-mapped layout or intercepting other cameras.
struct SceneCameraPerspectiveScopeTh14 {
 using Callback=bool(*)(void*,const SceneCamera&,u32);
 SceneCamera* camera;void* owner;Callback callback;SceneCameraPerspectiveScopeTh14* previous;
 bool running=false,failed=false;
 static inline SceneCameraPerspectiveScopeTh14* active=nullptr;
 SceneCameraPerspectiveScopeTh14(SceneCamera& c,void* o,Callback f):camera(&c),owner(o),callback(f),previous(active){active=this;}
 ~SceneCameraPerspectiveScopeTh14(){active=previous;}
 SceneCameraPerspectiveScopeTh14(const SceneCameraPerspectiveScopeTh14&)=delete;
 SceneCameraPerspectiveScopeTh14& operator=(const SceneCameraPerspectiveScopeTh14&)=delete;
 static bool checkpoint(const SceneCamera& c,u32 point){
  for(auto* scope=active;scope;scope=scope->previous){
   if(scope->camera!=&c)continue;
   if(scope->failed)return false;
   if(scope->running)return true;
   scope->running=true;scope->failed=!scope->callback(scope->owner,c,point);scope->running=false;
   return !scope->failed;
  }
  return true;
 }
};
#endif

}
