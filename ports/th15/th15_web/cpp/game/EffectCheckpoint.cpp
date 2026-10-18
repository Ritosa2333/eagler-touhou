#include "EffectCheckpoint.hpp"
namespace th15 {
bool EffectCheckpoint::capture(){
    error.clear();available=false;cursor=effects.cursor;
    for(u32 i=0;i<handles.size();i++){handles[i]=effects.handles[i]?animations.capture(effects.handles[i],registry):0;if(!animations.error.empty()){error=animations.error;return false;}}
    available=true;return true;
}
bool EffectCheckpoint::restore(){
    error.clear();if(!available){error="No saved effect chapter";return false;}effects.cursor=cursor;
    for(u32 i=0;i<handles.size();i++){auto* live=registry.find(effects.handles[i]);if(live&&!registry.destroy_tree(*live)){error=registry.error;return false;}effects.handles[i]=handles[i]?animations.restore(handles[i],registry):0;if(!animations.error.empty()){error=animations.error;return false;}}
    return true;
}
}
