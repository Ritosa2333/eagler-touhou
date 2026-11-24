#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
namespace soew {
namespace score_gaiji {
// Original TH02 MIKOFT glyph indices (th02/gaiji/{gameover,hiscore}[data].asm).
inline constexpr std::uint8_t zero=0xa0, bullet=0xda, space=0xcf, all=0xf0;
inline constexpr std::array<std::uint8_t,8> title{0xb1,0xb2,0xcf,0xbc,0xac,0xb8,0xbb,0xae};
inline constexpr std::array<std::uint8_t,4> name{0xb6,0xaa,0xb7,0xae};
inline constexpr std::array<std::uint8_t,5> point{0xb9,0xb8,0xb2,0xb6,0xbd};
inline constexpr std::array<std::uint8_t,2> stage{0xbc,0xbd};
inline constexpr std::array<std::uint8_t,8> game_over{0xb0,0xaa,0xb7,0xae,0xb8,0xbf,0xae,0xbb};
inline constexpr std::array<std::uint8_t,9> continue_prompt{0xac,0xb8,0xb6,0xbd,0xb2,0xb6,0xbe,0xae,0xdd};
inline constexpr std::array<std::uint8_t,3> yes{0xc2,0xae,0xbc};
inline constexpr std::array<std::uint8_t,2> no{0xb6,0xb8};
inline constexpr std::array<std::uint8_t,6> credit{0xac,0xbb,0xae,0xad,0xb2,0xbd};
inline constexpr std::array<std::array<std::uint8_t,17>,3> alphabet{{
    {0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb7,0xb6,0xb8,0xb9,0xba},
    {0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0xda,0xdb,0xdc,0xdd,0xde,0xe0,2,3},
    {0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7}
}};
inline constexpr bool name_glyph(std::uint8_t code){
    if(code==space)return true;
    for(unsigned r=0;r<alphabet.size();++r)
        for(unsigned c=0;c<(r==2?13u:17u);++c)if(alphabet[r][c]==code)return true;
    return false;
}
// Ten convergence frames, then a fixed 30-frame hold (th02_main.asm:2177–2222).
inline constexpr unsigned fade_frames=42, converge_frames=10, hold_frames=30;
inline constexpr unsigned animation_frames=2*fade_frames+converge_frames+hold_frames;
struct GameOverPositions {int top,bottom,left,right;};
inline constexpr GameOverPositions game_over_positions(unsigned frame){
    const int n=frame>=converge_frames?int(converge_frames-1):int(frame);
    return {21-n,3+n,9+n,27-n}; // TRAM columns/rows, with 8x16 cells.
}
}
struct ScoreSignature {
    enum class Key {Up,Down,Left,Right,Select,Bomb,Space,Backspace};
    std::array<std::uint8_t,6> cells{score_gaiji::bullet,score_gaiji::bullet,score_gaiji::bullet,
        score_gaiji::bullet,score_gaiji::bullet,score_gaiji::bullet};
    unsigned row=0,column=0,position=0;
    bool update(Key key){
        const auto forward=[&]{if(position+1<cells.size())++position;};
        const auto back=[&]{if(position)--position;};
        switch(key){
        case Key::Up:row=(row+2)%3;break;
        case Key::Down:row=(row+1)%3;break;
        case Key::Left:column=(column+16)%17;break;
        case Key::Right:column=(column+1)%17;break;
        case Key::Bomb:cells[position]=score_gaiji::space;back();break;
        case Key::Space:cells[position]=score_gaiji::space;forward();break;
        case Key::Backspace:back();cells[position]=score_gaiji::space;break;
        case Key::Select:
            if(row!=2||column<13){
                cells[position]=score_gaiji::alphabet[row][column];
                if(position==cells.size()-1){row=2;column=16;}
                forward();
            }else if(column==13){cells[position]=score_gaiji::space;forward();}
            else if(column==14){back();cells[position]=score_gaiji::space;}
            else if(column==15)forward();
            else return true;
            break;
        }
        return false;
    }
    std::string name() const{return {reinterpret_cast<const char*>(cells.data()),cells.size()};}
};
struct ScoreEntry {
    std::uint32_t score=0;
    std::array<std::uint8_t,6> name{};
    std::uint8_t stage=0,shot=0,day=0,month=0;
    std::uint16_t year=0;
};
struct ScorePage {std::uint16_t cleared=0;std::array<ScoreEntry,10> entries;};
using Scores=std::array<ScorePage,5>;
struct ClearStatus {
    std::array<bool,3> main{},extra{};
    bool unlocked=false;
};
// Combine per-shot clears, not the input unlock flags: neither source needs
// to contain all three main-game clears by itself.
inline constexpr ClearStatus merge_clear_status(ClearStatus legacy, const std::array<bool,3>& native_main,
                                                 const std::array<bool,3>& native_extra) {
    legacy.unlocked=true;
    for(unsigned i=0;i<3;++i){
        legacy.main[i]=legacy.main[i]||native_main[i];
        legacy.extra[i]=legacy.extra[i]||native_extra[i];
        legacy.unlocked=legacy.unlocked&&legacy.main[i];
    }
    return legacy;
}
inline constexpr ClearStatus merge_clear_status(ClearStatus legacy, const ClearStatus& native) {
    return merge_clear_status(legacy,native.main,native.extra);
}
// Original layout stores shot-specific main clears in pages 0..2, Extra bits
// in page 3 (Lunatic), NOT page 4. This is intentional source behavior.
inline ClearStatus clear_status(const Scores &scores) {
    constexpr std::array<std::uint16_t,3> magic={318,118,218};
    ClearStatus result{};
    for(unsigned i=0;i<3;++i){result.main[i]=scores[i].cleared==magic[i];result.extra[i]=(scores[3].cleared&(1u<<i))!=0;}
    return merge_clear_status({},result);
}
inline std::uint32_t le(std::span<const std::byte> b,std::size_t at,unsigned n) {
    if(at+n>b.size())throw std::runtime_error("Truncated score field");
    std::uint32_t result=0;for(unsigned i=0;i<n;++i)result|=std::to_integer<std::uint32_t>(b[at+i])<<(i*8);return result;
}
// Strict original-only consumer, not Christian patch's extended format.
// No repair/recreation/deletion path: invalid data is reported, never rewritten.
inline Scores decode_scores(std::span<const std::byte> raw) {
    if(raw.size()!=910)throw std::runtime_error("Original HUUHI.DAT must be 910 bytes");
    Scores result{};
    for(unsigned page=0;page<5;++page) {
        std::array<std::byte,178> decoded{};std::uint32_t section=0;
        for(unsigned i=0;i<178;++i){const auto value=std::uint8_t(std::to_integer<unsigned>(raw[page*182+i])-0x12);decoded[i]=std::byte(value);section+=value;}
        if(section!=le(raw,page*182+178,4))throw std::runtime_error("HUUHI section checksum failed");
        auto &p=result[page];p.cleared=static_cast<std::uint16_t>(le(decoded,0,2));
        std::uint32_t sum=0;unsigned name_sum=0,stage_sum=0;
        for(unsigned i=0;i<10;++i) {
            auto &e=p.entries[i];e.score=le(decoded,2+i*4,4);sum+=e.score;
            for(unsigned j=0;j<6;++j)e.name[j]=std::to_integer<std::uint8_t>(decoded[46+i*7+j]);
            if(decoded[46+i*7+6]!=std::byte{0})throw std::runtime_error("Score name not terminated");
            name_sum+=e.name[0];e.stage=std::to_integer<std::uint8_t>(decoded[117+i]);stage_sum+=e.stage;
            e.year=static_cast<std::uint16_t>(le(decoded,128+i*4,2));e.day=std::to_integer<std::uint8_t>(decoded[130+i*4]);e.month=std::to_integer<std::uint8_t>(decoded[131+i*4]);
            e.shot=std::to_integer<std::uint8_t>(decoded[168+i]);
            if(e.score>99999999 || (e.stage>9&&e.stage!=127) || e.shot>2 || e.month<1 || e.month>12 || e.day<1 || e.day>31)throw std::runtime_error("Unsupported score record value");
        }
        if(sum!=le(decoded,42,4)||(name_sum&255)!=le(decoded,116,1)||(stage_sum&255)!=le(decoded,127,1))throw std::runtime_error("HUUHI record checksum failed");
    }
    return result;
}
}
