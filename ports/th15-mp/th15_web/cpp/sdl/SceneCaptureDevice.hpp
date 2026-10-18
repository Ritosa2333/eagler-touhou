#pragma once
#include "GraphicsDevice.hpp"
#include "../game/SceneCapture.hpp"
namespace th15::sdl {
class SceneCaptureDevice final:public SceneCaptureServices {
 GraphicsDevice& graphics;AnmRenderer& renderer;
public:
 SceneCaptureDevice(GraphicsDevice& g,AnmRenderer& r):graphics(g),renderer(r){}
 bool copy(const ScreenSurface& source,const std::array<i32,4>& from,const AnmResource& destination,u32 index,const std::array<i32,4>& to)override{
  renderer.flush();const u32 input=source.resource?graphics.texture(*source.resource,source.texture):GraphicsDevice::screen,output=graphics.texture(destination,index);
  return input&&output&&graphics.resample_surface(input,from.data(),output,to.data());
 }
};
}
