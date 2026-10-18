#include "file_tasks.hpp"
#include "../platform_window/frame_statistics.hpp"
#include <atomic>
#include <stdexcept>
namespace th20::source::sprite {
std::int32_t update_animation_file_tasks(Controller& controller,TextureContext& context,runtime::Log& log,platform_window::FrameStatistics& statistics){
    std::lock_guard lock(runtime::shared_locks().slot(17));begin_frame_interval(statistics,1);
    for(auto*& file:controller.files)if(file){
        if(file->fields_5c[1]){
            unload_animation_file(controller,static_cast<std::int32_t>(file->id));
            // Original44e06f dereferences this slot even though44c430 clears it.
            // Preserve a visible failure instead of invoking C++ null-write UB.
            if(!file)throw std::logic_error("Original ANM unload task dereferences its cleared file slot (44e06f)");
            file->fields_5c[1]=0;
        }else if(std::atomic_ref<std::uint32_t>(file->fields_5c[0]).load()){
            if(!postload_animation_file(*file,context,log))return -1;
            end_frame_interval(statistics,1);
            const double limit=_mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(1.0),_mm_set_sd(65.0)));
            if(elapsed_frame_interval(statistics.begin_times[1],statistics.end_times[1])>limit)return 0;
        }
    }return 0;
}
}
