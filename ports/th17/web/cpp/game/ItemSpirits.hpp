#pragma once
#include "ItemManager.hpp"
#include "RoaringSystem.hpp"
namespace th17 {
struct ItemSpiritHost:ItemCollectionHost {
    virtual bool trophy(i32)=0;
    virtual bool cancel_bullets(const Vec3&,float radius,i32 reward)=0;
    virtual bool cancel_lasers(const Vec3&,float radius,i32 reward,bool honor)=0;
    virtual bool active_player_shot()const=0;
};
struct ItemSpiritFrame {
    float rate=1,attraction_speed=5;i32 player_state=0,bomb_state=0,bomb_frame=0;
    u32 held=0;bool hud_collect=false,practice=false;
};
// The item, animal and Roaring callbacks run against the same player, score,
// random streams and animation owner. Scene scheduling chooses their phases.
class ItemSpirits final:private AnimalSpiritHost,private RoaringHost {
    AnmManager& animations;Rng& game_random;Rng& visual_random;PlayerMotion& player;
    PlayerBounds& bounds;PlayerLifeSession& session;ItemSpiritHost& world;i32 character;bool practice=false;
    ItemCollection collection;
    bool sound(i32 id)override{return world.sound(id,false);}
    bool sound(i32 id,bool queued)override{return world.sound(id,queued);}
    bool trophy(i32 id)override{return world.trophy(id);}
    bool collect_spirit(i32 kind)override;
    bool item(i32 kind,const Vec3&,float angle,float speed,i32 delay,i32 sound)override;
    bool spirit(i32 kind,const Vec3&,float angle)override;
    bool cancel_bullets(const Vec3& p,float r,i32 reward)override{return world.cancel_bullets(p,r,reward);}
    bool cancel_lasers(const Vec3& p,float r,i32 reward,bool honor)override{return world.cancel_lasers(p,r,reward,honor);}
    bool active_player_shot()const override{return world.active_player_shot();}
public:
    ItemManager items;AnimalSpirits spirits;RoaringSystem roaring;std::string error;
    ItemSpirits(AnmManager&,EffectManager&,Rng& game,Rng& visual,PlayerMotion&,PlayerBounds&,PlayerLifeSession&,Timer& invulnerability,PlayerSpellStatus&,DamageSources&,ItemScoreState&,ItemSpiritHost&,i32 character,i32 item_resource,i32 animal_resource);
    bool initialize(){if(roaring.initialize_hud())return true;error=roaring.error;return false;}
    void set_practice(bool value)noexcept{practice=value;}
    bool spawn(i32 kind,const Vec3&,float angle,float speed,i32 delay=0,i32 sound=-1);
    bool update_items(const ItemSpiritFrame&);
    bool update_spirits(const ItemSpiritFrame&);
    void before_player()noexcept;
    void after_player()noexcept;
    bool end_roaring(bool death);
};
}
