#pragma once
#include "draw.hpp"
#include "../effect_system/effect.hpp"
namespace th20::source::stone_menu {
class UpdateEnvironment {
public:
    virtual ~UpdateEnvironment()=default;
    virtual DrawEnvironment& choices()=0;
    virtual game_session::Player& player()=0;
    virtual bool pressed(unsigned)=0;
    virtual bool repeated(unsigned)=0;
    virtual void sound(int)=0;
    virtual void select_profile(int slot,int index)=0;
    virtual void set_used_stones(unsigned,unsigned)=0;
    virtual void consume_stone(unsigned)=0;
    virtual void refresh_overlay(int character)=0; //534080, separate overlay owner
    virtual sprite::Animation* animation(unsigned& handle,bool fallback)=0;
    virtual sprite::Animation* child_animation(unsigned& handle,int script)=0;
    virtual void texture_rectangle(sprite::Animation&,float,float,float,float)=0;
    virtual void immediate_interrupt(unsigned handle,int event)=0;
    virtual unsigned spawn_named(sprite::AnimationFile&,int script,const sprite::Vec3* position,unsigned flags)=0;
    virtual unsigned spawn_effect(int,const effects::Parameters&,bool secondary)=0;
    virtual bool overlay_filling()=0; //Context+2c+54==1
    virtual bool special_active()=0; //5c6118+b0
    virtual unsigned special_color()=0; //513f70
    virtual sprite::Vec3 player_position()=0; //460830/460850
};
UpdateEnvironment& update_environment();
int update(StoneMenuInf&,UpdateEnvironment&); //5167b0
void open(StoneMenuInf&,int mode,UpdateEnvironment&); //51a0b0
void open(StoneMenuInf&,int mode);
namespace unrecovered {
void refresh_overlay_selection(runtime::CallbackOwner&,int character); //534080
}
}
