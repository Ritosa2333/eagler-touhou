#pragma once
#include "GameResources.hpp"
#include "GameInput.hpp"
#include "MatchRules.hpp"
namespace th09 {
enum class InGameAction {resume,title,retry,replay_retry,save_score,continue_match,restart_extra};
struct InGameMenuServices {
    GameMode mode=GameMode::story;i32 difficulty=1,continues=0;u32 game_flags=0;
    bool capture_enabled=false;
    virtual ~InGameMenuServices()=default;
    virtual void menu_sound(i32)=0;
    virtual void menu_action(InGameAction)=0;
    virtual void menu_view()=0;
    virtual void menu_sprite(AnmVm&)=0;
};
template<u32 Count> struct MenuAnimations {
    i32 state=0,frames=0;std::array<AnmVm,Count> animations;
    MenuAnimations(){std::memset(animations.data(),0,sizeof(animations));}
};
class InGameMenus {
    GameResources& resources;InGameMenuServices& services;
    void start(AnmVm&,i32);void select(AnmVm&,bool,u32 selected_color=0xffffffff);
    template<u32 N> void interrupt(MenuAnimations<N>& m,u32 first,u32 count,i16 id){for(u32 n=first;n<first+count;++n)m.animations[n].pendingInterrupt=id;}
    template<u32 N> void hide(MenuAnimations<N>& m,u32 count){for(u32 n=0;n<count;++n)m.animations[n].flags&=~1u;}
    template<u32 N> void advance(MenuAnimations<N>& m,u32 count,bool capture){for(u32 n=0;n<count;++n)resources.advance(m.animations[n]);if(capture)resources.advance(m.animations[count]);++m.frames;}
    template<u32 N> void draw(MenuAnimations<N>& m,u32 count){services.menu_view();for(u32 n=0;n<count;++n)if(m.animations[n].visible)services.menu_sprite(m.animations[n]);}
public:
    MenuAnimations<8> pause;MenuAnimations<6> game_over;MenuAnimations<4> match_end;
    InGameMenus(GameResources& r,InGameMenuServices& s):resources(r),services(s){}
    void update_pause(const InputFrame&);
    bool update_game_over(const InputFrame&);
    bool update_match_end(const InputFrame&);
    // Read-only lifetime barriers. Ordinary navigation/animation only changes
    // rewindable menu state; these steps may replace the owning game world.
    // Pause hotkeys can cancel a due transition, so its predicate is deliberately
    // conservative. Keep the action-coverage test alongside any menu changes.
    bool pause_retires_world()const{return (pause.state==8||pause.state==9)&&pause.frames>=20;}
    bool game_over_retires_world(u32 flags,i32 continues)const{
        return (flags&8)||continues>2||((game_over.state==3||game_over.state==4)&&game_over.frames>19);
    }
    bool match_end_retires_world(u32 flags)const{
        return (flags&8)||(match_end.state>=4&&match_end.state<=6&&match_end.frames>19);
    }
    void draw_pause(){draw(pause,7);}
    void draw_game_over();
    void draw_match_end(){draw(match_end,3);}
};
}
