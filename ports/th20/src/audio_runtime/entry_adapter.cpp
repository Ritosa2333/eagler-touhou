#include "audio.hpp"
#include "../program_entry/program_entry.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
#include "../platform_window/platform_window.hpp"
#include "../platform_services/services.hpp"
#include "../archive/resource_manager.hpp"

namespace pe=th20::source::program_entry;
namespace th20::source::program_entry { ThreadRegistry thread_registry; }
namespace th20::source::audio {
void bind_game_services() {
    static Context services{pe::graphics_state.configuration,pe::log_buffer,
        [](const char* name) {return resources::read(name);},
        [] {return platform::read_clock(pe::window_state);},nullptr};
    services.graphics_window=pe::graphics_state.window_handle;
    pe::thread_registry.context=&services;
}
}
namespace th20::source::program_entry::unrecovered {
void fn_00426170(ThreadRegistry& sound) {sound.shutdown();}
}
namespace th20::source::platform_window::unrecovered {
int poll_background_jobs(program_entry::ThreadRegistry& sound) {return sound.poll();}
void stop_audio(program_entry::ThreadRegistry& sound,int operation,int value,const char* name) {sound.enqueue(operation,value,name);}
}
