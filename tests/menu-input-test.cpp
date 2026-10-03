#include "../th10_web/cpp/game/GameInput.hpp"
#include "../th10_web/cpp/game/TitleMain.hpp"
#include "../th10_web/cpp/game/MusicRoom.hpp"
#include "../th10_web/cpp/game/Results.hpp"
#include "../th10_web/cpp/game/Ending.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

using namespace th10;
namespace {
static_assert(std::is_same_v<decltype(TitleMainEnvironment::pressed),const u16*>);
static_assert(std::is_same_v<decltype(MusicRoomEnvironment::pressed),const u16*>);
static_assert(std::is_same_v<decltype(ResultsEnvironment::pressed),const u16*>);
static_assert(std::is_same_v<decltype(EndingEnvironment::pressed),const u16*>);
static_assert(std::is_same_v<decltype(EndingEnvironment::held),const u32*>);
static_assert(std::is_same_v<decltype(EndingEnvironment::engine_flags),const u32*>);
static_assert(offsetof(GameInput,raw_pressed)==6 && sizeof(GameInput)==0x58);
static_assert(sizeof(TitleMainEnvironment)==32 && sizeof(MusicRoomEnvironment)==48);
static_assert(sizeof(ResultsEnvironment)==88 && sizeof(EndingEnvironment)==92);

alignas(4) GameInput input{};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
multiplayer::TeamEconomy team{};
multiplayer::PilotEconomy pilot{};
GameEconomy economy{team,pilot};
#else
GameEconomy economy{};
#endif
AnmRegistry registry{};
float rate=1;
u32 calls=0;
template<class E> void pressed_layout(E& env,std::size_t expected){
    const auto base=reinterpret_cast<std::uintptr_t>(static_cast<E*>(&env));
    assert(reinterpret_cast<std::uintptr_t>(&env.pressed)-base==expected);
}
[[noreturn]] void unexpected(){std::fputs("Unexpected resource operation in menu input fixture\n",stderr);std::abort();}
template<class F> auto unchanged(F action){const auto before=input;auto result=action();assert(std::memcmp(&before,&input,sizeof(input))==0);++calls;return result;}

struct TitleEnvironment final:TitleMainEnvironment {
    u8 unlocked[6]{};std::vector<i32> sounds;
    TitleEnvironment(){registry=&::registry;rate=&::rate;game=&economy;extra_unlocked=unlocked;pressed=&input.raw_pressed;repeated=&input.raw_repeat;pressed_layout<TitleMainEnvironment>(*this,24);}
    u32 create(AnmFile&,i32)override{unexpected();}
    void sound(i32 id)override{sounds.push_back(id);}
    void interrupt_immediately(u32,i32)override{}
};
struct MusicEnvironment final:MusicRoomEnvironment {
    u8 tracks[32]{};u32 display=0;std::vector<i32> sounds,commands;u32 loads=0;
    MusicEnvironment(){registry=&::registry;rate=&::rate;unlocked=tracks;display_flags=&display;pressed=&input.raw_pressed;repeated=&input.raw_repeat;tracks[0]=1;pressed_layout<MusicRoomEnvironment>(*this,32);}
    u32 create(AnmFile&,i32)override{unexpected();}
    char* read_file(i32&)override{unexpected();}
    void free_file(char*)override{unexpected();}
    void text(AnmVm&,u32,const char*)override{unexpected();}
    void numbered_text(AnmVm&,u32,i32,const char*)override{unexpected();}
    void locked_text(AnmVm&,u32,i32)override{unexpected();}
    void sound(i32 id)override{sounds.push_back(id);}
    void music_command(i32 id)override{commands.push_back(id);}
    void load_music(const char* path)override{assert(std::strcmp(path,"fixture.wav")==0);++loads;}
    void play_music()override{unexpected();}
};
struct ResultsHost final:ResultsEnvironment {
    i32 mode=0,pending=0;u32 flags=0;std::vector<i32> sounds,interrupts;
    ResultsHost(){game=&economy;pressed=&input.raw_pressed;repeated=&input.raw_repeat;rate=&::rate;replay_mode=&mode;pending_screen=&pending;controller_flags=&flags;pressed_layout<ResultsEnvironment>(*this,56);}
    void sound(i32 id)override{sounds.push_back(id);}
    void music_command(i32,const char*)override{unexpected();}
    void result_music()override{unexpected();}
    u32 create_animation(AnmFile&,i32)override{unexpected();}
    void capture_background(u32)override{unexpected();}
    void interrupt(u32,i32 label)override{interrupts.push_back(label);}
    u32 child(u32&,i32 script)override{return u32(script);}
    void visible(u32,bool)override{unexpected();}
    void delete_animation(u32)override{unexpected();}
    void select_screen(i32)override{unexpected();}
    Replay* preview(const char*)override{unexpected();}
    void delete_replay(Replay*)override{unexpected();}
    void save_replay(const char*,const char*)override{unexpected();}
    void timestamp(i32&)override{unexpected();}
};
struct EndingHost final:EndingEnvironment {
    Ending value{};Ending* value_pointer=&value;u32 held_word=0,engine_word=0;i32 menu=7,pending=0;u32 sounds=0;
    EndingHost(){current=&value_pointer;game=&economy;registry=&::registry;rate=&::rate;pressed=&input.raw_pressed;held=&held_word;engine_flags=&engine_word;menu_state=&menu;pending_screen=&pending;pressed_layout<EndingEnvironment>(*this,76);}
    u32 begin_thread(CallbackToken,void*,u32,u32&)override{unexpected();}
    u32 wait_thread(u32,u32)override{unexpected();}
    void close_thread(u32)override{unexpected();}
    void sleep(u32)override{unexpected();}
    void* allocate(u32)override{unexpected();}
    void delete_object(void*)override{unexpected();}
    void free_bytes(void*)override{unexpected();}
    u8* read_file(const char*)override{unexpected();}
    void report_error()override{unexpected();}
    void show_loading()override{unexpected();}
    u32 create_animation(AnmFile&,i32)override{unexpected();}
    void draw_text(AnmVm*,u32,const char*)override{unexpected();}
    void sound(i32 id)override{assert(id==0);++sounds;}
    void fade(i32,i32)override{unexpected();}
    void load_music(const char*)override{unexpected();}
    void play_music(i32)override{unexpected();}
    void fade_music(float)override{unexpected();}
    AnmFile* load_animations(i32,const char*)override{unexpected();}
    void unload_animations(i32)override{unexpected();}
    void release_animations(AnmFile&)override{unexpected();}
    void release_capture()override{unexpected();}
};

TitleMenu main_menu(){TitleMenu m{};m.phase=2;m.menu.item_count=8;m.menu.wrap=1;m.menu.selected=3;return m;}
i32 title_update(TitleMenu& menu,TitleEnvironment& env){return unchanged([&]{return menu.update_main(env);});}
void title_inputs(){
    TitleEnvironment env;TitleMenu prompt{};prompt.phase=2;
    input={};input.raw_released=0xffff;input.raw_repeat=0x800;
    assert(unchanged([&]{return prompt.update_prompt(env);})==1);assert(prompt.phase==2&&env.sounds.empty());
    input.raw_pressed=0x160b;assert(unchanged([&]{return prompt.update_prompt(env);})==1);
    assert(prompt.phase==4&&env.sounds==std::vector<i32>{32});
    for(u16 key:{u16(1),u16(0x1000),u16(2),u16(8)}){
        auto menu=main_menu();env.sounds.clear();input={};input.update_raw(key);title_update(menu,env);
        if(key&0x1001)assert(menu.phase==4&&env.sounds==std::vector<i32>{10});
        else assert(menu.phase==2&&menu.menu.selected==7&&env.sounds==std::vector<i32>{11});
        input.update_raw(key);const auto sounds=env.sounds.size();title_update(menu,env);assert(env.sounds.size()==sounds);
        input.update_raw(0);title_update(menu,env);assert(env.sounds.size()==sounds);
    }
    auto menu=main_menu();input={};input.update_raw(0x20);title_update(menu,env);assert(menu.menu.selected==4);
    for(unsigned i=0;i<24;++i){input.update_raw(0x20);title_update(menu,env);assert(menu.menu.selected==4);}
    input.update_raw(0x20);assert(input.raw_repeat==0x20);title_update(menu,env);assert(menu.menu.selected==5);
    for(unsigned i=0;i<7;++i){input.update_raw(0x20);title_update(menu,env);assert(menu.menu.selected==5);}
    input.update_raw(0x20);title_update(menu,env);assert(menu.menu.selected==6);
    input.update_raw(0);title_update(menu,env);assert(menu.menu.selected==6);
    input.update_raw(0x20);title_update(menu,env);assert(menu.menu.selected==7);
}
void music_inputs(){
    MusicEnvironment env;TitleMenu menu{};menu.phase=2;menu.music_filled_comments=8;menu.menu.item_count=1;std::strcpy(menu.music_files[0],"fixture.wav");
    input={};input.raw_released=0xffff;input.raw_repeat=0x800;
    unchanged([&]{return MusicRoom{menu,env}.update();});assert(menu.phase==2&&env.loads==0&&env.commands.empty()&&env.sounds.empty());
    input={};input.update_raw(1);unchanged([&]{return MusicRoom{menu,env}.update();});assert(env.loads==1&&env.commands==std::vector<i32>{2});
    // Keep comment filling out of this key-consumer fixture.
    menu.music_filled_comments=8;input.update_raw(1);unchanged([&]{return MusicRoom{menu,env}.update();});assert(env.loads==1);
    input.update_raw(0);unchanged([&]{return MusicRoom{menu,env}.update();});assert(env.loads==1);
    input.update_raw(2);unchanged([&]{return MusicRoom{menu,env}.update();});assert(menu.phase==3&&env.sounds==std::vector<i32>{11});
}
Results paused_menu(){Results r{};r.state=2;r.menu.item_count=3;r.menu.wrap=1;return r;}
void pause_update(Results& r,ResultsHost& env){unchanged([&]{r.update_pause(env);return 0;});}
void results_inputs(){
    ResultsHost env;auto r=paused_menu();input={};input.raw_released=0xffff;input.raw_repeat=0x800;
    pause_update(r,env);assert(r.state==2&&r.menu.selected==0&&env.sounds.empty()&&env.interrupts.empty());
    for(u16 key:{u16(1),u16(0x1000),u16(8),u16(0x200),u16(0x4000)}){
        r=paused_menu();env.sounds.clear();input={};input.update_raw(key);pause_update(r,env);assert(r.state==3);
        assert(r.menu.selected==(key==0x200?1:key==0x4000?2:0));
        assert(env.sounds.size()==(key==8?0u:1u));
        input.update_raw(key);const auto sounds=env.sounds.size();pause_update(r,env);assert(env.sounds.size()==sounds);
        input.update_raw(0);pause_update(r,env);assert(env.sounds.size()==sounds);
    }
    r=paused_menu();input={};input.update_raw(0x20);pause_update(r,env);assert(r.menu.selected==1&&r.state==2);
    input.update_raw(0x20);pause_update(r,env);assert(r.menu.selected==1);
    input.raw_repeat=0x20;pause_update(r,env);assert(r.menu.selected==2);
    input.update_raw(0);pause_update(r,env);assert(r.menu.selected==2);
    // Simultaneous confirm/retry/quit/cancel preserve their independent order.
    r=paused_menu();input.raw_pressed=0x1001|0x4000|0x200|8;input.raw_repeat=0;pause_update(r,env);assert(r.state==3&&r.menu.selected==0);
}
struct MessageFixture {MessageInstruction page{0,6,4};i32 delay=100;MessageInstruction end{1000,0,0};};
static_assert(offsetof(MessageFixture,delay)==4&&offsetof(MessageFixture,end)==8);
EndingScript wait_script(MessageFixture& message){
    EndingScript script{};script.instruction=&message.page;script.script_time.rate=&rate;script.wait.rate=&rate;script.elapsed.rate=&rate;return script;
}
void ending_inputs(){
    EndingHost env;MessageFixture message;auto script=wait_script(message);
    input={};input.raw_released=0xffff;input.raw_repeat=0x1001;
    assert(unchanged([&]{return script.update(env);})==0);assert(script.instruction==&message.page&&script.wait.current==99&&env.sounds==0&&env.menu==7);
    for(u16 key:{u16(1),u16(0x1000)}){
        script=wait_script(message);env.menu=7;const auto sounds=env.sounds;input={};input.update_raw(key);
        assert(unchanged([&]{return script.update(env);})==0);assert(script.instruction==&message.end&&env.sounds==sounds+1&&env.menu==0);
        // A following page cannot consume a held key or its release as a new edge.
        script=wait_script(message);input.update_raw(key);unchanged([&]{return script.update(env);});assert(script.instruction==&message.page&&env.sounds==sounds+1);
        input.update_raw(0);unchanged([&]{return script.update(env);});assert(script.instruction==&message.page&&env.sounds==sounds+1);
        input.update_raw(key);unchanged([&]{return script.update(env);});assert(script.instruction==&message.end&&env.sounds==sounds+2);
    }
}
} // namespace
int main(){
    assert(reinterpret_cast<std::uintptr_t>(&input.raw_pressed)%alignof(u32)==2);
    title_inputs();music_inputs();results_inputs();ending_inputs();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    constexpr const char* variant="multiplayer";
#else
    constexpr const char* variant="ordinary";
#endif
    std::printf("{\"suite\":\"menu-input\",\"variant\":\"%s\",\"consumer_calls\":%u,\"pressed_alignment_mod4\":2,\"input_unchanged\":true,\"passed\":true}\n",variant,calls);
}
