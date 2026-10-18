#include "GameInput.hpp"
namespace th15 {
void GameInput::calculate()noexcept{
 repeated=long_held=0;for(u32 i=0;i<32;i++){const u32 mask=u32(1)<<i;if(held&mask){repeat_age[i]++;duration[i]++;if(repeat_age[i]>=8)long_held|=mask;if(repeat_age[i]>=26){repeated|=mask;repeat_age[i]-=8;}}else repeat_age[i]=duration[i]=0;}
 const u32 changed=held^previous;pressed=changed&held;released=changed&~held;
}
}
