#pragma once
#include "background.hpp"
namespace th20::source::background {
void normalize_camera_vector(float (&output)[3],const float (&input)[3]); //45cf10
int update_objects(Background&); //4739e0
void update_mesh_distortion(ScriptState&,const float* rate); //4722e0
}
