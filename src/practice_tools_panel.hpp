#pragma once
#include "practice_advanced.hpp"
#include "practice_assist.hpp"

namespace soew {
struct PracticeToolsPanel {
    enum class Mode { Setup, Live, Practice };
    enum class Kind { Tool, Option, Retry, Speed, FreeSpeed, Apply, Language, ReactionMode, ReactionProgress, Trials, ReactionStart };
    struct Row {Kind kind;int index;PracticeBounds bounds;};

    static constexpr std::array<std::optional<Language>,5> choices={std::nullopt,Language::Japanese,
        Language::English,Language::ChineseSimplified,Language::ChineseTraditional};
    static constexpr LocalizedText title{L"練習ツール",L"Practice Tools",L"练习工具",L"練習工具"};
    static constexpr LocalizedText follow_game{L"ゲームに合わせる",L"Follow game",L"跟随游戏",L"跟隨遊戲"};
    static constexpr int pitch=18,rows_y=48,left_count=16;
    static constexpr PracticeBounds base{8,8,632,392};
    static constexpr PracticeSlider speed_track{348,497},trials_track{462,559};
    static constexpr int speed_row=left_count,retry_row=left_count-1,first_language=speed_row+3;
    static constexpr int reaction_row=first_language+int(choices.size()),count=reaction_row+4;

    PracticeWindow window;
    Mode mode=Mode::Setup;
    bool open=false;
    int focus=speed_row;
    std::optional<int> slider_drag;
    PracticeAdvanced options;
    int speed_draft=PracticeAdvanced::native_speed;
    PracticeReaction reaction;
    std::uint64_t reaction_now=0;

    Row row(int i)const{
        if(i<left_count){
            const int y=rows_y+i*pitch;
            const PracticeBounds b{29,y,325,y+pitch};
            if(i<PracticeAssist::tool_rows)return {Kind::Tool,i,b};
            if(i==retry_row)return {Kind::Retry,0,b};
            return {Kind::Option,i-PracticeAssist::tool_rows,b};
        }
        const int y=i<first_language?rows_y+(i-speed_row)*pitch:
            i<reaction_row?130+(i-first_language)*pitch:246+(i-reaction_row)*pitch;
        const PracticeBounds b{340,y,611,y+pitch};
        if(i<first_language){constexpr std::array kinds={Kind::Speed,Kind::FreeSpeed,Kind::Apply};return {kinds[i-speed_row],0,b};}
        if(i<reaction_row)return {Kind::Language,i-first_language,b};
        constexpr std::array kinds={Kind::ReactionMode,Kind::ReactionProgress,Kind::Trials,Kind::ReactionStart};
        return {kinds[i-reaction_row],0,b};
    }
    bool available_row(int i)const{
        return (i!=retry_row||mode==Mode::Practice)&&(!reaction.active()||i<reaction_row);
    }
    bool toggle(bool available,bool repeat,Mode context){
        if(repeat||(!open&&!available))return false;
        if(open)close();
        else{open=true;mode=context;focus=mode==Mode::Setup?speed_row:0;speed_draft=int(options.draft());}
        return true;
    }
    void close(){open=false;reaction={};slider_drag.reset();}
    void navigate(int delta){do{focus=(focus+delta+count)%count;}while(!available_row(focus));}
    bool in_header(PracticePoint p)const{return PracticeBounds{base.left,base.top,base.right,rows_y-6}.contains(window.local(p));}
    int row_at(PracticePoint p)const{
        const auto local=window.local(p);
        for(int i=0;i<count;++i)if(available_row(i)&&row(i).bounds.contains(local))return i;
        return -1;
    }
    void toggle_free_speed(){
        options.free_fps=!options.free_fps;
        speed_draft=int(options.free_fps?options.preset_to_fps(speed_draft):options.fps_to_preset(speed_draft));
    }
    bool speed_dirty()const{return options.free_fps!=options.speed_in_fps||speed_draft!=int(options.speed);}
    int speed_max()const{return options.free_fps?PracticeAdvanced::max_fps:PracticeAdvanced::max_preset;}
    void apply_speed(){options.speed=unsigned(speed_draft);options.speed_in_fps=options.free_fps;}
    bool adjust(int delta){
        switch(row(focus).kind){
        case Kind::Speed:speed_draft=std::clamp(speed_draft+delta,1,speed_max());return true;
        case Kind::Trials:reaction.trials=std::clamp(reaction.trials+delta,1,20);reaction.reset_series();return true;
        default:return false;
        }
    }
    PracticeSlider track(int i)const{return i==speed_row?speed_track:trials_track;}
    bool begin_slider(int i,PracticePoint p){
        const auto kind=row(i).kind;
        const auto b=row(i).bounds;
        if((kind!=Kind::Speed&&kind!=Kind::Trials)||!track(i).hit(b.top,b.bottom).contains(window.local(p)))return false;
        focus=i;slider_drag=i;move_slider(p);return true;
    }
    void move_slider(PracticePoint p){
        const auto i=*slider_drag;
        const int value=track(i).value(window.local(p).x,1,i==speed_row?speed_max():20);
        if(i==speed_row)speed_draft=value;
        else if(reaction.trials!=value){reaction.trials=value;reaction.reset_series();}
    }
};
}
