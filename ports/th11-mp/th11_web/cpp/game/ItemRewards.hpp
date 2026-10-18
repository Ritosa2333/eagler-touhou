#pragma once
#include "GameEconomy.hpp"
namespace th11 {
struct ItemState;
struct ItemRewardEffects {
    virtual ~ItemRewardEffects()=default;
    virtual bool sound(i32,float,bool){return false;}
    virtual bool popup(Vec3,i32,u32){return false;}
    virtual bool notify(i32){return false;}
    virtual bool power_changed(){return false;}
    virtual bool lives_changed(i32,i32){return false;}
};
class ItemRewards {
public:
    ItemRewards(GameEconomy& s,ItemRewardEffects& e):state(s),effects(e){}
    bool collect(const ItemState&,bool& convert_power_items);
    bool add_power(i32 amount,bool& level_changed);
    bool add_life();
    bool add_life_fragment();
private:
    GameEconomy& state;ItemRewardEffects& effects;
};
}
