#pragma once
#include "cursor.hpp"
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_state/state.hpp"
#include "../game_session/session.hpp"
#include "../sprite_renderer/sprite.hpp"
#include <span>
#include <string_view>
namespace th20::source::stone_menu {
struct Environment {
    virtual ~Environment()=default;
    virtual void select_view(int)=0;
    virtual sprite::AnimationFile* load_animation(int,const char*)=0;
    virtual std::string text_resource(const char*)=0;
    virtual void load_error()=0;
    virtual void interrupt(std::uint32_t,int)=0;
    virtual void request_delete(std::uint32_t&)=0;
    virtual void unload_animation(int)=0;
};
Environment& environment();
struct StoneMenuInf final:runtime::CallbackOwner {
    sprite::AnimationFile* file;
    std::int32_t visible;
    recovered::Timer age;
    menu::Cursor category,selection;
    std::int32_t saved_selection;
    std::uint32_t animation_handles[9];
    sprite::Vec3 selection_position;
    char names[88][256];
    char descriptions[88][5][256];
    std::int32_t state;
    std::uint8_t field_210f8,padding_210f9[3];
    std::int32_t view_index;
    game_session::Context* context;
    StoneMenuInf(); //515b90
    ~StoneMenuInf() override; //516170
    void select_context(int) noexcept; //51c7a0
};
static_assert(sizeof(StoneMenuInf)==0x21104&&offsetof(StoneMenuInf,category)==0x28&&offsetof(StoneMenuInf,selection)==0x74&&offsetof(StoneMenuInf,names)==0xf4&&offsetof(StoneMenuInf,descriptions)==0x58f4&&offsetof(StoneMenuInf,state)==0x210f4);
void parse_text(StoneMenuInf&,std::string_view); //519960 text parsing portion
int update(StoneMenuInf&); //5167b0
int draw(StoneMenuInf&); //5189c0
int initialize(StoneMenuInf&,int,Environment&); //519960
int initialize(StoneMenuInf&,int); //519960
void hide(StoneMenuInf&,Environment&); //519840
void clear(StoneMenuInf&,Environment&); //5198d0
void hide(StoneMenuInf&);void clear(StoneMenuInf&);
StoneMenuInf* create_controller(int); //51cc20
void release(); //51b6d0
extern StoneMenuInf* controller; //unique original5c6120
}
