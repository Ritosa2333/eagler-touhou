#pragma once
#include "PlayerLife.hpp"
namespace th08 {
enum class PlayerBombKind:u32 {Reimu,Yukari,ReimuLast,YukariLast,LastWord,Marisa,Alice,MarisaLast,AliceLast,Sakuya,Remilia,SakuyaLast,RemiliaLast,Youmu,Yuyuko,YoumuLast,YuyukoLast,Invalid};
PlayerBombKind player_bomb_kind(u8 character,i32 type)noexcept;
struct PlayerBombState {
    i32 active=0,type=0,duration=0,reserved0c=0,consumed=0,sequence=0;Timer timer;
    i32 cooldown=0,triggered=0;
};
struct PlayerBombBoss {i32 life=0;u32 flags=0;};
struct PlayerBombContext {
    u16 buttons=0,previous_buttons=0;u8 gui_blocked=0,tampered=0,cheats=0,reserved=0;
    u32 regular_bombs=0,last_spells=0;PlayerBombBoss* bosses[8]{};
};
struct PlayerBombActions {
    virtual ~PlayerBombActions()=default;
    virtual void update(PlayerBombKind kind)=0;
    virtual void finish_spell_overlay()=0;
    virtual void defeat_boss(u32 slot)=0;
    virtual void last_word_flash()=0;
    virtual void add_gauge(i16 amount,bool force)=0;
    virtual void sound(i32 index,i32 mode)=0;
    virtual void reset_screen_color()=0;
    virtual void add_bombs(i32 amount)=0;
    virtual void set_bombs(i32 amount)=0;
    virtual void count_bomb(i32 amount)=0;
    virtual void subtract_rank(i32 amount)=0;
    virtual void fail_spell_with_bomb()=0;
};
// Original 0044c650: release gating, normal/deathbomb choice and callback order.
// The selected callback owns each bomb's animation, duration, damage and motion.
bool update_player_bomb(PlayerBombState&,PlayerBombContext&,PlayerLifeState&,PlayerLifeContext&,PlayerMovementState&,AnmVm&,const ShotProfile&,const FrameTiming&,PlayerBombActions&);
}
