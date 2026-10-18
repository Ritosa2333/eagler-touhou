#pragma once
#include "background.hpp"
namespace th20::source::background {
void update_camera_motion(ScriptState&,const float* rate); //4751b0 camera-motion switch
void execute_script(ScriptState&,const float* rate);       //4751b0
namespace vm_environment {
void set_clear_color(std::uint32_t);                     //5c5b20
void assign_animation(ScriptState&,int index,int script);//438380/44c240
void reset_meshes(ScriptState&);                        //471210/4712d0
void interrupt_animations(Background&,std::uint32_t);     //477480/477450
void set_primary_rotation_flag(std::uint32_t);           //477790
}
}
