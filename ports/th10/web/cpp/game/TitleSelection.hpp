#pragma once
#include "TitleMain.hpp"
#include "ScoreData.hpp"
#include "GameProgression.hpp"
namespace th10 {
struct TitleSelectionEnvironment : TitleMainEnvironment {
    ScoreData** scores;
    AnmFile** effects;
    const StageConfiguration* stage_table;
    const StageConfiguration** current_stage;
    i32 *remembered_stage,*practice_shortcut,*pending_screen;
    u8* keyboard;
    virtual void show_loading(float x,float y)=0;
    virtual void hide_screen()=0;
    virtual void fade_music(float seconds)=0;
    virtual bool read_keyboard()=0;
};
struct TitleSelection {
    TitleMenu& title;
    TitleSelectionEnvironment& environment;
    i32 difficulty();
    i32 character();
    i32 shot();
    i32 stage();
private:
    void move(u32 previous,u32 next,i32 script);
    void select_child(i32 parent,i32 child,i32 label);
    void stage_configuration(i32 stage);
};
}
