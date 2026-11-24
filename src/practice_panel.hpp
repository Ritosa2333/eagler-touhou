#pragma once
#include "practice_menu.hpp"

namespace soew {
// Navigation and layout only. PracticeMenu still owns the recipe and numeric editor.
struct PracticePanel {
    using Row=PracticeMenu::Row;
    enum class Page { Shots, Stages, Setup, Advanced };
    enum class Field { Section, Chapter, Phase, Rank, DynamicRank, Power, Lives, Bombs,
        RespawnBombs, Dialogue, SeedMode, Seed, NewSeed, Start, Advanced, Back };
    static constexpr int shot_count=3;
    Page page=Page::Shots;
    Field focus=Field::Start;
    int stage_cursor=0,shot_cursor=0;

    void open(const PracticeMenu &menu,Page target=Page::Shots){
        page=target;stage_cursor=menu.spec.stage;shot_cursor=menu.spec.shot;
        focus=target==Page::Advanced?Field::Power:Field::Start;
    }
    static Row recipe_row(Field f){
        constexpr std::array rows={Row::Section,Row::Chapter,Row::Phase,Row::Rank,
            Row::DynamicRank,Row::Power,Row::Lives,Row::Bombs,Row::RespawnBombs,Row::Dialogue,
            Row::SeedMode,Row::Seed,Row::NewSeed,Row::Start};
        return rows[unsigned(f)];
    }
    static bool is_value(Field f){return f<Field::NewSeed;}
    std::vector<Field> fields(const PracticeMenu &menu) const {
        using F=Field;
        if(page==Page::Advanced){
            std::vector<F> out={F::Power,F::Lives,F::Bombs,F::RespawnBombs,F::DynamicRank};
            if(!menu.spec.boss_phase)out.push_back(F::Dialogue);
            out.push_back(F::SeedMode);
            if(menu.spec.seed_mode==PracticeSeedMode::Fixed){out.push_back(F::Seed);out.push_back(F::NewSeed);}
            out.push_back(F::Back);return out;
        }
        std::vector<F> out={F::Section};
        const auto &s=menu.spec;
        if(s.section==PracticeSection::Chapter)out.push_back(F::Chapter);
        if(s.section==PracticeSection::Boss&&!menu.capabilities.boss_phases[s.stage].empty())out.push_back(F::Phase);
        if(s.stage!=practice_extra_stage)out.push_back(F::Rank);
        out.push_back(F::Start);out.push_back(F::Advanced);return out;
    }
    bool focused(Field field)const{return (page==Page::Setup||page==Page::Advanced)&&focus==field;}
    PracticeMenu::Action update(PracticeMenu &menu,unsigned input){
        using A=PracticeMenu::Action;using F=Field;using M=PracticeMenu;
        if(!input||!menu.allowed||menu.number_editing())return menu.update(input);
        if(page==Page::Shots){
            menu.allowed=false;
            if(input&M::cancel)return A::Back;
            if(input&M::left)shot_cursor=Menu::ring(shot_cursor,-1,shot_count);
            if(input&M::right)shot_cursor=Menu::ring(shot_cursor,1,shot_count);
            if(input&M::confirm)page=Page::Stages;
            return A::None;
        }
        if(page==Page::Stages){
            menu.allowed=false;
            if(input&M::cancel){page=Page::Shots;return A::None;}
            if(input&M::up)stage_cursor=Menu::ring(stage_cursor,-1,practice_stage_count);
            if(input&M::down)stage_cursor=Menu::ring(stage_cursor,1,practice_stage_count);
            if(input&M::confirm){menu.spec.shot=shot_cursor;menu.select_stage(stage_cursor);page=Page::Setup;focus=F::Start;}
            return A::None;
        }
        const auto back=[&]{
            if(page==Page::Advanced){page=Page::Setup;focus=F::Advanced;}
            else page=Page::Stages;
            menu.allowed=false;
        };
        if(input&M::cancel){back();return A::None;}
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
            if(focus==F::Start||focus==F::Advanced)focus=focus==F::Start?F::Advanced:F::Start;
        }else if(input&M::confirm){
            if(focus==F::Back){back();return A::None;}
            if(focus==F::Advanced){page=Page::Advanced;focus=F::Power;}
            else{
                const auto row=recipe_row(focus);
                menu.selected=menu.index_of(row);
                if(focus==F::Section||focus==F::Chapter||focus==F::Phase||focus==F::Rank||focus==F::Dialogue)
                    return menu.update(M::right);
                return menu.update(input);
            }
        }
        menu.allowed=false;return A::None;
    }
};

