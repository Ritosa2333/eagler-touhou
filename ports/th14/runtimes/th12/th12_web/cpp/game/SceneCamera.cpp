#include "SceneCamera.hpp"
#include <cmath>
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
#include "SceneCameraMatrixWordsTh14.hpp"
#include "SceneCameraRightWordsTh14.hpp"
#endif
namespace th11 {
void SceneCamera::perspective(){
#if PORT_TITLE == 14
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,0))return;
#endif
    const auto add=[](Vec3 a,Vec3 b){return Vec3{float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};};
#if PORT_TITLE == 13 || PORT_TITLE == 14
    // 43ba20 uses the direction cached by the preceding Stage update.
    const auto eye=add(position,eye_offset);
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_view(view,GraphicsMath::look_at(eye,add(eye,reserved24),up));
#else
    view=GraphicsMath::look_at(eye,add(eye,reserved24),up);
#endif
#else
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_view(view,GraphicsMath::look_at(add(position,eye_offset),add(position,direction),up));
#else
    view=GraphicsMath::look_at(add(position,eye_offset),add(position,direction),up);
#endif
#endif
#if PORT_TITLE == 14
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,1))return;
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_projection(projection,GraphicsMath::perspective(fov,float(double(viewport.width)/viewport.height),30,kPortTitle==14?8000:1800));
#else
    projection=GraphicsMath::perspective(fov,float(double(viewport.width)/viewport.height),30,kPortTitle==14?8000:1800);
#endif
#if PORT_TITLE == 14
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,2))return;
    // Retail446539..44659d rounds each SSE product before subtraction.
    const auto& d=reserved24;
    const Vec3 axis={float(float(d.y*up.z)-float(d.z*up.y)),
        float(float(d.z*up.x)-float(d.x*up.z)),float(float(d.x*up.y)-float(d.y*up.x))};
    // Native446590 stores XY,44659d stores Z, then D3DX normalizes in place.
#if defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_right_xy(right,axis);
#else
    right.x=axis.x;right.y=axis.y;
#endif
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,3))return;
#if defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_right_word(right.z,axis.z);
#else
    right.z=axis.z;
#endif
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,4))return;
    // Native D3DX SSE stores normalized X, Y, Z separately. Preserve the
    // already-computed local result while the worker may read between stores.
    const auto normalized=GraphicsMath::normalize(right);
#if defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_right_word(right.x,normalized.x);
#else
    right.x=normalized.x;
#endif
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,6))return;
#if defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_right_word(right.y,normalized.y);
#else
    right.y=normalized.y;
#endif
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,7))return;
#if defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_right_word(right.z,normalized.z);
#else
    right.z=normalized.z;
#endif
    if(!SceneCameraPerspectiveScopeTh14::checkpoint(*this,5))return;
#else
    right=GraphicsMath::normalize(GraphicsMath::cross(kPortTitle==13||kPortTitle==14?reserved24:direction,up));
#endif
}
void SceneCamera::screen(){
#if PORT_TITLE == 14
    // Retail446250 includes viewport origins and uses the camera's own FOV.
    // The center keeps a floating half; only the distance uses height>>1.
    const float width=float(viewport.width),height=float(viewport.height);
    const float x=float(width*.5f+float(viewport.x)),y=float(height*.5f+float(viewport.y));
    const float tangent=float(std::tan(double(float(fov*.5f)))),distance=float(float(viewport.height>>1)/tangent);
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_view(view,GraphicsMath::look_at({x,y,distance},{x,y,0},{0,-1,0}));
#else
    view=GraphicsMath::look_at({x,y,distance},{x,y,0},{0,-1,0});
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_projection(projection,GraphicsMath::perspective(fov,float(width/height),1,10000));
#else
    projection=GraphicsMath::perspective(fov,float(width/height),1,10000);
#endif
#else
    const float width=float(viewport.width),height=float(viewport.height),x=float(double(width)*.5),y=float(double(height)*.5);
    const float tangent=float(std::tan(double(0.15707963705062866f))),distance=float(double(y)/tangent);
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_view(view,GraphicsMath::look_at({x,y,distance},{x,y,0},{0,-1,0}));
#else
    view=GraphicsMath::look_at({x,y,distance},{x,y,0},{0,-1,0});
#endif
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_WORKER)
    th14_publish_camera_projection(projection,GraphicsMath::perspective(0.3141592741012573f,float(double(width)/height),1,10000));
#else
    projection=GraphicsMath::perspective(0.3141592741012573f,float(double(width)/height),1,10000);
#endif
#endif
}
}
