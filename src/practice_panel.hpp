#pragma once
#include "practice_menu.hpp"
#include "practice_window.hpp"

namespace soew {
// Navigation and layout only. PracticeMenu still owns the recipe and numeric editor.
struct PracticePanel {
    using Row=PracticeMenu::Row;
    enum class Page { Stages, Setup };
    enum class Field { Section, Chapter, Phase, Rank, DynamicRank, Power, Lives, Bombs,
        RespawnBombs, Dialogue, SeedMode, Seed, NewSeed, Start };
    Page page=Page::Stages;
    Field focus=Field::Start;
    int stage_cursor=0,shot=0;
    PracticeWindow window;
    static constexpr int shadow=practice_shadow_extent,stage_pitch=24,setup_pitch=19;
    static constexpr PracticeBounds stages_box{280,140,592,316},setup_box{232,44,592,352};
    static constexpr int stages_y=160,setup_y=80;

    PracticeBounds base()const{return page==Page::Stages?stages_box:setup_box;}
    bool in_header(PracticePoint p)const{
        auto header=base();header.bottom=page==Page::Stages?stages_y:setup_y;
        return header.contains(window.local(p));
    }
    void show(Page target){page=target;window.move(base(),window.offset,shadow);}

    void open(const PracticeMenu &menu,Page target=Page::Stages){
        show(target);stage_cursor=menu.spec.stage;shot=menu.spec.shot;
        focus=Field::Start;
    }
    bool open_from_shot(PracticeMenu &menu,Menu &native){
        if(!native.practice_shot_select()||!native.shot_confirmed)return false;
        open(menu);shot=native.shot;
        native.shot_screen=native.shot_confirmed=false;
        menu.allowed=false;
        return true;
    }
    static Row recipe_row(Field f){
        constexpr std::array rows={Row::Section,Row::Chapter,Row::Phase,Row::Rank,
            Row::DynamicRank,Row::Power,Row::Lives,Row::Bombs,Row::RespawnBombs,Row::Dialogue,
            Row::SeedMode,Row::Seed,Row::NewSeed,Row::Start};
        return rows[unsigned(f)];
    }
    static bool is_value(Field f){return f<Field::NewSeed;}
    static std::vector<Field> fields(const PracticeMenu &menu){
        using F=Field;
        std::vector<F> out={F::Section};
        const auto &s=menu.spec;
        if(s.section==PracticeSection::Chapter)out.push_back(F::Chapter);
        if(s.section==PracticeSection::Boss&&!menu.capabilities.boss_phases[s.stage].empty())out.push_back(F::Phase);
        if(s.stage!=practice_extra_stage)out.push_back(F::Rank);
        out.insert(out.end(),{F::Power,F::Lives,F::Bombs,F::RespawnBombs,F::DynamicRank});
        if(!s.boss_phase)out.push_back(F::Dialogue);
        out.push_back(F::SeedMode);
        if(s.seed_mode==PracticeSeedMode::Fixed){out.push_back(F::Seed);out.push_back(F::NewSeed);}
        out.push_back(F::Start);return out;
    }
    bool focused(Field field)const{return page==Page::Setup&&focus==field;}
    unsigned point(PracticeMenu &menu,PracticePoint p){
        if(menu.number_editing())return 0;
        p=window.local(p);
        if(!base().contains(p))return 0;
        if(page==Page::Stages){
            if(p.y<stages_y||p.y>=stages_y+stage_pitch*practice_stage_count)return 0;
            stage_cursor=(p.y-stages_y)/stage_pitch;
            return PracticeMenu::confirm;
        }
        if(p.y<setup_y)return 0;
        const auto visible=fields(menu);
        const auto slot=std::size_t((p.y-setup_y)/setup_pitch);
        if(slot>=visible.size())return 0;
        focus=visible[slot];
        if(!is_value(focus))return PracticeMenu::confirm;
        if(p.x<425)return 0;
        if(focus==Field::Seed||focus==Field::DynamicRank||focus==Field::Power||focus==Field::Lives||
            focus==Field::Bombs||focus==Field::RespawnBombs)return PracticeMenu::confirm;
        return p.x<500?PracticeMenu::left:PracticeMenu::right;
    }
    PracticeMenu::Action update(PracticeMenu &menu,unsigned input){
        using A=PracticeMenu::Action;using F=Field;using M=PracticeMenu;
        if(!input||!menu.allowed||menu.number_editing())return menu.update(input);
        if(page==Page::Stages){
            menu.allowed=false;
            if(input&M::cancel)return A::Back;
            if(input&M::up)stage_cursor=Menu::ring(stage_cursor,-1,practice_stage_count);
            if(input&M::down)stage_cursor=Menu::ring(stage_cursor,1,practice_stage_count);
            if(input&M::confirm){menu.spec.shot=shot;menu.select_stage(stage_cursor);show(Page::Setup);focus=F::Start;}
            return A::None;
        }
        if(input&M::cancel){show(Page::Stages);menu.allowed=false;return A::None;}
        if(input&(M::up|M::down)){
            const auto visible=fields(menu);
            const auto at=std::find(visible.begin(),visible.end(),focus)-visible.begin();
            focus=visible[Menu::ring(int(at),(input&M::up)?-1:1,int(visible.size()))];
            menu.allowed=false;return A::None;
        }
        if(input&(M::left|M::right)){
            if(is_value(focus)){
                menu.selected=menu.index_of(recipe_row(focus));
                return menu.update(input);
            }
        }else if(input&M::confirm){
            menu.selected=menu.index_of(recipe_row(focus));
            if(focus==F::Section||focus==F::Chapter||focus==F::Phase||focus==F::Rank||focus==F::Dialogue)
                return menu.update(M::right);
            return menu.update(input);
        }
        menu.allowed=false;return A::None;
    }
};

enum class PracticeText { Body, Small, Stage };

inline std::wstring practice_stage_name(Language lang,int stage){
    return stage==practice_extra_stage?std::wstring(lang==Language::English?L"EXTRA":text::extra(lang)):
        localized(lang,L"ステージ ",L"STAGE ",L"第",L"第")+std::to_wstring(stage+1)+
        localized(lang,L"",L"",L"关",L"關");
}

constexpr std::array<LocalizedText,14> practice_panel_labels={{{L"開始地点",L"Start point",L"起点",L"起點"},{L"区間",L"Chapter",L"区段",L"區段"},
    {L"ボスの段階",L"Boss phase",L"首领阶段",L"頭目階段"},{L"難易度",L"Difficulty",L"难度",L"難度"},
    {L"変動ランク",L"Dynamic rank",L"动态难度",L"動態難度"},{L"パワー",L"Power",L"火力",L"火力"},{L"残機",L"Spare lives",L"剩余命数",L"剩餘命數"},
    {L"ボム数",L"Bombs",L"灵击次数",L"靈擊次數"},{L"復帰時ボム数",L"Respawn bombs",L"复活时灵击次数",L"復活時靈擊次數"},
    {L"会話",L"Dialogue",L"对话",L"對話"},{L"シード方式",L"Seed mode",L"种子模式",L"種子模式"},
    {L"固定シード",L"Fixed seed",L"固定种子",L"固定種子"},{L"固定シード更新",L"New fixed seed",L"更换固定种子",L"更換固定種子"},
    {L"開始",L"Start",L"开始",L"開始"}}};

// TH05 native-practice: overlay the selection screen, with six gaiji rows
// at a 24 px pitch. SoEW's 400-line framebuffer omits TH05's 40 px top margin.
template<class Box,class Draw>
void draw_practice_panel(std::vector<std::uint32_t> &pixels,const PracticePanel &panel,const PracticeMenu &menu,
                         Language lang,bool loading,std::uint32_t active,std::uint32_t normal,Box &&box,Draw &&draw){
    using P=PracticePanel::Page;using F=PracticePanel::Field;using T=PracticeText;
    const auto offset=panel.window.offset;
    const auto bounds=panel.window.bounds(panel.base());
    const auto rect=[&](int l,int t,int r,int b,std::uint32_t color){
        for(int y=t;y<b;++y)std::fill_n(pixels.begin()+(y+offset.y)*640+l+offset.x,r-l,color);
    };
    const auto tr=[&](const wchar_t *ja,const wchar_t *en,const wchar_t *hans,const wchar_t *hant){return localized(lang,ja,en,hans,hant);};
    const auto label=[&](F field){return practice_panel_labels[unsigned(field)](lang);};
    const auto text=[&](std::wstring_view value,int x,int y,int r,int h,std::uint32_t color,T style=T::Body,bool right=false){
        draw(value,x+offset.x,y+offset.y,r+offset.x,h,color,style,right);
    };
    for(auto &p:pixels)p=(p&0xfefefeu)>>1;
    box(bounds.left,bounds.top,bounds.right,bounds.bottom,active);
    if(panel.page==P::Stages){
        for(int stage=0;stage<practice_stage_count;++stage)
            text(practice_stage_name(lang,stage),296,PracticePanel::stages_y+stage*PracticePanel::stage_pitch,576,16,stage==panel.stage_cursor?active:normal,T::Stage);
        return;
    }
    constexpr int content_left=248,content_right=576,pitch=PracticePanel::setup_pitch;
    text(practice_stage_name(lang,menu.spec.stage),content_left,57,content_right,16,active,T::Stage);
    int slot=0;
    for(const auto field:PracticePanel::fields(menu)){
        const int y=PracticePanel::setup_y+slot++*pitch;
        const bool selected=panel.focused(field),editing=selected&&menu.number_editing();
        if(selected)rect(240,y+5,243,y+14,active);
        text(label(field),content_left,y,424,pitch,selected?active:normal);
        if(PracticePanel::is_value(field)){
            if(editing)rect(430,y+1,577,y+pitch-1,(active&0xfcfcfcu)>>2);
            else if(selected)text(L"›",572,y,586,pitch,active);
            text(practice_value(menu,PracticePanel::recipe_row(field),lang),434,y,568,pitch,selected?active:normal,T::Body,true);
        }
    }
    const auto status=[&]() -> std::wstring {
        if(loading)return tr(L"練習を準備中…",L"Preparing practice…",L"正在准备练习…",L"正在準備練習…");
        if(!menu.warning.empty())return practice_warning(menu.warning,lang);
        if(menu.number_editing())return tr(L"Enter：確定　Esc：取消",L"Enter: apply    Esc: cancel",L"Enter：确认    Esc：取消",L"Enter：確認    Esc：取消");
        return {};
    };
    if(const auto message=status();!message.empty())
        text(message,content_left,331,content_right,18,loading||!menu.warning.empty()?active:normal,T::Small);
}
}
