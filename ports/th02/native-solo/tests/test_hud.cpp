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
void test_coop_layout(){
    namespace hud=soew::coop_hud;
    namespace solo=soew::original_hud;
    check(hud::first_player_y>(solo::score_row+1)*16&&hud::first_player_y<128);
    check(hud::player_end(2)<hud::encounter_top(2)&&hud::player_end(3)<hud::encounter_top(3));
    check(hud::bottom==solo::rank_row*16&&hud::bottom-hud::encounter_top(3)>=86);
    check(hud::left+17<hud::option_x&&hud::option_x<504);
    check(hud::option_x+16<hud::rescue_right-17&&hud::rescue_right<hud::ghost_x);
    check(hud::power_width>80&&hud::power_x+hud::power_width<hud::power_value_right-29);
}
void test_coop_power(){
    namespace hud=soew::coop_hud;
    constexpr std::uint32_t black=0xff000000u,ink=0xff66bbffu;
    constexpr int y=hud::first_player_y+2*hud::row_height;
    for(const unsigned power:{0u,1u,37u,80u}){
        std::vector<std::uint32_t> pixels(640*400,black);
        hud::draw_power_gauge(pixels,power,y,ink);
        const auto at=[&](int x,int dy){return pixels[(y+dy)*640+x];};
        check(at(hud::power_x-1,5)==hud::power_edge&&at(hud::power_x+hud::power_width,5)==hud::power_edge);
        for(int dx=0;dx<hud::power_width;++dx){
            const int x=hud::power_x+dx;
            check(at(x,4)==hud::power_edge&&at(x,11)==hud::power_edge);
            check(at(x,5)==(unsigned(dx)<power*hud::power_width/hud::power_max?ink:hud::power_empty));
        }
        using namespace std::string_view_literals;
        const auto value=power==0?"0/80"sv:power==1?"1/80"sv:power==37?"37/80"sv:"80/80"sv;
        constexpr std::array<unsigned,10> digit_tops={14,4,14,30,2,31,14,31,14,14};
        const int start=hud::power_value_right-int(value.size())*6+1;
        for(int i=0;i<int(value.size());++i){
            const auto top=value[i]=='/'?1:digit_tops[value[i]-'0'];
            for(int dx=0;dx<5;++dx)check(at(start+6*i+dx,5)==(top&(16u>>dx)?hud::detail_ink:black));
        }
    }
}
void test_practice_keys(){
    namespace keys=soew::practice_keys;
    using namespace soew;
    check(keys::displayed(UpLeft)==(Up|Left)&&keys::displayed(UpRight)==(Up|Right));
    check(keys::displayed(DownLeft)==(Down|Left)&&keys::displayed(DownRight)==(Down|Right));
    check(keys::displayed(Up|Right|Focus|Shot|Bomb)==(Up|Right|Focus|Shot|Bomb));
    check(keys::displayed(UpLeft|DownRight|Down)==Down);
    check(keys::displayed(Cancel|OK|Q)==0);
    std::vector<std::uint32_t> original(640*400,0xff305070u);
    mask_gameplay_tram(original);
    draw_original_hud(original,fixture_font(),HudState{});
    auto idle=original,active=original;
    keys::draw(idle,0);keys::draw(active,DownRight|Focus|Shot);
    check(idle!=original&&active!=idle);
    for(int y=0;y<400;++y)for(int x=0;x<640;++x){
        const auto p=y*640+x;
        if(x<433||x>=573||y<6||y>=46)check(idle[p]==original[p]&&active[p]==original[p]);
    }
    for(const auto &key:keys::keys){
        const auto p=key.y*640+key.x;
        const bool pressed=key.input&(Down|Right|Focus|Shot);
        check(idle[p]==original[p]&&active[p]==original[p]); // clipped corner
        check(idle[p+2]==keys::white&&active[p+2]==keys::white); // outline stays white
        check(idle[p+3*640+3]==keys::white&&active[p+3*640+3]==(pressed?keys::purple:keys::white));
    }
    auto arrows=original;
    keys::draw(arrows,Up|Down|Left|Right);
    check(arrows[11*640+539]==keys::white&&arrows[36*640+539]==keys::white); // up and down tips
    check(arrows[33*640+516]==keys::white&&arrows[33*640+562]==keys::white); // left and right tips
}
}
int main(){
    try{test_layout();test_draw();test_gameplay_tram();test_coop_layout();test_coop_power();test_practice_keys();std::cout<<"hud_checks="<<checks<<" failures=0\n";return 0;}
    catch(const std::exception &error){std::cerr<<error.what()<<'\n';return 1;}
}
