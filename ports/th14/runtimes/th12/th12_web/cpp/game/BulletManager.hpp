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
    virtual ~BulletWorld()=default;
#if PORT_TITLE == 14
    virtual bool th14_spawn_enemy(const char*,const EnemySpawn&){return false;}
    virtual bool th14_bullet_hit_effect(const Vec3&,i32,const Vec3&){return false;}
#endif
    virtual bool sound(i32,float,bool){return false;}
    virtual i32 collision(const BulletState&){return -2;}
    virtual i32 th14_probe_collision(const BulletState&){return -2;}
    virtual bool th14_curve_graze(Vec3){return false;}
    virtual bool th16_hit_effect(const Vec3&,i32,const Vec3&){return false;}
    virtual bool th16_graze(const Vec3&){return false;}
    virtual bool effect(const Vec3&,i32){return false;}
    virtual bool colored_effect(const Vec3&,i32,u32){return false;}
    virtual bool register_graze(){return false;}
    virtual bool reward_graze(){return false;}
    virtual bool cancel_reward(const Vec3&){return false;}
    virtual bool th14_cancel_item(const Vec3&){return false;}
    virtual bool th14_cancel_reward(const Vec3& p,u32 rewards,u32){return rewards==1&&th14_cancel_item(p);}
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
    // Original manager +0x44/+0x50, zeroed by 408390/408710.
    Vec2 cancel_center{},cancel_size{};
    i32 spawn(const BulletEmitter&,i32 index,i32 layer,float aim)noexcept;
    bool fire(const BulletEmitter&)override;
#if PORT_TITLE == 16
    i32 spawn_th16(const Th16BulletEmitter&,i32,i32,float)noexcept;
    bool fire_th16(const Th16BulletEmitter&);
    i32 update_th16(BulletState&)noexcept;
    BulletState* first_th16()noexcept{return th16_active;}
    bool cancel_th16(BulletState&,u32 rewards=0);
    bool cancel_circle_th16(const Vec3&,float,bool,u32 rewards=0);
    bool cancel_all_th16();
    u32 th16_reward_count=0;
    Vec2 th16_bounce_bounds{}; // Own manager+48/+4c, ECL803.
#endif
#if PORT_TITLE == 14
    i32 spawn_th14(const Th14BulletEmitter&,i32 index,i32 layer,float aim)noexcept;
    bool fire_th14(const Th14BulletEmitter&)override;
    bool th14_spawn_enemy(const char* name,const EnemySpawn& value)override{return world.th14_spawn_enemy(name,value);}
    i32 update_th14(BulletState&)noexcept;
    bool cancel_th14(BulletState&,u32 rewards=0);
    bool cancel_circle_th14(Vec3,float,u32 rewards=0,bool skip_delayed=false);
    bool cancel_rectangle_th14(Vec3,Vec2,float angle,u32 rewards=0);
    bool cancel_all_th14();
    const BulletState* first_th14()const noexcept{return th14_active;}
    BulletState* first_th14()noexcept{return th14_active;}
    u32 th14_reward_count=0;
#endif
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
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    friend class GameBattle;
    BulletManager(AnmResource&,AnmEnvironment&,BulletWorld&,u16 file_id,bool defer_storage);
    void initialize_storage();
#endif
    std::unique_ptr<BulletState[]> storage;
    AnmResource& resource;AnmEnvironment& animations;BulletWorld& world;u16 file_id;
    std::array<BulletState*,6> heads{},tails{};u32 fire_depth=0;
    bool appearance(BulletState&,i32,i32,bool);
#if PORT_TITLE == 16
    BulletState* th16_free=nullptr;BulletState* th16_active=nullptr;
    i32 collision_th16(BulletState&);
    bool appearance_th16(BulletState&,i32,u32)noexcept;
#endif
#if PORT_TITLE == 14
    bool appearance_th14(BulletState&,i32,i32,bool,bool advance=true);
    i32 collision_th14(BulletState&,bool probe=false);
    BulletState* th14_free=nullptr;BulletState* th14_active=nullptr;
#endif
};
}
