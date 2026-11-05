#include "EffectManager.hpp"
namespace th15 {
i32 EffectManager::reserve(){
    for(i32 i=0;i<1024;i++){const i32 prior=cursor;cursor=wrapping_add(cursor,1)%1024;if(cursor<0){error="Negative effect cursor";return -1;}auto& handle=handles[u32(cursor)];if(handle==0||animations.registry.find(handle))return prior;handle=0;}return -1;
}
u32 EffectManager::create(i32 script,const Vec3& position,float rotation){const auto handle=animations.create(resource,script,-1,0,position,rotation);if(!handle)error=animations.error;return handle;}
u32 EffectManager::tracked(i32 script,const Vec3& position,float rotation){const auto slot=reserve();if(slot<0)return 0;const auto handle=create(script,position,rotation);if(!handle)return 0;handles[u32(slot)]=handle;return u32(slot)|0x80000000u;}
u32 EffectManager::trail(const Vec3& position,Rng& random){const i32 slot=reserve();if(slot<0)return 0;const u32 handle=animations.create(resource,0,-1,0);if(!handle){error=animations.error;return 0;}auto* vm=animations.registry.find(handle);vm->geometry.clear();vm->geometry.trail=std::make_unique<AnmTrail>();vm->geometry.trail->initialize(random);vm->geometry.allocation_bytes=0x318;vm->visual.translation=position;vm->visual.flags&=~0x1e0u;vm->visual.layer=19;vm->visual.render_flags=(vm->visual.render_flags&~0x80000u)|0x40000;handles[u32(slot)]=handle;return u32(slot)|0x80000000u;}
}
