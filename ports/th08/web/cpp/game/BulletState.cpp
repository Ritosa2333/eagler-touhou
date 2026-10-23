#include "BulletState.hpp"
namespace th08 {
void BulletManagerState::reset()noexcept{
    std::memset(this,0,sizeof(*this));next_slot=&bullets[0];bullets[bullet_slot_count].state=6;bonus_item=6;
    for(u32 index=0;index<bullet_slot_count;++index)for(auto& vm:bullets[index].sprites.animation)vm.scriptIndex=-1;
}
u32 BulletManagerState::live_bullet_count()const noexcept{
    u32 count=0;for(u32 index=0;index<bullet_slot_count;++index)count+=bullets[index].state!=0;return count;
}
}
