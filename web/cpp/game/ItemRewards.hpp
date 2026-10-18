#pragma once
#include "ItemPool.hpp"
#include "GameGauge.hpp"
namespace th08 {
struct ItemRewardContext {
    float collect_line=0;u32 hud_flags=0;i32 difficulty=0,bomb_triggered=0,bomb_active=0;
    Timer gauge_lock;u8 focused=0,power_flag=0,time_spell=0,padding=0;
};
struct ItemRewardActions {
    virtual ~ItemRewardActions()=default;
    virtual void popup(const Vec3&,i32 value,u32 color,bool small)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void gui_popup(i32 value,i32 type)=0;
    virtual void cancel_bullets()=0;
    virtual void spell_time(i32 value)=0;
};
class ItemRewards {
    ItemRewardContext& context;GameGlobals& globals;GameValues& values;GameGauge& gauge;GameRank& rank;HighScore& high_score;ItemPool& pool;ItemRewardActions& actions;
    void power(ItemState&,bool big);
    void point(ItemState&,bool small);
public:
    bool failed=false;
    ItemRewards(ItemRewardContext& c,GameGlobals& g,GameValues& v,GameGauge& gauge,GameRank& r,HighScore& h,ItemPool& p,ItemRewardActions& a):context(c),globals(g),values(v),gauge(gauge),rank(r),high_score(h),pool(p),actions(a){}
    void collect(ItemState&);
    void extend();
    void time_orb(ItemState*);
};
}
