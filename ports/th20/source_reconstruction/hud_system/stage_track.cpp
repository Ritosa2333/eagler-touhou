#include "dialogue.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../progress_state/manager.hpp"
#include "../runtime_state/state.hpp"
#include <stdexcept>
namespace th20::source::hud {
void play_stage_track(int index,int unlock){
    auto& sound=program_entry::thread_registry;if(program_entry::graphics_state.configuration.flags&0x10u)sound.enqueue(4,0,"dummy");sound.enqueue(2,index,"dummy"); //571b14
    if(unlock<0||static_cast<unsigned>(unlock)+0x36>=sizeof(progress::Metadata))throw std::out_of_range("Music-room unlock index outside original metadata");progress::manager->current.metadata.bytes[0x36+unlock]=1;
}
void fade_stage_track(float seconds){const auto clock=state::clock_scale;const auto scaled=clock==0||!(clock<=1)?seconds:seconds/clock;program_entry::thread_registry.enqueue(5,recovered::truncate32(scaled),"");}
}
