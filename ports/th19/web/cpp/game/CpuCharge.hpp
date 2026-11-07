#pragma once
#include <algorithm>
#include <array>
namespace th19 {
// Initial charge preference 2: 0x4fb6f0 -> hit controller +36eeb0.
// Base charge decision: 0x4fa491..0x4fa702. Later target/obstacle branches
// can override this result; it is not by itself a full input controller.
struct CpuCharge {
 int preferred=2;
 template<class Random> bool baseIntent(int energy,int currentCharge,const std::array<int,4>&thresholds,Random&&random){
  energy=std::clamp(energy,0,2500);
  auto ready=[&](int tier){int threshold=std::clamp(thresholds[tier-1],0,2500);return energy>=threshold&&currentCharge<threshold;};
  if((preferred==2&&ready(2))||(preferred==3&&ready(3))||ready(4)){
   preferred=int(random()%3)+2;return true;
  }
  return false;
 }
};
}
