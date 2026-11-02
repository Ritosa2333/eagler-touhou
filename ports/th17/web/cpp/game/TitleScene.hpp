#pragma once
#include "TitleFrame.hpp"
#include "TitleMainMenu.hpp"
#include "TitleAnimalMenu.hpp"
#include "TitleOptionsMenu.hpp"
#include "TitleControllerMenu.hpp"
#include "TitleMusicRoom.hpp"
#include "TitleManualMenu.hpp"
#include "TitleResultsMenu.hpp"
#include "TitleReplaySave.hpp"
#include "TitlePlayerData.hpp"
#include "TitleTrophies.hpp"
#include "FrameScheduler.hpp"
namespace th17 {
struct TitleSceneResources {i32 title=16,portrait=17,ascii=0,text=6,help=19;};
struct TitleScenePlatform:TitleFrameServices,TitleTrophyServices,TitleOptionsServices,TitleControllerServices,TitleMusicServices,TitleReplayServices,ManualServices,TitleResultsServices,TitleReplaySaveServices,TitlePlayerDataServices,HudDrawServices,ReplayCalendarServices {
 virtual bool sound(i32)override=0;virtual bool text(AnmVm&,const TextureText&)override=0;virtual bool keyboard(TitleKeyboard&)override=0;
 virtual bool music(const std::string&)override=0;virtual bool music(const std::string&,i32)override=0;
 virtual bool begin_transition(u32&)override=0;virtual bool release_live_replay()override=0;
};
// One set of named controllers owns the original title cursor stack and ANM
// handles. File, audio and screen transitions are supplied by the application.
class TitleScene final:private TitleFrameServices {
public:
 TitleState state;ReplayCatalog catalog;ReplayRecording* live=nullptr;i32 saved_replay_selection=0;std::string error;
private:
 GameRun& progress;GameSelection& selection;RecordStore& records;AnmManager& animations;TitleScenePlatform& host;FrameScheduler& scheduler;TitleSceneResources resources;TitleFrameInput input;u32 gamepad_buttons=0;
 std::unique_ptr<Manual> manual;std::unique_ptr<TitleManualMenu> manual_menu;FrameCallback manual_callback;
 TitleMainMenu main;TitleDifficultyMenu difficulty;TitleCharacterMenu character;TitleAnimalMenu animal;TitleStageMenu practice;
 TitleOptionsMenu options;TitleControllerMenu controller;TitleMusicRoom music_room;TitleReplayMenu replay_menu;TitleResultsMenu results;TitleReplaySave replay_save;TitlePlayerData player_data;TitleSpellPractice spell_practice;TitleTrophies trophies;
 bool check(bool,const std::string&);bool start_manual();bool manual_step();void close_manual();
 bool release_transient_animations()override;bool clear_title_overlay()override;bool read_demo(i32,std::shared_ptr<Replay>&)override;bool begin_demo(const DemoRequest&)override;
 bool queue_title_music(i32,i32,const std::string&)override;bool clear_current_wave()override;bool unlock_title_music()override;bool selection_return_transition()override;
 bool title_exit(i32)override;bool fade_out_title_music()override;bool update_title_menu(TitleScreen,const TitleFrameInput&)override;bool draw_title_menu(TitleScreen)override;
 bool ending_active()const override;bool ending_complete()const override;bool retire_ending()override;
public:
 TitleFrame frame;
 TitleScene(GameRun&,GameSelection&,PlayerLifeSession&,ItemScoreState&,RecordStore&,TitleSelectionSettings&,TitleAudioSettings&,TitleControllerSettings&,const MusicComments&,const TrophyCatalog&,TitleSpellMemory&,AnmManager&,TitleScenePlatform&,FrameScheduler&,TitleSceneResources={});
 ~TitleScene();TitleScene(const TitleScene&)=delete;TitleScene& operator=(const TitleScene&)=delete;
 void controls(const TitleFrameInput& value,u32 pad=0)noexcept{input=value;gamepad_buttons=pad;}
 bool update(){return check(frame.update(input),frame.error);}bool draw(){return check(frame.draw(),frame.error);}
 bool manual_page_ready();void replay_catalog_ready()noexcept{replay_menu.catalog_finished();}
};
}
