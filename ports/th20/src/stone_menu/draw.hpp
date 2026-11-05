#pragma once
#include "stone.hpp"
namespace th20::source::stone_menu {
enum class TextField:unsigned {color=0x1a1c0,shadow=0x1a1c4,blend=0x1a1c8,font=0x1a1e4};
class DrawEnvironment {
public:
    virtual ~DrawEnvironment()=default;
    virtual int character()=0;
    virtual int difficulty()=0;
    virtual int selected_profile(int slot)=0;
    virtual unsigned stone_count(unsigned)=0;
    virtual unsigned used_stone_count(unsigned)=0;
    virtual bool extra_unlocked(unsigned)=0;
    virtual float screen_scale()=0;
    virtual sprite::Vec3 animation_position(std::uint32_t&)=0;
    virtual void set_animation_position(std::uint32_t,const sprite::Vec3&)=0;
    virtual void set_animation_scale(std::uint32_t,float,float)=0;
    virtual void text_style(unsigned,unsigned)=0;
    virtual void text_field(TextField,unsigned)=0;
    virtual void clear_ascii_lines()=0;
    virtual void write_text(const sprite::Vec3&,const char*)=0;
};
DrawEnvironment& draw_environment();
int draw(StoneMenuInf&,DrawEnvironment&); //5189c0
}
