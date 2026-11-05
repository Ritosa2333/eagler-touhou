#pragma once
#include "../ecl_vm/vm.hpp"
namespace th20::source::state {
// 0x422c50, 28 bytes; four CRT-constructed instances at 0x5ba4a8..0x5ba518.
struct Random {
    std::uint32_t field_00=0,state=1,minimum=0,upper=0x00ffff00;
    std::uint32_t modulus=0,last=0,id;
    explicit Random(std::uint32_t stream_id):id(stream_id) {}
};
static_assert(sizeof(Random)==28);
extern Random random_streams[4];
void seed(Random&,std::uint32_t);                  // 0x4d9940
std::uint32_t normalize_seed(std::uint32_t) noexcept; // 0x422c00
ecl::RandomStream stream(Random&);                 // references actual shared state and lock slot 10
std::uint32_t next(Random&);                       // 0x423ee0
std::uint32_t bounded(Random&,std::uint32_t);       // 0x423ea0
float unit(Random&);                              // 0x429830
float signed_unit(Random&);                       // 0x4298e0
extern float clock_scale;                         // 0x5aefe4, initialized data 1.0
extern const float* timer_rate;                   // 0x5aefe0, initialized pointer to clock_scale
void set_clock_scale(float) noexcept;             // 0x4292a0 on global clock
}
