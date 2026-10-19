#pragma once
#include "TitleScene.hpp"
#include "ImageRenderer.hpp"
namespace th075 {
struct SceneFadeSignals {virtual ~SceneFadeSignals()=default;virtual void render_scene()=0;virtual void present_scene()=0;};
class SceneFade {
public:
 i32 count=-1;u16 style=0;
 u16 advance(u16 code,bool animated,SceneFadeSignals&);
 void render(ImageRenderer&,u32 captureTexture)const;
};
}
