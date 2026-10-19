#pragma once
#include "ItemPool.hpp"
#include "ShotResource.hpp"
#include <array>
namespace th08 {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
namespace multiplayer {class PoolsJournal;}
#endif
struct ItemUpdateContext {
    Vec3 player;float height=448;i32 power=0;u8 focused=0,character=0;i8 player_state=0;u8 padding=0;
    Timer shooting,gauge_lock;u16 replay_flags=0,reserved=0;
};
// A permanently bound player view used by one shared item update.  The
// context and shot profiles stay owned by ItemSystem; this descriptor only
// gives the update loop the owner-specific values it needs for movement.
struct ItemUpdatePlayer {
    ItemUpdateContext* context=nullptr;
    const ShotProfile* human=nullptr;
    const ShotProfile* focused=nullptr;
    bool available=false;
};
struct ItemUpdateActions {
    virtual ~ItemUpdateActions()=default;
    // Select and retain the logical owner for this item. The implementation
    // owns the ItemState* -> seat index; ItemUpdate only consumes the result.
    virtual u32 owner_for(ItemState& item)=0;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Normal multiplayer Point-of-Collection admission. Returns a stable seat
    // and fixes the item's owner when one or more pilots qualify; directed
    // gifts remain owned by their promised recipient.
    virtual u32 auto_collect_owner(ItemState& item)=0;
#endif
    virtual bool touching(u32 seat,const Vec3& position,const Vec3& size)=0;
    // Collection may change power or item max_value; the shared context must
    // reflect value changes before the next item in the same update.
    virtual void collect(u32 seat,ItemState&)=0;
    virtual void item_sound(u32 seat,i32 index,i32 mode)=0;
    virtual void removed(ItemState&)=0;
    virtual void animation_step(AnmVm&)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void subtract_rank(i32 value)=0;
};
class ItemUpdate {
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    friend class multiplayer::PoolsJournal;
#endif
    ItemPoolState& state;ItemPool& pool;ItemUpdateActions& actions;std::array<ItemUpdatePlayer,3> players{};u32 player_count=0;
    ItemUpdatePlayer* first_player()noexcept;
    ItemUpdatePlayer* player(u32 seat)noexcept{return seat<players.size()&&players[seat].context?&players[seat]:nullptr;}
public:
    FrameTiming timing;
    ItemUpdate(ItemPoolState& s,ItemPool& p,ItemUpdateActions& a):state(s),pool(p),actions(a){}
    ItemUpdate(ItemPoolState& s,ItemPool& p,ItemUpdateContext& c,const ShotProfile& h,const ShotProfile& f,ItemUpdateActions& a):state(s),pool(p),actions(a){bind_player(0,c,h,f);}
    void bind_player(u32 seat,ItemUpdateContext& c,const ShotProfile& h,const ShotProfile& f){if(seat>=players.size())return;players[seat]={&c,&h,&f,true};if(player_count<=seat)player_count=seat+1;}
    void set_player_available(u32 seat,bool available){if(seat<players.size()&&players[seat].context)players[seat].available=available;}
    void update();
};
}
