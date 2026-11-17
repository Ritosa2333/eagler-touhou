// H-ANM-VEC2-WEIGHT: 466812 calls473a40, whose polynomial weight is stored
// once after its x87 intermediate chain. Values captured from native4665f0.
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/Interpolation.hpp"
using namespace th13;
TEST(vec2_scale_preserves_original_polynomial_rounding) {
    Vec2Interpolation scale;scale.begin(5,4,{0,0},{1,1});
    auto value=scale.step();
    CHECK_EQ(float_to_bits(value[0]),0x3eb851ecu);
    CHECK_EQ(float_to_bits(value[1]),0x3eb851ecu);
    for(int i=1;i<5;++i)value=scale.step();
    CHECK_EQ(value[0],1.0f);CHECK_EQ(value[1],1.0f);
    CHECK_EQ(scale.duration,0);
}
int main(){return th13test::run_all("anm-vec2-precision");}
