#include "ItemCheckpoint.hpp"
namespace th15 {
namespace {void copy(std::unique_ptr<AnmVm>& out,const std::unique_ptr<AnmVm>& source){if(source){if(!out)out=std::make_unique<AnmVm>();*out=*source;}else out.reset();}}
bool ItemCheckpoint::capture(){
    error.clear();scale=manager.motion_scale;
    for(u32 i=0;i<slots.size();i++){const auto& in=manager.slots[i];auto& out=slots[i];out.state=in.state;out.arrow_ended=in.arrow_ended;copy(out.body,in.body);copy(out.arrow,in.arrow);}
    available=true;return true;
}
bool ItemCheckpoint::restore(){
    error.clear();if(!available){error="No saved item chapter";return false;}manager.motion_scale=scale;for(auto& free:manager.free_slots)free.clear();
    for(u32 i=0;i<slots.size();i++){const auto& in=slots[i];auto& out=manager.slots[i];out.state=in.state;out.arrow_ended=in.arrow_ended;copy(out.body,in.body);copy(out.arrow,in.arrow);if(out.state.state==0)manager.free_slots[i>=ItemManager::ordinary_count].push_back(i);}
    // Request, active-density and stamp counters are outside the original
    // copied item block. The global alternating-piece counter belongs to run
    // progress, restored by ChapterCheckpoint, rather than to this pool.
    return true;
}
}
