#include "player_data.hpp"
#include "player_data_constants.hpp"
#include "../card_system/records.hpp"
#include "../runtime_core/runtime_core.hpp"
namespace th20::source::title {
DataUnlockState data_unlock_state;
void unlock_all_data(progress::SaveManager& manager,state::Random& random){
    std::lock_guard<std::recursive_mutex> guard(runtime::shared_locks().slot(20));
    for(int i=0;i<113;++i)card::increment_record(card::record(progress::fallback_profile(manager),i),0xc8,0);
    for(int character=0;character<2;++character)for(int profile=0;profile<9;++profile){
        auto& p=*progress::find_profile(manager.current,character,profile);progress::write(p.bytes,0x76f0,1u);
        for(int difficulty=0;difficulty<6;++difficulty)for(int stage=0;stage<8;++stage)p.bytes[0x7701+difficulty*0x90+stage*16]=1;
    }
    progress::update_metadata_checksum(manager.current.metadata,random);
}
}
