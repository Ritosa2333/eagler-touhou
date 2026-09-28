#pragma once
#include "BulletExtras.hpp"
#include "PoolCheckpoint.hpp"
#include <array>
#include <vector>
namespace th09 {
enum class BulletAnimation:u32 {body,spawn_small,spawn_medium,spawn_large,cancel};
struct BulletFrameActions:BulletExtraActions {
    virtual bool prepare(Bullet&,u32 index,i32 type,i32 color,u32 flags)=0;
    virtual bool advance_animation(Bullet&,BulletAnimation)=0;
    virtual i32 probe_attacks(Bullet&)=0;
    virtual i32 collide_attacks(Bullet&)=0;
    virtual void collide_player(Bullet&)=0;
};
class BulletManager {
    void add_draw(Bullet&,u32 index) noexcept;
public:
    PoolCheckpoint* checkpoint=nullptr;
    static constexpr u32 first_capacity=175,second_begin=176,second_capacity=360,update_count=536;
    std::vector<Bullet> pool;
    i32 total=0,first_count=0,second_count=0,cancel_frames=0,frame=0;
    Timer lifetime{0,0,0};std::array<i32,6> draw_heads;
    BulletManager():pool(update_count+1){pool[first_capacity].state=6;pool[update_count].state=6;draw_heads.fill(-1);}
    void reset_pool();
    bool update_bullets(const FrameTiming&,const Vec3& player,u32 field_flags,u32 game_flags,BulletFrameActions&);
    Bullet* create(const BulletEmission&,i32 index,i32 layer,float aim,bool second_pool,const FrameTiming&,Rng&,const Vec3& player,BulletFrameActions&);
    bool emit(const BulletEmission&,bool second_pool,const FrameTiming&,Rng&,const Vec3& player,BulletFrameActions&);
    // Laser simulation runs between update_bullets and finish_frame in TH09.
    void finish_frame(const FrameTiming& timing,u32 field_flags,u32 game_flags) noexcept {
        if((game_flags&0x1800)||(field_flags&1))return;
        if(cancel_frames)cancel_frames=wrapping_sub(cancel_frames,1);lifetime.tick(timing);frame=wrapping_add(frame,1);
    }
};
}
