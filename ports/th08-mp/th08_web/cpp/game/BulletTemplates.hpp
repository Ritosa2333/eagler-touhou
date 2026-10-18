#pragma once
#include "AnmExecutor.hpp"
namespace th08 {
struct BulletTemplate {
    AnmVm animation[5];Vec3 hitbox;u8 reserved_d40=0,height=0,layer=0,padding=0;
};
static_assert(sizeof(BulletTemplate)==0xd44&&offsetof(BulletTemplate,hitbox)==0xd34);
class BulletTemplates {
public:
    BulletTemplate types[32];
    BulletTemplates(){reset();}
    void reset()noexcept{std::memset(types,0,sizeof(types));}
    bool load(AnmLoaded& file,Rng& random,const FrameTiming& timing);
};
}
