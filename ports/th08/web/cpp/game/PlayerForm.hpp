#pragma once
#include "PlayerMovement.hpp"
#include "PlayerOptions.hpp"
namespace th08 {
struct PlayerFormState {u8 focused=0,youkai=0,reserved[2]{};i32 frames=0;Timer transition;AnmVm* focus_effect=nullptr;};
struct PlayerFormActions {
    virtual ~PlayerFormActions()=default;
    virtual void animation(i32 script)=0;
    virtual void effect(i32 type,const Vec3& position,u32 color)=0;
    virtual AnmVm* focus_effect(const Vec3& position)=0;
};
void update_player_form(PlayerFormState&,PlayerMovementState&,PlayerOption* options,u8 character,u16 buttons,i32 bomb,i32 bomb_type,bool always_hitbox,PlayerFormActions&);
}
