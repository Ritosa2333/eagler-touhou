#pragma once
#include "PlayerMotion.hpp"
#include "PlayerBomb.hpp"
#include "PlayerShots.hpp"
namespace th08 {
struct PlayerResourceNames {const char* animation;const char* human;const char* focused;};
const PlayerResourceNames* player_resource_names(u8 character)noexcept;
struct PlayerSetupContext {u8 character=0;bool initial=true,spell_practice=false;u8 section_warp=0;Vec2 extent{384,448};};
struct PlayerSetupActions {
    virtual ~PlayerSetupActions()=default;
    virtual bool load_shots(bool focused,const char* path)=0;
    virtual bool load_animation(const char* path)=0;
    virtual void retained_animation()=0;
    virtual void animation(i32 script)=0;
    virtual void hud_interrupt(i32 value)=0;
    virtual void hud_group(i32 index,i32 interrupt)=0;
    virtual void time_item_threshold(i32 value)=0;
};
// Original 0044d650; the owner supplies real SHT/ANM resource loading. The
// human profile reference remains valid when load_shots populates the resource.
bool initialize_player(PlayerMotionState&,PlayerLifeState&,PlayerBombState&,PlayerShotsState&,GaugeThresholds&,const ShotProfile& human,const PlayerSetupContext&,PlayerSetupActions&);
}
