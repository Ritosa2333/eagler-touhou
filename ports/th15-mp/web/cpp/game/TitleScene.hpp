#pragma once
#include "TitleFrame.hpp"
#include "TitleMainMenu.hpp"
#include "TitleSelectionMenu.hpp"
#include "TitlePracticeMenu.hpp"
#include "TitlePracticeDraw.hpp"
#include "TitleOptionsMenu.hpp"
#include "TitleControllerMenu.hpp"
#include "TitleMusicRoom.hpp"
#include "TitleManualMenu.hpp"
#include "TitleResultsMenu.hpp"
#include "TitleReplaySave.hpp"
#include "TitlePlayerData.hpp"
namespace th15 {
struct TitleSceneResources {i32 title=16,portrait=17,ascii=5,text=0,help=19;};
struct TitleScenePlatform:TitleFramePlatform,TitleResumeServices,TitleOptionsServices,TitleControllerServices,TitleMusicServices,TitleReplayServices,ManualServices,TitleResultsServices,TitleReplaySaveServices,TitlePlayerDataServices,HudDrawServices,ReplayCalendarServices {
 virtual bool sound(i32)override=0;virtual bool text(AnmVm&,const DialogueText&)override=0;
 virtual bool music(const std::string&)override=0;virtual bool music(const std::string&,i32)override=0;
 virtual bool begin_transition(u32&)override=0;virtual bool release_live_replay()override=0;
};
// All original title screens share one cursor history, named record store,
// animation registry and callback scheduler. The platform owns file/audio/GPU
// work and queues app destinations until this update pass has completed.
class TitleScene final:private TitleFrameServices {
public:
 TitleState state;ReplayCatalog catalog;ReplayRecording* live=nullptr;i32 saved_replay_selection=0;std::string error;
private:
 SessionState& progress;PlayerLifeSession& player;RecordStore& records;AnmManager& animations;TitleScenePlatform& host;FrameScheduler& scheduler;TitleSceneResources resources;TitleFrameInput input;u32 gamepad_buttons=0;i32 numbered_chapter=0;
 std::unique_ptr<Manual> manual;std::unique_ptr<TitleManualMenu> manual_menu;FrameCallback manual_callback;
 TitleMainMenu main;TitleModeMenu mode;TitleDifficultyMenu difficulty;TitleCharacterMenu character;TitlePracticeMenu practice;TitleResumeMenu resume;
 TitleOptionsMenu options;TitleControllerMenu controller;TitleMusicRoom music_room;TitleReplayMenu replay_menu;TitlePracticeDraw practice_draw;TitleResultsMenu results;TitleReplaySave replay_save;TitlePlayerData player_data;
 bool check(bool,const std::string&);bool start_manual();bool manual_step();void close_manual();
 bool release_transient_animations()override;bool clear_title_overlay()override;bool read_demo(i32,std::shared_ptr<Replay>&)override;bool begin_demo(const ReplayStartRequest&)override;
 bool queue_title_music(i32,i32,const std::string&)override;bool clear_current_wave()override;bool reset_replay_selection()override;bool return_practice_transition()override;
 bool title_exit(i32)override;bool fade_out_title_music()override;bool update_title_menu(TitleScreen,const TitleFrameInput&)override;bool draw_title_menu(TitleScreen)override;
public:
 TitleFrame frame;
 TitleScene(SessionState&,PlayerLifeSession&,ItemScoreState&,RecordStore&,TitleSelectionSettings&,TitleAudioSettings&,TitleControllerSettings&,const MusicComments&,AnmManager&,TitleScenePlatform&,FrameScheduler&,TitleSceneResources={});
 ~TitleScene();TitleScene(const TitleScene&)=delete;TitleScene& operator=(const TitleScene&)=delete;
 void controls(const TitleFrameInput&,u32 gamepad=0,i32 chapter=0)noexcept;bool manual_page_ready();void replay_catalog_ready()noexcept{replay_menu.catalog_finished();}
};
}
