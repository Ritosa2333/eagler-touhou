#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_core/worker.hpp"
#include "../stone_menu/cursor.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::title {
// TitleInf, original vtable574fbc. Unknown data retains its actual storage;
// it is not a substitute implementation for the page that owns it.
struct TitleInf final:runtime::CallbackOwner {
    sprite::AnimationFile* files[2];
    int state,previous_state,phase;
    menu::Cursor cursor,cursor70,cursorbc,cursor108;
    recovered::Timer age;
    std::uint32_t handles[139],handle390,handles394[16],handles3d4[32],handles454[8],handle474;
    std::atomic<bool> flag478;
    std::uint8_t padding479[3];
    std::uint32_t words47c[5];
    std::uint8_t data490[0x800],datac90[0x840],data14d0[0x4200];
    std::uint32_t word56d0,word56d4,word56d8;
    std::uint16_t value56dc;
    std::uint8_t padding56de[2];
    std::uint32_t word56e0,word56e4;
    menu::Cursor cursor56e8;
    std::uint32_t words5734[3];
    runtime::CallbackOwner* metadata[100];
    int music_delay;
    std::uint32_t ui_flags,words58d8[11];
    recovered::Timer selection_age,flash_age;
    sprite::RenderMesh* mesh;
    float angle,wave;
    std::uint32_t color[4],color5940[4],color5950[4];
    int previous5960,previous5964;
    runtime::Worker worker;
    TitleInf(); //51db00
    ~TitleInf() override; //51dea0
};
static_assert(offsetof(TitleInf,cursor)==0x24&&offsetof(TitleInf,age)==0x154&&offsetof(TitleInf,handle390)==0x390);
static_assert(offsetof(TitleInf,data490)==0x490&&offsetof(TitleInf,cursor56e8)==0x56e8&&offsetof(TitleInf,metadata)==0x5740&&offsetof(TitleInf,selection_age)==0x5904&&offsetof(TitleInf,worker)==0x5968);
// Total size depends on the STL layout (runtime::Worker holds a jthread);
// binding on the MSVC x86 build only.
#ifndef TH_SDL3
static_assert(sizeof(TitleInf)==0x5978);
#endif
TitleInf* controller();
TitleInf* create(); //52cfe0->51d900
int initialize(TitleInf&); //51f3c0
int load_worker(); //51f7e0
void set_state(TitleInf&,int); //52ced0
void set_phase(TitleInf&,int); //52ce50
void spawn(TitleInf&,int); //52cf80
void activate(TitleInf&,int); //52cbf0
void deactivate(TitleInf&,int); //52cc90
void retire_animation(TitleInf&,int); //52cc30
bool animation_exists(TitleInf&,int); //52c750
void set_mode(game_session::Session&,int); //50a4a0
void set_character(game_session::Session&,int); //50a3e0
extern int last_difficulty; //5b0a60, initialized1
extern int last_character; //5c6140, BSS
struct MainEnvironment {
    game_session::Session& session;
    int& last_difficulty;int& last_character;
    MainEnvironment(game_session::Session& s,int& d,int& c):session(s),last_difficulty(d),last_character(c){}
    virtual ~MainEnvironment()=default;
    virtual bool pressed(std::uint32_t)=0;
    virtual bool repeated(std::uint32_t)=0;
    virtual bool extra_unlocked()=0;
    virtual bool notice_present()=0;
    virtual bool notice_finished()=0;
    virtual bool notice_pending()=0;
    virtual void create_notice()=0;
    virtual void retire_notice()=0;
    virtual void sound(int)=0;
    virtual void spawn(TitleInf&,int)=0;
    virtual void interrupt(TitleInf&,int,int,bool)=0;
    virtual bool exists(std::uint32_t)=0;
    virtual bool decoration_exists(TitleInf&)=0;
    virtual void interrupt_handle(std::uint32_t,int)=0;
    virtual void spawn_decoration(TitleInf&)=0;
};
MainEnvironment& main_environment();
int update_main_menu(TitleInf&,MainEnvironment&); //529e80
void draw_main_menu(TitleInf&,text::Renderer&); //52a700
void update_options(TitleInf&); //520fb0
int update_help(TitleInf&); //5204c0
int update(TitleInf&); //51e3c0
int draw(TitleInf&); //51f220
void initialize_background(TitleInf&); //52c030
void update_background(TitleInf&); //51ed90
}
