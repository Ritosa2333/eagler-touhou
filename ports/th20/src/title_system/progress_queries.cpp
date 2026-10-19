#include "progress_queries.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <stdexcept>
namespace th20::source::title {
bool character_extra_unlocked(progress::SaveManager& o,int character){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));o.verify_metadata();
    for(int i=0;i<8;++i)if(o.extra_unlocked(character,i))return true;return false;
}
bool any_extra_unlocked(progress::SaveManager& o){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));o.verify_metadata();return character_extra_unlocked(o,0)||character_extra_unlocked(o,1);}
bool notice_pending(progress::SaveManager& o){o.verify_metadata();return static_cast<std::int32_t>(o.field_124280)>0;}
int pop_notice(progress::SaveManager& o){
    std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));--o.field_124280;
    if(o.field_124280>15)throw std::out_of_range("invalid array<T, N> subscript"); //484980->428c80
    const auto result=o.fields_124284[o.field_124280];progress::update_metadata_checksum(o.current.metadata,state::random_streams[1]);return static_cast<int>(result);
}
int profile_clear_count(progress::SaveManager& o,int difficulty,int character,int index){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));o.verify_metadata();return progress::read<int>(progress::find_profile(o.current,character,index)->bytes,0x76d4+difficulty*4);}
bool character_all_cleared(progress::SaveManager& o,int difficulty,int character){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));o.verify_metadata();for(int i=0;i<8;++i)if(profile_clear_count(o,difficulty,character,i)==0)return false;return true;}
bool difficulty_all_cleared(progress::SaveManager& o,int difficulty){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));o.verify_metadata();return character_all_cleared(o,difficulty,0)&&character_all_cleared(o,difficulty,1);}
}
