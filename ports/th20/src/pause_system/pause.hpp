#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../stone_menu/cursor.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../gameplay/gameplay.hpp"
#include "../game_session/session.hpp"
namespace th20::source::pause {
struct PauseInf final:runtime::CallbackOwner {
    recovered::Timer age,secondary_age;               //10,20
    menu::Cursor cursor,name_cursor;                   //30,7c
    std::uint32_t panel_handle,background_handle;      //c8,cc
    std::int32_t state,previous_state,substate,name_length; //d0..dc
    std::int32_t completed,field_e4,cancel_disabled;
    std::uint32_t saved_input;                        //ec
    runtime::CallbackOwner* metadata[25];             //f0..150
    std::uint8_t retained_154[0x64];
    char player_name[9];                              //1b8..1c0
    std::uint8_t untouched_1c1[3];
    float saved_clock_scale;                         //1c4
    double saved_music_position;                     //1c8
    char saved_music_name[256];                      //1d0
    std::uint32_t menu_flags;                         //2d0, low 3 initialized only
    sprite::AnimationFile* file;                     //2d4
    PauseInf();                                      //4e1ce0
    ~PauseInf() override;                            //4e1e90
};
static_assert(sizeof(PauseInf)==0x2d8&&offsetof(PauseInf,cursor)==0x30&&offsetof(PauseInf,metadata)==0xf0&&offsetof(PauseInf,menu_flags)==0x2d0);
class Services {
public:
    virtual ~Services()=default;
    virtual gameplay::GameController& game()=0;
    virtual game_session::Session& session()=0;
    virtual std::uint32_t& input_latch()=0;
    virtual std::uint32_t graphics_flags()=0;
    virtual int& replay_selection()=0;
    virtual float clock_scale()=0;
    virtual void set_clock_scale(float)=0;
    virtual bool pressed(std::uint32_t)=0;
    virtual void select_scene(int,bool guarded)=0;
    virtual void update_playtime()=0;
    virtual void reset_playtime_origin()=0;
    virtual sprite::AnimationFile* hud_file()=0;
    virtual std::uint32_t spawn_panel(sprite::AnimationFile&,int)=0;
    virtual void delete_animation(std::uint32_t&)=0;
    virtual void interrupt_animation(std::uint32_t&,int)=0;
    virtual void capture_background(PauseInf&)=0;
    virtual void capture_practice_background(PauseInf&)=0;
    virtual void stop_effects()=0;
    virtual void effect(int)=0;
    virtual void pause_music()=0;
    virtual int poll_audio()=0;
    virtual const char* music_name()=0;
    virtual double music_position()=0;
    virtual void play_game_over_music()=0;
    virtual bool dialogue_present()=0;
    virtual void show_dialogue(bool)=0;
    virtual void show_hud_message(bool)=0;
    virtual void hide_hud_numbers()=0;
    virtual bool replay_finished()=0;
    virtual void mark_replay_finished()=0;
};
PauseInf* controller();
Services& services();
PauseInf* create();                                   //4e6b80
int initialize(PauseInf&);                            //4e5980
void set_state(PauseInf&,int);                        //4e6a90
void set_substate(PauseInf&,int);                     //4e69a0
void open_pause(PauseInf&,Services&);                 //4e5d60
void finish_game(PauseInf&,Services&);                //4e5bd0
void finish_practice(PauseInf&,Services&);            //4e59e0
void finish_replay(PauseInf&,Services&);              //4e5f30
void restore_after_pause(PauseInf&,Services&);        //4e58f0
void restore_after_result(PauseInf&,Services&);       //4e58a0
int update(PauseInf&,Services&);                     //4e20d0
void update_menu(PauseInf&,Services&);                //4e2260
int draw(PauseInf&);                                 //4e4d50, EAX=1
}
