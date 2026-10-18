#pragma once
#include "title.hpp"
#include "../ending_scene/ending.hpp"
#include "../replay_system/replay.hpp"
namespace th20::source::title {
// Actual process state used by 51e3c0. The environment separates OS/resource
// boundaries from the complete frame control flow; production uses one adapter.
struct FrameEnvironment {
    MainEnvironment& main;
    std::uint32_t& menu_selection;
    std::uint32_t& slowdown_frames;
    int& replay_selection;
    char (&replay_filename)[256];
    std::int32_t& next_scene;
    char& queued_track_first;
    int& data_character;
    int& data_profile;
    FrameEnvironment(MainEnvironment& m,std::uint32_t& menu,std::uint32_t& slow,int& selection,char (&name)[256],std::int32_t& next,char& queued,int& character,int& profile)
        :main(m),menu_selection(menu),slowdown_frames(slow),replay_selection(selection),replay_filename(name),next_scene(next),queued_track_first(queued),data_character(character),data_profile(profile){}
    virtual ~FrameEnvironment()=default;
    virtual ending::EndingInf* current_ending()=0;
    virtual const input::ButtonState* buttons()=0;
    virtual replay::ReplayInf* read_replay(const char*)=0;
    virtual void retire(runtime::CallbackOwner*)=0;
    virtual void select_stage(int)=0;
    virtual void select_profile(int,int)=0;
    virtual void stop_music()=0;
    virtual void play_music(const char*)=0;
    virtual std::uint32_t entropy()=0;
    virtual void seed(int,std::uint32_t)=0;
    virtual void enable_stones()=0;
    virtual void release_mesh(sprite::RenderMesh*)=0;
    virtual void hide_file(int)=0;
    virtual void clear_loading()=0;
    virtual void background(TitleInf&)=0;
    virtual void transition_effect()=0;
    virtual void request_scene(int)=0;
    virtual void update_page(TitleInf&,int)=0;
    virtual void draw_page(TitleInf&,int)=0;
};
FrameEnvironment& frame_environment();
int update(TitleInf&,FrameEnvironment&); //51e3c0 (outer52c1e0 returns1)
int draw(TitleInf&,FrameEnvironment&); //51f220
}
