#pragma once
#include "dialogue_core.hpp"
#include "locale.hpp"
#include "opening.hpp"
#include "score_core.hpp"
#include <array>
#include <algorithm>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
namespace soew {
namespace stage_overlay {
// PC-98 text RAM cells are 8x16; MIKOFT gaiji occupy two cells.
inline constexpr int cell_w=8,cell_h=16;
inline constexpr int playfield_left=4,playfield_right=52,playfield_center=28;
inline constexpr unsigned opening_frames=160,bgm_timer_end=0xa0;
inline constexpr int stage_row=12,subtitle_row=13,bgm_row=23,bgm_note_column=24,bgm_title_column=26;
inline constexpr std::array<unsigned,5> stage_gaiji={0xbc,0xbd,0xaa,0xb0,0xae};
inline constexpr std::array<unsigned,12> extra_gaiji={0xae,0xc1,0xbd,0xbb,0xaa,0xcf,0xcf,0xbc,0xbd,0xaa,0xb0,0xae};
inline constexpr std::array<unsigned,5> bonus_gaiji={0xab,0xb8,0xb6,0xbe,0xbc};
inline constexpr unsigned note_gaiji=0xd8,digit_zero_gaiji=0xa0;
// th02_main.asm:26046–26113. Retain the printed padding and centering table.
// All six originals use Shift-JIS 81 60 (JIS 21 41), a full-width PC-98 text glyph.
inline constexpr wchar_t subtitle_wave=L'\uff5e';
inline constexpr int subtitle_wave_y=6;
inline constexpr std::array<std::uint16_t,3> subtitle_wave_bits={0x1e04,0x318c,0x2078}; // MSB first
inline void paint_subtitle_wave(std::span<std::uint32_t> pixels,int stride,int x,int y,std::uint32_t color){
    for(unsigned row=0;row<subtitle_wave_bits.size();++row)
        for(int col=0;col<2*cell_w;++col)
            if(subtitle_wave_bits[row]&(0x8000u>>col))pixels[(y+subtitle_wave_y+row)*stride+x+col]=color;
}
inline constexpr std::array<std::wstring_view,6> subtitles={
    L"紫野　～ Purple Dawn",L"雷獣　～Midnight Rainstorm",L"紅夢　～ Scarlet Dream",
    L"霊魔　～Revengeful Ghost",L"封魔　～東方封魔録 and ...",L"異空　～for Lunatic Gamers"};
inline constexpr std::array<int,6> subtitle_half_lengths={10,13,11,12,13,13};
// Azinn-style romanization of the original heading, not a patch transcript.
// The Stage 2 spelling follows the supplied reference image.
inline constexpr std::array<std::wstring_view,6> subtitles_en={
    L"Murasakino ～ Purple Dawn",L"Raijuu ～ Midnight Rainstorm",L"Koumu ～ Scarlet Dream",
    L"Reima ～ Revengeful Ghost",L"Fuuma ～ Story of Eastern Wonderland and ...",L"Ikuu ～ For Lunatic Gamers"};
inline constexpr std::array<std::wstring_view,6> subtitles_hans={
    L"紫野 ～ 紫色黎明",L"雷兽 ～ 午夜暴雨",L"红梦 ～ 绯红之梦",
    L"灵魔 ～ 复仇之魂",L"封魔 ～ 东方封魔录与……",L"异空 ～ 献给疯狂的玩家"};
inline constexpr std::array<std::wstring_view,6> subtitles_hant={
    L"紫野 ～ 紫色黎明",L"雷獸 ～ 午夜暴雨",L"紅夢 ～ 緋紅之夢",
    L"靈魔 ～ 復仇之魂",L"封魔 ～ 東方封魔錄與……",L"異空 ～ 獻給瘋狂的玩家"};
constexpr std::wstring_view subtitle(Language language,std::size_t stage){
    return (language==Language::ChineseSimplified?subtitles_hans:language==Language::ChineseTraditional?subtitles_hant:
        language==Language::English?subtitles_en:subtitles).at(stage);
}
struct SubtitleLayout { int text_x,wave_x;std::size_t wave_at; };
inline SubtitleLayout layout_subtitle(std::wstring_view text,std::size_t stage,bool original_layout,const auto &width){
    const int x=playfield_center*cell_w-(original_layout?subtitle_half_lengths[stage]*cell_w:width(text)/2);
    const auto wave=text.find(subtitle_wave);
    return {x,x+width(text.substr(0,wave)),wave};
}
inline constexpr std::array<std::wstring_view,12> bgm_titles={
    L"　 博麗　～Eastern Wind ",L"　  She's in a temper!! ",L"　   End of Daylight　  ",L"   　 やみのちから　　  ",
    L"　　　　　幻夢界　　　　",L" 　　　死を賭して　　　 ",L"ひもろぎ、むらさきにもえ",L"   　 恋色マジック 　   ",
    L"　東方封魔録　～幽幻乱舞",L"   Complete Darkness    ",L"   　 エキストララブ    ",L"  戦車むすめのみるゆめ  "};
constexpr bool show_bgm(std::uint64_t elapsed){return elapsed>0&&elapsed<bgm_timer_end-1;}
// th02/main/stage/bonus.cpp; the same title and expression are used in Extra.
inline constexpr std::array<std::wstring_view,6> regular_labels={L" 難易度",L"ステージ",L"ボム",L"ミス",L"靈撃初期数",L"靈夢初期数"};
inline constexpr std::array<std::wstring_view,4> extra_labels={L"クリア",L"ミス回数",L"ボム回数",L"クリアタイム"};
inline constexpr std::array<std::wstring_view,6> regular_labels_hans={L"难度",L"关卡表现",L"灵击次数",L"失误次数",L"初始灵击",L"初始残机"};
inline constexpr std::array<std::wstring_view,6> regular_labels_hant={L"難度",L"關卡表現",L"靈擊次數",L"失誤次數",L"初始靈擊",L"初始殘機"};
inline constexpr std::array<std::wstring_view,4> extra_labels_hans={L"通关",L"失误次数",L"灵击次数",L"通关时间"};
inline constexpr std::array<std::wstring_view,4> extra_labels_hant={L"通關",L"失誤次數",L"靈擊次數",L"通關時間"};
inline constexpr int title_column=24,title_row=4,label_column=8,value_column=22,value_digits=5;
inline constexpr int first_row=6,formula_row=18,point_column=6,point_value_column=10,point_digits=3,
    times_column=18,sum_column=22,equals_column=34;
constexpr int result_column(int digits){return playfield_right-2-digits*2;}
}
// ReC98 th02_main.asm:982–995,1248–1251: EYE.PI at (96,144) and a
// minimum of 100 retraces. Only the host-only entry wait uses this clock.
struct StageIntro {
    static constexpr int x=96,y=144,width=256,height=80;
    static constexpr unsigned wait_frames=100,reveal_frames=42,total_frames=wait_frames+reveal_frames;
    PresentationClock clock;
    unsigned elapsed=total_frames;
    bool active()const{return elapsed<total_frames;}
    bool showing_eye()const{return elapsed<wait_frames;}
    bool revealing()const{return active()&&!showing_eye();}
    unsigned reveal_frame()const{return elapsed-wait_frames;}
    void cancel(){elapsed=total_frames;}
    void arm(std::uint64_t now,bool enabled){elapsed=enabled?0:total_frames;clock.defer_batch(now);}
    void advance(std::uint64_t now,bool foreground,unsigned speed=1){
        if(active())elapsed=std::min(total_frames,elapsed+clock.advance(now,foreground)*speed);
    }
    static void paint(std::span<std::uint32_t> frame,std::span<const std::uint32_t> eye){
        if(frame.size()!=640u*400u||eye.size()!=std::size_t(width*height))return;
        std::fill(frame.begin(),frame.end(),0);
        for(int row=0;row<height;++row)
            std::copy_n(eye.begin()+row*width,width,frame.begin()+(y+row)*640+x);
    }
};
// Original th02/main/stage/bonus.cpp; all public display rows are x10 units.
struct StageBonus {
    std::array<int,6> rows{};
    std::uint32_t subtotal=0,point_items=0,internal_bonus=0;
    static StageBonus regular(int rank,int playperf,unsigned used,unsigned misses,unsigned start_bombs,unsigned start_lives,unsigned points){
        if(rank<0||rank>3||start_bombs>3||start_lives>4)throw std::runtime_error("Invalid regular stage bonus context");
        StageBonus b;b.rows={rank*2000,(playperf+16)*200,std::max(2500-int(used)*500,0),std::max(3000-int(misses)*1000,0),(4-int(start_bombs))*800,(4-int(start_lives))*1000};
        int sum=0;for(int v:b.rows)sum+=v/10;b.subtotal=unsigned(std::min(sum,25600));b.point_items=points;b.internal_bonus=b.subtotal*points;return b;
    }
};
// th02/main/stage/bonus.cpp:48–119,160–192. Rows are displayed x10.
struct ExtraBonus {
    std::array<unsigned,4> rows{};
    std::uint32_t subtotal=0,point_items=0,internal_bonus=0;
    static ExtraBonus calculate(unsigned misses,unsigned bombs,std::uint32_t sigma_frames,unsigned points){
        ExtraBonus bonus;
        const auto stock_row=[](unsigned used){return used<5?20000u-4000u*used:0u;};
        bonus.rows={10000,stock_row(misses),stock_row(bombs),sigma_frames<20000?(20000u-sigma_frames)/10u*10u:0u};
        for(const auto row:bonus.rows)bonus.subtotal+=row/10u;
        bonus.point_items=points;
        bonus.internal_bonus=bonus.subtotal*bonus.point_items;
        return bonus;
    }
};

// Separate terminal controller: never feeds StageTransition::Ready.
struct ExtraSettlement {
    enum class Phase { Inactive, BonusAck, FinalAck, Clear, GameOver, Lost };
    Phase phase=Phase::Inactive;
    ExtraBonus bonus;
    StageDialogue::KeyDelay gate;
    bool begin(ExtraBonus value,bool key){
        if(phase!=Phase::Inactive)return false;
        bonus=value;phase=Phase::BonusAck;gate.reset(key);return true;
    }
    unsigned loss_frames=0;
    bool lose(bool key){
        if(phase!=Phase::Inactive)return false;
        phase=Phase::GameOver;gate.reset(key);return true;
    }
    void tick(bool key){
        if(phase==Phase::GameOver){
            if(loss_frames<30){++loss_frames;gate.reset(key);}
            else if(gate.tick(key))phase=Phase::Lost;
            return;
        }
        if(phase==Phase::BonusAck&&gate.tick(key)){phase=Phase::FinalAck;gate.reset(key);}
        else if(phase==Phase::FinalAck&&gate.tick(key))phase=Phase::Clear;
    }
    bool terminal()const{return phase==Phase::Clear||phase==Phase::Lost;}
    bool showing_bonus()const{return phase==Phase::BonusAck||phase==Phase::FinalAck;}
};

struct StageTransition {
    enum class Phase {Battle,Postboss,BonusAck,ExtraAck,Leave,LoadWait,Ready,Entry};
    Phase phase=Phase::Battle;
    StageBonus bonus;
    StageDialogue::KeyDelay gate;
    bool opening_caption=true;
    std::optional<std::uint64_t> boss_music_frame;
    unsigned frame=0;
    static constexpr unsigned fade_frames=42; // (384/16/2 + 9 cels)*2
    bool active()const{return phase!=Phase::Battle;}
    void bonus_ready(bool key){phase=Phase::BonusAck;gate.reset(key);frame=0;}
    void tick(bool key){
        switch(phase){
        case Phase::BonusAck:if(gate.tick(key)){phase=Phase::ExtraAck;gate.reset(key);}break;
        case Phase::ExtraAck:if(gate.tick(key)){phase=Phase::Leave;frame=0;}break;
        case Phase::Leave:if(++frame>=fade_frames){phase=Phase::LoadWait;frame=0;}break;
        case Phase::LoadWait:if(++frame>=100){phase=Phase::Ready;frame=0;}break;
        case Phase::Entry:if(++frame>=fade_frames){phase=Phase::Battle;frame=0;}break;
        default:break;
        }
    }
};
}
