#pragma once
#include "ItemPool.hpp"
#include "ShotResource.hpp"
namespace th08 {
struct ItemUpdateContext {
    Vec3 player;float height=448;i32 power=0;u8 focused=0,character=0;i8 player_state=0;u8 padding=0;
    Timer shooting,gauge_lock;u16 replay_flags=0,reserved=0;
};
struct ItemUpdateActions {
    virtual ~ItemUpdateActions()=default;
    virtual bool touching(const Vec3& position,const Vec3& size)=0;
    // Collection may change power or item max_value; the shared context must
    // reflect value changes before the next item in the same update.
    virtual void collect(ItemState&)=0;
    virtual void animation_step(AnmVm&)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void subtract_rank(i32 value)=0;
};
class ItemUpdate {
    ItemPoolState& state;ItemPool& pool;ItemUpdateContext& context;const ShotProfile &human,&focused;ItemUpdateActions& actions;
public:
    FrameTiming timing;
    ItemUpdate(ItemPoolState& s,ItemPool& p,ItemUpdateContext& c,const ShotProfile& h,const ShotProfile& f,ItemUpdateActions& a):state(s),pool(p),context(c),human(h),focused(f),actions(a){}
    void update();
};
}
