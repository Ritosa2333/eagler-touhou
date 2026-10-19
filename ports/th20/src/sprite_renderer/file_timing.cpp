#include "file_tasks.hpp"
#include "../platform_window/frame_statistics.hpp"
#include <chrono>
#include <cstring>
namespace th20::source::sprite {
double elapsed_frame_interval(std::int64_t begin,std::int64_t end) noexcept {
    const auto difference=static_cast<std::uint64_t>(end)-static_cast<std::uint64_t>(begin);std::int64_t signed_difference;std::memcpy(&signed_difference,&difference,8);
    const auto microseconds=signed_difference/1000;
    return _mm_cvtsd_f64(_mm_div_sd(_mm_set_sd(static_cast<double>(microseconds)),_mm_set_sd(1000000.0)));
}
void begin_frame_interval(platform_window::FrameStatistics& statistics,unsigned index){static_assert(std::chrono::steady_clock::period::num==1&&std::chrono::steady_clock::period::den==1000000000);statistics.end_times[index]=std::chrono::steady_clock::now().time_since_epoch().count();statistics.begin_times[index]=statistics.end_times[index];}
void end_frame_interval(platform_window::FrameStatistics& statistics,unsigned index){statistics.end_times[index]=std::chrono::steady_clock::now().time_since_epoch().count();}
}
