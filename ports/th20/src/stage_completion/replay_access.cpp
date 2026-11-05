#include "replay_access.hpp"
#include "../startup_scene/startup.hpp"
#include "../progress_state/records.hpp"
namespace th20::source::replay {
namespace {const void* owner() noexcept{return startup::unrecovered::owner_005c60fc;}}
bool is_playback() noexcept{return progress::read<int>(owner(),0x10)==1;}
void* recording_stage(int index) noexcept{return progress::read<void*>(owner(),0x20u+static_cast<unsigned>(index)*4u);}
const void* recorded_stage(int index) noexcept{return progress::read<const void*>(owner(),0xe8u+static_cast<unsigned>(index)*0x2cu+0x10u);}
std::uint8_t inherited_stone(int index) noexcept{return progress::read<std::uint8_t>(progress::read<const void*>(owner(),0x1c),0xecu+static_cast<unsigned>(index));}
}
namespace th20::source::stage_completion::unrecovered {bool replay_has_stage(int index){return replay::recorded_stage(index)!=nullptr;}}
namespace th20::source::overlay::unrecovered {std::uint8_t replay_inherited_stone(int index){return replay::inherited_stone(index);}}
