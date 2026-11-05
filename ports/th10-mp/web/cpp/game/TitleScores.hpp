#pragma once
#include "TitleMain.hpp"
#include "ScoreData.hpp"
#include "ResultsDraw.hpp"
#include <initializer_list>
namespace th10 {
struct TitleScoreEnvironment : TitleMainEnvironment {
    ScoreData** scores;
    AnmFile **effects,**rows;
    const std::int8_t* spell_difficulties;
    const char* unknown_spell_format;
    const u32* unlock_sequence;
    u32* unlock_cursor;
    i32* unlock_elapsed;
    u8 *keyboard,*previous_keyboard,*pressed_keyboard;
    virtual bool read_keyboard()=0;
    virtual void text(AnmVm* vm,u32 color,const char* format,const u32* arguments,u32 count)=0;
    void print(AnmVm* vm,u32 color,const char* format,std::initializer_list<u32> args={}){text(vm,color,format,args.begin(),args.size());}
};
struct TitleScores {
    TitleMenu& title;
    TitleScoreEnvironment& environment;
    i32 update();
    i32 fill_spell_list();
    void update_unlock_code();
};
i32 count_spells(const std::int8_t* difficulties,i32 difficulty) noexcept;
void unlock_score_records(ScoreData& score) noexcept;
void draw_score_entry(i32 character,i32 difficulty,i32 row,const Vec3& position,ResultsDrawEnvironment& environment);
i32 draw_title_scores(const TitleMenu& title,ResultsDrawEnvironment& environment);
}
