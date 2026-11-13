#pragma once
#include "ShtResource.hpp"
#include "ShotSchedule.hpp"
#include "AnmManager.hpp"
#include "EnemyState.hpp"
#include "DamageArea.hpp"
#include "GameEconomy.hpp"
#include "ShotDamageTh14.hpp"
#if PORT_TITLE == 14
#include "ShotPoolTh14.hpp"
#endif
#if PORT_TITLE == 16
#include "ShotTh16.hpp"
#endif
#include <array>
namespace th11 {
struct ShotState {
    Timer timer;Movement movement;
    i32 state=0;u32 animation=0,extra_animation=0;
    Enemy* target=nullptr;i32 hit_this_frame=0,hit_last_frame=0,damage=0;
    float acceleration=0;const ShotSpec* spec=nullptr;
#if PORT_TITLE == 16
    Th16ShotSpawn th16;
#endif
#if PORT_TITLE == 14
    Timer th14_steering,th14_decay;u32 th14_target_id=0,th14_area_flags=0;
    float th14_area_radius=0,th14_area_growth=0,th14_saved_speed=0;
    u32 th14_area_slot=256,th14_was_focused=0,th14_shot_flags=0;
    float th14_beam_length=0;
    Vec3 th14_attachment_offset{};
    // Preserve the native stale-target validation frame without retaining freed pointers.
    bool th14_homing_target_retired=false;
#endif
};
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(ShotState)==0x6c+(kPortTitle==14?0x5c:0)
#if PORT_TITLE == 16
    +sizeof(Th16ShotSpawn)
#endif
);
#endif
struct ShotOption {i32 x=0,y=0;float angle=0;u32 animation=0;};
struct ShotPlayer {
    Vec3 position;std::array<ShotOption,kPortTitle==16?12:8> options{};
    i32 option_count=0,state=1,warp_state=0,power=0,power_step=20,character=0,subtype=0,weapon_mode=0;
    u32 held=0,focused=0,bomb_active=0,enemies_present=0,special_active=0;
#if PORT_TITLE == 16
    bool th16_enemy_manager=false;
    bool th16_dialogue=false;i32 th16_season_count=0;
#endif
#if PORT_TITLE == 14
    i32 th14_cached_power=0;u32 th14_shooting_blocked=0,th14_focused_hits=0,th14_plain_hits=0,th14_hits_184bc=0;
    // Retail +184b8 is focused_hits; +184b4 is plain_hits. Both are
    // consumed by 453a90, so individual callback fixtures must keep this map.
    bool th14_dialogue_blocked=false,th14_enemy_manager=false;
    i32 th14_attached_count=0;
    NativeStackTh14* th14_native_stack=nullptr;
#endif
};
#if PORT_TITLE == 14
void shot_th14_homing_update(ShotState&,bool manager_present,EnemyLink*)noexcept;
void shot_th14_delayed_turn(ShotState&)noexcept;
bool shot_th14_focused_spawn(ShotState&,const ShotPlayer&,AnmVm&,EnemyLink*,const float*)noexcept;
i32 shot_th14_focused_update(ShotState&,ShotPlayer&,AnmVm&,EnemyLink*,const ShtResource&,bool firing,const float*)noexcept;
// 452050: a request at the ANM/sound boundary, not an allocated damage area.
struct Th14FocusedHit {Vec3 position;float angle=0;i32 damage=0;bool active=false;};
bool shot_th14_focused_hit(ShotState&,ShotPlayer&,Rng&,const float*,Th14FocusedHit&)noexcept;
struct Th14PlainHit {Vec3 position;float angle=0;u32 color=0;bool effect=false;};
bool shot_th14_plain_hit(ShotState&,ShotPlayer&,Rng&,Th14PlainHit&)noexcept;
void shot_th14_spread_spawn(ShotState&,Rng&)noexcept;
void shot_th14_accelerating_update(ShotState&)noexcept;
void shot_th14_flame_spawn(ShotState&,Rng&)noexcept;
i32 shot_th14_flame_update(ShotState&,Th14DamageArea*,Rng&)noexcept;
i32 shot_th14_small_flame_update(ShotState&)noexcept;
bool shot_th14_explosive_hit(ShotState&,ShotPlayer&,Th14DamageArea&)noexcept;
struct Th14BeamUpdate {i32 pan=0,interrupt=0;bool pan_active=false,stop=false;};
void shot_th14_beam_spawn(ShotState&,Th14DamageArea&)noexcept;
bool shot_th14_beam_update(ShotState&,const ShotPlayer&,Th14DamageArea&,AnmVm*,bool firing,u32& cached_power,Th14BeamUpdate&)noexcept;
struct Th14BeamHit {Vec3 position{},travel{};float angle=0;bool effect=false,interrupt=false;i32 damage=0;};
bool shot_th14_beam_intersection(Vec3,Vec3,Vec2,float,float,Vec2&)noexcept;
bool shot_th14_beam_hit(ShotState&,ShotPlayer&,Th14DamageArea&,Vec3,const Vec2*,float,float,Th14BeamHit&)noexcept;
i32 shot_th14_sakuya_spawn(ShotState&,ShotPlayer&,const float*)noexcept;
struct Th14SakuyaUpdate {i32 interrupt=0,level=0,sound=0;bool explode=false,retired=false;Enemy* release=nullptr;float slowdown=0;};
bool shot_th14_sakuya_update(ShotState&,ShotPlayer&,Th14DamageArea&,AnmVm*,EnemyLink*,bool,const float*,Th14SakuyaUpdate&)noexcept;
struct Th14SakuyaHit {i32 interrupt=0,sound=0,damage=0;Enemy* attachment=nullptr;float slowdown=0;};
bool shot_th14_sakuya_hit(ShotState&,ShotPlayer&,Th14DamageArea&,Vec3,const Vec2*,float,EnemyLink*,Th14SakuyaHit&,Enemy* hit_target=nullptr)noexcept;
void shot_th14_sakuya_slowdown(EnemyState&,float,u32,i32)noexcept;
#endif
struct ShotWorld {
    virtual ~ShotWorld()=default;
    virtual bool sound(i32,float){return false;}
    virtual bool special_damage(const Vec3&,const Vec2&,i32&){return false;}
#if PORT_TITLE == 16
    virtual bool th16_shot_score(i32){return false;}
    virtual bool th16_beam_pan(i32){return false;}
    virtual bool th16_beam_stop(){return false;}
    virtual AnmVm* th16_freeze_effect(Vec3,float){return nullptr;}
    virtual bool th16_register_hit_effect(u32){return false;}
#endif
#if PORT_TITLE == 14
    virtual bool th14_hit_item(i32,Vec3,float,float){return false;}
    virtual bool th14_hit_effect(Vec3,float){return false;}
    virtual bool th14_plain_hit_effect(Vec3,float,u32){return false;}
    virtual bool th14_beam_pan(i32){return false;}
    virtual bool th14_beam_stop(){return false;}
    virtual bool th14_beam_hit_effect(Vec3,float,Vec3){return false;}
    virtual bool th14_knife_slowdown(Enemy&,float){return false;}
    virtual bool th14_reimu_bomb_hit_effect(Vec3,float){return false;}
#endif
    // The owner supplies the live enemy list. Targets must be invalidated before deletion.
    EnemyLink* enemies=nullptr;
};
#if PORT_TITLE == 14
bool shot_th14_hit_rewards(ShotPlayer&,Rng&,Vec3,ShotWorld&)noexcept;
Vec3 shot_th14_reward_position(const Th14DamageArea&,Vec3)noexcept;
void shot_th14_reimu_bomb_hit(const Th14DamageArea&,Vec3,Rng&,Vec3&,float&,float polar_z=0)noexcept;
#endif
class ShotManager
#if PORT_TITLE == 14
    :private Th14DamageWorld
