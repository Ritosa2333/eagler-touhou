#pragma once
#include "BulletState.hpp"
#include "BulletPattern.hpp"
namespace th08 {
struct BulletCreationActions {
    virtual ~BulletCreationActions()=default;
    virtual void sound(i32 index,float position,bool panned)=0;
    virtual void reemit(BulletEmission& parameters)=0;
};
// TH08 1.00d 0042f5f0 / 0042ffc0 / 00430e10. Owns no emulated memory.
class BulletCreation {
public:
    BulletCreation(BulletManagerState& state,Rng& random):state(state),random(random){}
    FrameTiming timing;BulletCreationActions* actions=nullptr;
    enum class Failure {None,MissingAnimation,InvalidTemplate,InvalidExtra};
    Failure failure=Failure::None;
    i32 create(const BulletEmission&,i32 index,i32 layer,float aim);
    bool initialize_extra(BulletState&);
    void emit(BulletEmission&,float aim,u16* replay_flags=nullptr);
private:
    BulletManagerState& state;Rng& random;
    bool set_sprite(AnmVm&,i32 index);
    bool set_palette(AnmVm&,const AnmVm&,const BulletState&,i32 color);
};
}
