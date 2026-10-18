#pragma once
#include "selection.hpp"
#include "../progress_state/profile.hpp"
namespace th20::source::title {
struct RankEnvironment {
    SelectionEnvironment& selection;
    explicit RankEnvironment(SelectionEnvironment& s):selection(s){}
    virtual ~RankEnvironment()=default;
    virtual void play_score_music()=0;
    virtual void play_title_music()=0;
    virtual void open_stones()=0;
    virtual void select_stone(int)=0;
    virtual void hide_stones()=0;
    virtual int insert_score()=0;
    virtual progress::Profile& profile()=0;
    virtual char* default_name()=0;
    virtual void select_stage_zero()=0;
    virtual void retire_replay()=0;
};
RankEnvironment& rank_environment();
char* entered_name(TitleInf&) noexcept;
int update_rank_entry(TitleInf&,RankEnvironment&); //5223e0, rank/name input
void draw_rank_entry(TitleInf&); //522e20
}
