#pragma once
#include "practice.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/pool.hpp"
namespace practice_fixture {
namespace ti=th20::source::title;namespace sp=th20::source::sprite;namespace gs=th20::source::game_session;namespace pg=th20::source::progress;
[[noreturn]] inline void excluded(){throw std::runtime_error("Practice cards fixture reached an excluded menu-update dependency");}
struct Main final:ti::MainEnvironment {
    sp::Controller& sprites;int last[2]{};
    Main(gs::Session& s,sp::Controller& c):MainEnvironment(s,last[0],last[1]),sprites(c){}
    bool pressed(unsigned)override{excluded();}bool repeated(unsigned)override{excluded();}bool extra_unlocked()override{excluded();}bool notice_present()override{excluded();}bool notice_finished()override{excluded();}bool notice_pending()override{excluded();}
    void create_notice()override{excluded();}void retire_notice()override{excluded();}void sound(int)override{excluded();}
    void spawn(ti::TitleInf& o,int i)override{ti::spawn(o,i);}
    void interrupt(ti::TitleInf&,int,int,bool)override{excluded();}bool exists(unsigned)override{excluded();}bool decoration_exists(ti::TitleInf&)override{excluded();}void interrupt_handle(unsigned,int)override{excluded();}void spawn_decoration(ti::TitleInf&)override{excluded();}
};
struct Selection final:ti::SelectionEnvironment {
    sp::Controller& sprites;unsigned menu{};int profile{};
    Selection(Main& m):SelectionEnvironment(m,menu,profile),sprites(m.sprites){}
    void signal(ti::TitleInf& o,int i,int event,bool execute)override{if(execute)sp::execute_animation_interrupt(sprites,o.handles[i],event);else sp::interrupt_animation_children(sprites,o.handles[i],event);}
    void spawn_text_overlay(ti::TitleInf&)override{excluded();}void child_visible(ti::TitleInf&,int,int,bool)override{excluded();}void child_signal(ti::TitleInf&,int,int,int,bool)override{excluded();}bool all_cleared(int,int)override{excluded();}bool character_extra_unlocked(int)override{excluded();}int selected_profile(int,int)override{excluded();}void commit_progress()override{excluded();}void set_weapon(int,int,int)override{excluded();}
};
struct Stage final:ti::StageEnvironment {
    unsigned menu{};int last{};Stage(gs::Session& s):StageEnvironment(s,menu,last){}
    bool pressed(unsigned)override{excluded();}bool repeated(unsigned)override{excluded();}const pg::Profile& profile()override{excluded();}int keyboard_number()override{excluded();}void spawn_heading(ti::TitleInf&)override{excluded();}void retire_heading(ti::TitleInf&)override{excluded();}void sound(int)override{excluded();}void fade(float)override{excluded();}bool effects_ready()override{excluded();}void loading()override{excluded();}void request_start(int)override{excluded();}
};
struct Environment final:ti::PracticeEnvironment {
    sp::Controller& sprites;int resume[3]{};
    Environment(Selection& s,Stage& t):PracticeEnvironment(s,t,resume[0],resume[1],resume[2]),sprites(s.sprites){}
    void delete_animation(ti::TitleInf& o,int i)override{sp::request_animation_deletion(sp::find_animation(sprites,o.handles[i]));}
    bool card_exists(ti::TitleInf& o,int i)override{return sp::resolve_animation_handle(sprites,o.handles394[i])!=nullptr;}
    void card_interrupt(ti::TitleInf& o,int i,int event)override{sp::interrupt_animation_children(sprites,o.handles394[i],event);}
    bool group_available(int,int)override{excluded();}bool card_playable(int)override{excluded();}void refresh_cards(ti::TitleInf&,int,int,int)override{excluded();}void select_card(ti::TitleInf&,int)override{excluded();}void launch(int,int)override{excluded();}
};
}
