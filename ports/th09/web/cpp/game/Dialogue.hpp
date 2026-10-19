#pragma once
#include "MessageResource.hpp"
#include "GameInput.hpp"
#include "CharacterAttacks.hpp"
#include "MatchRules.hpp"
namespace th09 {
namespace multiplayer { class WorldState; }
enum class DialogueResource {ascii,left_portrait,right_portrait,additional_portraits};
enum class DialogueTransition {next_stage,game_over,match_complete};
struct DialogueCharacter {i32 attack_script=0,face_script=0,face_sprite=0;};
struct DialogueServices {
    FrameTiming timing;GameMode mode=GameMode::story;i32 stage_music=0;DialogueCharacter characters[2];
    virtual ~DialogueServices()=default;
    virtual void start_animation(AnmVm&,DialogueResource,i32 script)=0;
    virtual void set_sprite(AnmVm&,DialogueResource,i32 sprite)=0;
    virtual void advance_animation(AnmVm&)=0;
    virtual void draw_animation(AnmVm&,bool right_portrait)=0;
    virtual void text(AnmVm&,u32 color,u32 shadow,const std::string& shift_jis)=0;
    virtual void music(i32 track)=0;
    virtual void fade_music()=0;
    virtual void show_results()=0;
    virtual void transition(DialogueTransition)=0;
    virtual void white_transition()=0;
    virtual void enter_player(i32 side)=0;
    virtual void show_huds()=0;
    virtual void background_setting(i32 side,i32 value)=0;
    virtual void draw_panel(const AttackColorVertex*,u32 count)=0;
};
class Dialogue {
    friend class multiplayer::WorldState;
    DialogueServices& services;MessageResource* resources[2]{};MessageResource* current=nullptr;
    void setup();
    bool show_portrait(i32 index);
    bool expression(i32 index,i32 face);
    bool line(i32 speaker,i32 line,const std::string&);
    bool text_instruction(const MessageInstruction&,u32 offset,std::string&);
    void advance_animations(const GameInput&);
public:
    u32 cursor=0;i32 id=-1,inverted=0;std::array<AnmVm,11> animations;
    u32 colors[4]{},shadows[4]{};Timer time{0,0,0};i32 wait_frames=0,minimum_wait=0,font_size=0;Timer box_time{0,0,0};
    u8 speaker=0,new_page=0,line_number=0,previous_speaker=0;i32 counter=0;u8 skippable=0,box_visible=0;
    bool invalid=false;
    explicit Dialogue(DialogueServices& s):services(s){}
    void set_resources(MessageResource& primary,MessageResource* secondary=nullptr){resources[0]=&primary;resources[1]=secondary;}
    bool begin(i32 script,i32 flip=0);
    bool begin_victory(i32 winning_side,i32 opposing_character,Rng&);
    i32 update(const GameInput&);
    void draw();
    bool blocks_gameplay()const noexcept{return id>=0||id==-2;}
};
}
