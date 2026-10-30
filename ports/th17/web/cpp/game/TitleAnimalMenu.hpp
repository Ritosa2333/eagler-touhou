#pragma once
#include "TitleSelectionMenu.hpp"
#include "HudDrawData.hpp"
namespace th17 {
struct GameRecords;
struct TitleGameStartServices {
    virtual ~TitleGameStartServices()=default;
    virtual bool sound(i32)=0;
    virtual bool fade_menu_music()=0;
    virtual bool show_loading_indicator()=0;
    virtual bool begin_transition(u32& animation)=0;
    virtual bool start_game(i32 stage)=0;
    virtual i32 checkpoint_key()const=0;
};
class TitleAnimalMenu {
    TitleState& state;GameSelection& selection;TitleSelectionSettings& settings;AnmManager& animations;TitleAnimations visuals;TitleGameStartServices& host;
    bool check(bool,const char*);bool visual(bool);bool choose_stage(i32 root);
    u32 transition=0;
public:
    std::string error;
    TitleAnimalMenu(TitleState& s,GameSelection& p,TitleSelectionSettings& settings,AnmManager& a,TitleGameStartServices& host):state(s),selection(p),settings(settings),animations(a),visuals(s,a),host(host){}
    bool update(u32 pressed,u32 repeated,const TitleRecords&);
};
class TitleStageMenu {
    TitleState& state;GameSelection& selection;TitleSelectionSettings& settings;AnmManager& animations;TitleAnimations visuals;TitleGameStartServices& host;
    bool check(bool,const char*);bool visual(bool);u32 transition=0;
public:
    std::string error;
    TitleStageMenu(TitleState& s,GameSelection& p,TitleSelectionSettings& settings,AnmManager& a,TitleGameStartServices& host):state(s),selection(p),settings(settings),animations(a),visuals(s,a),host(host){}
    bool update(u32 pressed,u32 repeated,const TitleRecords&);
    bool draw(const GameRecords&,HudDrawServices&);
};
}
