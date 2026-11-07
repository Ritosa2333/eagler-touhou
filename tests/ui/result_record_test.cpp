#include "game/ResultRecord.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace th13;
static u32 bits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
int main() {
    assert(result_slowdown_percent(0, 0) == 0);
    assert(result_slowdown_percent(60, 60) == 0);
    assert(result_slowdown_percent(30, 60) == 50);
    // Original fstp float ratio: 1/3 rounds to 0x3eaaaaab, then 100-r*100.
    assert(bits(result_slowdown_percent(1, 3)) == 0x42855555u);
    const float ratio = 1.f / 3.f;
    assert(result_slowdown_percent(1, 3) == float(100.0 - double(ratio) * 100.0));
    // This input distinguishes premature/double-only ratio evaluation.
    const double a = 59.0, b = 60.0;
    assert(bits(result_slowdown_percent(a, b)) != bits(float(100.0 - a / b * 100.0)));
    assert(result_record_stage(true, 0) == 8);
    assert(result_record_stage(true, 6) == 8);
    assert(result_record_stage(false, 4) == 4);
    assert(result_record_stage(false, 0) == 0);
    std::puts("result_record_test: PASS (clear preview, slowdown semantics, original float rounding)");
}
