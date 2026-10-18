#pragma once
#include "selection.hpp"
#include "stage_select.hpp"
namespace th20::source::title {
extern int practice_resume_stage,practice_resume_difficulty,practice_resume_boss; //5b0a54/58/5c
class PracticeEnvironment {
public:
    SelectionEnvironment& selection;StageEnvironment& stage;
    int& resume_stage;int& resume_difficulty;int& resume_boss;
    PracticeEnvironment(SelectionEnvironment& s,StageEnvironment& t,int& a,int& b,int& c):selection(s),stage(t),resume_stage(a),resume_difficulty(b),resume_boss(c){}
    virtual ~PracticeEnvironment()=default;
    virtual void delete_animation(TitleInf&,int)=0;
    virtual bool group_available(int stage,int boss)=0;
    virtual bool card_playable(int id)=0;
    virtual void refresh_cards(TitleInf&,int stage,int boss,int selected)=0;
    virtual void select_card(TitleInf&,int)=0;
    virtual bool card_exists(TitleInf&,int slot)=0; //485a10 resolves and clears stale handles
    virtual void card_interrupt(TitleInf&,int slot,int event)=0;
    virtual void launch(int stage,int card)=0;
};
PracticeEnvironment& practice_environment();
int update_practice_stage(TitleInf&,PracticeEnvironment&); //528fc0
int update_practice_boss(TitleInf&,PracticeEnvironment&); //5284f0
int update_practice_difficulty(TitleInf&,PracticeEnvironment&); //527f00
bool practice_group_available(const progress::Profile&,int,int); //52cb30
int refresh_practice_cards(TitleInf&,text::Renderer&,const progress::Profile& fallback,const progress::Profile& current,int stage,int boss,int selected,PracticeEnvironment&); //51feb0
int select_practice_card(TitleInf&,int,PracticeEnvironment&); //51fd80
int draw_practice_scores(TitleInf&,text::Renderer&,const progress::Profile& fallback,const progress::Profile& current); //528a50
}
