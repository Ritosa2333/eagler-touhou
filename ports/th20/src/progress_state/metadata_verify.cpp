#include "manager.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::progress {namespace pe=program_entry;
void SaveManager::verify_metadata(){std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(20));if(current.metadata.bytes[0x1d1]!=metadata_checksum(current.metadata))pe::window_state.quit_requested=-2;}
}
