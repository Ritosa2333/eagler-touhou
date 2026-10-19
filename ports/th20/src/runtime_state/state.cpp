#include "state.hpp"
#include "../runtime_core/runtime_core.hpp"
#include <emmintrin.h>
namespace th20::source::state {
Random random_streams[4]={Random(0),Random(1),Random(2),Random(3)};
float clock_scale=1.f;
const float* timer_rate=&clock_scale;
void set_clock_scale(float value) noexcept {clock_scale=value;}
std::uint32_t normalize_seed(std::uint32_t value) noexcept {value%=0x7fffffffu;return value?value:1;}
void seed(Random& random,std::uint32_t value) {
    std::lock_guard<std::recursive_mutex> guard(runtime::shared_locks().slot(10));
    random.minimum=0;random.upper=0x7fffffffu;random.modulus=random.upper-random.minimum;
    random.last=value;random.state=normalize_seed(value);
}
ecl::RandomStream stream(Random& random) {return {random.state,random.last,random.modulus,runtime::shared_locks().slot(10)};}
std::uint32_t next(Random& random) {return stream(random).next();}
std::uint32_t bounded(Random& random,std::uint32_t count) {return count?next(random)%count:0;}
float unit(Random& random) {
    const auto numerator=_mm_set_ss(static_cast<float>(static_cast<double>(next(random))));
    const auto denominator=_mm_sub_ss(_mm_set_ss(static_cast<float>(static_cast<double>(random.modulus))),_mm_set_ss(1.f));
    return _mm_cvtss_f32(_mm_div_ss(numerator,denominator));
}
float signed_unit(Random& random) {return stream(random).signed_unit();}
}
