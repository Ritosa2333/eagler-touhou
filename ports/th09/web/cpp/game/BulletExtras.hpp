#pragma once
#include "Bullet.hpp"
namespace th09 {
struct BulletExtraActions {
    virtual ~BulletExtraActions()=default;
    virtual void play_sound(i32 sound,i32 pan)=0;
    virtual void play_positioned_sound(i32 sound,float x)=0;
    virtual void change_type(Bullet&,i32 type,i32 color)=0;
    virtual bool emit_children(const BulletEmission&)=0;
};
class BulletExtras {
    FrameTiming timing;Vec3 player;BulletExtraActions& actions;
    void sound(const Bullet& b){if(b.transform_sound>=0)actions.play_sound(b.transform_sound,0);}
public:
    BulletExtras(const FrameTiming& t,const Vec3& p,BulletExtraActions& a):timing(t),player(p),actions(a){}
    bool begin(Bullet&);
    void update(Bullet&);
};
}
