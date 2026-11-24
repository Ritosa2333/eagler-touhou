#pragma once
#include "practice_assist.hpp"

namespace soew {
struct PracticeToolsPanel {
    enum class Mode { Setup, Live, Practice };
    enum class Kind { Tool, Retry, LanguageHeading, Language };
    struct Row { Kind kind; int index=0; };

    static constexpr std::array<std::optional<Language>,5> choices={std::nullopt,Language::Japanese,
        Language::English,Language::ChineseSimplified,Language::ChineseTraditional};
    static constexpr LocalizedText title{L"練習ツール",L"Practice Tools",L"练习工具",L"練習工具"};
    static constexpr LocalizedText follow_game{L"ゲームに合わせる",L"Follow game",L"跟随游戏",L"跟隨遊戲"};
    static constexpr int pitch=18,visible=8,rows_y=161;
    static constexpr PracticeBounds base{16,135,222,rows_y+visible*pitch+6};
    static constexpr int rail_left=base.right-12;

    PracticeWindow window;
    Mode mode=Mode::Setup;
    bool open=false;
    int first=0,focus=1;
    int wheel_remainder=0;
    std::optional<int> thumb_grab;

    int tool_count()const{return mode==Mode::Setup?0:PracticeAssist::tool_rows;}
    int language_heading()const{return tool_count()+int(mode==Mode::Practice);}
    int count()const{return language_heading()+1+int(choices.size());}
    int max_first()const{return std::max(0,count()-visible);}
    Row row(int index)const{
        if(index<tool_count())return {Kind::Tool,index};
        if(index<language_heading())return {Kind::Retry};
        if(index==language_heading())return {Kind::LanguageHeading};
        return {Kind::Language,index-language_heading()-1};
    }
    bool toggle(bool available,bool repeat,Mode context,std::optional<Language> current){
        if(repeat||(!open&&!available))return false;
        open=!open;
        if(open){
            mode=context;first=wheel_remainder=0;
            focus=mode==Mode::Setup?1+int(std::find(choices.begin(),choices.end(),current)-choices.begin()):0;
        }
        return true;
    }
    void navigate(int delta){
        do {focus=(focus+delta+count())%count();} while(focus==language_heading());
        select(focus);
    }
    void select(int index){
        focus=index;
        first=std::clamp(first,std::max(0,focus-visible+1),std::min(focus,max_first()));
    }
    void scroll_to(int index){
        first=std::clamp(index,0,max_first());
        focus=std::clamp(focus,first,std::min(count()-1,first+visible-1));
        if(focus==language_heading())focus+=focus<first+visible-1?1:-1;
    }
    void wheel(int delta){
        wheel_remainder+=delta;
        scroll_to(first-3*(wheel_remainder/120));
        wheel_remainder%=120;
    }
    bool in_header(PracticePoint p)const{
        return PracticeBounds{base.left,base.top,base.right,rows_y}.contains(window.local(p));
    }
    int row_at(PracticePoint p)const{
        p=window.local(p);
        if(!PracticeBounds{base.left,rows_y,max_first()?rail_left:base.right,rows_y+visible*pitch}.contains(p))return -1;
        const int index=first+(p.y-rows_y)/pitch;
        return index<count()&&index!=language_heading()?index:-1;
    }
    PracticeBounds rail()const{return {rail_left,rows_y,base.right-4,rows_y+visible*pitch};}
    int thumb_height()const{return visible*visible*pitch/count();}
    PracticeBounds thumb()const{
        const auto track=rail();
        const int height=thumb_height(),travel=track.bottom-track.top-height;
        const int y=track.top+(first*travel+max_first()/2)/max_first();
        return {track.left,y,track.right,y+height};
    }
    bool scrollbar_click(PracticePoint p){
        p=window.local(p);
        if(!max_first()||!rail().contains(p))return false;
        const auto handle=thumb();
        thumb_grab=handle.contains(p)?p.y-handle.top:thumb_height()/2;
        move_thumb_local(p.y);
        return true;
    }
    void move_thumb_local(int y){
        const auto track=rail();
        const int travel=track.bottom-track.top-thumb_height();
        const int position=std::clamp(y-track.top-*thumb_grab,0,travel);
        scroll_to((position*max_first()+travel/2)/travel);
    }
    void move_thumb(PracticePoint p){move_thumb_local(window.local(p).y);}
};
}
