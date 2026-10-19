#pragma once
#include "SpellState.hpp"
#include "AnmExecutor.hpp"
#include "AnmText.hpp"
namespace th08 {
struct SpellPresentationContext {
    AnmLoaded* text=nullptr;u32 game_flags=0;i16 current_spell=-1;i32 hud_redraw=0;
};
struct SpellPresentationActions {
    virtual ~SpellPresentationActions()=default;
    virtual void sound(i32 index,i32 mode)=0;
};
// Original 00415d60 / 00415f00 announcements. The same animation objects are
// subsequently advanced and drawn by the spellcard controller.
class SpellPresentation {
    SpellState& state;AnmExecutor& anm;TextWriter& text;SpellPresentationActions& actions;
    bool start(i32 slot,AnmLoaded* file,i32 script);
    i32* live_hud_redraw=nullptr;
    void redraw(){context.hud_redraw=2;if(live_hud_redraw)*live_hud_redraw=2;}
public:
    SpellPresentationContext context;
    SpellPresentation(SpellState& s,AnmExecutor& a,TextWriter& t,SpellPresentationActions& e):state(s),anm(a),text(t),actions(e){}
    void bind_redraw(i32& value){live_hud_redraw=&value;}
    bool player(i32 form,const char* name,i32 style);
    bool enemy(i32 portrait,const char* name,i32 style);
    void end_player(){state.end_player_announcement();}
    void end_enemy(){state.end_enemy_announcement();}
};
}