#endif
{
public:
#if PORT_TITLE == 14
    // Preserve rejected beam inputs for native comparison after a frame fails.
    std::array<float,12> th14_beam_failure{};
#endif
    ShotManager(ShtResource&,AnmResource&,AnmManager&,ShotWorld&,u16 file=7);
    ShotPlayer player;ShotSchedule schedule;
    std::array<u32,9> laser_active{};
    DamageAreas damage_areas;
    Enemy* locked_target=nullptr;u8 target_locked=0;
    i32 last_error=0;
#if PORT_TITLE == 16
    ShtResource* th16_season_resource=nullptr;
    AnmResource* th16_season_animation=nullptr;
    const Th16SeasonState* th16_season=nullptr;
    Th16ShotSchedule th16_schedule;
    Th16DamageSources th16_sources;
    std::array<u32,16> th16_lasers{};
    i32 spawn_th16(const ShotSpec&,u32 packed,i32 frame,Vec3);
    bool th16_fire_frames(i32,i32);
    bool update_th16();
    bool damage_th16(const Th16DamageQuery&,i32&);
#endif
    i32 spawn(const ShotSpec&,i32 firing_frame,const Vec3& origin);
    // Native laser-cut record 0x4a3a50 and callbacks 435030/435080.
    i32 spawn_converted_laser(Vec3,float);
    bool fire_frame(i32);
    bool fire();
    bool update();
    bool damage(const Vec3&,const Vec2&,bool frame_advanced,GameEconomy&,i32& result);
#if PORT_TITLE == 14
    // 44cdb7..44ce1d rebuilds the free list, retaining shot records;
    // 44d04e resets the damage-area allocation cursor, retaining areas.
    void start_stage_th14()noexcept;
    bool th14_empty_damage(bool frame_advanced,i32& result)const noexcept;
    bool damage_th14(Vec3,const Vec2*,float radius,float angle,bool frame_advanced,GameEconomy&,i32&,u32 argument=0,u32 stamp=0,Enemy* hit_target=nullptr,bool probe=false,bool* bomb_contact=nullptr);
    void update_th14_areas()noexcept;
    bool th14_death_area(Vec3);
    bool th14_bomb_circle(Vec3,float radius,float growth,i32 duration,i32 damage);
    bool th14_bomb_rectangle(Vec3,Vec2,float angle,i32 duration,i32 damage);
    bool th14_reimu_bomb_area(Vec3,Vec2,i32 duration,u32&);
    bool th14_bomb_orb_area(Vec3,u32&);
    Th14DamageArea* th14_bomb_area(u32 index)noexcept{return index<256?&th14_areas[index]:nullptr;}
    const Th14DamageArea* th14_bomb_area(u32 index)const noexcept{return index<256?&th14_areas[index]:nullptr;}
    const ShotState* th14_area_owner(u32 index)const noexcept{return index<256?th14_area_owners[index]:nullptr;}
#endif
    void invalidate_target(Enemy*)noexcept;
    ShotState& at(u32 index)noexcept{return shots[index];}
    const ShotState& at(u32 index)const noexcept{return shots[index];}
private:
    ShtResource& resource;AnmResource& animation_resource;AnmManager& animations;ShotWorld& world;u16 file_id;
    std::array<ShotState,256> shots{};
    ShotSpec converted_laser_spec{30,0,20,{0,0},{24,24},0,12,0,0,4,5,24,ShotSpawn::ConvertedLaser,ShotUpdate::ConvertedLaser,0,0};
    bool on_spawn(ShotState&,i32);
    bool on_update(ShotState&);
    Enemy* nearest(Vec3,float)const noexcept;
    void interrupt(u32,i16);void erase(u32);
#if PORT_TITLE == 14
    std::array<Th14DamageArea,256> th14_areas{};
    Th14ShotPool th14_shot_pool;
    std::array<ShotState*,256> th14_area_owners{};u32 th14_area_cursor=0;
    Enemy* th14_damage_target=nullptr;
    bool hit(u32,Th14DamageArea&,const Vec3&,const Vec2*,float,float,i32&)override;
    bool bind_th14_area(ShotState&);
    void sync_th14_area(ShotState&)noexcept;
#endif
};
}
