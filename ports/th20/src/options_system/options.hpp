#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../stone_menu/cursor.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../platform_services/configuration.hpp"
#include "../text_renderer/text.hpp"
namespace th20::source::audio {struct SoundInf;}
namespace th20::source::options {
struct OptionInf final:runtime::CallbackOwner { // 4dfc50, OptionInf, vtable572090
    std::uint32_t field_10;
    menu::Cursor cursor;
    recovered::Timer age;
    sprite::Vec3 position;
    recovered::Timer selection_age,key_config_age;
    std::int32_t allow_escape,state;
    OptionInf();
    ~OptionInf() override;
};
static_assert(sizeof(OptionInf)==0xa4&&offsetof(OptionInf,age)==0x60&&offsetof(OptionInf,position)==0x70&&offsetof(OptionInf,state)==0xa0);
struct Environment {
    platform::Configuration& configuration;
    std::int32_t& display_mode;
    const float* timer_rate;
    virtual bool key_config_active()=0;
    virtual bool pressed(std::uint32_t)=0;
    virtual bool repeated(std::uint32_t)=0;
    virtual void play_effect(int)=0;
    virtual void screen_change_effect()=0;
    virtual void apply_volume()=0;
    virtual void reset_device()=0;
    virtual void create_key_config(const sprite::Vec3&)=0;
    virtual void save_configuration()=0;
    virtual void retire(OptionInf&)=0;
    Environment(platform::Configuration& c,std::int32_t& mode,const float* rate):configuration(c),display_mode(mode),timer_rate(rate){}
};
Environment& environment();
OptionInf* controller();
OptionInf* create(const sprite::Vec3&); //4e1080
int initialize(OptionInf&,const sprite::Vec3&); //4e0e80
void set_state(OptionInf&,int); //4e0f80
void apply_volume(platform::Configuration&,audio::SoundInf&); //4e0fc0
int update(OptionInf&,Environment&); //4dfe00
int draw(OptionInf&,text::Renderer&,const platform::Configuration&,int display_mode,bool key_config_active); //4e07b0
// 4dd910 adds the original error diagnostic to the configuration write.
void save_configuration_with_diagnostic();
namespace unrecovered {runtime::CallbackOwner* create_key_config(const sprite::Vec3&);}
}
