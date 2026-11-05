#pragma once
#include "loading.hpp"
namespace th20::source::gameplay {
extern std::int32_t replay_selection;        // initialized5afcfc=-1
extern char replay_file[256];                // BSS5c4b20;523440 bounded copy size100
extern std::uint32_t slowdown_frames;        // BSS5ba518;4ba4a0 countdown,4bad40 reset
extern std::uint32_t loading_overlay_state;  // BSS5c4d3c,4d94e0/4d95b0 reset
extern std::uint32_t loading_animation_handles[3]; // BSS5c5b28/2c/30, outside the original de8-byte Graphics
LoadingServices& loading_services();
namespace unrecovered {
// Save operations retain their actual unknown owning manager. No empty profile
// storage or success return is fabricated while that manager remains pending.
std::int32_t selected_profile(int slot,int character);      //464100
Profile& progress_profile(int character,int index);        //4bd460 on5c6108
void configure_overlay(int character);                    //534080 on464230(0)
player_state::BombObserver* bomb_observer();               //5c06a4 +4b8650
int update_game(GameController&);                         //4ba4a0
int draw_game(GameController&);                           //4ba870
runtime::CallbackOwner* create_004ffff0(int);
runtime::CallbackOwner* create_0050a930(int,const char*);
runtime::CallbackOwner* create_004b9090();
runtime::CallbackOwner* create_00486630(int);
runtime::CallbackOwner* create_004c5010(int);
runtime::CallbackOwner* create_004d7e40(int);
runtime::CallbackOwner* create_004e6b80();
runtime::CallbackOwner* create_005108d0(int);
runtime::CallbackOwner* create_00478300(int);
runtime::CallbackOwner* create_00488b20(int);
void create_auxiliary_owner();                            //514240 ->5117e0
void reset_replay_owner();                                //50a6b0
void reset_hud_owner();                                   //4b5960
void commit_progress();                                   //50f660 nonnullable
bool effects_ready();                                    //437520(0)->438520
void finish_entity_initialization();                      //460830(0)->4faca0(-1)
}
}
