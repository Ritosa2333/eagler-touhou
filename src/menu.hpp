#pragma once
#include "th03/assets/decode.hpp"
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <string>
#include "japanese.hpp"
#include "music_room.hpp"
#include "collision_display.hpp"
#include "locale.hpp"

// Independent host-side model of ReC98 th02/op_01.cpp:500-617.
// No attract demo or game launch is implemented yet.
struct Menu {
    static constexpr int practice_entry=6;
    int selected = 0;
    bool unlocked = false;
    bool allowed = false;
    unsigned held = 0;
    bool options = false;
    bool shot_screen=false;
    bool extra_mode=false;
    bool music_room=false;
    int music_selected=0,music_comment=0;
    std::array<bool,3> cleared_main{},cleared_extra{};
    struct LaunchRules {int rank,stage,lives,bombs,shot;};
    LaunchRules rules()const {return {extra_mode?4:rank,extra_mode?5:0,extra_mode?2:lives,extra_mode?1:bombs,shot};}
    int shot=1;
    bool shot_confirmed=false;
    void begin_shot_select(int initial_shot,bool extra=false){
        shot=initial_shot;extra_mode=extra;shot_screen=true;shot_confirmed=false;
    }
    void begin_practice_shot_select(int initial_shot){
        selected=practice_entry;begin_shot_select(initial_shot);allowed=false;
    }
    bool practice_shot_select()const{return shot_screen&&selected==practice_entry;}
    int rank=1, music=1, lives=2, bombs=3;
    bool reduced=false;
    bool show_more_info=false,fullscreen=false,stage_intro=true,smooth_background=true;
    soew::HitboxMode hitboxes=soew::HitboxMode::Off;
    bool autofire=false,fast_autofire=true; // Host-only assist; old ON setting used the 2+2 waveform.
    soew::Language language=soew::Language::Japanese;

