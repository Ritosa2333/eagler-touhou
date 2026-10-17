#include "EffectCheckpoint.hpp"
#include "AnmManager.hpp"
namespace th15 {
bool EffectCheckpoint::write_file(std::vector<u8>& out,AnmManager& manager){
    error.clear();out.clear();if(!available){error="No effect checkpoint to serialize";return false;}out.resize(4);u32 count=0;
    for(const u32 handle:handles)if(handle){if(!animations.write_tree(handle,manager,out)){error=animations.error;out.clear();return false;}count++;}
    std::memcpy(out.data(),&count,4);return true;
}
bool EffectCheckpoint::read_file(const u8* data,u32 size,u32& consumed,AnmManager& manager){
    error.clear();consumed=0;if(!data||size<4){error="Truncated effect checkpoint count";return false;}u32 count;std::memcpy(&count,data,4);if(count>handles.size()){error="Effect checkpoint count exceeds ring capacity";return false;}
    const auto mark=animations.mark();std::array<u32,1024> next{};u32 offset=4;
    for(u32 i=0;i<count;i++){u32 used=0;next[i]=animations.read_tree(data+offset,size-offset,used,manager);if(!next[i]){error=animations.error;animations.rollback(mark);return false;}offset+=used;}
    // Original 413370 packs active entries into the front of the saved ring;
    // its current saved cursor is outside this module's file record.
    handles=next;available=true;consumed=offset;return true;
}
}
