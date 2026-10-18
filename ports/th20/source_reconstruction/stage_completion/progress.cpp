#include "progress.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <stdexcept>
namespace th20::source::progress {
namespace {void check_flag(unsigned index){if(index>=64)throw std::out_of_range("invalid array<T, N> subscript");}}
bool unlock_flag(SaveManager& owner,unsigned index){std::lock_guard lock(runtime::shared_locks().slot(20));owner.verify_metadata();check_flag(index);return owner.current.metadata.bytes[0xe8+index]!=0;}
void notify_unlock(SaveManager& owner,unsigned index){std::lock_guard lock(runtime::shared_locks().slot(20));if(owner.field_124280>=16)throw std::out_of_range("invalid array<T, N> subscript");owner.fields_124284[owner.field_124280]=index;++owner.field_124280;check_flag(index);owner.current.metadata.bytes[0xe8+index]=1;update_metadata_checksum(owner.current.metadata,state::random_streams[1]);}
void grant_stone(SaveManager& owner,unsigned index){std::lock_guard lock(runtime::shared_locks().slot(20));if(index>=9)throw std::out_of_range("invalid array<T, N> subscript");auto* data=owner.current.metadata.bytes;const auto value=read<unsigned>(data,0x168+index*4);if(value<9)write(data,0x168+index*4,value+1);update_metadata_checksum(owner.current.metadata,state::random_streams[1]);}
}
