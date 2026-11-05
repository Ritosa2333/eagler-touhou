#include "ApplicationCamera.hpp"
namespace th10 {
i32 select_world_camera(ApplicationState& application,u32 index,ApplicationCameraEnvironment& env){
    if(index>1)return -1;application.active_camera=index?&application.ui_camera:&application.world_camera;
    env.configure_world(*application.active_camera);const i32 result=env.set_viewport(application.device,application.active_camera->viewport);application.screen_space=index;return result;
}
}
