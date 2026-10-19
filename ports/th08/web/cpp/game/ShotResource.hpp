#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
// Callback identities are game behaviours, never executable addresses from the PE.
enum class ShotCreate : u32 {Periodic,EnemyAimed,OutsideBomb,Laser,TargetAimed,OptionAimed,RandomSpread,OrbitAimed};
enum class ShotUpdate : u32 {None,Homing,Accelerating,PlayerLaser,OptionLaser};
enum class ShotDraw : u32 {None,Laser};
enum class ShotHit : u32 {None,Explosion,Laser};
struct ShotProfile {
    u16 reserved=0,stream_count=0;float initial_bombs=0;i32 deathbomb_limit=0;
    float hit_size=0,graze_size=0,item_homing_speed=0,item_radius=0,item_collect_line=0;
    i32 reserved20=0;float normal_speed=0,focus_speed=0,normal_diagonal=0,focus_diagonal=0,item_fall_speed=0;
};
static_assert(sizeof(ShotProfile)==56);
struct ShotDefinition {
    i16 interval=0,phase=0;Vec2 offset,size;float angle=0,speed=0;
    i16 damage=0,gauge=0,option=0,reserved22=0,animation=0,sound=0;
    ShotCreate create=ShotCreate::Periodic;ShotUpdate update=ShotUpdate::None;
    ShotDraw draw=ShotDraw::None;ShotHit hit=ShotHit::None;
};
static_assert(sizeof(ShotDefinition)==56);
struct ShotStream {i32 power_limit=0;std::vector<ShotDefinition> shots;};
class ShotResource {
public:
    bool load(const u8* bytes,u32 size);
    void release(){profile={};streams.clear();}
    const ShotProfile& settings()const noexcept{return profile;}
    const ShotStream* stream(u32 index)const noexcept{return index<streams.size()?&streams[index]:nullptr;}
    // 00450f60: threshold is exclusive; Remilia's bomb uses two extra banks.
    i32 select(i32 power,u8 character,bool bomb_active,i32 bomb_type,bool bomb_ready)const noexcept;
private:
    ShotProfile profile;std::vector<ShotStream> streams;
};
}
