#include "hud_renderer.hpp"
#include <iostream>
#include <limits>

namespace {
unsigned checks=0;
void check(bool value){++checks;if(!value)throw std::runtime_error("Original HUD check="+std::to_string(checks));}
const soew::original_hud::Cell &at(const std::vector<soew::original_hud::Cell> &cells,int column,int row){
    const auto it=std::find_if(cells.begin(),cells.end(),[&](const auto &cell){return cell.column==column&&cell.row==row;});
    if(it==cells.end())throw std::runtime_error("Missing original HUD cell");return *it;
}
th03::assets::BfntSheet fixture_font(){
    th03::assets::BfntSheet font{};font.header.first_code=0; font.header.last_code=255;
    for(int i=0;i<256;++i){
        th03::assets::MonochromeBitmap1 mono{};mono.bits.resize(32);mono.bits[0]=0x80;
        if(i==soew::original_hud::blank)mono.bits[0]=0;
        font.glyphs.push_back({0,mono});
    }
    return font;
}
void test_layout(){
    using namespace soew;using namespace original_hud;
    check(left==448&&right==576&&labeled_column==62&&label_column==57);
    check(score_row==6&&high_score_row==4&&bombs_row==15&&lives_row==17&&power_row==20&&rank_row==22);
    HudState state;state.score=1234567;state.high_score=7654321;state.continues=4;state.high_continues=2;
    const auto c=cells(state);
    check(c.size()==51);
    check(at(c,61,5).code==0xc4&&at(c,63,5).code==0xc5&&at(c,65,5).code==0xc6);
    check(at(c,60,3).code==0xce&&at(c,62,3).code==0xc4);
    check(at(c,57,17).code==0xc9&&at(c,59,17).code==0xca);
    check(at(c,57,15).code==0xcc&&at(c,59,15).code==0xcd);
    check(at(c,57,20).code==0xc7&&at(c,59,20).code==0xc8);
    for(int i=0;i<7;++i){check(at(c,56+2*i,6).code==0xa1+i);check(at(c,56+2*i,4).code==0xa7-i);}
    check(at(c,70,6).code==0xa4&&at(c,70,4).code==0xa2);
    for(const auto &cell:c){check(cell.column*8>=left&&cell.column*8+16<=right&&cell.row>=0&&cell.row*16+16<=400);check(cell.code>=0&&cell.code<=255);}
    state.score=std::numeric_limits<std::uint32_t>::max();state.high_score=0;
    state.continues=std::numeric_limits<unsigned>::max();state.high_continues=100;
    const auto bounded=cells(state);
    for(int i=0;i<7;++i){check(at(bounded,56+2*i,6).code==0xa9);check(at(bounded,56+2*i,4).code==0xa0);}
    check(at(bounded,70,6).code==0xa9&&at(bounded,70,4).code==0xa9);
    for(int stocks=-1;stocks<=6;++stocks){
        state.lives=stocks;state.bombs=stocks;const auto tally=cells(state);
        for(int i=0;i<5;++i){check(at(tally,62+i*2,17).code==(i<std::clamp(stocks,0,5)?2:blank));check(at(tally,62+i*2,15).code==(i<std::clamp(stocks,0,5)?3:blank));}
    }
    for(int power=-1;power<=81;++power){
        state.power=power;const auto bar=cells(state);const auto bounded_power=std::clamp(power,1,80);
        for(int i=0;i<5;++i){
            const auto &cell=at(bar,62+2*i,20);const int remaining=bounded_power-i*16;
            if(bounded_power==80)check(cell.code==0x40+i&&cell.color==white);
            else {check(cell.code==(remaining>0?0x30+std::min(remaining,16)-1:blank));check(cell.color==power_colors[power_to_level[bounded_power>>2]]);}
        }
    }
    const std::array<std::uint32_t,5> colors={green,cyan,magenta,red,red};
    for(int rank=0;rank<5;++rank){
        state.difficulty=rank;const auto text=cells(state);
        for(int i=0;i<7;++i)check(at(text,57+i*2,22).code==::glyph_code(ranks[rank][i])&&at(text,57+i*2,22).color==colors[rank]);
    }
    state.difficulty=1;const auto normal=cells(state);
    check(at(normal,57,22).code==0xb6&&at(normal,63,22).code==0xb7);
}
void test_gameplay_tram(){
    using namespace soew;
    constexpr std::uint32_t tone=0x888888u;
    std::vector<std::uint32_t> pixels(640*400,tone);
    mask_gameplay_tram(pixels);
    for(int y=0;y<400;++y)for(int x=0;x<640;++x){
        const bool visible=x>=32&&x<416&&y>=16&&y<384;
        check(pixels[y*640+x]==(visible?tone:original_hud::black));
    }
    draw_original_hud(pixels,fixture_font(),HudState{});
    check(pixels[96*640+448]==original_hud::white);
    check(pixels[96*640+576]==original_hud::black);
    check(pixels[319*640+64]==tone&&pixels[320*640+64]==tone);
}
void test_draw(){
    using namespace soew;using namespace original_hud;
    auto font=fixture_font();const HudState state;
    constexpr std::uint32_t sentinel=0xff123456u;
    std::vector<std::uint32_t> pixels(640*400,sentinel);draw_original_hud(pixels,font,state);
    for(int y=0;y<400;++y)for(int x=0;x<640;++x)if(x<left||x>=right)check(pixels[y*640+x]==sentinel);
    check(pixels[0*640+left]==black);
    check(pixels[5*16*640+61*8]==yellow);
    check(pixels[6*16*640+56*8]==white);
    check(pixels[20*16*640+62*8]==white);
    check(pixels[17*16*640+62*8]==white&&pixels[17*16*640+66*8]==black);
    auto bad_pixels=std::vector<std::uint32_t>(1,sentinel);bool threw=false;
    try{draw_original_hud(bad_pixels,font,state);}catch(const std::runtime_error &){threw=true;}
    check(threw&&bad_pixels[0]==sentinel);
    font.header.last_code=0;pixels.assign(640*400,sentinel);threw=false;
    try{draw_original_hud(pixels,font,state);}catch(const std::runtime_error &){threw=true;}
    check(threw&&std::all_of(pixels.begin(),pixels.end(),[&](auto p){return p==sentinel;}));
}
}
int main(){
    try{test_layout();test_draw();test_gameplay_tram();std::cout<<"hud_checks="<<checks<<" failures=0\n";return 0;}
    catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}
}
