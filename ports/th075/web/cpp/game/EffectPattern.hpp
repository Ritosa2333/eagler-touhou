#pragma once
#include "FighterPattern.hpp"
namespace th075 {
struct EffectSequence {bool present=false;std::vector<ImageFrame> frames;};
class EffectPattern {
public:
    std::vector<EffectSequence> sequences;std::string error;
    bool load(const void*,std::size_t);
};
}
