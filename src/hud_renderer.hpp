#pragma once
#include "menu.hpp"
#include <algorithm>
#include <vector>

namespace soew {
struct HudState {
    std::uint32_t score=0,high_score=0;
    unsigned continues=0,high_continues=0;
    int lives=2,bombs=3,power=80,difficulty=1;
};
namespace original_hud {
// ReC98 th02/main/hud/hud.hpp:6-24; TRAM cells are 8x16, gaiji are 16x16.
inline constexpr int left=448,right=576,top=0,bottom=400;
inline constexpr int score_row=6,high_score_row=4,bombs_row=15,lives_row=17,power_row=20,rank_row=22;
inline constexpr int labeled_column=62,label_column=57;
inline constexpr std::uint32_t black=0xff000000u,blue=0xff0000ffu,red=0xffff0000u,magenta=0xffff00ffu,
    green=0xff00ff00u,cyan=0xff00ffffu,yellow=0xffffff00u,white=0xffffffffu;
inline constexpr int blank=0xcf,digit_zero=0xa0;
inline constexpr std::array<int,21> power_to_level={0,1,2,2,3,3,4,4,5,5,6,6,6,7,7,7,8,8,8,8,9};
inline constexpr std::array<std::uint32_t,9> power_colors={red,red,red,magenta,magenta,blue,green,cyan,yellow};
inline constexpr std::array<std::string_view,5> ranks={"EASY   ","NORMAL ","HARD   ","LUNATIC","EXTRA  "};
struct Cell {
    int column,row,code;
    std::uint32_t color;
};
inline std::vector<Cell> cells(const HudState &state){
    std::vector<Cell> result;
    const auto put=[&](int column,int row,int code,std::uint32_t color=white){result.push_back({column,row,code,color});};
    // th02_main.asm:26303-26307; gaiji.h follows the 36 bold glyphs A0..C3.
    put(61,5,0xc4,yellow);put(63,5,0xc5,yellow);put(65,5,0xc6,yellow);
    put(60,3,0xce,yellow);put(62,3,0xc4,yellow);put(64,3,0xc5,yellow);put(66,3,0xc6,yellow);
    const auto score=[&](int row,std::uint32_t value,unsigned continues){
        // hud.cpp:53-75: seven internal-score digits, not score multiplied by ten.
        value=std::min(value,9999999u);
        for(std::uint32_t divisor=1000000,column=56;divisor;divisor/=10,column+=2){
            put(static_cast<int>(column),row,digit_zero+static_cast<int>(value/divisor));value%=divisor;
        }
        put(70,row,digit_zero+static_cast<int>(std::min(continues,9u)));
    };
    score(score_row,state.score,state.continues);score(high_score_row,state.high_score,state.high_continues);
    put(label_column,lives_row,0xc9,yellow);put(label_column+2,lives_row,0xca,yellow);
    put(label_column,bombs_row,0xcc,yellow);put(label_column+2,bombs_row,0xcd,yellow);
    put(label_column,power_row,0xc7,yellow);put(label_column+2,power_row,0xc8,yellow);
    for(int i=0;i<5;++i){
        put(labeled_column+i*2,lives_row,i<std::clamp(state.lives,0,5)?0x02:blank);
        put(labeled_column+i*2,bombs_row,i<std::clamp(state.bombs,0,5)?0x03:blank);
    }
    const int power=std::clamp(state.power,1,80),level=power_to_level[power>>2];
    // hud.cpp:98-118 and hud/power[data].asm. Clamp zero to avoid its original 16-power rendering bug.
    for(int i=0;i<5;++i){
        int code=blank;const int remaining=power-i*16;
        if(level==9)code=0x40+i;
        else if(remaining>0)code=0x30+std::min(remaining,16)-1;
        put(labeled_column+i*2,power_row,code,level==9?white:power_colors[level]);
    }
    const int difficulty=std::clamp(state.difficulty,0,4);
    const std::array<std::uint32_t,5> rank_colors={green,cyan,magenta,red,red};
    for(int i=0;i<7;++i)put(label_column+i*2,rank_row,::glyph_code(ranks[difficulty][i]),rank_colors[difficulty]);
    return result;
}
}
// Stage text RAM exposes the playfield and hides the graphics gutters, HUD
// backing and tile-source VRAM. Apply after palette/scroll effects, before HUD.
inline void mask_gameplay_tram(std::vector<std::uint32_t> &pixels){
    if(pixels.size()!=640u*400u)throw std::runtime_error("Gameplay text mask needs a 640x400 framebuffer");
    constexpr int stride=640,left=32,right=416,top=16,bottom=384;
    std::fill_n(pixels.begin(),top*stride,original_hud::black);
    for(int y=top;y<bottom;++y){
        auto row=pixels.begin()+y*stride;
        std::fill(row,row+left,original_hud::black);
        std::fill(row+right,row+stride,original_hud::black);
    }
    std::fill(pixels.begin()+bottom*stride,pixels.end(),original_hud::black);
}
inline void draw_original_hud(std::vector<std::uint32_t> &pixels,const th03::assets::BfntSheet &font,const HudState &state){
    if(pixels.size()!=640u*400u)throw std::runtime_error("Original HUD needs a 640x400 framebuffer");
    const auto cells=original_hud::cells(state);
    for(const auto &cell:cells){
        if(cell.code<font.header.first_code||cell.code>font.header.last_code)throw std::runtime_error("Original HUD gaiji unavailable");
    }
    for(int y=original_hud::top;y<original_hud::bottom;++y)
        std::fill(pixels.begin()+y*640+original_hud::left,pixels.begin()+y*640+original_hud::right,original_hud::black);
    for(const auto &cell:cells)::draw_glyph(pixels,font,cell.code,cell.column*8,cell.row*16,cell.color);
}
}
