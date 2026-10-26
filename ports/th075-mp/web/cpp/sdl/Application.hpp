#pragma once
#include "FileResources.hpp"
#include "TextureBank.hpp"
#include "FontDevice.hpp"
#include "AudioDevice.hpp"
#include "../game/BattleLoading.hpp"
#include "../game/BattleRenderer.hpp"
#include "../game/BattleStory.hpp"
#include "../game/OptionsScene.hpp"
#include "../game/MusicRoomScene.hpp"
#include "../game/CinemaScene.hpp"
#include "../game/NameEntryScene.hpp"
#include "../game/ReplayBrowserScene.hpp"
#include "../game/ReplayConfiguration.hpp"
#include "../game/ResultListScene.hpp"
#include "../game/ResultScene.hpp"
#include "../game/SceneFade.hpp"
namespace th075::sdl {
struct RollbackState;
class Application final:public BattleScenePlatform,public BattleLoadSignals,public BattleStoryPlatform,public BattleTextDrawing,public CharacterSelectSignals,public OptionsSignals,public MusicRoomSignals,public CinemaSignals,public NameEntrySignals,public ReplayBrowserSignals,public ResultSignals,public ResultListSignals,public SceneFadeSignals {
public:
 GraphicsDevice graphics;ZunGraphics zun{graphics};FileResources resources;FontDevice fonts{graphics};AudioDevice audio;
private:
 friend struct RollbackState;
 struct ResetEvents final:FighterCoreEvents {Application& app;u8 side;ResetEvents(Application& a,u8 s):app(a),side(s){}void clear_owned_objects(FighterActor&)override;void spell_attempt(u8)override;void reset_hud_alpha()override;};
 std::array<ResetEvents,2> resetEvents{ResetEvents(*this,0),ResetEvents(*this,1)};
 TextureBank sceneImages,selectionCharacters,selectionBackgrounds,stageImages,hudImages,effectImages,windowImages;
 std::map<std::pair<ResourceEdition,std::string>,std::shared_ptr<TextureBank>> menuImages;
 bool warm_menu_images();
 std::array<std::unique_ptr<TextureBank>,2> characterImages,portraitImages,captionImages;
 std::array<CharacterAssets,2> characterAssets;EffectPattern effects;
 TextSurface textSurface;u32 dialogueTexture=0,captureTexture=0,menuText=0;
 std::vector<u8> readBuffer;std::vector<u32> transientTextures;MusicRoomArtwork musicArtwork;CardList cards;
 TitleScene title;CharacterSelectScene select;OptionsScene options;MusicRoomScene music;CinemaScene cinema;ResultScene result;ResultListScene resultList;NameEntryScene nameEntry;ReplayBrowserScene replays;SceneFade fade;
 ReplayFile recording,playback;ReplayStreams loadingReplay;StoryDialogueCache dialogueCache;BattleSession pendingSession;RoundRules pendingRules;
 std::unique_ptr<BattleArtwork> battleArt;std::unique_ptr<BattleRenderer> battleRenderer;
 u16 sceneCode=0x2004;u8 scene=4;u32 logoFrames=0,sceneStart=0;bool loaderReady=false,recordingFinished=false;std::string saveReplayPath;u8 saveReplayOpponent=0;u32 playbackCursor=0;
 bool load_images(TextureBank&,ResourceEdition,const std::string&,u8 palette=1);
 bool enter(u8);bool load_battle();void draw_scene();void change(u16);bool prepare_text(u32&,const std::string&,TextRasterOptions);void clear_transient();void release_battle(bool characters);
 void scan_replays();void choose_replay_name();void update_music_catalog();
 MenuInput menu_input(const BattleSystemInput&)const;
public:

 GameSettings settings;GameProfile profile;GameSelection selection;SelectionSettings playerSettings;SelectionProgress progress;BattleLoading loading;
 std::unique_ptr<BattleScene> battle;std::string error;u32 frame=0,now=0;bool initialized=false,exiting=false;
 Application();~Application();bool initialize(u32 scale);bool tick(const BattleSystemInput&,u32 milliseconds);bool save_scores();bool save_replay(u32 slot,const char*);bool return_title();
 u8 current_scene()const{return scene;}
 bool room_preview=false;
 bool preview_room(bool on){if(on&&(!initialized||!return_title()))return false;room_preview=on;return true;}
 void preview_title_tick(BattleSystemInput& input){
  title.startedAt=now;title.elapsed=0;
  if(scene==2){
   input.keys[1]=0;
   for(auto& player:input.players){player.buttons[1]=0;if(title.selected!=8)player.buttons[0]=0;}
  }
 }
 bool restart();bool probe_scene(u8);bool probe_battle(u8,u8,u8,u8);
 bool prepare_multiplayer(u32,u8,u8,u8,u8,u8);bool save_multiplayer_replay();bool show_replays();
 struct RollbackSound {i16 id;u8 side;bool character;bool operator==(const RollbackSound& other)const{return id==other.id&&side==other.side&&character==other.character;}};
 bool rollbackResimulating=false;bool collectRollbackSound=false;std::vector<RollbackSound> rollbackSounds;
 std::unique_ptr<RollbackState> capture_rollback()const;bool restore_rollback(const RollbackState&);
 bool rollback_predictable()const;
 bool multiplayer_complete()const{return (sceneCode&0xff)!=0&&(sceneCode&0xff)!=1;}
 u8 phase()const{return exiting?4:battle&&scene==0?(battle->progress.paused?2:1):0;}
 void sound(i16)override;void sound(u16 v)override{sound(i16(v));}void character_sound(u8,i16)override;void fade_music(i32,i32,u32,u32)override;void select_music(i16)override;void start_music()override;void stop_music()override;void stop_music(i32)override;
 bool load_character(u8,const CharacterDefinition&,u8,CharacterController&)override;void load_story_artwork(CharacterController&,u8)override;FighterCoreEvents& reset_events(u8)override;void save_profile(const std::vector<u8>&)override;void prepare_round_recording(const GameSelection&,const BattleSession&,const RoundRules&,const FighterActor&)override;
 void combo(u8,i16,i16,i32)override;void spell_attempt(u8,u8)override;void advance_stage()override;void toggle_fullscreen()override;
 void render_battle(BattleScene&,bool)override;void present_battle()override;void begin_round_recording()override;void banner(bool)override;void finish_replay_round()override;void dialogue_origin(float,float,u32)override;void prepare_victory(RoundFighterState&)override;void show_victory_line(const std::string&,u8)override;void end_story_dialogue()override;void render_before_story_load()override;void release_battle_loader()override;void stage_story_completed()override;void reset_fighter(u32,RoundFighterState&)override;void reset_stage_effects()override;void advance_overlay()override;
 void text(const std::string&,u8,u8,u8,u32,u8,bool)override;void origin(float,float,bool)override;void reload_stage(u8)override;void render()override;
 void render_selection()override;void present_selection()override;void prepare_battle()override;
 void render_options()override;void present_options()override;void description(u8)override;void music_volume(u8)override;void sound_volume(u8)override;void start_music(i32)override;void save_settings()override;
 void render_music_room()override;void present_music_room()override;void update_title(u8)override;void update_details(u8)override;
 void render_cinema()override;void present_cinema()override;
 void render_name_entry()override;void present_name_entry()override;void save_score_name(u8)override;void save_replay_name()override;
 void render_replays()override;void present_replays()override;void start_replay(u32,u8)override;
 void render_result()override;void present_result()override;
 void render_result_list()override;void present_result_list()override;void load_result_character(u8)override;void update_result_text(bool)override;
 void render_scene()override;void present_scene()override;
};
}
