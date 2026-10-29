// Port of upstream src/title-scenes.mjs (TH12 1.00b TitleInf 43f760/43fde0):
// menu input, clocks and scene requests. Gameplay, loading, audio and saves
// belong to Game, which drains the actions this emits.
#pragma once
#include "Profile.hpp"
#include "Replay.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace th12host {
enum class MenuScene {Title,Difficulty,Character,Shot,Practice,Replay,ReplayStage,PlayerData,Options,KeyConfig,MusicRoom,Quit,Closed,
    Playing,Demo,Pause,PauseConfirm,Continue,ResultMenu,Name,ReplaySave,Ending};
enum RunMode:i32 {ModeStory=0,ModePractice=1,ModeExtra=2,ModeReplay=3,ModeDemo=4};
struct MenuRun {i32 mode=ModeStory,difficulty=1,character=0,shot=0,stage=1;std::string replay_file;};
struct MenuChoice {std::u32string label;i32 id=0;bool disabled=false;};
struct MenuEvent {enum Type {Ready,Selection,Confirm,Cancel,Resume,RetryShortcut,ConfirmNo,Fade,DataHeader} type=Selection;i32 previous=0,selection=0,reset_interrupt=3,selection_interrupt=7,frames=0;};
// A run as the result pages see it (upstream resultData).
struct MenuContext {
    MenuRun run;bool can_continue=true,can_save_replay=false,clear=false,ranked=true;
    i32 continues=0,stage=1,score_stored=0;i64 timestamp=0;float slowdown=0;
    i64 score()const{return i64(score_stored)*10+continues;}
};
struct MusicTrackInfo {std::string id;std::u32string label,title;std::vector<std::u32string> lines;};
struct MenuAction {
    enum Type {Start,Retry,Continue,ReturnTitle,Quit,MenuMusic,FadeMusic,Sound,Pause,Resume,StopEffects,Settings,LoadingIndicator,MusicWarning,
        PlayReplay,Demo,Result,SaveName,SaveReplay,SaveExit} type=Sound;
    MenuRun run;std::string track,name;bool restart=false,music_room=false,needs_name=false;i32 id=0,slot=0,stage=0;double seconds=0;MenuContext result;
};
enum class NameKind {Score,Replay};
enum class NameContext {Title,Pause};
enum class AfterName {Continue,ResultMenu,ReplaySave};
struct MenuState {
    MenuScene scene=MenuScene::Title,from=MenuScene::Title;i32 selection=0,age=0,ready_at=7,serial=0,page=0;
    enum class Phase {In,Active,Out} phase=Phase::In;i32 transition_age=0;std::string transition_action;
    std::vector<MenuEvent> events;std::vector<MenuChoice> choices;bool intro=false;
    MenuContext context;i32 pause_selection=0;std::string pause_action;bool failed=false;
    i32 music_top=0,comment_serial=0;std::string comment_track,playing_track;bool music_warning=false;
    std::array<i32,5> joy_draft{};i32 capture_joy=-1;bool capture_ready=false;
    // Replay / ReplayStage
    i32 file_index=0;std::shared_ptr<const Replay> replay;
    // Player Data
    i32 data_difficulty=1,data_page=0;
    // Name entry (Score Ranking or a replay's name) and ReplaySave
    MenuContext result;std::string name;NameKind name_kind=NameKind::Score;NameContext name_context=NameContext::Title;AfterName after_name=AfterName::ReplaySave;
    i32 rank=-1,reveal_at=0,slot=1;bool ranked=true;std::array<Profile::RankingRow,10> ranking{};
};
// The name grid (4a0e88): 88 characters, then space, delete and end.
extern const char name_characters[89];
// Pages of Player Data at a difficulty: scores, then ten cards a page.
i32 spell_page_count(i32 difficulty);
std::vector<i32> spell_page_cards(i32 difficulty,i32 page);

class TitleScenes {
public:
    struct Settings {i32 bgm_volume=100,se_volume=80;std::array<i32,5> joy_buttons{0,1,2,4,3};};
    TitleScenes(Profile& profile,std::vector<MusicTrackInfo> tracks,std::string replay_folder);
    MenuState state;MenuRun run;Settings settings;
    std::vector<MenuAction> drain(){auto out=std::move(actions);actions.clear();return out;}
    bool has_actions()const{return !actions.empty();}
    // Actions that replace or reinitialise the session (upstream tick()).
    bool blocks_session()const{
        for(const auto& a:actions)if(a.type==MenuAction::Start||a.type==MenuAction::Retry||a.type==MenuAction::Continue||a.type==MenuAction::Resume||a.type==MenuAction::ReturnTitle||
            a.type==MenuAction::PlayReplay||a.type==MenuAction::Demo||a.type==MenuAction::SaveExit)return true;
        return false;}
    // One 60 Hz UI tick. Bits as the core's input word plus 0x80000 Enter and
    // 0x200000 the Retry shortcut.
    void tick(u32 held,u32 pulses=0);
    bool capture_button(i32 button);
    // Upstream clearKeys; buttons still held count as held, not as new presses.
    void clear_keys(u32 held=0){previous_held=held;hold_ages.fill(0);}
    void open_title(bool intro=false);
    void open_pause(const MenuContext& context);
    void open_continue(const MenuContext& context);
    void open_replay_save(const MenuContext& result,const std::string& name,NameContext context);
    void finish(const MenuContext& result);
    void enter(MenuScene scene,i32 selection=0,std::function<void(MenuState&)> values={});
    bool ready()const{return !pending&&state.age>=state.ready_at;}
    void return_title();
    const std::vector<MusicTrackInfo>& music()const{return tracks;}
    const std::vector<ReplayEntry>& replays()const{return replay_files;}
    void rescan_replays();
    const std::string& replay_folder()const{return folder;}
private:
    Profile& profile;std::vector<MusicTrackInfo> tracks;std::vector<MenuAction> actions;std::string folder;std::vector<ReplayEntry> replay_files;
    struct Pending {i32 frames;std::function<void()> callback;};std::optional<Pending> pending;
    u32 previous_held=0;std::array<i32,32> hold_ages{};i32 idle_frames=0,demo_index=0;
    struct {i32 title=0,difficulty=1,character=0,shot=0,practice=0,music=0,replay=0,replay_save=0;} remember;
    void emit(MenuAction a){actions.push_back(std::move(a));}
    void sound(i32 id){MenuAction a;a.type=MenuAction::Sound;a.id=id;emit(a);}
    void menu_music(const std::string& track,bool restart=false,bool music_room=false){MenuAction a;a.type=MenuAction::MenuMusic;a.track=track;a.restart=restart;a.music_room=music_room;emit(a);}
    void simple(MenuAction::Type t){MenuAction a;a.type=t;emit(a);}
    void result_action(const MenuContext& result,bool needs_name){MenuAction a;a.type=MenuAction::Result;a.result=result;a.needs_name=needs_name;emit(a);}
    void refresh_choices();
    void wait(i32 frames,std::function<void()> callback,const std::string& action="confirm");
    void animate(MenuEvent::Type type);
    bool select(i32 index,bool initial=false);
    void move(i32 delta);
    void horizontal(i32 delta);
    void scroll_music();
    void settings_changed();
    void cancel();
    void confirm();
    void resume();
    bool quick_retry();
    void return_from_pause_confirm();
    void pause_action(const std::string& action,const MenuContext& context);
    void open_result_menu(const MenuContext& result,bool from_name=false,NameContext context=NameContext::Pause);
    void complete_name();
    void score_ranking(MenuState& s,const MenuContext& result,bool eligible)const;
    void start();
};
}
