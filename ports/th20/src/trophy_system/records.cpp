#include "trophy.hpp"
#include "../progress_state/manager.hpp"
#include <stdexcept>
namespace th20::source::trophy {
bool achieved(unsigned index){
    std::lock_guard lock(runtime::shared_locks().slot(20));progress::manager->verify_metadata();
    if(index>=128)throw std::out_of_range("Achievement index outside original128 records");
    return progress::manager->current.metadata.bytes[0x68+index]!=0;
}
void mark_achieved(unsigned index){
    std::lock_guard lock(runtime::shared_locks().slot(20));if(index>=128)throw std::out_of_range("Achievement index outside original128 records");
    auto& metadata=progress::manager->current.metadata;metadata.bytes[0x68+index]=1;progress::update_metadata_checksum(metadata,state::random_streams[1]);
}
}
