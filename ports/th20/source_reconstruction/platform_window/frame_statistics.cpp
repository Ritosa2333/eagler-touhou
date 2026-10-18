#include "frame_statistics.hpp"
#include "platform_window.hpp"
#include "data_constants.hpp"
#include "../platform_services/services.hpp"
#include <chrono>
#include <cstring>
#include <new>
#include <emmintrin.h>

namespace th20::source::platform_window {
namespace pe=program_entry;
namespace {
double add(double a,double b) {return _mm_cvtsd_f64(_mm_add_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double subtract(double a,double b) {return _mm_cvtsd_f64(_mm_sub_sd(_mm_set_sd(a),_mm_set_sd(b)));}
double divide(double a,double b) {return _mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(a),_mm_set_sd(b)));}
int __cdecl draw_statistics(void* object) { // 4ac0b0 ->4abf30
    unrecovered::draw_frame_rate_text(static_cast<FrameStatistics*>(object)->frames_per_second);
    return 1; // explicit EAX1 at4ac065, including suppressed display modes
}
}
FrameStatistics::FrameStatistics() {
    field_10=0;sample_time=0.0;fast_intervals=0;frames=0;
    actual_frames=0.0;target_frames=0.0;frames_per_second=0.0f;
    fields_40[0]=0.0;fields_40[1]=0.0;
    for(auto& value:begin_times) value=0;for(auto& value:end_times) value=0;wall_time_ticks=0;
    unrecovered::scheduler_object_005c4a00=this;
    // 451520 slot0 -> 44dde0 is std::chrono::steady_clock::now().
    end_times[0]=std::chrono::steady_clock::now().time_since_epoch().count();
    begin_times[0]=end_times[0];
}
FrameStatistics::~FrameStatistics() {
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    unrecovered::scheduler_object_005c4a00=nullptr;
}
void FrameStatistics::update_local_time() {
#ifdef TH_SDL3
    // libc++ system_clock ticks at a different period than MSVC's 100ns clock;
    // derive unix seconds from the actual period instead of asserting on it.
    const auto ticks=std::chrono::system_clock::now().time_since_epoch();
    wall_time_ticks=ticks.count();
    unix_seconds=std::chrono::duration_cast<std::chrono::seconds>(ticks).count();
#else
    static_assert(std::chrono::system_clock::period::den==10000000);
    wall_time_ticks=std::chrono::system_clock::now().time_since_epoch().count();
    unix_seconds=wall_time_ticks/10000000;
#endif
    _localtime64_s(&local_time,&unix_seconds);
}
int FrameStatistics::update() {
    const double now=platform::read_clock(pe::window_state);
    if(now<sample_time) sample_time=now;
    if(data::value_0056c468<=subtract(now,sample_time)) {
        const double elapsed=subtract(now,sample_time);sample_time=add(sample_time,elapsed);
        frames_per_second=static_cast<float>(divide(static_cast<double>(frames),elapsed));
        if(frames_per_second<=data::value_00570618) fast_intervals=0;else ++fast_intervals;
        auto* game_flags=unrecovered::current_game_flags();
        if(game_flags && !(*game_flags&0x10) && !(*game_flags&4)) {
            target_frames=add(target_frames,data::value_0056c470);
            actual_frames=frames_per_second<=data::value_00570614?
                add(static_cast<double>(frames_per_second),actual_frames):add(actual_frames,data::value_0056c470);
        }
        if(game_flags) *game_flags&=~0x100u;
        frames=0;
    }
    frames+=pe::graphics_state.configuration.frame_skip+1;
    update_local_time();return 1;
}
int FrameStatistics::register_draw() {
    draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,0x60,draw_statistics,this,true,true);
    return 0; // original4ac080 always returns0 after insertion
}
FrameStatistics* create_frame_statistics() {
    void* memory=::operator new(sizeof(FrameStatistics),std::nothrow);
    if(!memory) return nullptr;
    std::memset(memory,0,sizeof(FrameStatistics));
    FrameStatistics* object;
    try {object=::new(memory) FrameStatistics;} catch(...) {::operator delete(memory);throw;}
    if(object->register_draw()!=0) {runtime::retire_callback_owner(object);return nullptr;}
    return object;
}
namespace unrecovered {
runtime::CallbackOwner* scheduler_object_005c4a00=nullptr;
void update_frame_statistics(void* object) {static_cast<FrameStatistics*>(object)->update();}
}
}