    enum class Option { Rank, Music, Lives, Bombs, Autofire, Effects, MoreInfo, Fullscreen, Hitboxes, Language, StageIntro, SmoothBackground, Reset, Back, Empty };
    static constexpr std::array<std::array<Option,2>,7> option_rows={{
        {Option::Rank,Option::Effects}, {Option::Music,Option::MoreInfo}, {Option::Lives,Option::Fullscreen},
        {Option::Bombs,Option::Hitboxes}, {Option::Autofire,Option::Language}, {Option::StageIntro,Option::SmoothBackground}, {Option::Reset,Option::Back}
    }};
    unsigned option_row=0,option_column=0;
    Option option()const{return option_rows[option_row][option_column];}
    int option_value(Option item)const {
        switch(item){
        case Option::Rank: return rank;
        case Option::Music: return music;
        case Option::Lives: return lives;
        case Option::Bombs: return bombs;
        case Option::Autofire: return !autofire?0:fast_autofire?2:1;
        case Option::Effects: return reduced;
        case Option::MoreInfo: return show_more_info;
        case Option::Fullscreen: return fullscreen;
        case Option::Hitboxes: return static_cast<int>(hitboxes);
        case Option::Language: return static_cast<int>(language);
        case Option::StageIntro: return stage_intro;
        case Option::SmoothBackground: return smooth_background;
        case Option::Reset: case Option::Back: case Option::Empty: return 0;
        }
        return 0;
    }
    static constexpr int option_count(Option item){
        switch(item){
        case Option::Lives: return 5;
        case Option::Rank: case Option::Bombs: case Option::Language: return 4;
        case Option::Music: case Option::Autofire: case Option::Hitboxes: return 3;
        case Option::Effects: case Option::MoreInfo: case Option::Fullscreen: case Option::StageIntro: case Option::SmoothBackground: return 2;
        case Option::Reset: case Option::Back: case Option::Empty: return 0;
        }
        return 0;
    }
    void set_option(Option item,int value){
        switch(item){
        case Option::Rank: rank=value;break;
        case Option::Music: music=value;break;
        case Option::Lives: lives=value;break;
        case Option::Bombs: bombs=value;break;
        case Option::Autofire: autofire=value!=0;fast_autofire=value!=1;break;
        case Option::Effects: reduced=value!=0;break;
        case Option::MoreInfo: show_more_info=value!=0;break;
        case Option::Fullscreen: fullscreen=value!=0;break;
        case Option::Hitboxes: hitboxes=static_cast<soew::HitboxMode>(value);break;
        case Option::Language: language=static_cast<soew::Language>(value);break;
        case Option::StageIntro: stage_intro=value!=0;break;
        case Option::SmoothBackground: smooth_background=value!=0;break;
        case Option::Reset: case Option::Back: case Option::Empty: break;
        }
    }
    enum : unsigned { up=1, down=2, confirm=4, cancel=8, left=16, right=32, tab=64 };
    static int ring(int value,int delta,int count) {return (value+delta+count)%count;}
    void adjust_option(Option item,int delta){
        if(const int count=option_count(item))set_option(item,ring(option_value(item),delta,count));
    }
    void update_options(unsigned input){
        const auto item=option();
        if((input&cancel)||((input&confirm)&&item==Option::Back)){options=false;selected=3;return;}
        if((input&confirm)&&item==Option::Reset){
            const Menu defaults;
            for(const auto &row:option_rows)for(const auto option:row)set_option(option,defaults.option_value(option));
            return;
        }
        if(const int delta=int(bool(input&down))-int(bool(input&up));delta){
            constexpr int rows=static_cast<int>(option_rows.size()),columns=static_cast<int>(option_rows.front().size());
            auto next=static_cast<int>(option_column)*rows+static_cast<int>(option_row);
            do{next=ring(next,delta,rows*columns);}while(option_rows[next%rows][next/rows]==Option::Empty);
            option_row=next%rows;option_column=next/rows;
        }else if(input&tab){
            option_column=1-option_column;
            if(option()==Option::Empty)--option_row;
        }else if(const int delta=int(bool(input&right))-int(bool(input&left));delta){
            adjust_option(item,delta);
        }
    }
    int update(unsigned input) {
        held = input;
        if(!input) { allowed = true; return -1; }
        if(!allowed) return -1;
        if(music_room){
            if(input&up)music_selected=soew::music_move(music_selected,-1);
            if(input&down)music_selected=soew::music_move(music_selected,1);
            if((input&cancel)||((input&confirm)&&music_selected==16)){music_room=false;selected=4;}
            else if(input&confirm)music_comment=music_selected;
            allowed=false;return -1;
        }
        if(shot_screen) {
            if(input & left) {shot=ring(shot,-1,3);shot_confirmed=false;}
            if(input & right) {shot=ring(shot,1,3);shot_confirmed=false;}
            if(input & confirm) shot_confirmed=true;
            // Diagnostic escape hatch; original shottype_menu has no cancel branch.
            if(input & cancel) {selected=practice_shot_select()?practice_entry:extra_mode?1:0;shot_screen=false;shot_confirmed=false;extra_mode=false;}
            allowed=false;return -1;
        }
        if(options){update_options(input);allowed=false;return -1;}
        if(input & up) move(-1);
        if(input & down) move(1);
        int action = -1;
        if(input & confirm) {
            if(selected==0 || (selected==1&&unlocked)) begin_shot_select(1,selected==1);
            else if(selected==1) { /* Locked entry cannot launch even via direct selection. */ }
            else if(selected==3) {options=true;option_row=option_column=0;}
            else if(selected==4) {music_room=true;music_selected=music_comment;}
            else action=selected;
        }
        if(input & cancel) action=5;
        allowed = false;
        return action;
    }
    void move(int delta) {
        constexpr std::array<int,8> order={0,1,6,7,2,3,4,5};
        int at=0;while(at<8&&order[at]!=selected)++at;if(at==8)at=0;
        do{at=ring(at,delta,8);selected=order[at];}while(!unlocked&&selected==1);
    }
};
inline int glyph_code(char c) {
    if(c == ' ') return 0xcf;
    if(c >= '0' && c <= '9') return 0xa0 + c - '0';
    if(c >= 'A' && c <= 'Z') {
        // TH02 MIKOFT swaps M/N (ReC98 th02/gaiji/from_2.h).
        if(c == 'M') return 0xb7;
        if(c == 'N') return 0xb6;
        return 0xaa + c - 'A';
    }
    throw std::runtime_error("Unsupported gaiji character");
}
inline void draw_glyph(std::vector<std::uint32_t> &pixels, const th03::assets::BfntSheet &font, int code, int x, int y, std::uint32_t color) {
    if(code < font.header.first_code || code > font.header.last_code) throw std::runtime_error("Gaiji code out of range");
    const auto &glyph = font.glyphs.at(code - font.header.first_code);
    const auto *mono = std::get_if<th03::assets::MonochromeBitmap1>(&glyph.pixels);
    if(!mono || mono->bits.size() != 32) throw std::runtime_error("Unsupported gaiji bitmap");
    for(int row=0; row<16; ++row) for(int col=0; col<16; ++col) {
        const int px=x+col, py=y+row;
        if(px>=0 && px<640 && py>=0 && py<400 && (mono->bits[row*2+col/8] & (0x80 >> (col%8)))) pixels[py*640+px]=color;
    }
}
inline void draw_label(std::vector<std::uint32_t> &pixels, const th03::assets::BfntSheet &font, std::string_view text, int x, int y, std::uint32_t color) {
    for(char c:text) { draw_glyph(pixels,font,glyph_code(c),x,y,color); x+=16; }
}
inline int menu_gaiji_code(wchar_t c) {
    if(c>=L'a'&&c<=L'z')c-=L'a'-L'A';
    if(c==L' '||(c>=L'0'&&c<=L'9')||(c>=L'A'&&c<=L'Z'))return glyph_code(static_cast<char>(c));
    switch(c){
    case L'.': return 0xdb;
    case L'!': return 0xdc;
    case L'?': return 0xdd;
    case L'←': return 0xe5;
    case L'→': return 0xe6;
    default: return -1;
    }
}
inline int menu_text_width(std::wstring_view text) {
    int width=0;
    for(const auto c:text)width+=menu_gaiji_code(c)>=0?16:jp::cell_width({&c,1});
    return width;
}
template<class Fallback>
void draw_menu_text(std::vector<std::uint32_t> &pixels,const th03::assets::BfntSheet &font,
    std::wstring_view text,int x,int y,std::uint32_t color,Fallback&& fallback){
    while(!text.empty()){
        if(const int code=menu_gaiji_code(text.front());code>=0){
            draw_glyph(pixels,font,code,x,y,color);x+=16;text.remove_prefix(1);
        }else{
            std::size_t end=1;
            while(end<text.size()&&menu_gaiji_code(text[end])<0)++end;
            const auto run=text.substr(0,end);
            fallback(run,x,y,color);x+=jp::cell_width(run);text.remove_prefix(end);
        }
    }
}
inline constexpr std::array<std::string_view,4> rank_names={"  EASY "," NORMAL","  HARD ","LUNATIC"};
namespace soew::text {
inline constexpr LocalizedText options_title{L"設定",L"Options",L"设置",L"設定"},
    options_controls{L"上下：移動　左右：変更　Tab：列切替　Z：実行　Esc：戻る",L"Up/Down: move  Left/Right: change  Tab: column  Z: act  Esc: back",L"上下：选择  左右：修改  Tab：换列  Z：确认  Esc：返回",L"上下：選擇  左右：修改  Tab：換欄  Z：確認  Esc：返回"},
    options_music_hint{L"音源を変えると現在の曲を新しい音源で再生します",L"Changing the source restarts the current track",L"切换音源后会从头播放当前曲目",L"切換音源後會從頭播放目前曲目"},
    options_effects_hint{L"一部軽減では一部の点滅や画面の揺れを抑えます",L"Reduced limits some flashes and screen shake",L"减弱部分闪光和画面震动",L"減弱部分閃光和畫面震動"};
inline constexpr std::array<LocalizedText,15> option_labels={{
    {L"難易度",L"Difficulty",L"难度",L"難度"}, {L"音楽",L"Music",L"音乐",L"音樂"}, {L"残機",L"Lives",L"初始命数",L"初始命數"}, {L"ボム",L"Bombs",L"初始灵击",L"初始靈擊"},
    {L"連射",L"Autofire",L"自动连射",L"自動連射"}, {L"演出",L"Effects",L"画面特效",L"畫面特效"}, {L"詳細情報",L"More Info",L"详细信息",L"詳細資訊"}, {L"全画面",L"Fullscreen",L"全屏",L"全螢幕"},
    {L"判定",L"Hitboxes",L"碰撞判定",L"碰撞判定"}, {L"言語",L"Language",L"语言",L"語言"}, {L"ステージ導入",L"Stage Intro",L"关卡开场",L"關卡開場"},
    {L"動きの補間",L"Smooth Motion",L"运动平滑",L"動態平滑"}, {L"初期設定に戻す",L"Restore Defaults",L"恢复默认设置",L"恢復預設設定"}, back, {L"",L""}
}};
inline constexpr std::array<const wchar_t*,4> language_names={L"日本語",L"English",L"简体中文",L"繁體中文"};
inline constexpr std::array<std::array<LocalizedText,3>,3> shot_description={{
    {{{L"",L"",L"不用阴阳玉之力",L"不用陰陽玉之力"},{L"",L"",L"射击范围广",L"射擊範圍廣"},{L"",L"",L"高机动型",L"高機動型"}}},
    {{{L"",L"",L"擅长灵击",L"擅長靈擊"},{L"",L"",L"性能均衡",L"性能均衡"},{L"",L"",L"防御型",L"防禦型"}}},
    {{{L"",L"",L"以阴阳玉作战",L"以陰陽玉作戰"},{L"",L"",L"火力出色",L"火力出色"},{L"",L"",L"攻击型",L"攻擊型"}}}
}};

inline std::wstring_view option_value(Language language,Menu::Option option,int value){
    using O=Menu::Option;
    constexpr std::array<LocalizedText,2> toggles={{{L"無効",L"Off",L"关闭",L"關閉"},{L"有効",L"On",L"开启",L"開啟"}}};
    constexpr std::array<LocalizedText,3> music={{toggles[0],{L"FM",L"FM",L"FM",L"FM"},{L"MIDI",L"MIDI",L"MIDI",L"MIDI"}}};
    constexpr std::array<LocalizedText,3> autofire={{toggles[0],{L"12 Hz",L"12 Hz",L"12 Hz",L"12 Hz"},{L"高速",L"Fast",L"高速",L"高速"}}};
    constexpr std::array<LocalizedText,2> effects={{{L"通常",L"Normal",L"正常",L"正常"},{L"一部軽減",L"Reduced",L"减弱",L"減弱"}}};
    constexpr std::array<LocalizedText,3> hitboxes={{toggles[0],{L"自機のみ",L"Player",L"仅自机",L"僅自機"},{L"すべて",L"All",L"全部",L"全部"}}};
    constexpr std::array<std::wstring_view,6> digits={L"0",L"1",L"2",L"3",L"4",L"5"};
    switch(option){
    case O::Rank: return difficulty[value](language);
    case O::Music: return music[value](language);
    case O::Lives: return digits[value+1];
    case O::Bombs: return digits[value];
    case O::Autofire: return autofire[value](language);
    case O::Effects: return effects[value](language);
    case O::MoreInfo: case O::Fullscreen: case O::StageIntro: case O::SmoothBackground: return toggles[value](language);
    case O::Hitboxes: return hitboxes[value](language);
    case O::Language: return language_names.at(value);
    case O::Reset: case O::Back: case O::Empty: return {};
    }
    return {};
}
}
inline void draw_title_credit(std::vector<std::uint32_t> &pixels,const th03::assets::BfntSheet &font){
    draw_glyph(pixels,font,0xdf,548,384,0);
    draw_label(pixels,font,"ZUN",564,384,0);
    draw_glyph(pixels,font,0xdf,544,380,0xffff00);
    draw_label(pixels,font,"ZUN",560,380,0xffff00);
}
inline void draw_menu(std::vector<std::uint32_t> &pixels, const th03::assets::BfntSheet &font, const Menu &menu) {
    constexpr std::array<std::string_view,8> labels={"START","EXTRA START","PRACTICE START","REPLAY","HISCORE","OPTION","MUSIC MODE","QUIT"};
    constexpr std::array<int,8> actions={0,1,6,7,2,3,4,5};
    // All languages share the original gaiji style and the English port labels.
    for(int i=0;i<8;++i) {
        const int x=320-static_cast<int>(labels[i].size())*8,y=224+i*16;
        draw_label(pixels,font,labels[i],x+4,y+4,0);
    }
    draw_label(pixels,font,"RANK",212,372,0);
    draw_label(pixels,font,rank_names[menu.rank],308,372,0);
    draw_title_credit(pixels,font);
    for(int i=0;i<8;++i) {
        const int x=320-static_cast<int>(labels[i].size())*8,y=224+i*16;
        const auto color= i==1 && !menu.unlocked ? 0x0000ffu : (menu.selected==actions[i] ? 0xffffffu : 0xffff00u);
        draw_label(pixels,font,labels[i],x,y,color);
    }
    draw_label(pixels,font,"RANK",208,368,0x00ff00);
    draw_label(pixels,font,rank_names[menu.rank],304,368,0x00ff00);
}
inline unsigned test_menu() {
    unsigned checks=0;
    const auto check=[&](bool v) {++checks;if(!v)throw std::runtime_error("Menu contract test failed");};
    Menu m;
    check(m.update(Menu::down)==-1 && m.selected==0);
    m.update(0);m.update(Menu::down);check(m.selected==6);
    m.update(Menu::down);check(m.selected==6);
    m.update(0);m.update(Menu::up);check(m.selected==0);
    m.update(0);m.update(Menu::up);check(m.selected==5);
    m.update(0);m.update(Menu::down);check(m.selected==0);
    for(bool unlocked:{false,true}) {
        m=Menu{};m.unlocked=unlocked;
        for(int i=0;i<30;++i) {m.update(0);m.update(Menu::down);check(m.selected>=0 && m.selected<=7 && (unlocked||m.selected!=1));}
    }
    m=Menu{};m.update(0);check(m.update(Menu::confirm)==-1 && m.shot_screen && m.shot==1);
    check(m.update(Menu::confirm)==-1);
    m.update(0);check(m.update(Menu::cancel)==-1 && !m.shot_screen);
    m.update(0);check(m.update(Menu::cancel)==5);
    check(glyph_code('M')==0xb7 && glyph_code('N')==0xb6 && glyph_code(' ')==0xcf);
    m=Menu{};m.update(0);m.update(Menu::up|Menu::down);check(m.selected==0);
    m=Menu{};m.update(0);check(m.update(Menu::down|Menu::confirm)==6&&m.selected==6);
    th03::assets::BfntSheet font{};font.header.first_code=0;font.header.last_code=0;
    th03::assets::MonochromeBitmap1 mono{};mono.bits.resize(32);mono.bits[0]=0x80;
    font.glyphs.push_back({0,mono});std::vector<std::uint32_t> image(640*400,0x123456);
    draw_glyph(image,font,0,0,0,0xffffff);check(image[0]==0xffffff && image[1]==0x123456 && image[640]==0x123456);
    draw_glyph(image,font,0,-16,-16,0);check(image[0]==0xffffff);
    font.header.last_code=255;
    font.glyphs.resize(256,{0,mono});
    std::get<th03::assets::MonochromeBitmap1>(font.glyphs[0xcf].pixels).bits[0]=0;
    m=Menu{};m.language=soew::Language::English;
    const auto render_title=[&]{
        std::vector<std::uint32_t> pixels(640*400,0x123456);
        draw_menu(pixels,font,m);
        return pixels;
    };
    for(bool unlocked:{false,true})for(int selected:{0,1,6,7,2,3,4,5}){
        m.unlocked=unlocked;m.selected=selected;m.language=soew::Language::English;
        const auto reference=render_title();
        check(reference[256*640+208]==(selected==6?0xffffffu:0xffff00u)); // Practice Start
        check(reference[272*640+272]==(selected==7?0xffffffu:0xffff00u)); // Replay
        check(reference[240*640+232]==(!unlocked?0x0000ffu:selected==1?0xffffffu:0xffff00u));
        check(reference[368*640+208]==0x00ff00u); // Rank
        check(reference[380*640+544]==0xffff00u); // Copyright
        for(const auto language:{soew::Language::Japanese,soew::Language::ChineseSimplified,soew::Language::ChineseTraditional}){
            m.language=language;check(render_title()==reference);
        }
    }
    using O=Menu::Option;
    m=Menu{};m.selected=3;
    const auto key=[&](unsigned input){m.update(0);m.update(input);};
    key(Menu::confirm);check(m.options&&m.selected==3&&m.option()==O::Rank);
    m.update(Menu::right);check(m.option()==O::Rank&&m.rank==1);
    key(Menu::up);check(m.option()==O::Back);
    key(Menu::down);check(m.option()==O::Rank);
    key(Menu::tab);check(m.option()==O::Effects);
    key(Menu::tab);check(m.option()==O::Rank);
    for(std::size_t column=0;column<Menu::option_rows.front().size();++column){
        for(const auto &row:Menu::option_rows){
            const auto item=row[column];
            if(item==O::Empty)continue;
            check(m.option()==item);
            const int value=m.option_value(item),count=Menu::option_count(item);
            if(count){
                key(Menu::confirm);check(m.option()==item&&m.option_value(item)==value);
                for(int step=1;step<=count;++step){key(Menu::right);check(m.option()==item&&m.option_value(item)==Menu::ring(value,step,count));}
                key(Menu::left);check(m.option_value(item)==Menu::ring(value,-1,count));
            }else{
                key(Menu::left);key(Menu::right);check(m.options&&m.option()==item);
            }
            key(Menu::down);
        }
    }
    check(m.option()==O::Rank);
    key(Menu::up);check(m.option()==O::Back);
    key(Menu::tab);check(m.option()==O::Reset);
    key(Menu::confirm);check(m.options&&m.option()==O::Reset);
    for(const auto &row:Menu::option_rows)for(const auto item:row)check(m.option_value(item)==Menu{}.option_value(item));
    key(Menu::tab);check(m.option()==O::Back);key(Menu::confirm);check(!m.options&&m.selected==3);
    key(Menu::confirm);check(m.option()==O::Rank);
    key(Menu::right);key(Menu::cancel);check(!m.options&&m.selected==3&&m.rank==2);
    m=Menu{};m.update(0);m.update(Menu::confirm);check(m.shot_screen && m.shot==1);
    for(int i=0;i<30;++i) {m.update(0);m.update(Menu::right);check(m.shot>=0&&m.shot<3);}
    check(m.shot==1);m.update(0);m.update(Menu::left);check(m.shot==0);
    m.update(0);m.update(Menu::left);check(m.shot==2);
    m.update(0);m.update(Menu::confirm);check(m.shot_confirmed);
    m.update(0);m.update(Menu::cancel);check(!m.shot_screen&&!m.shot_confirmed&&m.selected==0);
    m=Menu{};m.selected=1;m.update(0);m.update(Menu::confirm);check(!m.shot_screen&&!m.extra_mode);
    m.unlocked=true;m.rank=3;m.lives=4;m.bombs=3;m.update(0);m.update(Menu::confirm);
    check(m.shot_screen&&m.extra_mode&&m.shot==1);const auto extra_rules=m.rules();
    check(extra_rules.rank==4&&extra_rules.stage==5&&extra_rules.lives==2&&extra_rules.bombs==1&&extra_rules.shot==1);
    check(m.rank==3&&m.lives==4&&m.bombs==3);
    m.update(0);m.update(Menu::cancel);check(!m.shot_screen&&!m.extra_mode&&m.selected==1);
    m.selected=0;m.update(0);m.update(Menu::confirm);const auto normal_rules=m.rules();
    check(!m.extra_mode&&normal_rules.rank==3&&normal_rules.stage==0&&normal_rules.lives==4&&normal_rules.bombs==3);
    m=Menu{};m.selected=4;m.update(0);m.update(Menu::confirm);check(m.music_room&&m.music_selected==0);
    m.update(0);m.update(Menu::up);check(m.music_selected==16);
    m.update(0);m.update(Menu::up);check(m.music_selected==14);
    m.update(0);m.update(Menu::down);check(m.music_selected==16);
    m.update(0);m.update(Menu::down);check(m.music_selected==0);
    m.update(0);m.update(Menu::down);check(m.music_selected==1&&m.music_comment==0);
    m.update(0);m.update(Menu::confirm);check(m.music_comment==1);
    m.update(0);m.update(Menu::cancel);check(!m.music_room&&m.selected==4);
    return checks;
}
