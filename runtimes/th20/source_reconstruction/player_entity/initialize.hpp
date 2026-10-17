#pragma once
#include "owner.hpp"
namespace th20::source::player_entity {
void select_context(Player&,int,game_session::Session&) noexcept; //4ffe40
void select_context(Option&,int,game_session::Session&) noexcept; //4ffe80
void select_context(Feedback&,int,game_session::Session&) noexcept; //4ffe10
void select_context(Shot&,int,game_session::Session&) noexcept; //506d80
void initialize_shots(ShotController&,int,game_session::Session&); //504bd0
class InitializationServices {
public:
    virtual ~InitializationServices()=default;
    virtual game_session::Session& session()=0;
    virtual void select_view(int)=0;
    virtual sprite::AnimationFile* load_animation(int,const char*)=0;
    virtual void* load_shots(const char*)=0;
    virtual void resource_error()=0;
    virtual void bind_animation(sprite::AnimationFile&,sprite::Animation&,int)=0;
    virtual std::uint32_t create_feedback_animation()=0;      //51b960+10 ->450c70 stone14
    virtual void hide_feedback_animation(std::uint32_t)=0;   //450670, scale0,0
    virtual void create_damage(int)=0;                      //4c20d0
};
void initialize_feedback(Feedback&,int,InitializationServices&); //4f9470
int initialize(Player&,int,InitializationServices&);        //4f9520
Player* create_player(int,PlayerServices&,InitializationServices&); //4ffff0
InitializationServices& initialization_services();
Player* create_player(int index=0);
void destroy_player(int index=0);                           //4feff0
int draw_player(Player&);                                  //4f8af0
}
