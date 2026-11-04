#pragma once
#include <algorithm>
#include <cmath>
namespace th19 {
// Verified native 0x4d5e10 jump table. ANM, STD and ECL share these modes.
inline float nativeEase(unsigned mode,float t){
 t=std::clamp(t,0.f,1.f);float s=1-t;
 switch(mode){
 case 1:return t*t;case 2:return t*t*t;case 3:return t*t*t*t;
 case 4:return 1-s*s;case 5:return 1-s*s*s;case 6:return 1-s*s*s*s;
 case 9:return t<.5f?2*t*t:1-2*s*s;
 case 12:return t<.5f?.5f-.5f*(1-2*t)*(1-2*t):.5f+.5f*(2*t-1)*(2*t-1);
 case 28:return t>=1?1:1-((.7f-t)*(.7f-t)-.09f)/.4f;
 // 4d657e: the same native quadratic overshoot family, c=5aa208=.35.
 case 29:return t>=1?1:1-((.65f-t)*(.65f-t)-.1225f)/.3f;
 default:return t;
 }
}
}
