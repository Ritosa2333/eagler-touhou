#pragma once
#include "PlayerMotion.hpp"
#include "AnmManager.hpp"
#include <functional>
namespace th17 {
enum class ShotSoundAction {play,stop,pan};
struct PlayerShootingServices {
    virtual ~PlayerShootingServices()=default;
    virtual bool fire(i32 frame,i32 continuous)=0;
    virtual bool sound(i32,ShotSoundAction,float)=0;
    virtual u32 charge_indicator(i32,const Vec3&)=0;
    virtual bool retire(u32)=0;
    virtual bool interrupt(u32,i32)=0;
};
class PlayerShooting {
    PlayerMotion& motion;PlayerShootingServices& host;
    void wrap(Timer&,i32,float)noexcept;
    void move_origin(const Vec3&)noexcept;
    bool stop_charge();bool fire_charge(float);
public:
    Timer shot,continuous,charge;Vec3 charge_origin{};u32 charge_handle=0;
    i32 auxiliary=0;bool auxiliary_active=false;std::string error;
    PlayerShooting(PlayerMotion& p,PlayerShootingServices& h):motion(p),host(h){shot.set(-1);continuous.set(-1);charge.set(0);}
    bool update(i32 state,bool held,float rate,i32 character,i32 animal,u32 roaring_flags,i32 roaring_kind);
    bool blocked();
};
class PlayerShootingAnmHost final:public PlayerShootingServices {
    AnmManager& animations;i32 resource;
public:
    std::function<bool(i32,i32)> shoot;std::function<bool(i32,ShotSoundAction,float)> audio;
    PlayerShootingAnmHost(AnmManager& a,i32 player):animations(a),resource(player){}
    bool fire(i32 frame,i32 continuous)override{return shoot&&shoot(frame,continuous);}
    bool sound(i32 id,ShotSoundAction action,float x)override{return audio&&audio(id,action,x);}
    u32 charge_indicator(i32 script,const Vec3& p)override{return animations.create(resource,script,-1,0,p);}
    bool retire(u32 handle)override{return animations.retire(handle);}
    bool interrupt(u32 handle,i32 label)override{return animations.interrupt(handle,label);}
};
}
