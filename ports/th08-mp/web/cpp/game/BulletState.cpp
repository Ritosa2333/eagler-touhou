#include "BulletState.hpp"
namespace th08 {
void BulletManagerState::reset()noexcept{
    std::memset(this,0,sizeof(*this));next_slot=&bullets[0];bullets[1536].state=6;bonus_item=6;
    for(u32 index=0;index<1536;++index)for(auto& vm:bullets[index].sprites.animation)vm.scriptIndex=-1;
}
}
