#pragma once
#include "EclVm.hpp"
#include "Bullet.hpp"
#include "AttackQueueActions.hpp"
namespace th09 {
struct EclSpecialActions:AttackQueueActions {
    virtual ~EclSpecialActions()=default;
    virtual void play_sound(i32 sound,i32 pan)=0;
    virtual Bullet* bullets(i32 side,u32& count)=0;
    virtual void set_sprite(Bullet&,i32 sprite)=0;
};
class EclSpecial:public EclNativeServices {
    EclSpecialActions& actions;
    bool redirect_bullets(EclVm&);
public:
    explicit EclSpecial(EclSpecialActions& a):actions(a){}
    bool execute(EclVm&,i32 callback,const EclInstruction&) override;
};
}
