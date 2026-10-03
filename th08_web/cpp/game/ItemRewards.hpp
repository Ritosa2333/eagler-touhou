#pragma once
#include "ItemPool.hpp"
#include "GameGauge.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/PlayerResources.hpp"
#endif
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
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    virtual void team_extend()=0;
    virtual void convert_team_power(ItemState& collected)=0;
#endif
};
class ItemRewards {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    using Resources=PlayerResourceView;
    using Values=PlayerValues;
#else
    using Resources=GameGlobals;
    using Values=GameValues;
#endif
    ItemRewardContext& context;Resources& resources;Values& values;GameGauge& gauge;GameRank& rank;HighScore& high_score;ItemPool& pool;ItemRewardActions& actions;
    GameGlobals& shared_globals()noexcept{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        return resources.shared;
#else
        return resources;
#endif
    }
    void power(ItemState&,bool big);
    void point(ItemState&,bool small);
public:
    bool failed=false;
    ItemRewards(ItemRewardContext& c,Resources& r,Values& v,GameGauge& gauge,GameRank& rank,HighScore& h,ItemPool& p,ItemRewardActions& a):context(c),resources(r),values(v),gauge(gauge),rank(rank),high_score(h),pool(p),actions(a){}
    void collect(ItemState&);
    void extend();
    void time_orb(ItemState*);
};
}
