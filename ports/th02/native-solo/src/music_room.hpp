#pragma once
#include <array>
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include "locale.hpp"
namespace soew {
inline constexpr std::array<std::wstring_view,15> music_titles={
    L"東方封魔録　～浄土曼荼羅",L"　 博麗　～Eastern Wind ",L"　   End of Daylight　  ",L"　　　　　幻夢界　　　　",L"ひもろぎ、むらさきにもえ",
    L"　東方封魔録　～幽幻乱舞",L"　  She's in a temper!! ",L"　  　 やみのちから　　 ",L"　　　　死を賭して　　　",L" 　  　 恋色マジック 　 ",
    L"    Complete Darkness   ",L"     　　遠野の森　　　 ",L"    昔話わんだーらんど  ",L"   　 エキストララブ    ",L"   戦車むすめのみるゆめ "};
// The English patch leaves some track names in Japanese; these are port UI labels.
inline constexpr std::array<std::wstring_view,15> music_titles_en={
    L"SoEW ~ Pure Land Mandala",L"Hakurei ~ Eastern Wind",L"End of Daylight",L"Phantasm World",L"Himorogi, Burn in Violet",
    L"SoEW ~ Ghostly Dance",L"She's in a Temper!!",L"Power of Darkness",L"Bet Your Life",L"Love-Colored Magic",
    L"Complete Darkness",L"Forest of Tono",L"Folk Tale Wonderland",L"Extra Love",L"Tank Girl's Dream"};
inline constexpr std::array<std::wstring_view,15> music_titles_hans={
    L"东方封魔录 ～ 净土曼荼罗",L"博丽 ～ Eastern Wind",L"End of Daylight",L"幻梦界",L"神篱，燃作紫焰",
    L"东方封魔录 ～ 幽幻乱舞",L"She's in a Temper!!",L"黑暗之力",L"赌上性命",L"恋色魔法",
    L"Complete Darkness",L"远野之森",L"昔话仙境",L"Extra Love",L"战车少女的梦"};
inline constexpr std::array<std::wstring_view,15> music_titles_hant={
    L"東方封魔錄 ～ 淨土曼荼羅",L"博麗 ～ Eastern Wind",L"End of Daylight",L"幻夢界",L"神籬，燃作紫焰",
    L"東方封魔錄 ～ 幽幻亂舞",L"She's in a Temper!!",L"黑暗之力",L"賭上性命",L"戀色魔法",
    L"Complete Darkness",L"遠野之森",L"昔話仙境",L"Extra Love",L"戰車少女的夢"};
inline constexpr std::wstring_view music_title(unsigned track,Language lang){
    switch(lang){
    case Language::English:return music_titles_en.at(track);
    case Language::ChineseSimplified:return music_titles_hans.at(track);
    case Language::ChineseTraditional:return music_titles_hant.at(track);
    default:return music_titles.at(track);
    }
}
inline constexpr std::array<std::string_view,15> music_files={"OP.M","STAGE0.M","STAGE1.M","STAGE2.M","STAGE3.M","STAGE4.M","BOSS1.M","BOSS4.M","BOSS2.M","BOSS3.M","MIMA.M","END1.M","ENDING.M","STAGE5.M","BOSS5.M"};
// Stage0..5 song slots in the existing music room list. Boss1 begins BEFORE
// preboss dialogue in rika_init. Stage 4 switches just before Marisa's dialogue.
inline int gameplay_music_track(int stage,bool preboss,bool rika,bool marisa=false){
    if(stage<0||stage>5)throw std::runtime_error("Invalid music stage");
    if(stage==0&&(preboss||rika))return 6;
    if(stage==3&&marisa)return 9;
    return stage==5?13:stage+1;
}
inline bool gameplay_music_paused(bool foreground,bool manual_pause,bool gameover,bool boss_done){return !foreground||manual_pause||gameover||boss_done;}
// Preview shortcut acts on the visible cursor, never the previous comment.
// -1 means stop: same active track or the non-song Quit/blank row.
inline int preview_f4_target(int selected,int active_track,bool active){
    if(selected<0||selected>=static_cast<int>(music_files.size()))return -1;
    return active&&active_track==selected?-1:selected;
}
using MusicComments=std::array<std::array<std::string,20>,15>;
// Original MUSIC.TXT: 15*20*(40 bytes+CRLF). The English patch adds two unused spaces.
// Preserve fixed-width CP932 bytes. Unicode decoding is a separate host step.
inline MusicComments decode_music_comments(std::span<const std::byte> raw) {
    if(raw.size()!=12600 && (raw.size()!=12602 || raw[12600]!=std::byte{32} || raw[12601]!=std::byte{32}))throw std::runtime_error("Unsupported original MUSIC.TXT size/trailer");
    MusicComments result{};
    for(unsigned track=0;track<15;++track)for(unsigned line=0;line<20;++line){
        const auto at=(track*20+line)*42;
        if(raw[at+40]!=std::byte{13} || raw[at+41]!=std::byte{10})throw std::runtime_error("Music comment line separator invalid");
        auto &text=result[track][line];
        for(unsigned j=0;j<40;++j){const auto ch=std::to_integer<unsigned char>(raw[at+j]);if(ch<32)throw std::runtime_error("Music comment contains control byte");text.push_back(static_cast<char>(ch));}
    }
    return result;
}
inline int music_move(int selected,int direction){
    selected+=direction;
    if(selected<0)selected=16;
    if(selected>16)selected=0;
    if(selected==15)selected+=direction;
    return selected;
}
}
