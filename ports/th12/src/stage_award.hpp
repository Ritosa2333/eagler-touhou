// Original 421350 stage score and MSG21 final award arithmetic.
#pragma once
#include <algorithm>
#include <cstdint>
namespace th12::stage_award {
inline int roundedPointValue(int stored){const int displayed=stored/100;return displayed-displayed%10;}
inline int base(int stage){return stage*1000000;}
inline int final(int stage,int difficulty,int power,int lives,int bombs,int pointValueStored){
  const int points=roundedPointValue(pointValueStored);if(stage==6){
    if(difficulty==4)return (power+lives*100)*400000+points*100;
    constexpr int weight[4]={25,50,100,150};if(difficulty<0||difficulty>3)return 0;
    return (power+(bombs+lives*2)*weight[difficulty])*10000+points*100;
  }
  return stage==7?(points+(power+(bombs+lives*2)*150)*100)*100:0;
}
inline void add(int32_t& scoreStored,int displayed){scoreStored=int32_t(std::min<int64_t>(999999999,int64_t(scoreStored)+displayed/10));}
}
