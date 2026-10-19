#pragma once
#include "GameMath.hpp"
#include <cmath>

namespace th10 {
// Only the exact input promotion is shortened. Keep the original output
// stores, which own SoftFloat rounding, tininess and sticky exception flags.
inline void anm_rotation_values(float radians,float& c,float& s) noexcept {
#if defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY) && defined(__wasm__)
    u32 bits;std::memcpy(&bits,&radians,sizeof(bits));
    const auto exponent=(bits>>23)&255u;
    if(single_precision_nearest()&&exponent&&exponent!=255u){
        // For normal f32 values the skipped ExactFloat input conversions are
        // flag-free and produce exactly the same f64 bits as this promotion.
        const double angle=static_cast<double>(radians);
        c=Extended::from_double(std::cos(angle)).to_float();
        s=Extended::from_double(std::sin(angle)).to_float();
        return;
    }
#endif
    c=cosine(number(radians)).to_float();s=sine(number(radians)).to_float();
}
}
