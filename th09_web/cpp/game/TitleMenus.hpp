#pragma once
#include "GameResources.hpp"
#include "InputFrame.hpp"
#include "GameWorld.hpp"
#include "PlayerRecords.hpp"
#include "ReplayFile.hpp"
namespace th09 {
enum class TitleScreen:i32 {main=1,story_difficulty,story_character,extra_difficulty,extra_character,versus_type,versus_difficulty,versus_character,options,key_config,replays,music,rankings,score_name,replay_name,versus_stage};
struct InputBindings {std::array<i16,9> gamepad{};std::array<std::array<i16,9>,2> keyboard{};};
static_assert(sizeof(InputBindings)==54);
struct TitleSettings {
    u8 extra_lives=0,difficulty=1,frameskip=0,windowed=1,music_mode=1,effects=1,screen_mode=0,music_volume=100,sound_volume=100;
    i32 versus=1,characters[2]{0,1},health[2]{10,10};bool alternate[2]{};
    std::array<u8,16> versus_unlocked{1,1,1,1,1},story_unlocked{1,1,1,1,1},extra_unlocked{};
    std::array<u8,32> music_unlocked{1};
    u32 game_flags=0;u8 demo_index=0;
    std::array<InputBindings,2> bindings{};std::array<u8,2> devices{0,1},auto_focus{};
    i32 secret_progress=0,previous_button=32;
};
struct TitleState {
    i32 selection=0,name_cursor=0,ranking_character=0,ranking_slot=0,key_player=0,character_selection[2]{},confirmed[2]{},previous_selection=0,state=0,frames=0;
    TitleScreen previous_screen=TitleScreen(0),screen=TitleScreen::main;
    i32 animation_frames=0,layout_changed=0,load_frame=0,idle_frames=0,return_to_versus=0,selection_base=0,selection_count=0;
    i32 description=-1,health[2]{},health_adjustment=0;
    i32 replay_scroll=0,replay_count=0,replay_selected=0;
    i32 current_music=0,music_count=0,music_scroll=0,music_paused=0;
};
struct TitleServices {
    virtual ~TitleServices()=default;
    virtual bool title_background(const char*)=0;
    virtual void title_sound(i32)=0;
    virtual void title_music(i32)=0;
    virtual void title_music_file(const char*)=0;
    virtual void title_music_pause(bool)=0;
    virtual void title_music_fade()=0;
    virtual void title_text(AnmVm&,const char*,u32,u32)=0;
    virtual void title_sprite(AnmVm&,bool rotated)=0;
    virtual void title_begin_draw()=0;
    virtual void title_configuration()=0;
    virtual PlayerRecords& title_records()=0;
    virtual void title_save_records()=0;
    virtual void title_ascii(const Vec3&,const char*,u32,const Vec2&)=0;
    virtual i32 title_joy_button(i32)=0;
    virtual bool title_read_replay(const char*,std::vector<u8>&)=0;
    virtual std::vector<std::string> title_imported_replays()=0;
    virtual bool title_save_replay(const char* path,const char* name)=0;
    virtual bool title_replay_exists(const char*)=0;
    virtual void title_play_replay(const ReplayFile&,u32 round,const char* path)=0;
    virtual u32 title_clear_count(i32 character,i32 difficulty)=0;
    virtual void title_launch(const WorldConfiguration&)=0;
    virtual void title_demo(u32)=0;
    virtual void title_exit()=0;
    virtual void title_network(){}
};
class TitleMenus {
    GameResources& resources;TitleServices& output;Rng& random;
    bool main();bool difficulty();bool versus_type();bool character();bool versus_character();bool versus_stage();bool options();bool music();bool key_config();bool rankings();bool score_name();bool replays();bool replay_name();
    void tick(){++state.frames;++state.animation_frames;++state.load_frame;}
    void interrupt(i32);void advance();
    i32 navigate(i32& cursor,i32 count,const InputFrame&,bool horizontal=false);
    void select(i32,i32,i32);void select_character(i32,i32,i32,bool reverse=false);
    void skip_locked(i32,const std::array<u8,16>&,i32,i32);
    void portraits(i32,i32,i32,const std::array<u8,16>&);
    void portrait_color(i32,i32,u8);void life_icons();void character_list(const std::array<u8,16>&,i32);
    void launch(bool versus,i32 stage=-1);
    void music_list();void ranking_portrait(i32);void draw_rankings();void draw_name_grid(float,float);void move_name_cursor();
    void key_value(i32,i32);void key_numbers();void key_enabled();
    bool create_animations();
    void load_replays(bool imported);void draw_replays();void draw_replay_name();void replay_row(i32,const Vec3&,u32,bool saving);
    void replay_save_slots();bool save_replay(const char*);
public:
    TitleState state;TitleSettings& settings;InputFrame input[3];
    std::vector<AnmVm> animations;std::array<AnmVm,14> descriptions{};std::array<AnmVm,8> music_comments{};
    struct MusicEntry {std::string file,title;std::array<std::string,8> comments;};std::vector<MusicEntry> music_entries;
    struct ReplayEntry {std::string path,name="--------",date="--/--/--";ReplayFile file;u8 mode=0,difficulty=0,versus=0;bool valid=false;};
    std::array<ReplayEntry,50> replay_entries;std::array<char,9> replay_player_name{};bool replay_forbidden=false,automatic_replay_save=false;
    std::array<InputBindings,2> edited_bindings{};ScoreEntry score_candidate;GameMode result_mode=GameMode(0);
    std::array<i8,16> stages{};bool ready=false,leaving=false;std::string error;
    TitleMenus(GameResources& r,TitleServices& s,Rng& rng,TitleSettings& cfg):resources(r),output(s),random(rng),settings(cfg){}
    bool initialize();
    // The hosted lobby has already confirmed both characters and difficulty.
    // Use the same versus launch path as the title's stage confirmation.
    void launch_network_match(){state.health[0]=settings.health[0];state.health[1]=settings.health[1];launch(true,-1);}
    void change(TitleScreen next){state.previous_screen=state.screen;state.screen=next;state.frames=state.load_frame=state.state=state.animation_frames=0;}
    bool update(const InputFrame& left,const InputFrame& right,const InputFrame& menu);
    void draw();
    bool update_screen();
};
}
