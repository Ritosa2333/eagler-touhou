#pragma once
#include "TitleCharacterMenu.hpp"
namespace th15 {
class TitlePracticeMenu {
    TitleState& state;SessionState& progress;TitleSelectionSettings& settings;AnmManager& animations;TitleAnimations visuals;TitleCharacterServices& host;
    bool check(bool,const char*);bool visual(bool);
public:
    std::string error;
    TitlePracticeMenu(TitleState& s,SessionState& p,TitleSelectionSettings& settings,AnmManager& a,TitleCharacterServices& host,i32 title=16,i32 ascii=5):state(s),progress(p),settings(settings),animations(a),visuals(s,a,title,ascii),host(host){}
    bool update(u32 pressed,u32 repeated,i32 numbered_chapter,const TitleRecords&);
};
struct TitleResumeServices:TitleCharacterServices {virtual bool checkpoint_stage(i32 character,i32 difficulty,i32& stage)=0;};
class TitleResumeMenu {
    TitleState& state;SessionState& progress;PlayerLifeSession& player;AnmManager& animations;TitleAnimations visuals;TitleResumeServices& host;
    bool check(bool,const char*);bool visual(bool);
public:
    std::string error;
    TitleResumeMenu(TitleState& s,SessionState& p,PlayerLifeSession& player,AnmManager& a,TitleResumeServices& host,i32 title=16,i32 ascii=5):state(s),progress(p),player(player),animations(a),visuals(s,a,title,ascii),host(host){}
    bool update(u32 pressed,u32 repeated);
};
}
