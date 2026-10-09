#pragma once
#include "TitleAnimations.hpp"
#include "TitleRecords.hpp"
#include "SessionState.hpp"
namespace th15 {
struct TitleCharacterServices {
    virtual ~TitleCharacterServices()=default;
    virtual bool checkpoint_available(i32 character,i32 difficulty,bool& available)=0;
    virtual bool reset_resume_selection()=0;
    virtual bool sound(i32 id)=0;
    virtual bool prepare_game_music()=0;
    virtual bool begin_transition(u32& animation)=0;
    virtual bool start_game(i32 stage)=0;
    virtual bool practice_overlay_enabled()const{return false;}
    virtual void practice_overlay_state(i32){}
    // -1 cancel, 0 wait, 1 accept; selected stage is zero based.
    virtual i32 practice_overlay_action(i32&){return 0;}
};
class TitleCharacterMenu {
    TitleState& state;SessionState& progress;PlayerLifeSession& player;TitleSelectionSettings& settings;
    AnmManager& animations;TitleAnimations visuals;TitleCharacterServices& host;
    bool check(bool,const char*);bool visual(bool);bool available(bool&);
    bool selected_children(i32 root);bool choose_stage();bool choose_resume();
public:
    std::string error;
    TitleCharacterMenu(TitleState& s,SessionState& p,PlayerLifeSession& player,TitleSelectionSettings& settings,AnmManager& a,TitleCharacterServices& host,i32 title=16,i32 ascii=5):state(s),progress(p),player(player),settings(settings),animations(a),visuals(s,a,title,ascii),host(host){}
    bool update(u32 pressed,u32 repeated,const TitleRecords&);
};
}
