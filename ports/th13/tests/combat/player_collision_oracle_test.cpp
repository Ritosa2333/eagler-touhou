#include "../harness/test_framework.hpp"
#include "rebuild/X87.hpp"
#include <algorithm>
#include <cmath>
#include <random>
using namespace th13;
namespace {

int circle_result(float bx, float by, float px, float py, float size, float radius) {
    const double dx = double(px) - double(bx);
    const double dy = double(py) - double(by);
    const float distance2 = rebuild::to_float(rebuild::square_sum_80(dx, dy));
    if (rebuild::below_square_sum(distance2, size, radius)) return 1;
    const float span = std::max(float(double(radius) / 2.5), 40.0f);
    if (rebuild::below_square_sum(distance2, double(radius), double(span) + double(size))) return 2;
    return 0;
}

TEST(circle_contact_boundary_matches_original_444140) {
    CHECK_EQ(circle_result(0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f), 1);
    CHECK_EQ(circle_result(2.23f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f), 1);
    CHECK_EQ(circle_result(2.24f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f), 2);
    CHECK_EQ(circle_result(0.0f, 2.23f, 0.0f, 0.0f, 2.0f, 1.0f), 1);
    CHECK_EQ(circle_result(42.011f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f), 2);
    CHECK_EQ(circle_result(42.012f, 0.0f, 0.0f, 0.0f, 2.0f, 1.0f), 0);
    CHECK_EQ(circle_result(3.0f, 4.0f, 0.0f, 0.0f, 3.0f, 2.0f), 2);
    CHECK_EQ(circle_result(0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 2.0f), 1);
}

TEST(square_sum_80_rounds_once_to_64_bits) {
    CHECK(rebuild::below_square_sum(1.0f, 1.0f, std::ldexp(1.0f, -31)));
    CHECK(!rebuild::below_square_sum(1.0f, 1.0f, std::ldexp(1.0f, -32)));
    CHECK(rebuild::below_square_sum(1.0f, 1.0f, std::ldexp(5.0f, -34)));
    CHECK(rebuild::below_square_sum(1.0f, std::ldexp(1.0f, -30), 1.0));
    CHECK(!rebuild::below_square_sum(1.0f, 1.0f, 0.0));
    CHECK(rebuild::below_square_sum(0.0f, 0.0f, std::ldexp(1.0f, -60)));
    CHECK_EQ(rebuild::to_float(rebuild::square_sum_80(3.0, 4.0)), 25.0f);
    CHECK_EQ(rebuild::to_float(rebuild::square_sum_80(0.0, 0.0)), 0.0f);
    CHECK_EQ(rebuild::square_sum_80(1.0, 0.0).significand, u64(1) << 63);
}

#if defined(__x86_64__)
TEST(square_sum_80_matches_host_x87_on_random_inputs) {
    std::mt19937 generator(12345);
    std::uniform_real_distribution<float> unit(0.0f, 1.0f), angle(0.0f, 6.2831855f);
    int mismatches = 0;
    for (int index = 0; index < 200000; ++index) {
        const float radius = unit(generator) * 300.0f, heading = angle(generator);
        const float px = unit(generator) * 640.0f, py = unit(generator) * 480.0f;
        const float bx = px + radius * std::cos(heading), by = py + radius * std::sin(heading);
        const float size = (index % 3) ? 2.0f : 3.0f;
        const float bullet = std::ldexp(0.5f + unit(generator) * 3.0f, (index % 13) - 6);
        const long double dx = (long double)px - (long double)bx, dy = (long double)py - (long double)by;
        const long double exact = dx * dx + dy * dy;
        const float squared = (float)exact;
        const float span = std::max(float(double(bullet) / 2.5), 40.0f);
        const long double reach = (long double)span + (long double)size;
        const bool contact = exact < (long double)size * size + (long double)bullet * bullet;
        const bool graze = exact < (long double)bullet * bullet + reach * reach;
        if (rebuild::to_float(rebuild::square_sum_80(double(px) - double(bx), double(py) - double(by))) != squared ||
            rebuild::below_square_sum(squared, size, bullet) != contact ||
            rebuild::below_square_sum(squared, double(bullet), double(span) + double(size)) != graze)
            ++mismatches;
    }
    CHECK_EQ(mismatches, 0);
}
#endif

}
int main() { return th13test::run_all("player collision oracle"); }
