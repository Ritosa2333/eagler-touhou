#pragma once
#include "ApplicationState.hpp"
namespace th10 {
struct ApplicationCameraEnvironment {
    virtual void configure_world(Camera& camera)=0;
    virtual i32 set_viewport(void* device,const CameraViewport& viewport)=0;
};
i32 select_world_camera(ApplicationState& application,u32 index,ApplicationCameraEnvironment& environment);
}