enum class PracticeText { Body, Small, Heading };

constexpr std::array<LocalizedText,16> practice_panel_labels={{{L"開始地点",L"Start point",L"起点",L"起點"},{L"区間",L"Chapter",L"区段",L"區段"},
    {L"ボスの段階",L"Boss phase",L"首领阶段",L"頭目階段"},{L"難易度",L"Difficulty",L"难度",L"難度"},
    {L"変動ランク",L"Dynamic rank",L"动态难度",L"動態難度"},{L"パワー",L"Power",L"火力",L"火力"},{L"残機",L"Spare lives",L"剩余命数",L"剩餘命數"},
    {L"ボム数",L"Bombs",L"灵击次数",L"靈擊次數"},{L"復帰時ボム数",L"Respawn bombs",L"复活时灵击次数",L"復活時靈擊次數"},
    {L"会話",L"Dialogue",L"对话",L"對話"},{L"シード方式",L"Seed mode",L"种子模式",L"種子模式"},
    {L"固定シード",L"Fixed seed",L"固定种子",L"固定種子"},{L"固定シードを更新",L"New fixed seed",L"更换固定种子",L"更換固定種子"},
    {L"開始",L"Start",L"开始",L"開始"},{L"詳細設定",L"Advanced",L"高级设置",L"進階設定"},text::back}};

