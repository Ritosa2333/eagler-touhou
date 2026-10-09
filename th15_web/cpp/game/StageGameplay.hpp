#pragma once
#include "StageAssets.hpp"
#include "BattleSceneBindings.hpp"
#include "BattleCheckpoint.hpp"
#include "ChapterReward.hpp"
#include "AnmSceneEffects.hpp"
#include "StageDeformation.hpp"
#include "GameInput.hpp"
#include "RunStageObjects.hpp"
namespace th15 {
// Platform/session work stays explicit: fonts, sound, records, persistence and
// scene destinations are outside the stage's game-object ownership.
struct StageGameplayServices:BattleWorldServices,StageSceneServices,MessageSceneServices,DialogueSceneServices,CheckpointSceneServices {};
struct StageGameplayFrame {BattleFrame battle;DialogueInput dialogue;StageSceneFrame background;};
class StageGameplay {
 struct Services;std::unique_ptr<Services> services;
 AnmManager& animations;AnmEnvironment& environment;SessionState& progress;StageAssets& assets;StageGameplayServices& platform;
 // The scheduler must outlive all externally owned callbacks, including battle.
 FrameScheduler owned_scheduler;FrameScheduler& scheduler;std::array<FrameCallback,3> callbacks;
 StageGameplayFrame input;bool prepared=false,started=false,suspended=false,entry_updates=false,background_ready=false,main_started=false;
 std::unique_ptr<RunStageObjects> owned_run;RunStageObjects& run;
 std::unique_ptr<StageDeformation> background_deformation;
 bool fail(const std::string&);i32 update_background();i32 update_popups();i32 update_dialogue();
public:
 GameBattle& battle;StageScene background;GameHud& hud;PopupManager& popups;
 MessageSceneState music;DialogueAnmHost dialogue_host;MessageController messages;
 BattleSceneBindings bindings;ChapterReward reward;BattleCheckpoint checkpoint;
 std::string error;
 std::function<bool()> stage_completion,game_over_begin;
 StageGameplay(StageAssets&,AnmManager&,AnmEnvironment&,Rng& game,Rng& visual,SessionState&,StageGameplayServices&,i32* requested_chapter=nullptr,RunStageObjects* retained=nullptr,FrameScheduler* shared_scheduler=nullptr);
 ~StageGameplay();
 bool prepare(const StageCamera&);bool reset_for_entry();bool start_for_entry(i32 scene_destination);bool begin(i32 scene_destination);bool step(const StageGameplayFrame&);
 bool initialize_player(bool configure=true);bool initialize_background(const StageCamera&);bool initialize_popups();bool enable_entry_updates();
 bool start_main_script();bool initialize_hud(i32 scene_destination);bool enable_game_callbacks();
 bool chapter_reward(bool boss);bool capture(i32 chapter);bool restore();bool clear_dialogue_field();
 bool skip_chapter_reward()noexcept{return assets.practice_active&&assets.practice_effects.skip_reward();}
 u32 practice_music_start_offset()noexcept;
 bool suspend_for_transition();
 void retire_projectile_effect_animations()noexcept{animations.retire_resource(assets.effect);animations.retire_resource(assets.bullet);}
 void apply_input(const GameInput&)noexcept;
 FrameScheduler& frame_scheduler()noexcept{return scheduler;}
};
}
