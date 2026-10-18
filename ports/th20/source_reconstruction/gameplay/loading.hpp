#pragma once
#include "gameplay.hpp"
#include "stage_data.hpp"
#include "../progress_state/profile.hpp"
namespace th20::source::gameplay {
// Exact profile record stride used by4bd460. Owning save-manager construction,
// decryption, checksum and disk writes must be supplied by its real module.
using Profile=progress::Profile;
enum class Entity {
    fn_004ffff0,fn_0050a930,fn_00477880,fn_004b9090,fn_00486630,
    fn_004c5010,fn_004d7e40,fn_004e6b80,fn_005108d0,
    fn_004aba70,fn_00478300,fn_00488b20
};
class LoadingServices {
public:
    virtual ~LoadingServices()=default;
    virtual game_session::Session& session()=0;
    virtual void begin_frame_interval()=0;                  //451520(5c4a00,0)
    virtual bool sprite_task_pending()=0;                   //Controller.draw_state[0][0]>=0
    virtual std::uint32_t graphics_events()=0;              //Graphics+b48
    virtual std::uint32_t new_game_state()=0;               //49c000: Graphics+b18
    virtual void sleep(std::uint32_t milliseconds)=0;       //real Win32 Sleep
    virtual void interrupt_surface(unsigned,bool execute)=0;//4388f0/42b5d0 or486330
    virtual std::int32_t selected_profile(int slot,int character)=0; //464100 exact two stack args
    virtual Profile& profile(int character,int index)=0;    //4bd460
    virtual void configure_overlay(int character)=0;        //534080
    virtual player_state::BombObserver* bomb_observer()=0;  //nullable5c06a4
    virtual std::int32_t replay_selection()=0;              //5afcfc, initialized -1
    virtual const char* replay_path()=0;                    //5c4b20
    virtual void register_callbacks(GameController&)=0;     //412310(priority19)/4123b0(priority2)
    virtual const platform::Configuration& configuration()=0;
    virtual bool create(Entity,int,const char*)=0;           //actual factory return !=null
    virtual void create_auxiliary_owner()=0;                //514240, unchecked original result
    virtual void reset_replay_owner()=0;                    //50a6b0(5c60fc)
    virtual void reset_hud_owner()=0;                       //4b5960(5c06a4)
    virtual void reset_existing_stage()=0;                  //478060(0)->4a7040
    virtual void commit_progress()=0;                       //50f660 (non-null required here)
    virtual void queue_track(int,const char*)=0;            //4d9a70 append .wav enqueue1
    virtual void reset_frame_statistics()=0;                //4bdcb0/4be200
    virtual bool audio_commands_pending()=0;                //SoundInf.commands[0].type
    virtual bool effects_ready()=0;                         //437520(0)->438520
    virtual void finish_entity_initialization()=0;          //460830(0)->4faca0(-1)
    virtual void finish_loading_display(bool success)=0;    //4d95b0/4d94e0
    virtual void clear_slowdown_counter()=0;                //5ba518, actual shared global
    virtual double read_clock()=0;                         //41cb10
};
std::int32_t load_gameplay(GameController&,LoadingServices&); //4bad40
}
