#include "EnemyDropRules.hpp"
#include <algorithm>
#include <cmath>

namespace th128::enemy {
void resetAdditionalDrops(DropState& drops) noexcept {std::fill(drops.words.begin()+1,drops.words.end(),0);}
bool setDropWord(DropState& drops,int index,int value) noexcept {
    if(index<0||std::size_t(index)>=drops.words.size())return false;
    drops.words[std::size_t(index)]=value;return true;
}
bool setDropSpread(DropState& drops,float width,float height) noexcept {
    if(!std::isfinite(width)||!std::isfinite(height)||std::abs(width)>65536||std::abs(height)>65536)return false;
    drops.width=width;drops.height=height;return true;
}
}
