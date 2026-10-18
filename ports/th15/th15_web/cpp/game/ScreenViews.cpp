#include "ScreenViews.hpp"
#include <cmath>
namespace th15 {
ScreenView screen_view(const GraphicsViewport& viewport,float fov,Vec2 offset)noexcept{
 ScreenView out;out.viewport=viewport;out.offset=offset;
 const float x=float(float(viewport.x)+float(float(viewport.width)*.5f)),y=float(float(viewport.y)+float(float(viewport.height)*.5f));
 const float half=float(fov*.5f),distance=float(float(viewport.height>>1)/float(std::tan(double(half))));
 out.camera.view=scene_look_at({x,y,distance},{x,y,0},{0,-1,0});out.camera.projection=scene_perspective(fov,float(float(viewport.width)/float(viewport.height)),1,10000);return out;
}
void ScreenViews::configure(const AnmEnvironment& environment){const float scale=environment.resolution_scale;const auto scaled=[&](float value){return u32(truncate_int(float(value*scale)));};
 views[2]=screen_view({0,0,environment.screen_width,environment.screen_height,0,1},fov);
 views[0]=screen_view({scaled(32),scaled(16),scaled(384),scaled(448),0,1},fov);
 views[1]=screen_view({scaled(128),scaled(16),scaled(384),scaled(448),0,1},fov);
 views[3]=screen_view({u32(truncate_int(float(float(float(environment.screen_width)-408.f)*.5f))),u32(truncate_int(float(float(float(environment.screen_height)-472.f)*.5f))),408,472,0,1},fov);
}
bool ScreenViews::camera(DrawCamera index,bool refresh){auto& view=views[u32(index)];if(refresh)view=screen_view(view.viewport,fov,view.offset);renderer.set_viewport(view.viewport);renderer.set_camera(view.camera);renderer.offset=view.offset;return true;}
}
