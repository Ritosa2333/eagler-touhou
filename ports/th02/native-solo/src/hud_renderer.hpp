#pragma once
#include "core_contract.hpp"
#include "menu.hpp"
#include <algorithm>
#include <span>
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

namespace coop_hud {
// DOS multiplayer TRAM layout: player rows 8..16, enemy rows 19..20.
inline constexpr int left=448,right=624,first_player_y=128,player_pitch=48,row_height=16;
inline constexpr int player_bottom=first_player_y+3*player_pitch,option_x=504,ghost_x=528,rescue_right=620;
inline constexpr int boss_title_y=304,boss_gauge_y=320,boss_gauge_width=128;
inline constexpr int lives_x=left,bombs_x=512,stock_digit_dx=18,stock_limit_dx=34;
inline constexpr int power_x=468,power_width=80,power_value_right=584;
inline constexpr std::uint32_t detail_ink=0xffc0c0c0u,power_edge=0xff505050u,power_empty=0xff202020u;
inline void draw_small_number(std::vector<std::uint32_t> &pixels,std::string_view text,int right_x,int y,std::uint32_t ink){
    // Integer-pixel 5x7 digits and slash for stock limits and exact Power.
    constexpr std::array<std::array<unsigned,7>,11> glyphs={{
        {14,17,17,17,17,17,14}, {4,12,4,4,4,4,14}, {14,17,1,2,4,8,31},
        {30,1,1,14,1,1,30}, {2,6,10,18,31,2,2}, {31,16,16,30,1,1,30},
        {14,16,16,30,17,17,14}, {31,1,2,4,8,8,8}, {14,17,17,14,17,17,14},
        {14,17,17,15,1,1,14}, {1,1,2,4,8,16,16}
    }};
    int x=right_x-int(text.size())*6+1;
    for(const auto ch:text){
        const auto &glyph=glyphs[ch=='/'?10:ch-'0'];
        for(int dy=0;dy<7;++dy)for(int dx=0;dx<5;++dx)
            if(glyph[dy]&(16u>>dx))pixels[(y+dy)*640+x+dx]=ink;
        x+=6;
    }
}
inline void draw_power_gauge(std::vector<std::uint32_t> &pixels,unsigned power,int row_y,std::uint32_t ink){
    for(int dy=4;dy<12;++dy){
        auto row=pixels.begin()+(row_y+dy)*640+power_x;
        std::fill_n(row-1,power_width+2,power_edge);
        if(dy!=4&&dy!=11){
            std::fill_n(row,power_width,power_empty);
            std::fill_n(row,power,ink);
        }
    }
    draw_small_number(pixels,std::to_string(power)+"/80",power_value_right,row_y+5,detail_ink);
}
inline void draw_boss_gauge(std::vector<std::uint32_t> &pixels,const th03::assets::BfntSheet &font,
    unsigned health,unsigned max_health,std::span<const unsigned> cuts){
    // Same eight Power gaiji and fill colors as the original multiplayer HUD.
    constexpr std::array colors={original_hud::red,original_hud::magenta,original_hud::cyan,
        original_hud::yellow,original_hud::white};
    const int fill=int((health*boss_gauge_width+max_health-1)/max_health);
    for(int x=0;x<fill;x+=16)
        ::draw_glyph(pixels,font,0x30+std::min(16,fill-x)-1,left+x,boss_gauge_y,colors[fill>>5]);
    for(const auto cut:cuts){
        const int x=int(cut*boss_gauge_width/max_health);
        for(int y=2;y<14;++y)pixels[(boss_gauge_y+y)*640+left+x]=x<fill?0xff181818u:0xff505050u;
    }
}
}

namespace practice_keys {
// Leave the original HiScore label at y=48 untouched. TH05 groups the
// action keys separately from the inverted-T direction keys.
struct Key {int x,y,w;Input input;};
inline constexpr int height=16;
inline constexpr std::uint32_t white=0xffffffffu,purple=0xff9c48dau,ink=0xff422058u;
inline constexpr std::array<Key,7> keys={{{437,16,28,Focus},{468,16,17,Shot},{488,16,17,Bomb},
    {531,7,17,Up},{511,26,17,Left},{531,26,17,Down},{551,26,17,Right}}};

constexpr std::uint16_t displayed(std::uint16_t input){
    auto mask=input&(Up|Down|Left|Right|Shot|Bomb|Focus);
    switch(input&(UpLeft|UpRight|DownLeft|DownRight)){
    case UpLeft:mask|=Up|Left;break;
    case UpRight:mask|=Up|Right;break;
    case DownLeft:mask|=Down|Left;break;
    case DownRight:mask|=Down|Right;break;
    }
    return static_cast<std::uint16_t>(mask);
}

inline void draw(std::vector<std::uint32_t> &pixels,std::uint16_t input){
    // 3x5 action labels and a 7x7 arrow, drawn in framebuffer pixels.
    constexpr std::array<std::uint8_t,5> s={7,4,7,1,7},h={5,5,7,5,5},i={7,2,2,2,7},
        f={7,4,6,4,4},t={7,2,2,2,2},z={7,1,2,4,7},x={5,5,2,5,5};
    constexpr std::array shift={s,h,i,f,t};
    constexpr std::array<std::uint8_t,7> up={0b0001000,0b0011100,0b0111110,0b1111111,0b0011100,0b0011100,0b0011100};
    const auto mask=displayed(input);
    for(const auto &key:keys){
        const bool pressed=mask&key.input;
        const auto pixel=[&](int dx,int dy,std::uint32_t color){pixels[(key.y+dy)*640+key.x+dx]=color;};
        for(int dy=0;dy<height;++dy){
            const int cut=dy==0||dy==height-1?2:dy==1||dy==height-2?1:0;
            for(int dx=cut;dx<key.w-cut;++dx){
                const bool edge=dx==cut||dx==key.w-1-cut||dy==0||dy==height-1;
                pixel(dx,dy,pressed&&!edge?purple:white);
            }
        }
        const auto color=pressed?white:ink;
        const auto glyph=[&](const auto &bits,int left,int top){
            for(int dy=0;dy<5;++dy)for(int dx=0;dx<3;++dx)
                if(bits[dy]&(4>>dx))pixel(left+dx,top+dy,color);
        };
        if(key.input==Focus){
            for(int n=0;n<int(shift.size());++n)glyph(shift[n],(key.w-19)/2+n*4,5);
        }else if(key.input==Shot||key.input==Bomb){
            glyph(key.input==Shot?z:x,(key.w-3)/2,5);
        }else{
            for(int dy=0;dy<7;++dy)for(int dx=0;dx<7;++dx)if(up[dy]&(64>>dx)){
                const int gx=key.input==Left?dy:key.input==Right?6-dy:dx;
                const int gy=key.input==Down?6-dy:key.input==Left||key.input==Right?dx:dy;
                pixel((key.w-7)/2+gx,(height-7)/2+gy,color);
            }
        }
    }
}
}
}
