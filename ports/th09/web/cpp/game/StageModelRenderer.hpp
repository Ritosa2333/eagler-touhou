#pragma once
#include "Camera.hpp"
#include <vector>
namespace th09 {
struct StageDrawServices {
    Viewport viewport;Camera camera;BackgroundCamera first_camera;bool capture_positions=false;std::vector<Vec3> captured;
    virtual ~StageDrawServices()=default;
    virtual void begin_models(const Camera&)=0;
    virtual void fog_enabled(bool)=0;
    virtual void world_sprite(AnmVm&)=0;
    virtual void screen_sprite(AnmVm&)=0;
    virtual void screen_quad(AnmVm&,const SpriteVertex*)=0;
};
class StageModelRenderer {
public:
    static void draw(Background&,i32 layer,StageDrawServices&);
};
}
