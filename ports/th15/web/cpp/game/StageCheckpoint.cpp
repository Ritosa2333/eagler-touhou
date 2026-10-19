#include "StageCheckpoint.hpp"
namespace th15 {
bool StageCheckpoint::capture(){
    error.clear();if(!scene.initialized){error="Background checkpoint before initialization";return false;}
    state=scene.script.state;direction=scene.view_direction;primitives.resize(scene.primitives.size());
    for(u32 i=0;i<embedded.size();i++)embedded[i].copy_checkpoint(scene.embedded[i]);
    for(u32 i=0;i<primitives.size();i++)primitives[i].copy_checkpoint(scene.primitives[i]);
    available=true;return true;
}
bool StageCheckpoint::restore(){
    error.clear();if(!available||primitives.size()!=scene.primitives.size()){error="No matching background checkpoint";return false;}
    scene.script.state=state;scene.view_direction=direction;
    for(u32 i=0;i<embedded.size();i++)scene.embedded[i].copy_checkpoint(embedded[i]);
    for(u32 i=0;i<primitives.size();i++)scene.primitives[i].copy_checkpoint(primitives[i]);
    return true;
}
}
