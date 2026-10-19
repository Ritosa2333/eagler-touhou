// TH08 ending scripts and layout; MIT reference: GensokyoClub/th08.
#pragma once
#include "GameplaySession.hpp"
#include "AnmLibrary.hpp"
#include "AnmRenderer.hpp"
#include "AnmText.hpp"
#include "Chain.hpp"
namespace th08 {
struct EndingState {
    ChainElement *calculation=nullptr,*drawing=nullptr;Vec2 background;float scroll=0;AnmVm vms[16];
    const u8* file=nullptr;i32 seen=0,seen_staff=0;AnmLoaded* animation=nullptr;
    Timer elapsed,wait,reset_wait;i32 minimum_reset=0,minimum_wait=0,line_delay=8,fast_delay=0,unused=0,lines=0;
    u32 text_color=0,fade_color=0;i32 fade_timer=0,fade_duration=0,fade_mode=0;const u8* cursor=nullptr;
    EndingState(){std::memset(this,0,sizeof(*this));line_delay=8;elapsed.set(0);wait.set(0);}
};
static_assert(sizeof(void*)!=4||sizeof(EndingState)==0x2ab8);
struct EndingActions {
    virtual ~EndingActions()=default;
    virtual std::vector<u8> read_ending(const char* name)=0;
    virtual bool load_background(const char* name)=0;
    virtual void draw_background(i32 x,i32 y)=0;
    virtual void present()=0;
    virtual void release_background()=0;
    virtual void music(const char* name)=0;
    virtual void fade_music(float seconds)=0;
    virtual void finished()=0;
};
class Ending {
    AnmLibrary& library;AnmExecutor& animations;AnmRenderer& renderer;TextWriter& text;EndingActions& actions;
    std::vector<u8> data;bool failed=false;u16 input=0,previous=0;Chain* owner=nullptr;ChainElement calculation,drawing;
    bool start_vm(AnmVm&,AnmLoaded*,i32 script);bool in_file()const;bool newline();i32 parameter();const char* string();bool parse();
public:
    EndingState state;
    Ending(AnmLibrary& l,AnmExecutor& a,AnmRenderer& r,TextWriter& t,EndingActions& p):library(l),animations(a),renderer(r),text(t),actions(p){}
    ~Ending(){detach();}
    void reset(){state=EndingState{};data.clear();failed=false;}
    bool load(const char* name);
    bool setup(GameplaySession&,i32 character,i32 difficulty,i32 stage,u32 flags);
    JobResult update(u16 keys,u16 previous_keys);
    void fade();bool draw();
    bool attach(Chain&,GameplaySession&,i32 character,i32 difficulty,i32 stage,u32 flags);
    void set_input(u16 keys,u16 previous_keys){input=keys;previous=previous_keys;}
    void detach();void release();
    bool invalid()const{return failed||animations.invalid;}
    u32 file_size()const{return data.size();}
};
}
