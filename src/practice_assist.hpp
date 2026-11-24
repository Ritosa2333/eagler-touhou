#pragma once
#include "locale.hpp"
#include "practice_window.hpp"
#include <array>
#include <cstdint>

namespace soew {
enum class PracticeTool : unsigned { Invincible, Lives, Rank, Bombs, Power, Timeout, AutoBomb, EnemyInvincible };
struct PracticeAssist {
    std::uint8_t tools=0;
    bool open=false,info=false;
    PracticeWindow window;
    bool enabled(PracticeTool tool)const{return (tools&(1u<<unsigned(tool)))!=0;}
    void toggle(PracticeTool tool){tools^=std::uint8_t(1u<<unsigned(tool));}
    static constexpr int left=40,top=24,width=208,pitch=18,tool_rows=8,rows=tool_rows+1;
    static constexpr int header=12;
    static constexpr PracticeBounds base{left,top-header,left+width,top+rows*pitch};
    bool active_row(int row)const{return row==tool_rows?info:enabled(PracticeTool(row));}
    void toggle_row(int row){if(row==tool_rows)info=!info;else toggle(PracticeTool(row));}
    static int row_at(int x,int y){
        return x>=left&&x<left+width&&y>=top&&y<top+rows*pitch?(y-top)/pitch:-1;
    }
    int row_at(PracticePoint p)const{p=window.local(p);return row_at(p.x,p.y);}
    bool in_header(PracticePoint p)const{
        return PracticeBounds{left,top-header,left+width,top}.contains(window.local(p));
    }
};
inline constexpr std::array<LocalizedText,PracticeAssist::rows> practice_tool_labels={{{L"無敵",L"Invincible",L"无敌",L"無敵"},
    {L"残機無限",L"Infinite lives",L"无限残机",L"無限殘機"},{L"ランク固定",L"Lock rank",L"锁动态难度",L"鎖動態難度"},
    {L"ボム無限",L"Infinite bombs",L"无限灵击",L"無限靈擊"},{L"パワー維持",L"Keep power",L"火力不减",L"火力不減"},
    {L"時間切れなし",L"Lock timeout",L"锁时",L"鎖時"},{L"オートボム",L"Auto bomb",L"自动灵击",L"自動靈擊"},
    {L"敵無敵",L"Enemy invincible",L"敌人无敌",L"敵人無敵"},{L"詳細情報",L"More info",L"更多游戏信息",L"更多遊戲資訊"}}};
}
