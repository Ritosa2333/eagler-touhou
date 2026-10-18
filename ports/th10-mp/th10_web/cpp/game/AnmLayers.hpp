#pragma once
#include "Camera.hpp"
#include "AnmFrame.hpp"
namespace th10 {
struct AnmLayers {
    Camera& world;
    Camera& ui;
    Camera*& active;
    u32& screen_space;
    u32& fog_enabled;
    CameraEnvironment& platform;
    AnmFrameEnvironment& frame;
    i32 draw(AnmManager& manager,u32 layer);
};
}
