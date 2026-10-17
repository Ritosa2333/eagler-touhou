#include "update.hpp"
#include "../program_entry/program_entry.hpp"
#include "../platform_window/platform_window.hpp"
#include "../audio_runtime/audio.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../progress_state/manager.hpp"
#include "../item_system/rewards.hpp"
#include "../special_state/special.hpp"
#include <cstring>
namespace th20::source::stone_menu {
namespace pe=program_entry;
namespace {
struct GameUpdateEnvironment final:UpdateEnvironment {
    DrawEnvironment& choices() override{return draw_environment();}
    game_session::Player& player() override{return *game_session::context(0).current_player;}
    bool pressed(unsigned mask) override{auto* buttons=input::button_slot(0);return buttons&&platform_window::pressed(*buttons,mask)!=0;}
    bool repeated(unsigned mask) override{auto* buttons=input::button_slot(0);return buttons&&platform_window::repeated_or_pressed(*buttons,mask)!=0;}
    void sound(int id) override{pe::thread_registry.request_effect(id,0);}
    void select_profile(int slot,int index) override{progress::manager->select_profile(slot,player().fields_00[2],index);}
    void set_used_stones(unsigned index,unsigned count) override{progress::manager->set_used_stone_count(index,count);}
    void consume_stone(unsigned index) override{progress::manager->consume_stone(index);}
    void refresh_overlay(int character) override{unrecovered::refresh_overlay_selection(*game_session::context(0).overlay_owner,character);}
    sprite::Animation* animation(unsigned& handle,bool fallback) override{auto* value=sprite::resolve_animation_handle(*pe::sprite_controller,handle);return !value&&fallback?&pe::sprite_controller->animation_dc:value;}
    sprite::Animation* child_animation(unsigned& handle,int script) override{return sprite::find_animation_child(*pe::sprite_controller,handle,script,0);}
    void texture_rectangle(sprite::Animation& animation,float x,float y,float w,float h) override{sprite::set_animation_texture_rectangle(*pe::sprite_controller,animation,x,y,w,h);}
    void immediate_interrupt(unsigned handle,int event) override{sprite::execute_animation_interrupt(*pe::sprite_controller,handle,event);}
    unsigned spawn_named(sprite::AnimationFile& file,int script,const sprite::Vec3* position,unsigned flags) override{unsigned handle;sprite::spawn_named_animation(*pe::sprite_controller,file,handle,"stone",script,position,0,-1,flags);return handle;}
    unsigned spawn_effect(int type,const effects::Parameters& parameters,bool secondary) override{return effects::controller(0)->spawn(type,&parameters,nullptr,secondary);}
    bool overlay_filling() override{int phase;std::memcpy(&phase,reinterpret_cast<const std::uint8_t*>(game_session::context(0).overlay_owner)+0x54,4);return phase==1;}
    bool special_active() override{std::uint8_t active;std::memcpy(&active,reinterpret_cast<const std::uint8_t*>(item::unrecovered::special_state_00513dd0())+0xb0,1);return active!=0;}
    unsigned special_color() override{return special_state::color(*item::unrecovered::special_state_00513dd0(),player());}
    sprite::Vec3 player_position() override{sprite::Vec3 value;std::memcpy(&value,reinterpret_cast<const std::uint8_t*>(game_session::context(0).objects_04[0])+0x614,12);return value;}
};
}
UpdateEnvironment& update_environment(){static GameUpdateEnvironment host;return host;}
}
