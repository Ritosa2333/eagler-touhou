#pragma once
#include "item.hpp"
namespace th20::source::special_state {struct Controller;}
namespace th20::source::item {
// Calls crossing into the HUD, score glyph owner and player entity are required
// source dependencies. Tests may observe those boundaries without replacing the
// arithmetic, Player mutations, collection ordering or overlay counter logic.
class RewardEnvironment {
public:
    virtual ~RewardEnvironment()=default;
    virtual bool hud_available()=0;
    virtual void hud_bombs(int count,int fragments,int maximum)=0; //4b8650
    virtual void hud_lives(int count,int fragments,int maximum)=0; //4b8bf0
    virtual void hud_notice(int type,int value)=0; //4b90e0
    virtual void sound(int id)=0; //426d70, second argument zero
    virtual void sound_at(int id,float x)=0; //426eb0
    virtual void floating_score(void* owner,const sprite::Vec3&,int amount,std::uint32_t color)=0; //510710
    virtual void refresh_power(void* player,int)=0; //4faca0
    virtual bool boss_collecting()=0; //478160
    virtual bool special_active()=0; //513dd0->485a40
    virtual void start_special_phase()=0; //534d00
};
RewardEnvironment& reward_environment();
bool add_power(game_session::Player&,int,RewardEnvironment&); //4e1410
void add_bombs(game_session::Player&,int,RewardEnvironment&); //4e10e0
void add_bomb_fragments(game_session::Player&,int,RewardEnvironment&); //4e11a0
int add_lives(game_session::Player&,int,RewardEnvironment&); //4e1250
void extend_life(game_session::Player&,RewardEnvironment&); //4e1510
void add_life_fragments(game_session::Player&,int,int difficulty,RewardEnvironment&); //4e1310
void add_special_counter(game_session::Player&,unsigned index,int) noexcept; //4a9ec0/4a9d80/4a9f20/4a9e60
void add_point_items(game_session::Player&,int) noexcept; //4c46a0
void add_special_items(game_session::Player&,int) noexcept; //4a9fb0
void add_overlay_meter(runtime::CallbackOwner&,int,RewardEnvironment&); //533780
void collect_point(Item&,RewardEnvironment&); //4c4960
void collect_small_power(Item&,RewardEnvironment&); //4c4b90
void collect_large_power(Item&,RewardEnvironment&); //4c4da0
void collect_full_power(Item&,RewardEnvironment&); //4c47c0
void collect_item(Item&,RewardEnvironment&); //4c25a0 pickup dispatch
namespace unrecovered {
void hud_bombs_004b8650(void*,int,int,int);
void hud_lives_004b8bf0(void*,int,int,int);
void hud_notice_004b90e0(void*,int,int);
void floating_score_00510710(void*,const sprite::Vec3&,int,std::uint32_t);
void refresh_player_power_004faca0(runtime::CallbackOwner&,int);
special_state::Controller* special_state_00513dd0();
void start_special_phase_00534d00();
}
}
