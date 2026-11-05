#pragma once
#include "TitleMain.hpp"
#include "ResultsDraw.hpp"
namespace th10 {
struct TitleClearEnvironment : TitleMainEnvironment {
    ScoreData** scores;
    Replay** replay;
    AnmFile** effects;
    const StageConfiguration* stages;
    const StageConfiguration** current_stage;
    const char* alphabet;
    ResultsEnvironment* ranking;
    virtual void play_music(bool result)=0;
    virtual void timestamp(i32& timestamp)=0;
    virtual Replay* preview(const char* filename)=0;
    virtual void delete_replay(Replay* replay)=0;
    virtual void save_replay(const char* filename,const char* player)=0;
};
struct TitleClear {
    TitleMenu& title;
    TitleClearEnvironment& environment;
    void set_stage(i32 stage);
    void initialize_name();
    void move_keyboard();
    bool enter_character(bool for_replay);
    i32 update_rank();
    i32 update_save();
};
i32 draw_title_clear_rank(const TitleMenu& title,ResultsDrawEnvironment& environment);
i32 draw_title_clear_save(const TitleMenu& title,ResultsDrawEnvironment& environment,const char* const* difficulties);
}
