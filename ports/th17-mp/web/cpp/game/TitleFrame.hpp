#pragma once
#include "TitleAnimations.hpp"
#include "Replay.hpp"
#include <memory>
namespace th17 {
struct TitleFrameInput {u32 held=0,pressed=0,repeated=0,system_flags=0;float rate=1;};
struct DemoRequest {std::string filename;i32 index=0,stage=1,character=0,animal=0,difficulty=1;};
struct TitleFrameServices {
 virtual ~TitleFrameServices()=default;
 virtual bool release_transient_animations()=0;virtual bool clear_title_overlay()=0;
 virtual bool read_demo(i32,std::shared_ptr<Replay>&)=0;virtual bool begin_demo(const DemoRequest&)=0;
 virtual bool queue_title_music(i32,i32,const std::string&)=0;virtual bool clear_current_wave()=0;virtual bool unlock_title_music()=0;
 virtual bool selection_return_transition()=0;virtual bool title_exit(i32)=0;virtual bool fade_out_title_music()=0;
 virtual bool update_title_menu(TitleScreen,const TitleFrameInput&)=0;virtual bool draw_title_menu(TitleScreen)=0;
 virtual bool ending_active()const=0;virtual bool ending_complete()const=0;virtual bool retire_ending()=0;
};
// TH17 title callback 0x4515d0 and text callback 0x451dd0. The application
// supplies file/audio destinations; menu controllers own their ANM trees.
class TitleFrame {
 TitleState& state;GameSelection& selection;TitleFrameServices& host;TitleAnimations visuals;
 bool check(bool,const char*);bool menu(const TitleFrameInput&);bool initialize(const TitleFrameInput&);bool demo(const TitleFrameInput&);bool restore_music(bool);
public:
 i32 demo_idle=0,demo_index=0,saved_difficulty=0,music_age=0;bool alternate_audio=false,draw_enabled=false;std::string error;
 TitleFrame(TitleState& s,GameSelection& g,AnmManager& a,TitleFrameServices& h,i32 title=16,i32 ascii=0):state(s),selection(g),host(h),visuals(s,a,title,ascii){}
 bool update(const TitleFrameInput&);bool draw();
};
}
