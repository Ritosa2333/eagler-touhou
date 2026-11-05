#include "animation_file.hpp"
#include <stdexcept>
#ifdef TH_SDL3
#include "platform/Time.hpp"
#include "../platform_window/graphics_callbacks.hpp"
#endif
#include <atomic>
namespace th20::source::sprite {
AnimationFile* load_animation_file(Controller& controller,std::int32_t index,const char* name,runtime::Log& log,std::uint32_t& graphics_flags) {
    if(index<0||index>=42)throw std::out_of_range("ANM file slot outside original array");
    if(controller.files[index])return controller.files[index];
    auto* file=preload_animation_file(controller,index,name,log);if(!file)return nullptr;
    auto stage=std::atomic_ref<std::uint32_t>(file->fields_5c[0]);stage.store(1);
    auto flags=std::atomic_ref<std::uint32_t>(graphics_flags);
#ifdef TH_SDL3
    // Threadless build: the caller IS the main thread, so pump the per-frame
    // background jobs (postload included) until this file is ready.
    while(stage.load()!=0&&(flags.load()&0x60u)==0)platform_window::unrecovered::pump_background_jobs(controller);
#else
    while(stage.load()!=0&&(flags.load()&0x60u)==0)Sleep(1);
#endif
    return file;
}
}
