// 原5C4B80..5C50DC按八个方向顺序翻转棋盘，格子保存在普通弹0x9C字段。
#pragma once
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
namespace kaikeidou {
inline void ex_boss_middle_timeline_color(ProjectilePools& pool,std::uint32_t slot,std::uint32_t r,std::uint32_t g,std::uint32_t b){
    auto view=stage6_boss_timeline_detail::BulletView{pool,slot*240u};view.word(0x18,r);view.word(0x1c,g);view.word(0x20,b);
}
inline void ex_boss_middle_timeline_grid(ProjectilePools& pool,std::uint32_t column,std::uint32_t row){
    const auto at=[&](std::uint32_t x,std::uint32_t y){return stage6_boss_timeline_detail::BulletView{pool,(y*10u+x)*240u};};
    if(at(column,row).word(0x9c))return;at(column,row).word(0x9c,1);
    constexpr std::array<std::array<int,2>,8> directions{{{{-1,0}},{{1,0}},{{0,-1}},{{0,1}},{{-1,-1}},{{-1,1}},{{1,-1}},{{1,1}}}};
    for(const auto& direction:directions){const auto dx=direction[0],dy=direction[1];const auto origin_x=static_cast<std::int32_t>(column),origin_y=static_cast<std::int32_t>(row);
        for(auto x=origin_x+dx,y=origin_y+dy;x>=0&&x<=7&&y>=0&&y<=11;x+=dx,y+=dy){const auto cell=at(static_cast<std::uint32_t>(x),static_cast<std::uint32_t>(y)).word(0x9c);
            if(cell==1){for(auto xx=origin_x+dx,yy=origin_y+dy;xx!=x||yy!=y;xx+=dx,yy+=dy)at(static_cast<std::uint32_t>(xx),static_cast<std::uint32_t>(yy)).word(0x9c,1);break;}if(cell==0)break;
        }
    }
}
}
