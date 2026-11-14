#pragma once
#include "GameEconomy.hpp"
namespace th11 {
struct ItemState;
struct ItemPlayer;
struct ItemRewardEffects {
    virtual ~ItemRewardEffects()=default;
    virtual bool sound(i32,float,bool){return false;}
    virtual bool popup(Vec3,i32,u32){return false;}
    virtual bool notify(i32){return false;}
    virtual bool power_changed(){return false;}
    virtual bool lives_changed(i32,i32){return false;}
#if PORT_TITLE == 14 || PORT_TITLE == 16
    virtual bool bombs_changed(i32,i32){return false;}
#endif
#if PORT_TITLE == 14
    virtual bool th14_combo_popup(Vec3,i32,u32,float){return false;}
    virtual bool th14_combo_drop(i32,Vec3,i32){return false;}
#endif
};
class ItemRewards {
public:
    ItemRewards(GameEconomy& s,ItemRewardEffects& e):state(s),effects(e){}
#if PORT_TITLE == 14
    ItemRewards(GameEconomy& stock,ItemRewardEffects& e,GameEconomy& counters)
        :state(stock),effects(e),th14_counters(&counters){}
#endif
    bool collect(const ItemState&,bool& convert_power_items,const ItemPlayer* player=nullptr);
    bool add_power(i32 amount,bool& level_changed);
    bool add_life();
    bool add_life_fragment();
#if PORT_TITLE == 16
    bool th16_add_score(i32 amount);
    bool th16_life(bool& awarded);
    bool th16_bomb();
    bool th16_bomb_fragment();
    bool th16_collect(const ItemState&,const ItemPlayer&);
#endif
#if PORT_TITLE == 14
    bool th14_bomb_fragment(i32 pieces=8);
    bool th14_life_fragment(i32 life_pieces=0,i32 bomb_pieces=8);
    bool th14_life();
    bool th14_bomb();
    bool th14_combo_settle(bool dialogue);
    bool th14_collect_with_combo(const ItemState&,bool&,const ItemPlayer*,const ItemPlayer*,Th14ItemCombo* combo_owner=nullptr);
#endif
private:
    GameEconomy& state;ItemRewardEffects& effects;
#if PORT_TITLE == 14
    GameEconomy* th14_counters=&state;
#endif
};
}
