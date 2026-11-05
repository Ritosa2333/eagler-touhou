#include "manager.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../game_session/session.hpp"
namespace th20::source::progress {
void SaveManager::select_profile(int slot,int character,int index){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));
    if(slot<0||slot>=4||character<0||character>=2||index<0||index>=9)return;
    const auto table=game_session::session.player_table.field_1e0==4?0x148:0x128;
    write(current.metadata.bytes,table+character*16+slot*4,index);
    update_metadata_checksum(current.metadata,state::random_streams[1]);
}
bool SaveManager::extra_unlocked(int character,int index){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));verify_metadata();
    return read<std::uint32_t>(find_profile(current,character,index)->bytes,0x76f0)!=0;
}
std::uint32_t SaveManager::stone_count(unsigned index){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));verify_metadata();
    return read<std::uint32_t>(current.metadata.bytes,0x168+index*4);
}
std::uint32_t SaveManager::used_stone_count(unsigned index){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));verify_metadata();
    return read<std::uint32_t>(current.metadata.bytes,0x18c+index*4);
}
void SaveManager::set_used_stone_count(unsigned index,std::uint32_t value){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));
    write(current.metadata.bytes,0x18c+index*4,value);update_metadata_checksum(current.metadata,state::random_streams[1]);
}
void SaveManager::consume_stone(unsigned index){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));
    const auto used=read<std::uint32_t>(current.metadata.bytes,0x18c+index*4),available=read<std::uint32_t>(current.metadata.bytes,0x168+index*4);
    if(used<available)write(current.metadata.bytes,0x18c+index*4,used+1);
    // A saturated counter still changes the checksum salts and advances RNG.
    update_metadata_checksum(current.metadata,state::random_streams[1]);
}
}