template<class Draw,class Portrait>
void draw_practice_panel(std::vector<std::uint32_t> &pixels,const PracticePanel &panel,const PracticeMenu &menu,
                         Language lang,bool loading,Draw &&draw,Portrait &&portrait){
    using P=PracticePanel::Page;using F=PracticePanel::Field;using T=PracticeText;
    constexpr std::uint32_t ink=0xf6edf9,muted=0xada3bc,accent=0xffdb91,border=0x53435e,
        base=0x16121c,rail=0x211827,row=0x231b2a,active=0x493340,accent_ink=0x241b23;
    const auto rect=[&](int l,int t,int r,int b,std::uint32_t color){
        for(int y=t;y<b;++y)std::fill_n(pixels.begin()+y*640+l,r-l,color);
    };
    const auto tr=[&](const wchar_t *ja,const wchar_t *en,const wchar_t *hans,const wchar_t *hant){return localized(lang,ja,en,hans,hant);};
    const auto label=[&](F field){return practice_panel_labels[unsigned(field)](lang);};
    const auto text=[&](std::wstring_view value,int x,int y,int r,int h,std::uint32_t color,T style=T::Body,bool right=false){
        draw(value,x,y,r,h,color,style,right);
    };
    for(auto &p:pixels)p=(p&0xfcfcfcu)>>2;
    rect(46,26,602,382,0x0b090e);rect(40,20,600,380,border);rect(41,21,599,379,base);
    rect(41,21,599,69,rail);rect(41,68,599,69,border);rect(41,21,45,69,accent);
    text(soew::text::practice_start(lang),62,21,352,48,ink,T::Heading);
    rect(57,336,583,337,border);
    if(panel.page==P::Shots){
        constexpr std::array<LocalizedText,PracticePanel::shot_count> styles={{{L"高機動型",L"High mobility",L"高机动型",L"高機動型"},
            {L"防御型",L"Defense",L"防御型",L"防禦型"},{L"攻撃型",L"Offense",L"攻击型",L"攻擊型"}}};
        text(tr(L"ショット選択",L"Select shot type",L"选择机体",L"選擇機體"),57,76,583,24,ink,T::Heading);
        for(int shot=0;shot<PracticePanel::shot_count;++shot){
            const int x=57+178*shot;
            const bool selected=shot==panel.shot_cursor;
            rect(x,110,x+169,316,selected?accent:border);
            rect(x+1,111,x+168,315,selected?active:row);
            portrait(shot,x+5,115,159,151,selected);
            text(std::wstring(1,wchar_t(L'A'+shot)),x+11,272,x+35,34,selected?accent:muted,T::Heading);
            text(styles[shot](lang),x+37,272,x+160,34,selected?ink:muted);
        }
        text(tr(L"← → 選択    Z / Enter 決定    X / Esc 戻る",L"← → Select    Z / Enter Confirm    X / Esc Back",L"← → 选择    Z / Enter 确认    X / Esc 返回",L"← → 選擇    Z / Enter 確認    X / Esc 返回"),57,338,583,32,muted,T::Small);
        return;
    }
    rect(222,84,223,327,border);
    const auto stage_name=[&](int stage){
        if(is_chinese(lang)&&stage!=practice_extra_stage)return L"第 "+std::to_wstring(stage+1)+tr(L"",L"",L" 关",L" 關");
        return stage_label(lang,stage);
    };
    for(int stage=0;stage<practice_stage_count;++stage){
        const int y=86+stage*39;
        const bool cursor=stage==panel.stage_cursor,highlight=cursor&&panel.page==P::Stages;
        rect(57,y,207,y+34,highlight?active:cursor?rail:base);
        if(cursor)rect(57,y,60,y+34,highlight?accent:border);
        text(stage_name(stage),70,y,194,34,highlight?accent:cursor?ink:muted);
    }

    std::optional<PracticeMenu> preview;
    if(panel.page==P::Stages&&panel.stage_cursor!=menu.spec.stage){preview=menu;preview->select_stage(panel.stage_cursor);}
    const auto &model=preview?*preview:menu;
    const auto fields=panel.fields(model);
    const bool dense=panel.page==P::Advanced;
    const int height=dense?20:34, pitch=dense?23:39;
    text(dense?label(F::Advanced):tr(L"練習設定",L"Practice setup",L"练习设置",L"練習設定"),241,76,582,24,ink,T::Heading);
    int slot=0;
    for(const auto field:fields){
        if(field==F::Start||field==F::Advanced)continue;
        const int y=104+slot++*pitch;
        const bool selected=panel.focused(field),editing=selected&&menu.number_editing();
        rect(241,y,582,y+height,selected?active:row);
        if(selected)rect(241,y,243,y+height,accent);
        text(label(field),252,y,401,height,selected?ink:muted);
        if(PracticePanel::is_value(field)){
            const auto value=practice_value(model,PracticePanel::recipe_row(field),lang);
            if(selected){
                if(editing)rect(402,y+2,572,y+height-2,base);
                else{
                    text(L"‹",403,y,414,height,accent);
                    text(L"›",562,y,573,height,accent);
                }
            }
            text(value,419,y,554,height,selected?accent:ink,T::Body,true);
        }
    }
    if(panel.page==P::Setup){
        const auto button=[&](F field,int l,int r){
            const bool selected=panel.focused(field),primary=field==F::Start;
            rect(l,292,r,325,selected?(primary?accent:active):rail);
            if(selected&&!primary)rect(l,292,l+2,325,accent);
            text(label(field),l+12,292,r-10,33,selected&&primary?accent_ink:selected?accent:ink);
        };
        button(F::Start,241,402);button(F::Advanced,412,582);
    }
    const auto status=[&]() -> std::wstring {
        if(loading)return tr(L"練習を準備中…",L"Preparing practice…",L"正在准备练习…",L"正在準備練習…");
        if(!menu.warning.empty())return practice_warning(menu.warning,lang);
        if(menu.number_editing())return tr(L"Enter：確定　Esc：取消",L"Enter: apply    Esc: cancel",L"Enter：确认    Esc：取消",L"Enter：確認    Esc：取消");
        if(panel.page==P::Stages)return tr(L"↑ ↓ 選択    Z / Enter 決定    X / Esc 戻る",L"↑ ↓ Select    Z / Enter Confirm    X / Esc Back",L"↑ ↓ 选择    Z / Enter 确认    X / Esc 返回",L"↑ ↓ 選擇    Z / Enter 確認    X / Esc 返回");
        return tr(L"↑ ↓ 移動    ← → 変更    Z / Enter 決定    X / Esc 戻る",L"↑ ↓ Move    ← → Change    Z / Enter Confirm    X / Esc Back",L"↑ ↓ 移动    ← → 修改    Z / Enter 确认    X / Esc 返回",L"↑ ↓ 移動    ← → 修改    Z / Enter 確認    X / Esc 返回");
    };
    text(status(),57,338,583,32,loading||!menu.warning.empty()?accent:muted,T::Small);
}
}
