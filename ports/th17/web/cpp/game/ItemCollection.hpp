#pragma once
#include "ItemState.hpp"
#include "PlayerLife.hpp"
namespace th17 {
struct ItemScoreState {i32 score=0,point_value=0,max_point_value=0,point_items=0,life_piece_tier=0,difficulty=0,max_power=400;};
struct ItemCollectionHost {
    virtual ~ItemCollectionHost()=default;
    virtual bool options_changed()=0;virtual bool notice(i32)=0;virtual bool sound(i32,bool queued)=0;
    virtual bool popup(const Vec3&,i32,u32)=0;virtual bool life_hud(i32,i32)=0;virtual bool bomb_hud(i32,i32)=0;
};
class ItemCollection {
    PlayerLifeSession& player;ItemScoreState& score;PlayerMotion& motion;ItemCollectionHost& world;i32 character;
    void add_score(i32 displayed)noexcept;
    i32 point_value(const ItemState&,u32& color)const noexcept;
    bool power(const ItemState&,bool large);bool point(const ItemState&);bool full_power(const ItemState&);
    bool life();bool life_piece();bool bomb();bool bomb_piece();
public:
    std::string error;
    ItemCollection(PlayerLifeSession& p,ItemScoreState& s,PlayerMotion& m,ItemCollectionHost& h,i32 ch):player(p),score(s),motion(m),world(h),character(ch){}
    bool award(const ItemState&);
};
}
