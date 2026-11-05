#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../stone_menu/cursor.hpp"
#include "../input/input.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::key_config {
struct KeyConfigInf final:runtime::CallbackOwner {
    std::uint32_t field_10;
    menu::Cursor cursor;
    std::int16_t bindings[3][16]; //60/80/a0; only first eight are copied by4c5ee0
    recovered::Timer age,transition_age;
    sprite::Vec3 position;
    recovered::Timer selection_age;
    std::int32_t state,phase,selected_devices[2],selected_slot,refresh_devices;
    KeyConfigInf(); //4c5530
    ~KeyConfigInf() override; //4c56b0
};
static_assert(sizeof(KeyConfigInf)==0x114&&offsetof(KeyConfigInf,bindings)==0x60&&offsetof(KeyConfigInf,age)==0xc0&&offsetof(KeyConfigInf,state)==0xfc);
struct Environment {
    input::Controller& input;
    platform::Configuration& configuration;
    const platform::KeyBindings& defaults;
    const float* timer_rate;
    Environment(input::Controller& i,platform::Configuration& c,const platform::KeyBindings& d,const float* r):input(i),configuration(c),defaults(d),timer_rate(r){}
    virtual bool pressed(int slot,std::uint32_t mask)=0;
    virtual bool repeated(int slot,std::uint32_t mask)=0;
    virtual void play_effect(int)=0;
    virtual void rebuild_devices()=0;
    virtual void retire(KeyConfigInf&)=0;
};
Environment& environment();
KeyConfigInf* controller();
KeyConfigInf* create(const sprite::Vec3&); //4c7cb0
int initialize(KeyConfigInf&,const sprite::Vec3&); //4c58d0
void set_state(KeyConfigInf&,int); //4c61d0
void set_phase(KeyConfigInf&,int); //4c61a0
int update(KeyConfigInf&,Environment&); //4c5790
int update_devices(KeyConfigInf&,Environment&); //4c7160
int update_bindings(KeyConfigInf&,Environment&); //4c6230
void assign_button(KeyConfigInf&,Environment&,int action,int button); //4c7dd0
int draw(KeyConfigInf&,text::Renderer&,input::Controller&); //4c5870
void draw_devices(KeyConfigInf&,text::Renderer&,input::Controller&); //4c7770
void draw_bindings(KeyConfigInf&,text::Renderer&,input::Controller&); //4c6b30
const char* button_name(int kind,int button); //421890, shared15-lock formatted buffer
const platform::KeyBindings& default_bindings(); //actual CRT4010f0 ->41fb10 at5b9eb0
}
