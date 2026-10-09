#pragma once
#include "BulletState.hpp"
#include "BulletAppearance.hpp"
#include <array>
#include <memory>
namespace th11 {
class AnmRenderer;
struct BulletCancelContext {
    u32 spell_flags=0;i32 spell_id=0;
    bool protected_spell()const noexcept{return (spell_flags&1)&&spell_id>=158&&spell_id<=161;}
};
struct BulletWorld {
#ifdef TH11_MULTIPLAYER
    virtual bool multiplayer_world()const{return false;}
    virtual Vec3 multiplayer_target(Vec3)const{return {};}
    virtual bool multiplayer_bullet_collision(const BulletState&,u8&,i32&){return false;}
#endif
    virtual ~BulletWorld()=default;
    virtual bool sound(i32,float,bool){return false;}
    virtual i32 collision(const BulletState&){return -2;}
    virtual bool effect(const Vec3&,i32){return false;}
    virtual bool colored_effect(const Vec3&,i32,u32){return false;}
    virtual bool register_graze(){return false;}
    virtual bool reward_graze(){return false;}
    virtual bool cancel_reward(const Vec3&){return false;}
    virtual bool cancel_enemies(const Vec3&,float,bool){return false;}
    virtual bool cancel_shot(Vec3,float){return false;}
    virtual bool convert_enemies(const Vec3&,float,bool){return false;}
    virtual bool cancel_enemy_beam(const Vec3&,float,bool){return false;}
};
class BulletManager final:public BulletEnvironment {
public:
    static constexpr u32 capacity=2000;
    BulletManager(AnmResource&,AnmEnvironment&,BulletWorld&,u16 file_id=6);
    BulletState& at(u32 index)noexcept{return storage[index];}
    BulletState* group_first(u32 group)const noexcept{return group<6?heads[group]:nullptr;}
    u32 cursor=0,active_count=0;float exclusion_squared=0;i32 last_error=0;
#ifdef TH11_MULTIPLAYER
    std::array<u8,capacity> multiplayer_grazed{};
#endif
    // Original manager +0x44/+0x50, zeroed by 408390/408710.
    Vec2 cancel_center{},cancel_size{};
    i32 spawn(const BulletEmitter&,i32 index,i32 layer,float aim)noexcept;
    bool fire(const BulletEmitter&)override;
    bool update(bool paused=false)noexcept;
    bool draw_group(u32 group,AnmRenderer& renderer);
    i32 update_one(BulletState&)noexcept;
    bool change_appearance(BulletState&,i32,i32,bool)override;
    bool cancel(BulletState&)override;
    bool cancel_all(bool skip_delayed,const BulletCancelContext& context={});
    bool cancel_rectangle(bool reward,const BulletCancelContext& context={});
    bool cancel_circle(const Vec3&,float radius,bool reward,bool skip_delayed,const BulletCancelContext& context={});
    bool convert_circle(const Vec3&,float radius,bool reward,const BulletCancelContext& context={});
    bool cancel_beam(const Vec3&,float half_width,bool reward,const BulletCancelContext& context={});
    bool sound(i32 id,float x,bool positional)override{return world.sound(id,x,positional);}
    void release(BulletState&)noexcept;
    void reset()noexcept;
private:
    std::unique_ptr<BulletState[]> storage;
    AnmResource& resource;AnmEnvironment& animations;BulletWorld& world;u16 file_id;
    std::array<BulletState*,6> heads{},tails{};u32 fire_depth=0;
    bool appearance(BulletState&,i32,i32,bool);
};
}
