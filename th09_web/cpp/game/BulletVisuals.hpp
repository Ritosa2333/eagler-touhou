#pragma once
#include "PoolCheckpoint.hpp"
#include "BulletManager.hpp"
#include "AnmExecutor.hpp"
namespace th09 {
struct BulletAppearance {
    std::array<AnmVm,5> animations;
    Vec3 hitbox;u8 flags=0,height=0,draw_group=0;i32 base_sprite=0;
};
class BulletVisuals {
    AnmLoaded& file;AnmExecutor& executor;
    bool change_color(AnmVm&,i32 base,i32 color,float height);
    void refresh(Bullet&,u32 index);
    void copy_metadata(Bullet&,const BulletAppearance&);
public:
    PoolCheckpoint* checkpoint=nullptr;
    std::array<BulletAppearance,23> appearances;
    std::vector<std::array<AnmVm,5>> instances;
    BulletVisuals(AnmLoaded& f,AnmExecutor& e):file(f),executor(e),instances(BulletManager::update_count+1){
        for(auto& v:instances)for(auto& a:v)std::memset(&a,0,sizeof(a));
    }
    bool initialize();
    void reset_instances();
    bool prepare(Bullet&,u32 index,i32 type,i32 color,u32 flags);
    bool change_type(Bullet&,u32 index,i32 type,i32 color);
    bool set_sprite(Bullet&,u32 index,i32 sprite);
    bool advance(Bullet&,u32 index,BulletAnimation);
};
}
